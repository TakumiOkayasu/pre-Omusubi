#pragma once

#include "omusubi/interface/readable.h"
#include "omusubi/interface/writable.h"
#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"
#include "omusubi/core/string_view.h"
#include "omusubi/core/fixed_string.hpp"

namespace omusubi {

class BluetoothCommunication
    : public Readable
    , public Writable
    , public Connectable
    , public Scannable {
public:
    BluetoothCommunication() = default;
    ~BluetoothCommunication() override = default;
    BluetoothCommunication(const BluetoothCommunication&) = delete;
    BluetoothCommunication& operator=(const BluetoothCommunication&) = delete;
    BluetoothCommunication(BluetoothCommunication&&) = delete;
    BluetoothCommunication& operator=(BluetoothCommunication&&) = delete;
    
    virtual bool connect_to(StringView device_name, uint32_t timeout_ms = 0) = 0;
    virtual bool connect_to_address(StringView address, uint32_t timeout_ms = 0) = 0;
    virtual bool connect_to_found(uint8_t index) = 0;
    virtual void set_local_name(StringView name) = 0;
    virtual FixedString<64> get_local_name() const = 0;
    virtual FixedString<64> get_connected_device_name() const = 0;
    virtual FixedString<32> get_connected_device_address() const = 0;
    virtual FixedString<32> get_found_address(uint8_t index) const = 0;
    virtual int32_t get_found_signal_strength(uint8_t index) const = 0;
};

}  // namespace omusubi