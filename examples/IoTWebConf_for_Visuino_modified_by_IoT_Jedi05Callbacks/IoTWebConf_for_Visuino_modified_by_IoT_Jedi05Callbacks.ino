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
 * Example: Callbacks
 * Description:
 *   This example shows, what callbacks IotWebConf provides.
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

// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "dem3"

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
void wifiConnected();
void configSaved();
bool formValidator();

DNSServer dnsServer;
WebServer server(80);

char stringParamValue[STRING_LEN];

IotWebConf IoTWebConf_for_Visuino_modified_by_IoT_Jedi(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
// -- You can also use namespace formats e.g.: iotwebconf::TextParameter
IotWebConfTextParameter stringParam = IotWebConfTextParameter("String param", "stringParam", stringParamValue, STRING_LEN);

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setStatusPin(STATUS_PIN);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setConfigPin(CONFIG_PIN);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.addSystemParameter(&stringParam);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setConfigSavedCallback(&configSaved);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setFormValidator(&formValidator);
  IoTWebConf_for_Visuino_modified_by_IoT_Jedi.setWifiConnectionCallback(&wifiConnected);

  // -- Initializing the configuration.
  bool validConfig = IoTWebConf_for_Visuino_modified_by_IoT_Jedi.init();
  if (!validConfig)
  {
    stringParamValue[0] = '\0';
  }

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
  s += "<title>IotWebConf 05 Callbacks</title></head><body>Hello world!";
  s += "<ul>";
  s += "<li>String param value: ";
  s += stringParamValue;
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void wifiConnected()
{
  Serial.println("WiFi was connected.");
}

void configSaved()
{
  Serial.println("Configuration was updated.");
}

bool formValidator()
{
  Serial.println("Validating form.");
  bool valid = true;

  int l = server.arg(stringParam.getId()).length();
  if (l < 3)
  {
    stringParam.errorMessage = "Please provide at least 3 characters for this test!";
    valid = false;
  }

  return valid;
}

