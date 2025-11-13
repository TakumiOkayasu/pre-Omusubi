#pragma once

#include "omusubi/interface/readable.h"
#include "omusubi/interface/writable.h"
#include "omusubi/interface/connectable.h"

namespace omusubi {

/**
 * @brief シリアル通信デバイス
 */
class SerialContext
    : public Readable
    , public Writable
    , public Connectable {
public:
    SerialContext() = default;
    ~SerialContext() override = default;
    SerialContext(const SerialContext&) = delete;
    SerialContext& operator=(const SerialContext&) = delete;
    SerialContext(SerialContext&&) = delete;
    SerialContext& operator=(SerialContext&&) = delete;
};

}  // namespace omusubi