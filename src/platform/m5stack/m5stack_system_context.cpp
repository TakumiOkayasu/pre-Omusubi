#include "omusubi/platform/m5stack/m5stack_system_context.h"
#include <M5Stack.h>
#include <esp_system.h>
#include <memory>

namespace omusubi {
namespace platform {
namespace m5stack {

// ========================================
// 内部実装クラス
// ========================================

class M5StackSerialCommunication final : public SerialCommunication {
private:
    HardwareSerial* serial_;
    uint8_t port_;
    uint32_t baud_rate_;
    bool connected_;
    
public:
    explicit M5StackSerialCommunication(uint8_t port)
        : serial_(nullptr), port_(port), baud_rate_(115200), connected_(false) {
        switch (port) {
            case 0: serial_ = &Serial; break;
            case 1: serial_ = &Serial1; break;
            case 2: serial_ = &Serial2; break;
            default: break;
        }
    }
    
    // Readable実装
    FixedString<256> read_string() override {
        FixedString<256> result;
        if (!serial_ || !connected_) return result;
        
        while (serial_->available()) {
            char c = static_cast<char>(serial_->read());
            if (!result.append(c)) break;
        }
        return result;
    }
    
    FixedString<256> read_line() override {
        FixedString<256> result;
        if (!serial_ || !connected_) return result;
        
        while (serial_->available()) {
            char c = static_cast<char>(serial_->read());
            if (c == '\n') break;
            if (c != '\r' && !result.append(c)) break;
        }
        return result;
    }
    
    FixedString<256> read_string_wait() override {
        if (!serial_ || !connected_) return FixedString<256>();
        while (!serial_->available()) ::delay(1);
        return read_string();
    }
    
    FixedString<256> read_line_wait() override {
        FixedString<256> result;
        if (!serial_ || !connected_) return result;
        
        while (result.byte_length() < 255) {
            while (!serial_->available()) ::delay(1);
            char c = static_cast<char>(serial_->read());
            if (c == '\n') break;
            if (c != '\r' && !result.append(c)) break;
        }
        return result;
    }
    
    FixedBuffer<256> read_bytes() override {
        FixedBuffer<256> result;
        if (!serial_ || !connected_) return result;
        
        while (serial_->available()) {
            uint8_t byte = static_cast<uint8_t>(serial_->read());
            if (!result.append(byte)) break;
        }
        return result;
    }
    
    bool has_data() const override {
        return serial_ && connected_ && serial_->available() > 0;
    }
    
    // Writable実装
    void write(StringView text) override {
        if (!serial_ || !connected_) return;
        for (uint32_t i = 0; i < text.byte_length(); ++i) {
            serial_->write(static_cast<uint8_t>(text[i]));
        }
    }
    
    void write_bytes(const uint8_t* data, uint32_t length) override {
        if (!serial_ || !connected_ || !data || length == 0) return;
        serial_->write(data, length);
    }
    
    void write(int32_t value) override {
        if (!serial_ || !connected_) return;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d", value);
        write(StringView::from_c_string(buffer));
    }
    
    void write(uint32_t value) override {
        if (!serial_ || !connected_) return;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%u", value);
        write(StringView::from_c_string(buffer));
    }
    
    void write(float value) override {
        if (!serial_ || !connected_) return;
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f", static_cast<double>(value));
        write(StringView::from_c_string(buffer));
    }
    
    // Connectable実装
    bool connect() override {
        if (!serial_) return false;
        if (connected_) return true;
        serial_->begin(baud_rate_);
        connected_ = true;
        return true;
    }
    
    void disconnect() override {
        if (!serial_ || !connected_) return;
        serial_->end();
        connected_ = false;
    }
    
    bool is_connected() const override {
        return connected_;
    }
    
    // SerialCommunication実装
    void set_baud_rate(uint32_t baud_rate) override {
        baud_rate_ = baud_rate;
        if (connected_) {
            disconnect();
            connect();
        }
    }
    
    uint32_t get_baud_rate() const override {
        return baud_rate_;
    }
    
    void flush() override {
        if (serial_ && connected_) serial_->flush();
    }
    
    void clear() override {
        if (serial_ && connected_) {
            while (serial_->available()) serial_->read();
        }
    }
};

class M5StackBluetoothCommunication final : public BluetoothCommunication {
private:
    BluetoothSerial bt_;
    bool connected_;
    char local_name_[64];
    char last_device_name_[64];
    
    struct FoundDevice {
        char name[64];
        char address[32];
        int32_t rssi;
    };
    
    FoundDevice found_devices_[10];
    uint8_t found_count_;
    bool scanning_;
    
public:
    M5StackBluetoothCommunication()
        : connected_(false)
        , found_count_(0)
        , scanning_(false) {
        strncpy(local_name_, "M5Stack", sizeof(local_name_));
        local_name_[sizeof(local_name_) - 1] = '\0';
        last_device_name_[0] = '\0';
    }
    
    ~M5StackBluetoothCommunication() {
        if (connected_) {
            disconnect();
        }
    }
    
    // ========================================
    // Readable実装
    // ========================================
    
    FixedString<256> read_string() override {
        FixedString<256> result;
        if (!connected_) return result;
        
        while (bt_.available()) {
            char c = static_cast<char>(bt_.read());
            if (!result.append(c)) break;
        }
        return result;
    }
    
    FixedString<256> read_line() override {
        FixedString<256> result;
        if (!connected_) return result;
        
        while (bt_.available()) {
            char c = static_cast<char>(bt_.read());
            if (c == '\n') break;
            if (c != '\r') {
                if (!result.append(c)) break;
            }
        }
        return result;
    }
    
    FixedString<256> read_string_wait() override {
        if (!connected_) return FixedString<256>();
        
        while (!bt_.available()) {
            ::delay(1);
        }
        return read_string();
    }
    
    FixedString<256> read_line_wait() override {
        FixedString<256> result;
        if (!connected_) return result;
        
        while (result.byte_length() < 255) {
            while (!bt_.available()) {
                ::delay(1);
            }
            
            char c = static_cast<char>(bt_.read());
            if (c == '\n') break;
            if (c != '\r') {
                if (!result.append(c)) break;
            }
        }
        return result;
    }
    
    FixedBuffer<256> read_bytes() override {
        FixedBuffer<256> result;
        if (!connected_) return result;
        
        while (bt_.available()) {
            uint8_t byte = static_cast<uint8_t>(bt_.read());
            if (!result.append(byte)) break;
        }
        return result;
    }
    
    bool has_data() const override {
        return connected_ && bt_.available() > 0;
    }
    
    // ========================================
    // Writable実装
    // ========================================
    
    void write(StringView text) override {
        if (!connected_) return;
        
        for (uint32_t i = 0; i < text.byte_length(); ++i) {
            bt_.write(static_cast<uint8_t>(text[i]));
        }
    }
    
    void write_bytes(const uint8_t* data, uint32_t length) override {
        if (!connected_ || !data || length == 0) return;
        bt_.write(data, length);
    }
    
    void write(int32_t value) override {
        if (!connected_) return;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d", value);
        write(StringView::from_c_string(buffer));
    }
    
    void write(uint32_t value) override {
        if (!connected_) return;
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%u", value);
        write(StringView::from_c_string(buffer));
    }
    
    void write(float value) override {
        if (!connected_) return;
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f", static_cast<double>(value));
        write(StringView::from_c_string(buffer));
    }
    
    // ========================================
    // Connectable実装
    // ========================================
    
    bool connect() override {
        if (connected_) return true;
        
        // 最後に接続したデバイスに再接続
        if (last_device_name_[0] == '\0') {
            return false;
        }
        
        return connect_to(StringView::from_c_string(last_device_name_), 0);
    }
    
    void disconnect() override {
        if (!connected_) return;
        
        bt_.disconnect();
        connected_ = false;
    }
    
    bool is_connected() const override {
        return connected_ && bt_.connected();
    }
    
    // ========================================
    // Scannable実装
    // ========================================
    
    void start_scan() override {
        if (scanning_) return;
        
        found_count_ = 0;
        scanning_ = true;
        
        // BTデバイスをスキャン
        BTScanResults* results = bt_.discover(BT_DISCOVER_TIME);
        
        if (results) {
            uint8_t count = results->getCount();
            found_count_ = (count < 10) ? count : 10;
            
            for (uint8_t i = 0; i < found_count_; ++i) {
                BTAdvertisedDevice* device = results->getDevice(i);
                
                // デバイス名
                const char* name = device->getName().c_str();
                strncpy(found_devices_[i].name, name, sizeof(found_devices_[i].name) - 1);
                found_devices_[i].name[sizeof(found_devices_[i].name) - 1] = '\0';
                
                // MACアドレス
                const char* addr = device->getAddress().toString().c_str();
                strncpy(found_devices_[i].address, addr, sizeof(found_devices_[i].address) - 1);
                found_devices_[i].address[sizeof(found_devices_[i].address) - 1] = '\0';
                
                // RSSI
                found_devices_[i].rssi = device->getRSSI();
            }
        }
        
        scanning_ = false;
    }
    
    void stop_scan() override {
        scanning_ = false;
    }
    
    bool is_scanning() const override {
        return scanning_;
    }
    
    uint8_t get_found_count() const override {
        return found_count_;
    }
    
    FixedString<64> get_found_name(uint8_t index) const override {
        if (index >= found_count_) {
            return FixedString<64>();
        }
        return FixedString<64>(found_devices_[index].name);
    }
    
    // ========================================
    // BluetoothCommunication実装
    // ========================================
    
    bool connect_to(StringView device_name, uint32_t timeout_ms) override {
        if (connected_) {
            disconnect();
        }
        
        // デバイス名を保存
        uint32_t len = (device_name.byte_length() < sizeof(last_device_name_) - 1) ?
                       device_name.byte_length() : sizeof(last_device_name_) - 1;
        
        for (uint32_t i = 0; i < len; ++i) {
            last_device_name_[i] = device_name[i];
        }
        last_device_name_[len] = '\0';
        
        // 接続試行
        if (timeout_ms > 0) {
            uint32_t start = millis();
            while (millis() - start < timeout_ms) {
                if (bt_.connect(last_device_name_)) {
                    connected_ = true;
                    return true;
                }
                ::delay(100);
            }
            return false;
        } else {
            if (bt_.connect(last_device_name_)) {
                connected_ = true;
                return true;
            }
            return false;
        }
    }
    
    bool connect_to_address(StringView address, uint32_t timeout_ms) override {
        if (connected_) {
            disconnect();
        }
        
        // MACアドレスをバイト配列に変換
        uint8_t addr[6];
        char addr_str[32];
        
        uint32_t len = (address.byte_length() < sizeof(addr_str) - 1) ?
                       address.byte_length() : sizeof(addr_str) - 1;
        
        for (uint32_t i = 0; i < len; ++i) {
            addr_str[i] = address[i];
        }
        addr_str[len] = '\0';
        
        // "AA:BB:CC:DD:EE:FF" 形式をパース
        if (sscanf(addr_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                   &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]) != 6) {
            return false;
        }
        
        // 接続試行
        if (timeout_ms > 0) {
            uint32_t start = millis();
            while (millis() - start < timeout_ms) {
                if (bt_.connect(addr)) {
                    connected_ = true;
                    return true;
                }
                ::delay(100);
            }
            return false;
        } else {
            if (bt_.connect(addr)) {
                connected_ = true;
                return true;
            }
            return false;
        }
    }
    
    bool connect_to_found(uint8_t index) override {
        if (index >= found_count_) {
            return false;
        }
        
        return connect_to(
            StringView::from_c_string(found_devices_[index].name),
            0
        );
    }
    
    void set_local_name(StringView name) override {
        uint32_t len = (name.byte_length() < sizeof(local_name_) - 1) ?
                       name.byte_length() : sizeof(local_name_) - 1;
        
        for (uint32_t i = 0; i < len; ++i) {
            local_name_[i] = name[i];
        }
        local_name_[len] = '\0';
        
        bt_.begin(local_name_);
    }
    
    FixedString<64> get_local_name() const override {
        return FixedString<64>(local_name_);
    }
    
    FixedString<64> get_connected_device_name() const override {
        if (!connected_) {
            return FixedString<64>();
        }
        return FixedString<64>(last_device_name_);
    }
    
    FixedString<32> get_connected_device_address() const override {
        if (!connected_) {
            return FixedString<32>();
        }
        
        // ESP32のBluetooth実装では直接MACアドレスを取得できない場合がある
        // 簡易実装として空文字列を返す
        return FixedString<32>();
    }
    
    FixedString<32> get_found_address(uint8_t index) const override {
        if (index >= found_count_) {
            return FixedString<32>();
        }
        return FixedString<32>(found_devices_[index].address);
    }
    
    int32_t get_found_signal_strength(uint8_t index) const override {
        if (index >= found_count_) {
            return -100;
        }
        return found_devices_[index].rssi;
    }
};

class M5StackWiFiCommunication final : public WiFiCommunication {
private:
    bool connected_;
    bool ap_mode_;
    char last_ssid_[64];
    char last_password_[64];
    
    struct FoundNetwork {
        char ssid[64];
        int32_t rssi;
        bool is_open;
        uint8_t encryption_type;
    };
    
    FoundNetwork found_networks_[20];
    uint8_t found_count_;
    bool scanning_;
    
public:
    M5StackWiFiCommunication()
        : connected_(false)
        , ap_mode_(false)
        , found_count_(0)
        , scanning_(false) {
        last_ssid_[0] = '\0';
        last_password_[0] = '\0';
    }
    
    ~M5StackWiFiCommunication() {
        if (connected_) {
            disconnect();
        }
    }
    
    // ========================================
    // Connectable実装
    // ========================================
    
    bool connect() override {
        if (connected_) return true;
        
        // 最後に接続したネットワークに再接続
        if (last_ssid_[0] == '\0') {
            return false;
        }
        
        return connect_to(
            StringView::from_c_string(last_ssid_),
            StringView::from_c_string(last_password_),
            0
        );
    }
    
    void disconnect() override {
        if (!connected_) return;
        
        WiFi.disconnect(true);
        connected_ = false;
    }
    
    bool is_connected() const override {
        return connected_ && (WiFi.status() == WL_CONNECTED);
    }
    
    // ========================================
    // Scannable実装
    // ========================================
    
    void start_scan() override {
        if (scanning_) return;
        
        found_count_ = 0;
        scanning_ = true;
        
        // WiFiネットワークをスキャン
        int16_t count = WiFi.scanNetworks();
        
        if (count > 0) {
            found_count_ = (count < 20) ? static_cast<uint8_t>(count) : 20;
            
            for (uint8_t i = 0; i < found_count_; ++i) {
                // SSID
                String ssid = WiFi.SSID(i);
                strncpy(found_networks_[i].ssid, ssid.c_str(), 
                       sizeof(found_networks_[i].ssid) - 1);
                found_networks_[i].ssid[sizeof(found_networks_[i].ssid) - 1] = '\0';
                
                // RSSI
                found_networks_[i].rssi = WiFi.RSSI(i);
                
                // 暗号化タイプ
                found_networks_[i].encryption_type = WiFi.encryptionType(i);
                found_networks_[i].is_open = (found_networks_[i].encryption_type == WIFI_AUTH_OPEN);
            }
        }
        
        scanning_ = false;
    }
    
    void stop_scan() override {
        scanning_ = false;
        WiFi.scanDelete();
    }
    
    bool is_scanning() const override {
        return scanning_;
    }
    
    uint8_t get_found_count() const override {
        return found_count_;
    }
    
    FixedString<64> get_found_name(uint8_t index) const override {
        if (index >= found_count_) {
            return FixedString<64>();
        }
        return FixedString<64>(found_networks_[index].ssid);
    }
    
    // ========================================
    // WiFiCommunication実装
    // ========================================
    
    bool connect_to(StringView ssid, StringView password, uint32_t timeout_ms) override {
        if (connected_) {
            disconnect();
        }
        
        // SSID保存
        uint32_t ssid_len = (ssid.byte_length() < sizeof(last_ssid_) - 1) ?
                            ssid.byte_length() : sizeof(last_ssid_) - 1;
        for (uint32_t i = 0; i < ssid_len; ++i) {
            last_ssid_[i] = ssid[i];
        }
        last_ssid_[ssid_len] = '\0';
        
        // パスワード保存
        uint32_t pass_len = (password.byte_length() < sizeof(last_password_) - 1) ?
                            password.byte_length() : sizeof(last_password_) - 1;
        for (uint32_t i = 0; i < pass_len; ++i) {
            last_password_[i] = password[i];
        }
        last_password_[pass_len] = '\0';
        
        // WiFi接続開始
        WiFi.mode(WIFI_STA);
        WiFi.begin(last_ssid_, last_password_);
        
        // タイムアウト処理
        if (timeout_ms > 0) {
            uint32_t start = millis();
            while (WiFi.status() != WL_CONNECTED) {
                if (millis() - start > timeout_ms) {
                    return false;
                }
                ::delay(100);
            }
        } else {
            // タイムアウトなし（デフォルト30秒待機）
            uint32_t start = millis();
            while (WiFi.status() != WL_CONNECTED) {
                if (millis() - start > 30000) {
                    return false;
                }
                ::delay(100);
            }
        }
        
        connected_ = true;
        return true;
    }
    
    bool connect_to_open(StringView ssid, uint32_t timeout_ms) override {
        return connect_to(ssid, StringView("", 0), timeout_ms);
    }
    
    bool connect_to_found(uint8_t index, StringView password) override {
        if (index >= found_count_) {
            return false;
        }
        
        if (found_networks_[index].is_open) {
            return connect_to_open(
                StringView::from_c_string(found_networks_[index].ssid),
                0
            );
        } else {
            return connect_to(
                StringView::from_c_string(found_networks_[index].ssid),
                password,
                0
            );
        }
    }
    
    FixedString<16> get_ip_address() const override {
        if (!connected_) {
            return FixedString<16>("0.0.0.0");
        }
        
        IPAddress ip = WiFi.localIP();
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d",
                 ip[0], ip[1], ip[2], ip[3]);
        
        return FixedString<16>(buffer);
    }
    
    FixedString<32> get_mac_address() const override {
        uint8_t mac[6];
        WiFi.macAddress(mac);
        
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        
        return FixedString<32>(buffer);
    }
    
    FixedString<16> get_subnet_mask() const override {
        if (!connected_) {
            return FixedString<16>("0.0.0.0");
        }
        
        IPAddress subnet = WiFi.subnetMask();
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d",
                 subnet[0], subnet[1], subnet[2], subnet[3]);
        
        return FixedString<16>(buffer);
    }
    
    FixedString<16> get_gateway() const override {
        if (!connected_) {
            return FixedString<16>("0.0.0.0");
        }
        
        IPAddress gateway = WiFi.gatewayIP();
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d",
                 gateway[0], gateway[1], gateway[2], gateway[3]);
        
        return FixedString<16>(buffer);
    }
    
    FixedString<16> get_dns() const override {
        if (!connected_) {
            return FixedString<16>("0.0.0.0");
        }
        
        IPAddress dns = WiFi.dnsIP();
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d",
                 dns[0], dns[1], dns[2], dns[3]);
        
        return FixedString<16>(buffer);
    }
    
    FixedString<64> get_connected_ssid() const override {
        if (!connected_) {
            return FixedString<64>();
        }
        
        String ssid = WiFi.SSID();
        return FixedString<64>(ssid.c_str());
    }
    
    bool start_ap(StringView ssid, StringView password) override {
        if (connected_) {
            disconnect();
        }
        
        char ssid_buf[64];
        char pass_buf[64];
        
        // SSID
        uint32_t ssid_len = (ssid.byte_length() < sizeof(ssid_buf) - 1) ?
                            ssid.byte_length() : sizeof(ssid_buf) - 1;
        for (uint32_t i = 0; i < ssid_len; ++i) {
            ssid_buf[i] = ssid[i];
        }
        ssid_buf[ssid_len] = '\0';
        
        // パスワード
        uint32_t pass_len = (password.byte_length() < sizeof(pass_buf) - 1) ?
                            password.byte_length() : sizeof(pass_buf) - 1;
        for (uint32_t i = 0; i < pass_len; ++i) {
            pass_buf[i] = password[i];
        }
        pass_buf[pass_len] = '\0';
        
        // APモード開始
        WiFi.mode(WIFI_AP);
        
        if (pass_len > 0) {
            ap_mode_ = WiFi.softAP(ssid_buf, pass_buf);
        } else {
            ap_mode_ = WiFi.softAP(ssid_buf);
        }
        
        return ap_mode_;
    }
    
    void stop_ap() override {
        if (!ap_mode_) return;
        
        WiFi.softAPdisconnect(true);
        ap_mode_ = false;
    }
    
    bool is_ap_mode() const override {
        return ap_mode_;
    }
};

class M5StackButton final : public Pressable {
private:
    uint8_t pin_;
    bool current_state_;
    bool previous_state_;
    
public:
    explicit M5StackButton(uint8_t pin)
        : pin_(pin), current_state_(false), previous_state_(false) {
        pinMode(pin_, INPUT_PULLUP);
    }
    
    bool is_pressed() const override {
        return current_state_;
    }
    
    bool was_pressed() override {
        bool result = current_state_ && !previous_state_;
        previous_state_ = current_state_;
        return result;
    }
    
    bool was_released() override {
        bool result = !current_state_ && previous_state_;
        previous_state_ = current_state_;
        return result;
    }
    
    void update() {
        current_state_ = (digitalRead(pin_) == LOW);
    }
};

class M5StackAccelerometer final : public Measurable3D {
private:
    bool initialized_;
    
public:
    M5StackAccelerometer() : initialized_(false) {}
    
    void begin() {
        if (!initialized_) {
            M5.IMU.Init();
            initialized_ = true;
        }
    }
    
    Vector3 get_values() const override {
        if (!initialized_) return Vector3();
        float x, y, z;
        M5.IMU.getAccelData(&x, &y, &z);
        return Vector3(x, y, z);
    }
    
    float get_x() const override { return get_values().x; }
    float get_y() const override { return get_values().y; }
    float get_z() const override { return get_values().z; }
};

class M5StackGyroscope final : public Measurable3D {
private:
    bool initialized_;
    
public:
    M5StackGyroscope() : initialized_(false) {}
    
    void begin() {
        if (!initialized_) {
            M5.IMU.Init();
            initialized_ = true;
        }
    }
    
    Vector3 get_values() const override {
        if (!initialized_) return Vector3();
        float x, y, z;
        M5.IMU.getGyroData(&x, &y, &z);
        return Vector3(x, y, z);
    }
    
    float get_x() const override { return get_values().x; }
    float get_y() const override { return get_values().y; }
    float get_z() const override { return get_values().z; }
};

class M5StackDisplay final : public Displayable {
private:
    int32_t cursor_x_;
    int32_t cursor_y_;
    uint8_t text_size_;
    uint32_t text_color_;
    uint32_t bg_color_;
    
public:
    M5StackDisplay()
        : cursor_x_(0), cursor_y_(0), text_size_(1)
        , text_color_(0xFFFF), bg_color_(0x0000) {}
    
    // Writable実装
    void write(StringView text) override {
        for (uint32_t i = 0; i < text.byte_length(); ++i) {
            M5.Lcd.print(text[i]);
        }
    }
    
    void write_bytes(const uint8_t* data, uint32_t length) override {
        if (!data || length == 0) return;
        for (uint32_t i = 0; i < length; ++i) {
            M5.Lcd.print(static_cast<char>(data[i]));
        }
    }
    
    void write(int32_t value) override {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d", value);
        write(StringView::from_c_string(buffer));
    }
    
    void write(uint32_t value) override {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%u", value);
        write(StringView::from_c_string(buffer));
    }
    
    void write(float value) override {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f", static_cast<double>(value));
        write(StringView::from_c_string(buffer));
    }
    
    // Displayable実装
    void clear() override {
        M5.Lcd.fillScreen(bg_color_);
        cursor_x_ = 0;
        cursor_y_ = 0;
    }
    
    void set_cursor(int32_t x, int32_t y) override {
        cursor_x_ = x;
        cursor_y_ = y;
        M5.Lcd.setCursor(x, y);
    }
    
    void set_text_size(uint8_t size) override {
        text_size_ = size;
        M5.Lcd.setTextSize(size);
    }
    
    void set_text_color(uint32_t color) override {
        text_color_ = color;
        M5.Lcd.setTextColor(color);
    }
    
    void set_background_color(uint32_t color) override {
        bg_color_ = color;
    }
    
    uint16_t get_width() const override {
        return M5.Lcd.width();
    }
    
    uint16_t get_height() const override {
        return M5.Lcd.height();
    }
};

// ========================================
// M5StackSystemContext::Impl
// ========================================

class M5StackSystemContext::Impl {
public:
    M5StackSerialCommunication serial0{0};
    M5StackSerialCommunication serial1{1};
    M5StackBluetoothCommunication bluetooth;
    M5StackWiFiCommunication wifi;
    M5StackButton button_a{39};
    M5StackButton button_b{38};
    M5StackButton button_c{37};
    M5StackAccelerometer accelerometer;
    M5StackGyroscope gyroscope;
    M5StackDisplay display;
    bool initialized{false};
};

// ========================================
// M5StackSystemContext実装
// ========================================

M5StackSystemContext::M5StackSystemContext()
    : impl_(new Impl()) {
}

M5StackSystemContext::~M5StackSystemContext() {
    delete impl_;
}

M5StackSystemContext& M5StackSystemContext::get_instance() {
    static M5StackSystemContext instance;
    return instance;
}

const char* M5StackSystemContext::get_device_name() const {
    return "M5Stack";
}

const char* M5StackSystemContext::get_firmware_version() const {
    return "Omusubi 0.1.0";
}

FixedString<32> M5StackSystemContext::get_chip_id() const {
    uint64_t chip_id = ESP.getEfuseMac();
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%04X%08X",
             (uint16_t)(chip_id >> 32), (uint32_t)chip_id);
    return FixedString<32>(buffer);
}

uint32_t M5StackSystemContext::get_uptime_ms() const {
    return millis();
}

uint32_t M5StackSystemContext::get_free_memory() const {
    return esp_get_free_heap_size();
}

PowerState M5StackSystemContext::get_power_state() const {
    return PowerState::usb;  // 簡易実装
}

uint8_t M5StackSystemContext::get_battery_level() const {
    float voltage = M5.Power.getBatteryVoltage() / 1000.0f;
    if (voltage >= 4.1f) return 100;
    if (voltage >= 3.7f) return 50;
    if (voltage >= 3.5f) return 10;
    return 0;
}

SerialCommunication* M5StackSystemContext::get_serial(uint8_t port) {
    if (port == 0) return &impl_->serial0;
    if (port == 1) return &impl_->serial1;
    return nullptr;
}

BluetoothCommunication* M5StackSystemContext::get_bluetooth() {
    return &impl_->bluetooth;
}

WiFiCommunication* M5StackSystemContext::get_wifi() {
    return &impl_->wifi;
}

Pressable* M5StackSystemContext::get_button(uint8_t index) {
    if (index == 0) return &impl_->button_a;
    if (index == 1) return &impl_->button_b;
    if (index == 2) return &impl_->button_c;
    return nullptr;
}

uint8_t M5StackSystemContext::get_button_count() const {
    return 3;
}

Measurable3D* M5StackSystemContext::get_accelerometer() {
    return &impl_->accelerometer;
}

Measurable3D* M5StackSystemContext::get_gyroscope() {
    return &impl_->gyroscope;
}

Displayable* M5StackSystemContext::get_display() {
    return &impl_->display;
}

void M5StackSystemContext::begin() {
    if (impl_->initialized) return;
    
    M5.begin();
    impl_->serial0.connect();
    impl_->accelerometer.begin();
    impl_->gyroscope.begin();
    
    impl_->initialized = true;
}

void M5StackSystemContext::update() {
    M5.update();
    impl_->button_a.update();
    impl_->button_b.update();
    impl_->button_c.update();
}

void M5StackSystemContext::delay(uint32_t ms) {
    ::delay(ms);
}

void M5StackSystemContext::reset() {
    esp_restart();
}

}  // namespace m5stack
}  // namespace platform
}  // namespace omusubi