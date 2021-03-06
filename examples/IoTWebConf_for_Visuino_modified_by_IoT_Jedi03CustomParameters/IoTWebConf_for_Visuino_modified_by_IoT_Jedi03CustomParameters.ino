/**
 * IoTJediWebConfSettings.h -- IoTJediWebConf is an ESP8266/ESP32
 *   non blocking WiFi/AP web configuration library for Arduino.
 *   https://github.com/BadASszZ/IoTWebConf_for_Visuino_modified_by_IoT_Jedi
 * --- Based on https://github.com/prampec/IotWebConf ---
 * Copyright (C) 2020 Balazs Kelemen <prampec+arduino@gmail.com>
 * 
 * Changed by IoT_Jedi to work together with Visuino
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
 * Example: Custom parameters
 * Description:
 *   In this example it is shown how to attach your custom parameters
 *   to the config portal. Your parameters will be maintained by 
 *   IotWebConf. This means, they will be loaded from/saved to EEPROM,
 *   and will appear in the config portal.
 *   Note the configSaved and formValidator callbacks!
 *   (See previous examples for more details!)
 * 
 * Hardware setup for this example:
 *   - An LED is attached to LED_BUILTIN pin with setup On=LOW.
 *   - [Optional] A push button is attached to pin D2, the other leg of the
 *     button should be attached to GND.
 */

#include <IoTWebConf_for_Visuino_modified_by_IoT_Jedi.h>
#include <IoTWebConf_for_Visuino_modified_by_IoT_JediUsing.h> // This loads aliases for easier class names.

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "testThing";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "smrtTHNG8266";

#define STRING_LEN 128
#define NUMBER_LEN 32

// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "dem2"

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN D2

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN

// -- Method declarations.
void handleRoot();
// -- Callback methods.
void configSaved();
bool formValidator();

DNSServer dnsServer;
WebServer server(80);

char stringParamValue[STRING_LEN];
char intParamValue[NUMBER_LEN];
char floatParamValue[NUMBER_LEN];
char checkboxParamValue[STRING_LEN];
char chooserParamValue[STRING_LEN];

static char chooserValues[][STRING_LEN] = { "red", "blue", "darkYellow" };
static char chooserNames[][STRING_LEN] = { "Red", "Blue", "Dark yellow" };

IotWebConf IoTWebConf_for_Visuino_modified_by_IoT_Jedi(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
// -- You can also use namespace formats e.g.: iotwebconf::TextParameter
IotWebConfTextParameter stringParam = IotWebConfTextParameter("String param", "stringParam", stringParamValue, STRING_LEN);
IotWebConfParameterGroup group1 = IotWebConfParameterGroup("group1", "");
IotWebConfNumberParameter intParam = IotWebConfNumberParameter("Int param", "intParam", intParamValue, NUMBER_LEN, "20", "1..100", "min='1' max='100' step='1'");
// -- We can add a legend to the separator
IotWebConfParameterGroup group2 = IotWebConfParameterGroup("c_factor", "Calibration factor");
IotWebConfNumberParameter floatParam = IotWebConfNumberParameter("Float param", "floatParam", floatParamValue, NUMBER_LEN,  NULL, "e.g. 23.4", "step='0.1'");
IotWebConfCheckboxParameter checkboxParam = IotWebConfCheckboxParameter("Check param", "checkParam", checkboxParamValue, STRING_LEN,  true);
IotWebConfSelectParameter chooserParam = IotWebConfSelectParameter("Choose param", "chooseParam", chooserParamValue, STRING_LEN, (char*)chooserValues, (char*)chooserNames, sizeof(chooserValues) / STRING_LEN, STRING_LEN);

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  group1.addItem(&intParam);
  group2.addItem(&floatParam);
  group2.addItem(&checkboxParam);
  group2.addItem(&chooserParam);

  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setStatusPin(STATUS_PIN);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setConfigPin(CONFIG_PIN);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.addSystemParameter(&stringParam);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.addParameterGroup(&group1);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.addParameterGroup(&group2);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setConfigSavedCallback(&configSaved);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setFormValidator(&formValidator);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.getApTimeoutParameter()->visible = true;

  // -- Initializing the configuration.
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ IoTWebConf_for_Visuino_modified_by_IoT_Jedi.handleConfig(); });
  server.onNotFound([](){ IoTWebConf_for_Visuino_modified_by_IoT_Jedi.handleNotFound(); });

  Serial.println("Ready.");
}

void loop() 
{
  // -- doLoop should be called as frequently as possible.
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.doLoop();
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (ioTWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 03 Custom Parameters</title></head><body>Hello world!";
  s += "<ul>";
  s += "<li>String param value: ";
  s += stringParamValue;
  s += "<li>Int param value: ";
  s += atoi(intParamValue);
  s += "<li>Float param value: ";
  s += atof(floatParamValue);
  s += "<li>CheckBox selected: ";
//  s += checkboxParam.isChecked();
  s += "<li>Option selected: ";
  s += chooserParamValue;
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void configSaved()
{
  Serial.println("Configuration was updated.");
}

bool formValidator()
{
  Serial.println("Validating form.");
  bool valid = true;

/*
  int l = server.arg(stringParam.getId()).length();
  if (l < 3)
  {
    stringParam.errorMessage = "Please provide at least 3 characters for this test!";
    valid = false;
  }
*/
  return valid;
}

