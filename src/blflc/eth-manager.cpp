#include "eth-manager.h"
#include "types.h"

bool ethConnected = false;
bool ethGotIP = false;

void onEthEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_ETH_START:
            Serial.println(F("[Ethernet] Started"));
            ETH.setHostname(globalVariables.hostname);
            break;

        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println(F("[Ethernet] Link Up"));
            Serial.print(F("[Ethernet] Speed: "));
            Serial.print(ETH.linkSpeed());
            Serial.println(F(" Mbps"));
            Serial.print(F("[Ethernet] Full Duplex: "));
            Serial.println(ETH.fullDuplex() ? F("Yes") : F("No"));
            ethConnected = true;
            break;

        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println(F("[Ethernet] Got IP"));
            Serial.print(F("IP_ADDRESS:"));
            Serial.print(ETH.localIP());
            Serial.println(F("\n         "));
            Serial.print(F("[Ethernet] MAC: "));
            Serial.println(ETH.macAddress());
            Serial.print(F("[Ethernet] Gateway: "));
            Serial.println(ETH.gatewayIP());
            Serial.print(F("[Ethernet] DNS: "));
            Serial.println(ETH.dnsIP());
            Serial.println();
            Serial.print(F("Use web browser to access 'http://"));
            Serial.print(ETH.localIP());
            Serial.println(F("/' to view the setup page"));
            Serial.println();
            ethGotIP = true;
            break;

        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println(F("[Ethernet] Link Down"));
            ethConnected = false;
            ethGotIP = false;
            break;

        case ARDUINO_EVENT_ETH_STOP:
            Serial.println(F("[Ethernet] Stopped"));
            ethConnected = false;
            ethGotIP = false;
            break;

        default:
            break;
    }
}

void setupEthernet() {
    Serial.println(F("-------------------------------------"));
    Serial.println(F("[Ethernet] Initializing..."));
    Serial.print(F("[Ethernet] PHY Type: LAN8720, Addr: "));
    Serial.println(ETH_PHY_ADDR);
    Serial.print(F("[Ethernet] MDC: GPIO"));
    Serial.print(ETH_PHY_MDC);
    Serial.print(F(", MDIO: GPIO"));
    Serial.println(ETH_PHY_MDIO);
    Serial.print(F("[Ethernet] Power Pin: GPIO"));
    Serial.println(ETH_PHY_POWER);

    // Register event handler before starting ETH
    WiFi.onEvent(onEthEvent);

    // Initialize ethernet
    // ETH.begin() parameters: phy_addr, power, mdc, mdio, type, clk_mode
    if (!ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, (eth_phy_type_t)ETH_PHY_TYPE, (eth_clock_mode_t)ETH_CLK_MODE)) {
        Serial.println(F("[Ethernet] Failed to initialize!"));
        return;
    }

    Serial.println(F("[Ethernet] Waiting for link and DHCP..."));

    // Wait for connection with timeout
    unsigned long startTime = millis();
    const unsigned long timeout = 30000; // 30 second timeout

    while (!ethGotIP && (millis() - startTime < timeout)) {
        delay(100);
        if ((millis() - startTime) % 2000 < 100) {
            Serial.print(F("."));
        }
    }
    Serial.println();

    if (!ethGotIP) {
        Serial.println(F("[Ethernet] Failed to get IP address via DHCP"));
        Serial.println(F("[Ethernet] Check cable connection and DHCP server"));
    }
}

bool isEthernetConnected() {
    return ethConnected && ethGotIP;
}

IPAddress getEthernetIP() {
    return ETH.localIP();
}

String getEthernetMAC() {
    return ETH.macAddress();
}
