#pragma once

#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"

namespace omusubi {

/**
 * @brief BLE (Bluetooth Low Energy) 通信デバイス
 */
class BLEContext
    : public Connectable
    , public Scannable {
public:
    BLEContext() = default;
    ~BLEContext() override = default;
    BLEContext(const BLEContext&) = delete;
    BLEContext& operator=(const BLEContext&) = delete;
    BLEContext(BLEContext&&) = delete;
    BLEContext& operator=(BLEContext&&) = delete;
};

}  // namespace omusubi
