/**
 * IotWebConf03TypedParameters.ino -- IotWebConf is an ESP8266/ESP32
 *   non blocking WiFi/AP web configuration library for Arduino.
 *   https://github.com/prampec/IotWebConf
 *
 * Copyright (C) 2020 Balazs Kelemen <prampec+arduino@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
 * Example: Custom parameters
 * Description:
 *   Typed Parameters are a new approach to store/handle parameter data.
 *   This part of the software is very experimental, and certainly
 *   not recommended for beginners.
 *   The goal of this particular example is to compare the original
 *   approach of IotWebConf03CustomParameters to this new typed
 *   parameters, as both examples should work the same.
 *
 * Hardware setup for this example:
 *   - An LED is attached to LED_BUILTIN pin with setup On=LOW.
 *   - [Optional] A push button is attached to pin D2, the other leg of the
 *     button should be attached to GND.
 */

/*
********* Anpassungen für  customHtml   **************   //UPDATE: HK
E:\PlatformIO\_stdLibs\IotWebConf-3.2.1\src\IotWebConfTParameterBuilder.h
E:\PlatformIO\_stdLibs\IotWebConf-3.2.1\src\IotWebConfTParameter.h

*/

#include <Arduino.h>
#include <Streaming.h>
#include <myMacros.h>
#include <myUtils.h>

#define DEBUG__EIN  //"Schalter" zum aktivieren von DEBUG-Ausgaben
#include <myDebug.h>

#include <IotWebConf.h>
#include <IotWebConfTParameter.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "testThing";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = WLAN_AP_PASS;

#define STRING_LEN 128
#define NUMBER_LEN 32

// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "dem4"

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
bool formValidator(iotwebconf::WebRequestWrapper* webRequestWrapper);

DNSServer dnsServer;
WebServer server(80);

static const char chooserValues[][STRING_LEN] = { "red", "blue", "darkYellow" };
static const char chooserNames[][STRING_LEN] = { "Red", "Blue", "Dark yellow" };

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
iotwebconf::TextTParameter<STRING_LEN> stringParam =
  iotwebconf::Builder<iotwebconf::TextTParameter<STRING_LEN>>("stringParam").
  label("String param").
  defaultValue("").
  customHtml("style=\"float: left;\"").
  build();

iotwebconf::ParameterGroup group1 = iotwebconf::ParameterGroup("group1", "");
iotwebconf::IntTParameter<int16_t> intParam =
  iotwebconf::Builder<iotwebconf::IntTParameter<int16_t>>("intParam").customHtml("style=\"float: left;\"").
  label("Int param").
  defaultValue(30).
  min(1).
  max(100).
  step(1).
  placeholder("1..100").
  build();
// -- We can add a legend to the separator
iotwebconf::ParameterGroup group2 = iotwebconf::ParameterGroup("c_factor", "Calibration factor");
iotwebconf::FloatTParameter floatParam =
   iotwebconf::Builder<iotwebconf::FloatTParameter>("floatParam").customHtml("style=\"float: left;\"").
   label("Float param").
   defaultValue(0.0).
   step(0.1).
   placeholder("e.g. 23.4").
   build();
iotwebconf::CheckboxTParameter checkboxParam =
   iotwebconf::Builder<iotwebconf::CheckboxTParameter>("checkParam").customHtml("style=\"float: left;\"").
   label("Check param").
   defaultValue(true).
   build();
iotwebconf::SelectTParameter<STRING_LEN> chooserParam =
   iotwebconf::Builder<iotwebconf::SelectTParameter<STRING_LEN>>("chooseParam").customHtml("style=\"float: left;\"").
   label("Choose param").
   optionValues((const char*)chooserValues).
   optionNames((const char*)chooserNames).
   optionCount(sizeof(chooserValues) / STRING_LEN).
   nameLength(STRING_LEN).
   defaultValue("blue").
   build();
iotwebconf::ColorTParameter colorParam =
   iotwebconf::Builder<iotwebconf::ColorTParameter>("colorParam").customHtml("style=\"float: left;\"").
   label("Choose color").
   defaultValue("#FFDD88").
   build();
iotwebconf::DateTParameter dateParam =
   iotwebconf::Builder<iotwebconf::DateTParameter>("dateParam").customHtml("style=\"float: left;\"").
   label("Select date").
   defaultValue("").
   build();
iotwebconf::TimeTParameter timeParam =
   iotwebconf::Builder<iotwebconf::TimeTParameter>("timeParam").
   label("Select time").
   defaultValue("").
   customHtml("style=\"float: left;\"").
   build();


//**************************************************
// -- Javascript block will be added to the header.
const char CUSTOMHTML_SCRIPT_INNER[] PROGMEM = "\n\
document.addEventListener('DOMContentLoaded', function(event) {\n\
  let elements = document.querySelectorAll('input[type=\"password\"]');\n\
  for (let p of elements) {\n\
  	let btn = document.createElement('INPUT'); btn.type = 'button'; btn.value = '🔓'; btn.style.width = 'auto'; p.style.width = '90%'; p.parentNode.insertBefore(btn,p.nextSibling);\n\
    btn.onclick = function() { if (p.type === 'password') { p.type = 'text'; btn.value = '🔒'; } else { p.type = 'password'; btn.value = '🔓'; } }\n\
  };\n\
});\n";
// -- HTML element will be added inside the body element.
const char CUSTOMHTML_BODY_INNER[] PROGMEM = "<div><img src='data:image/png;base64, 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'/></div>\n";

const char CUSTOMHTML_STYLE_INNER[] PROGMEM ="\n\
label{display: block;}\
#content {width: 50%;}\
.floatParam, .dateParam, .timeParam {float:left;} #floatParam{color:red;}\
\n\"";

// -- This is an OOP technique to override behaviour of the existing
// IotWebConfHtmlFormatProvider. Here two method are overridden from
// the original class. See IotWebConf.h for all potentially overridable
// methods of IotWebConfHtmlFormatProvider .
class CustomHtmlFormatProvider : public iotwebconf::HtmlFormatProvider
{
protected:
  String getScriptInner() override
  {
    return
      HtmlFormatProvider::getScriptInner() +
      String(FPSTR(CUSTOMHTML_SCRIPT_INNER));
  }
  String getBodyInner() override
  {
    return
      String(FPSTR(CUSTOMHTML_BODY_INNER)) +
      HtmlFormatProvider::getBodyInner();
  }
  String getStyleInner()override
  {
    return
        HtmlFormatProvider::getStyleInner() + "\n" +
        String(FPSTR(CUSTOMHTML_STYLE_INNER));
  }
};

// -- An instance must be created from the class defined above.
CustomHtmlFormatProvider customHtmlFormatProvider;

//**************************************************

void setup()
{
  Serial.begin(BAUD);
  while (!Serial && (millis() < 3000));
  Serial << "\n\n" << ProjektName << " - " << VERSION << "  (" << BUILDDATE << "  " __TIME__ << ")" << endl;
  Serial.println();
  Serial.println("Starting up...");

  group1.addItem(&intParam);
  group2.addItem(&floatParam);
  group2.addItem(&checkboxParam);
  group2.addItem(&chooserParam);
  group2.addItem(&colorParam);
  group2.addItem(&dateParam);
  group2.addItem(&timeParam);

  iotWebConf.setStatusPin(STATUS_PIN);
  iotWebConf.setConfigPin(CONFIG_PIN);
  iotWebConf.addSystemParameter(&stringParam);
  iotWebConf.addParameterGroup(&group1);
  iotWebConf.addParameterGroup(&group2);
  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setFormValidator(&formValidator);
  iotWebConf.getApTimeoutParameter()->visible = true;

    // -- Applying the new HTML format to IotWebConf.
  iotWebConf.setHtmlFormatProvider(&customHtmlFormatProvider);

  // -- Initializing the configuration.
  bool validConfig = iotWebConf.init();

  if (!validConfig) {  // Wenn keine Konfiguration vorhanden, dann Default setzen
    DEBUG__PRINTLN("Keine gültige Konfig --> Default setzen");
    // iotWebConf.getWifiSsidParameter();
    // iotWebConf.getWifiPasswordParameter();

    // ??? strncpy(iotWebConf.getSystemParameterGroup()->valueBuffer, "My changed name", iotWebConf.getSystemParameterGroup()->getLength());
    // strncpy(iotWebConf.getThingNameParameter()->valueBuffer, thingName, iotWebConf.getThingNameParameter()->getLength());
    strncpy(iotWebConf.getApPasswordParameter()->valueBuffer, WLAN_AP_PASS, iotWebConf.getApPasswordParameter()->getLength());
    // ??? strncpy(iotWebConf.getWifiParameterGroup()->valueBuffer, "My changed name", iotWebConf.getWifiParameterGroup()->getLength());
    strncpy(iotWebConf.getWifiSsidParameter()->valueBuffer, WLAN_SSID, iotWebConf.getWifiSsidParameter()->getLength());
    strncpy(iotWebConf.getWifiPasswordParameter()->valueBuffer, WLAN_PASS, iotWebConf.getWifiPasswordParameter()->getLength());
    //strncpy(iotWebConf.getApTimeoutParameter()->valueBuffer, "My changed name", iotWebConf.getApTimeoutParameter()->getLength());

    iotWebConf.saveConfig();
  }

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("Ready.");
}

void loop()
{
  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 03 Custom Parameters</title></head><body>Hello world!";
  s += "<ul>";
  s += "<li>String param value: ";
  s += stringParam.value();
  s += "<li>Int param value: ";
  s += intParam.value();
  s += "<li>Float param value: ";
  s += floatParam.value();
  s += "<li>CheckBox selected: ";
  s += checkboxParam.isChecked();
  s += "<li>Option selected: ";
  s += chooserParam.value();
  s += "<li>Color selected: <div style='background-color:";
  s += colorParam.value();
  s += "';> sample </div> (";
  s += colorParam.value();
  s += ")";
  s += "<li>Date value: ";
  s += dateParam.value();
  s += "<li>Time value: ";
  s += timeParam.value();
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void configSaved()
{
  Serial.println("Configuration was updated.");
}

bool formValidator(iotwebconf::WebRequestWrapper* webRequestWrapper)
{
  Serial.println("Validating form.");
  bool valid = true;

/*
  int l = webRequestWrapper->arg(stringParam.getId()).length();
  if (l < 3)
  {
    stringParam.errorMessage = "Please provide at least 3 characters for this test!";
    valid = false;
  }
*/
  return valid;
}
