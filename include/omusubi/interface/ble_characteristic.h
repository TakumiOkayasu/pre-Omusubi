#pragma once

#include <omusubi/interface/readable.h>
#include <omusubi/interface/writable.h>

namespace omusubi {

/**
 * @brief BLE Characteristic インターフェース
 */
class BLECharacteristic : public ByteReadable, public ByteWritable {
public:
    BLECharacteristic() = default;
    ~BLECharacteristic() override = default;
    BLECharacteristic(const BLECharacteristic&) = delete;
    BLECharacteristic& operator=(const BLECharacteristic&) = delete;
    BLECharacteristic(BLECharacteristic&&) = delete;
    BLECharacteristic& operator=(BLECharacteristic&&) = delete;
};

} // namespace omusubi
