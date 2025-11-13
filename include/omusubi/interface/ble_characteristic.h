#pragma once

#include "readable.h"
#include "writable.h"

namespace omusubi {

/**
 * @brief BLE Characteristic インターフェース
 */
class BLECharacteristic : public Readable, public Writable {
public:
    BLECharacteristic() = default;
    ~BLECharacteristic() override = default;
    BLECharacteristic(const BLECharacteristic&) = delete;
    BLECharacteristic& operator=(const BLECharacteristic&) = delete;
    BLECharacteristic(BLECharacteristic&&) = delete;
    BLECharacteristic& operator=(BLECharacteristic&&) = delete;
};

}  // namespace omusubi