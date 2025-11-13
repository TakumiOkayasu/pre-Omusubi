#pragma once

#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"

namespace omusubi {

/**
 * @brief WiFi通信デバイス
 */
class WiFiContext
    : public Connectable
    , public Scannable {
public:
    WiFiContext() = default;
    ~WiFiContext() override = default;
    WiFiContext(const WiFiContext&) = delete;
    WiFiContext& operator=(const WiFiContext&) = delete;
    WiFiContext(WiFiContext&&) = delete;
    WiFiContext& operator=(WiFiContext&&) = delete;
};

}  // namespace omusubi