#pragma once

#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"
#include "omusubi/core/string_view.h"
#include "omusubi/core/fixed_string.hpp"

namespace omusubi {

class WiFiCommunication
    : public Connectable
    , public Scannable {
public:
    WiFiCommunication() = default;
    ~WiFiCommunication() override = default;
    WiFiCommunication(const WiFiCommunication&) = delete;
    WiFiCommunication& operator=(const WiFiCommunication&) = delete;
    WiFiCommunication(WiFiCommunication&&) = delete;
    WiFiCommunication& operator=(WiFiCommunication&&) = delete;
    
    virtual bool connect_to(StringView ssid, StringView password, uint32_t timeout_ms = 0) = 0;
    virtual bool connect_to_open(StringView ssid, uint32_t timeout_ms = 0) = 0;
    virtual bool connect_to_found(uint8_t index, StringView password) = 0;
    virtual FixedString<16> get_ip_address() const = 0;
    virtual FixedString<32> get_mac_address() const = 0;
    virtual FixedString<16> get_subnet_mask() const = 0;
    virtual FixedString<16> get_gateway() const = 0;
    virtual FixedString<16> get_dns() const = 0;
    virtual FixedString<64> get_connected_ssid() const = 0;
    virtual bool start_ap(StringView ssid, StringView password) = 0;
    virtual void stop_ap() = 0;
    virtual bool is_ap_mode() const = 0;
};

}  // namespace omusubi