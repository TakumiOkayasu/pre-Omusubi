#pragma once

#include "omusubi/interface/readable.h"
#include "omusubi/interface/writable.h"
#include "omusubi/interface/connectable.h"

namespace omusubi {

class SerialCommunication 
    : public Readable
    , public Writable
    , public Connectable {
public:
    SerialCommunication() = default;
    ~SerialCommunication() override = default;
    SerialCommunication(const SerialCommunication&) = delete;
    SerialCommunication& operator=(const SerialCommunication&) = delete;
    SerialCommunication(SerialCommunication&&) = delete;
    SerialCommunication& operator=(SerialCommunication&&) = delete;
    
    virtual void set_baud_rate(uint32_t baud_rate) = 0;
    virtual uint32_t get_baud_rate() const = 0;
    virtual void flush() = 0;
    virtual void clear() = 0;
};

}  // namespace omusubi