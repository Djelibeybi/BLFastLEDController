#include <Arduino.h>
bool shouldRestart = false;
unsigned long restartRequestTime = 0;
#include "./blflc/logserial.h"
#include "./blflc/leds.h"
#include "./blflc/filesystem.h"
#include "./blflc/types.h"
#include "./blflc/bblprinterdiscovery.h"
#include "./blflc/web-server.h"
#include "./blflc/mqttmanager.h"
#include "./blflc/ssdp.h"

#ifdef USE_ETHERNET
#include "./blflc/eth-manager.h"
#else
#include "./blflc/wifi-manager.h"
#include "./blflc/improv-serial.h"
int wifi_reconnect_count = 0;
#endif

void defaultcolors()
{
    LogSerial.println(F("Setting default customisable colors"));
    printerConfig.runningColor = hex2rgb("#FFFFFF"); // WHITE Running
    printerConfig.testColor = hex2rgb("#3F3CFB");    // Violet Test
    printerConfig.finishColor = hex2rgb("#00FF00");  // Green Finish

    printerConfig.stage14Color = hex2rgb("#000000"); // OFF Cleaning Nozzle
    printerConfig.stage1Color = hex2rgb("#000055");  // OFF Bed Leveling
    printerConfig.stage8Color = hex2rgb("#000000");  // OFF Calibrating Extrusion
    printerConfig.stage9Color = hex2rgb("#000000");  // OFF Scanning Bed Surface
    printerConfig.stage10Color = hex2rgb("#000000"); // OFF First Layer Inspection

    printerConfig.wifiRGB = hex2rgb("#FFA500"); // Orange Wifi Scan

    printerConfig.pauseRGB = hex2rgb("#0000FF");          // Blue Pause
    printerConfig.firstlayerRGB = hex2rgb("#0000FF");     // Blue
    printerConfig.nozzleclogRGB = hex2rgb("#0000FF");     // Blue
    printerConfig.hmsSeriousRGB = hex2rgb("#FF0000");     // Red
    printerConfig.hmsFatalRGB = hex2rgb("#FF0000");       // Red
    printerConfig.filamentRunoutRGB = hex2rgb("#FF0000"); // Red
    printerConfig.frontCoverRGB = hex2rgb("#FF0000");     // Red
    printerConfig.nozzleTempRGB = hex2rgb("#FF0000");     // Red
    printerConfig.bedTempRGB = hex2rgb("#FF0000");        // Red
}

void setup()
{
    Serial.begin(115200);
    delay(100);

    Serial.println(F("Initializing"));
    Serial.println(ESP.getFreeHeap());
    Serial.println("");
    Serial.print(F("** Using firmware version: "));
    Serial.print(globalVariables.FWVersion);
    Serial.println(F(" **"));
    Serial.println("");
    defaultcolors();

    setupFileSystem();
    loadFileSystem();

    Serial.println(F(""));

    setupRelay();
    setRelayState(true);

    setupLeds();

    setLedColor(CRGB(100, 100, 100)); // WHITE - ALL LEDS ON
    Serial.println(F(""));

    setLedColor(colorToCRGB(printerConfig.wifiRGB)); // Customisable - Default is ORANGE

#ifdef USE_ETHERNET
    // Ethernet mode - no WiFi configuration needed
    Serial.println(F("[Ethernet] Starting ethernet connection..."));
    setupEthernet();

    if (!isEthernetConnected())
    {
        Serial.println(F("[Ethernet] No connection - check cable"));
        setLedColor(CRGB(255, 0, 0)); // RED - error
        // Still start webserver for diagnostics if we have link but no IP
        setupWebserver();
        return;
    }
    else
    {
        Serial.println(F("[Ethernet] Connected. Starting webUI."));
        setLedColor(CRGB(0, 0, 255)); // BLUE
        setupWebserver();
    }
#else
    // WiFi mode
    if (strlen(globalVariables.SSID) == 0 || strlen(globalVariables.APPW) == 0)
    {
        Serial.println(F("SSID or password is missing. Starting Improv Serial for WiFi provisioning."));
        setLedColor(CRGB(100, 0, 100)); // PINK

        // Initialize Improv Serial for WiFi provisioning via ESP Web Tools
        setupImprovSerial();

        startAPMode();
        setupWebserver();
        return;
    }
    else

        if (!connectToWifi())
    {
        Serial.println(F("[WiFiManager] Not connected → AP Mode"));
        startAPMode();
        setupWebserver();
        return;
    }
    else
    {
        Serial.println(F("[WiFiManager] connected. Starting webUI."));
        setLedColor(CRGB(0, 0, 255)); // BLUE
        setupWebserver();
    }
#endif

    start_ssdp();

    setLedColor(CRGB(34, 224, 238)); // CYAN
    setupMqtt();
    // >>> Fix: prevent false offline after 30s
    printerVariables.disconnectMQTTms = millis();
    Serial.println();
    Serial.print(F("** BLFLC Controller started "));
    Serial.print(F("using firmware version: "));
    Serial.print(globalVariables.FWVersion);
    Serial.println(F(" **"));
    Serial.println();
    globalVariables.started = true;
    Serial.println(F("Updating LEDs from Setup"));
    updateleds();
}

void loop()
{
#ifndef USE_ETHERNET
    // Process Improv Serial for WiFi provisioning (WiFi mode only)
    loopImprovSerial();
#endif

    if (globalVariables.started)
    {
        websocketLoop();
        ledsloop();

#ifdef USE_ETHERNET
        // Ethernet mode - check connection status
        if (!isEthernetConnected())
        {
            // Ethernet disconnected - LED feedback handled by event handler
            // Connection will auto-restore when cable reconnected
        }
        else if (!printerVariables.online)
        {
            // Only search for printers if MQTT is not connected
            bblSearchPrinters();
        }
#else
        // WiFi mode - handle reconnection
        if (WiFi.status() != WL_CONNECTED)
        {
            LogSerial.print(F("Wifi connection dropped.  "));
            LogSerial.print(F("Wifi Status: "));
            LogSerial.println(wl_status_to_string(WiFi.status()));
            LogSerial.println(F("Attempting to reconnect to WiFi..."));
            wifi_reconnect_count += 1;
            if (wifi_reconnect_count <= 2)
            {
                WiFi.disconnect();
                delay(100);
                WiFi.reconnect();
            }
            else
            {
                // Not connecting after 10 simple disconnect / reconnects
                // Do something more drastic in case needing to switch to new AP
                scanNetwork();
                connectToWifi();
                wifi_reconnect_count = 0;
            }
        }
        if (WiFi.getMode() == WIFI_AP)
        {
            dnsServer.processNextRequest();
        }
        if (WiFi.status() == WL_CONNECTED && WiFi.getMode() != WIFI_AP && !printerVariables.online)
        {
            // Only search for printers if MQTT is not connected
            bblSearchPrinters();
        }
#endif
    }

#ifndef USE_ETHERNET
    if (printerConfig.rescanWiFiNetwork)
    {
        LogSerial.println(F("Web submitted refresh of Wifi Scan (assigning Strongest AP)"));
        setLedColor(colorToCRGB(printerConfig.wifiRGB)); // Customisable - Default is ORANGE
        scanNetwork();                                   // Sets the MAC address for following connection attempt
        printerConfig.rescanWiFiNetwork = false;
        updateleds();
    }
#endif

    if (shouldRestart && millis() - restartRequestTime > 1500)
    {
        LogSerial.println(F("[Setup] Restarting now..."));
        ESP.restart();
    }
}
