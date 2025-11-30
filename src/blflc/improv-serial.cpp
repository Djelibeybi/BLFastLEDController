#include "improv-serial.h"

#include <WiFi.h>
#include <DNSServer.h>
#include "improv.h"
#include "types.h"
#include "filesystem.h"

// External DNS server from wifi-manager
extern DNSServer dnsServer;

// Improv Serial buffer (needs to be large enough for SSID + password + overhead)
static uint8_t improv_buffer[128];
static uint8_t improv_position = 0;
static bool improv_active = false;

// Connection settings
#define MAX_ATTEMPTS_WIFI_CONNECTION 40  // 40 * 500ms = 20 seconds timeout

// Forward declarations
static void improv_set_state(improv::State state);
static void improv_set_error(improv::Error error);
static void improv_send_response(std::vector<uint8_t> &response);
static bool improv_connect_wifi(std::string ssid, std::string password);
static void improv_on_error_callback(improv::Error err);
static bool improv_on_command_callback(improv::ImprovCommand cmd);
static void improv_get_available_wifi_networks();
static std::vector<std::string> improv_get_local_url();

// Get the device URL after WiFi connection
static std::vector<std::string> improv_get_local_url() {
    return {
        String("http://" + WiFi.localIP().toString()).c_str()
    };
}

// Error callback
static void improv_on_error_callback(improv::Error err) {
    // Don't print to serial - it's used for Improv protocol
}

// Connect to WiFi with credentials from Improv
static bool improv_connect_wifi(std::string ssid, std::string password) {
    uint8_t count = 0;

    // Stop DNS server if running (from AP mode)
    dnsServer.stop();

    // Fully disconnect and clear any existing WiFi state
    WiFi.disconnect(true, true);
    delay(100);

    // Switch to station mode
    WiFi.mode(WIFI_STA);
    delay(100);

    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);

        if (count > MAX_ATTEMPTS_WIFI_CONNECTION) {
            WiFi.disconnect();
            return false;
        }
        count++;
    }

    return true;
}

// Command callback - handles all Improv RPC commands
static bool improv_on_command_callback(improv::ImprovCommand cmd) {
    // Use explicit values since enum has duplicates (IDENTIFY and GET_CURRENT_STATE both = 0x02)
    uint8_t cmd_value = static_cast<uint8_t>(cmd.command);

    if (cmd_value == 0x02) {  // GET_CURRENT_STATE / IDENTIFY
        if (WiFi.status() == WL_CONNECTED) {
            improv_set_state(improv::State::STATE_PROVISIONED);
            std::vector<uint8_t> data = improv::build_rpc_response(
                improv::GET_CURRENT_STATE, improv_get_local_url(), false);
            improv_send_response(data);
        } else {
            improv_set_state(improv::State::STATE_AUTHORIZED);
        }
        return true;
    }

    if (cmd_value == 0x03) {  // GET_DEVICE_INFO
        std::vector<std::string> infos = {
            "BLFLC",
            std::string(globalVariables.FWVersion.c_str()),
            "ESP32",
            std::string(globalVariables.Host.c_str())
        };
        std::vector<uint8_t> data = improv::build_rpc_response(
            improv::GET_DEVICE_INFO, infos, false);
        improv_send_response(data);
        return true;
    }

    if (cmd_value == 0x04) {  // GET_WIFI_NETWORKS
        improv_get_available_wifi_networks();
        return true;
    }

    if (cmd_value == 0x01) {  // WIFI_SETTINGS
        if (cmd.ssid.length() == 0) {
            improv_set_error(improv::Error::ERROR_INVALID_RPC);
            return false;
        }

        improv_set_state(improv::STATE_PROVISIONING);

        if (improv_connect_wifi(cmd.ssid, cmd.password)) {
            // Save credentials to filesystem
            strncpy(globalVariables.SSID, cmd.ssid.c_str(), sizeof(globalVariables.SSID) - 1);
            globalVariables.SSID[sizeof(globalVariables.SSID) - 1] = '\0';
            strncpy(globalVariables.APPW, cmd.password.c_str(), sizeof(globalVariables.APPW) - 1);
            globalVariables.APPW[sizeof(globalVariables.APPW) - 1] = '\0';
            saveFileSystem();

            improv_set_state(improv::STATE_PROVISIONED);
            std::vector<uint8_t> data = improv::build_rpc_response(
                improv::WIFI_SETTINGS, improv_get_local_url(), false);
            improv_send_response(data);

            // Restart device to properly initialize with new credentials
            delay(1000);
            ESP.restart();
        } else {
            improv_set_state(improv::STATE_STOPPED);
            improv_set_error(improv::Error::ERROR_UNABLE_TO_CONNECT);
        }
        return true;
    }

    // Unknown command
    improv_set_error(improv::ERROR_UNKNOWN_RPC);
    return false;
}

// Scan and send available WiFi networks
static void improv_get_available_wifi_networks() {
    int networkNum = WiFi.scanNetworks();

    for (int id = 0; id < networkNum; ++id) {
        std::vector<uint8_t> data = improv::build_rpc_response(
            improv::GET_WIFI_NETWORKS,
            {
                std::string(WiFi.SSID(id).c_str()),
                std::string(String(WiFi.RSSI(id)).c_str()),
                (WiFi.encryptionType(id) == WIFI_AUTH_OPEN ? "NO" : "YES")
            },
            false);
        improv_send_response(data);
        delay(1);
    }

    // Send final empty response to indicate end of list
    std::vector<uint8_t> data = improv::build_rpc_response(
        improv::GET_WIFI_NETWORKS, std::vector<std::string>{}, false);
    improv_send_response(data);
}

// Send current state packet
static void improv_set_state(improv::State state) {
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(11);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_CURRENT_STATE;
    data[8] = 1;
    data[9] = state;

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data[10] = checksum;

    Serial.write(data.data(), data.size());
}

// Send RPC response packet
static void improv_send_response(std::vector<uint8_t> &response) {
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(9);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_RPC_RESPONSE;
    data[8] = response.size();
    data.insert(data.end(), response.begin(), response.end());

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data.push_back(checksum);

    Serial.write(data.data(), data.size());
}

// Send error packet
static void improv_set_error(improv::Error error) {
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(11);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_ERROR_STATE;
    data[8] = 1;
    data[9] = error;

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data[10] = checksum;

    Serial.write(data.data(), data.size());
}

// Process Improv Serial - call this in loop()
// Uses the SDK's parse_improv_serial_byte() for packet parsing
void loopImprovSerial() {
    if (!improv_active) {
        return;
    }

    while (Serial.available() > 0) {
        uint8_t b = Serial.read();

        // Use SDK's parse_improv_serial_byte for packet parsing
        // It handles: header validation, checksum, and calls parse_improv_data internally
        if (!improv::parse_improv_serial_byte(
                improv_position,
                b,
                improv_buffer,
                [](improv::ImprovCommand cmd) -> bool {
                    return improv_on_command_callback(cmd);
                },
                [](improv::Error err) {
                    improv_on_error_callback(err);
                })) {
            // Invalid byte or packet complete - reset position
            improv_position = 0;
        } else {
            // Valid byte - store and advance
            improv_buffer[improv_position++] = b;

            // Prevent buffer overflow
            if (improv_position >= sizeof(improv_buffer)) {
                improv_position = 0;
            }
        }
    }
}

// Initialize Improv Serial - call this in setup() when no credentials exist
void setupImprovSerial() {
    improv_active = true;
    // Always report authorized state - we're ready for credentials
    improv_set_state(improv::STATE_AUTHORIZED);
}
