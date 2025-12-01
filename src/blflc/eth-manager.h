#ifndef _BLFLC_ETH_MANAGER
#define _BLFLC_ETH_MANAGER

#include <Arduino.h>
#include <ETH.h>

// Ethernet configuration - can be overridden via build flags
#ifndef ETH_PHY_TYPE
#define ETH_PHY_TYPE ETH_PHY_LAN8720
#endif

#ifndef ETH_PHY_ADDR
#define ETH_PHY_ADDR 1
#endif

#ifndef ETH_PHY_MDC
#define ETH_PHY_MDC 23
#endif

#ifndef ETH_PHY_MDIO
#define ETH_PHY_MDIO 33
#endif

#ifndef ETH_PHY_POWER
#define ETH_PHY_POWER 5
#endif

#ifndef ETH_CLK_MODE
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN
#endif

// Ethernet state
extern bool ethConnected;
extern bool ethGotIP;

// Initialize ethernet hardware
void setupEthernet();

// Check if ethernet is connected and has IP
bool isEthernetConnected();

// Get local IP address (mirrors WiFi.localIP() interface)
IPAddress getEthernetIP();

// Get MAC address as string
String getEthernetMAC();

// Event handler for ethernet events
void onEthEvent(WiFiEvent_t event);

#endif
