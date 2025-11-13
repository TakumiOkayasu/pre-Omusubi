#pragma once

#include "omusubi/interface/readable.h"
#include "omusubi/interface/writable.h"
#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"

namespace omusubi {

/**
 * @brief Bluetooth Classic通信デバイス
 */
class BluetoothContext
    : public Readable
    , public Writable
    , public Connectable
    , public Scannable {
public:
    BluetoothContext() = default;
    ~BluetoothContext() override = default;
    BluetoothContext(const BluetoothContext&) = delete;
    BluetoothContext& operator=(const BluetoothContext&) = delete;
    BluetoothContext(BluetoothContext&&) = delete;
    BluetoothContext& operator=(BluetoothContext&&) = delete;
};

}  // namespace omusubi