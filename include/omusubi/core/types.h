#pragma once

#include <cstdint>

namespace omusubi {

/**
 * @brief 電源状態
 */
enum class PowerState : uint8_t {
    battery,    ///< バッテリー駆動
    usb,        ///< USB給電
    external,   ///< 外部電源
    unknown     ///< 不明
};

/**
 * @brief 3次元ベクトル
 */
struct Vector3 {
    float x;  ///< X成分
    float y;  ///< Y成分
    float z;  ///< Z成分
    
    constexpr Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vector3(float x_val, float y_val, float z_val) noexcept 
        : x(x_val), y(y_val), z(z_val) {}
};

}  // namespace omusubi