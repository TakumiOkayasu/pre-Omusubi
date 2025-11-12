#pragma once

namespace omusubi {

class Connectable {
public:
    Connectable() = default;
    virtual ~Connectable() = default;
    Connectable(const Connectable&) = delete;
    Connectable& operator=(const Connectable&) = delete;
    Connectable(Connectable&&) = delete;
    Connectable& operator=(Connectable&&) = delete;
    
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool is_connected() const = 0;
};

}  // namespace omusubi