#pragma once

namespace omusubi {

/**
 * @brief 接続管理インターフェース
 */
class Connectable {
public:
    Connectable() = default;
    virtual ~Connectable() = default;
    Connectable(const Connectable&) = delete;
    Connectable& operator=(const Connectable&) = delete;
    Connectable(Connectable&&) = delete;
    Connectable& operator=(Connectable&&) = delete;

    /** @brief デバイスに接続 */
    virtual bool connect() = 0;
};

}  // namespace omusubi