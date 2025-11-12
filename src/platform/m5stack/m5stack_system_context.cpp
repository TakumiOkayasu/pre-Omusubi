#include "omusubi/platform/m5stack/m5stack_system_context.hpp"
#include <M5Stack.h>
#include <esp_system.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEClient.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <memory>
#include <new>

namespace omusubi {
namespace platform {
namespace m5stack {

// ========================================
// 内部実装クラス
// ========================================

class M5StackSerialContext final : public SerialContext {
private:
    HardwareSerial* serial_;
    uint8_t port_;
    uint32_t baud_rate_;
    bool connected_;
    
public:
    explicit M5StackSerialContext(uint8_t port)
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

class M5StackBluetoothContext final : public BluetoothContext {
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
    M5StackBluetoothContext()
        : connected_(false)
        , found_count_(0)
        , scanning_(false) {
        strncpy(local_name_, "M5Stack", sizeof(local_name_));
        local_name_[sizeof(local_name_) - 1] = '\0';
        last_device_name_[0] = '\0';
    }
    
    ~M5StackBluetoothContext() {
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

class M5StackWiFiContext final : public WiFiContext {
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
    M5StackWiFiContext()
        : connected_(false)
        , ap_mode_(false)
        , found_count_(0)
        , scanning_(false) {
        last_ssid_[0] = '\0';
        last_password_[0] = '\0';
    }
    
    ~M5StackWiFiContext() {
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

class M5StackButtonContext final : public Pressable {
private:
    uint8_t pin_;
    bool current_state_;
    bool previous_state_;
    
public:
    explicit M5StackButtonContext(uint8_t pin)
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

class M5StackAccelerometerContext final : public Measurable3D {
private:
    bool initialized_;
    
public:
    M5StackAccelerometerContext() : initialized_(false) {}
    
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

class M5StackGyroscopeContext final : public Measurable3D {
private:
    bool initialized_;
    
public:
    M5StackGyroscopeContext() : initialized_(false) {}
    
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

class M5StackDisplayContext final : public Displayable {
private:
    int32_t cursor_x_;
    int32_t cursor_y_;
    uint8_t text_size_;
    uint32_t text_color_;
    uint32_t bg_color_;
    
public:
    M5StackDisplayContext()
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

class M5StackBLECharacteristicContext final : public BLECharacteristic {
private:
    BLERemoteCharacteristic* remote_char_;  // Centralモード用
    ::BLECharacteristic* local_char_;        // Peripheralモード用
    char uuid_[64];
    uint16_t properties_;
    
public:
    M5StackBLECharacteristicContext(BLERemoteCharacteristic* remote_char)
        : remote_char_(remote_char)
        , local_char_(nullptr)
        , properties_(0) {
        if (remote_char_) {
            strncpy(uuid_, remote_char_->getUUID().toString().c_str(), sizeof(uuid_) - 1);
            uuid_[sizeof(uuid_) - 1] = '\0';
            
            // プロパティを取得
            if (remote_char_->canRead()) {
                properties_ |= static_cast<uint16_t>(BLECharacteristicProperty::read);
            }
            if (remote_char_->canWrite()) {
                properties_ |= static_cast<uint16_t>(BLECharacteristicProperty::write);
            }
            if (remote_char_->canNotify()) {
                properties_ |= static_cast<uint16_t>(BLECharacteristicProperty::notify);
            }
            if (remote_char_->canIndicate()) {
                properties_ |= static_cast<uint16_t>(BLECharacteristicProperty::indicate);
            }
            if (remote_char_->canWriteNoResponse()) {
                properties_ |= static_cast<uint16_t>(BLECharacteristicProperty::write_no_rsp);
            }
        }
    }
    
    M5StackBLECharacteristicContext(::BLECharacteristic* local_char, uint16_t properties)
        : remote_char_(nullptr)
        , local_char_(local_char)
        , properties_(properties) {
        if (local_char_) {
            strncpy(uuid_, local_char_->getUUID().toString().c_str(), sizeof(uuid_) - 1);
            uuid_[sizeof(uuid_) - 1] = '\0';
        }
    }
    
    FixedString<64> get_uuid() const override {
        return FixedString<64>(uuid_);
    }
    
    FixedBuffer<512> read() const override {
        FixedBuffer<512> result;
        
        if (remote_char_ && remote_char_->canRead()) {
            std::string value = remote_char_->readValue();
            for (size_t i = 0; i < value.length() && i < 512; ++i) {
                result.append(static_cast<uint8_t>(value[i]));
            }
        } else if (local_char_) {
            std::string value = local_char_->getValue();
            for (size_t i = 0; i < value.length() && i < 512; ++i) {
                result.append(static_cast<uint8_t>(value[i]));
            }
        }
        
        return result;
    }
    
    bool write(const uint8_t* data, uint32_t length) override {
        if (!data || length == 0) return false;
        
        if (remote_char_ && remote_char_->canWrite()) {
            remote_char_->writeValue(const_cast<uint8_t*>(data), length, true);
            return true;
        } else if (local_char_) {
            local_char_->setValue(const_cast<uint8_t*>(data), length);
            return true;
        }
        
        return false;
    }
    
    FixedString<256> read_string() const override {
        FixedString<256> result;
        
        if (remote_char_ && remote_char_->canRead()) {
            std::string value = remote_char_->readValue();
            for (size_t i = 0; i < value.length() && i < 256; ++i) {
                if (!result.append(value[i])) break;
            }
        } else if (local_char_) {
            std::string value = local_char_->getValue();
            for (size_t i = 0; i < value.length() && i < 256; ++i) {
                if (!result.append(value[i])) break;
            }
        }
        
        return result;
    }
    
    bool write_string(StringView value) override {
        if (remote_char_ && remote_char_->canWrite()) {
            std::string str;
            for (uint32_t i = 0; i < value.byte_length(); ++i) {
                str += value[i];
            }
            remote_char_->writeValue(str, true);
            return true;
        } else if (local_char_) {
            std::string str;
            for (uint32_t i = 0; i < value.byte_length(); ++i) {
                str += value[i];
            }
            local_char_->setValue(str);
            return true;
        }
        
        return false;
    }
    
    bool notify(const uint8_t* data, uint32_t length) override {
        if (!local_char_ || !data || length == 0) return false;
        
        local_char_->setValue(const_cast<uint8_t*>(data), length);
        local_char_->notify();
        return true;
    }
    
    uint16_t get_properties() const override {
        return properties_;
    }
};

// ========================================
// M5Stack BLE Service実装
// ========================================
class M5StackBLEServiceContext final : public BLEService {
private:
    BLERemoteService* remote_service_;  // Centralモード用
    ::BLEService* local_service_;        // Peripheralモード用
    char uuid_[64];
    
    M5StackBLECharacteristicContext* characteristics_[16];
    uint8_t characteristic_count_;
    
public:
    M5StackBLEServiceContext(BLERemoteService* remote_service)
        : remote_service_(remote_service)
        , local_service_(nullptr)
        , characteristic_count_(0) {
        if (remote_service_) {
            strncpy(uuid_, remote_service_->getUUID().toString().c_str(), sizeof(uuid_) - 1);
            uuid_[sizeof(uuid_) - 1] = '\0';
        }
        
        for (uint8_t i = 0; i < 16; ++i) {
            characteristics_[i] = nullptr;
        }
    }
    
    M5StackBLEServiceContext(::BLEService* local_service)
        : remote_service_(nullptr)
        , local_service_(local_service)
        , characteristic_count_(0) {
        if (local_service_) {
            strncpy(uuid_, local_service_->getUUID().toString().c_str(), sizeof(uuid_) - 1);
            uuid_[sizeof(uuid_) - 1] = '\0';
        }
        
        for (uint8_t i = 0; i < 16; ++i) {
            characteristics_[i] = nullptr;
        }
    }
    
    ~M5StackBLEServiceContext() {
        for (uint8_t i = 0; i < characteristic_count_; ++i) {
            delete characteristics_[i];
        }
    }
    
    FixedString<64> get_uuid() const override {
        return FixedString<64>(uuid_);
    }
    
    BLECharacteristic* add_characteristic(StringView uuid, uint16_t properties) override {
        if (!local_service_ || characteristic_count_ >= 16) {
            return nullptr;
        }
        
        char uuid_str[64];
        uint32_t len = (uuid.byte_length() < sizeof(uuid_str) - 1) ?
                       uuid.byte_length() : sizeof(uuid_str) - 1;
        for (uint32_t i = 0; i < len; ++i) {
            uuid_str[i] = uuid[i];
        }
        uuid_str[len] = '\0';
        
        ::BLECharacteristic* char_obj = local_service_->createCharacteristic(
            uuid_str,
            properties
        );
        
        if (!char_obj) return nullptr;
        
        M5StackBLECharacteristicContext* wrapper = new M5StackBLECharacteristicContext(char_obj, properties);
        characteristics_[characteristic_count_++] = wrapper;
        
        return wrapper;
    }
    
    BLECharacteristic* get_characteristic(StringView uuid) override {
        for (uint8_t i = 0; i < characteristic_count_; ++i) {
            if (characteristics_[i]->get_uuid().view() == uuid) {
                return characteristics_[i];
            }
        }
        
        if (remote_service_) {
            char uuid_str[64];
            uint32_t len = (uuid.byte_length() < sizeof(uuid_str) - 1) ?
                           uuid.byte_length() : sizeof(uuid_str) - 1;
            for (uint32_t i = 0; i < len; ++i) {
                uuid_str[i] = uuid[i];
            }
            uuid_str[len] = '\0';
            
            BLERemoteCharacteristic* remote_char = 
                remote_service_->getCharacteristic(BLEUUID(uuid_str));
            
            if (remote_char && characteristic_count_ < 16) {
                M5StackBLECharacteristicContext* wrapper = 
                    new M5StackBLECharacteristicContext(remote_char);
                characteristics_[characteristic_count_++] = wrapper;
                return wrapper;
            }
        }
        
        return nullptr;
    }
    
    uint8_t get_characteristic_count() const override {
        return characteristic_count_;
    }
    
    BLECharacteristic* get_characteristic_at(uint8_t index) override {
        if (index >= characteristic_count_) {
            return nullptr;
        }
        return characteristics_[index];
    }
};

// ========================================
// M5Stack BLE Communication実装
// ========================================
class M5StackBLEContext final : public BLEContext {
private:
    BLEMode mode_;
    bool initialized_;
    bool connected_;
    bool advertising_;
    char local_name_[64];
    
    // Central mode
    BLEClient* client_;
    BLEScan* scan_;
    
    // Peripheral mode
    BLEServer* server_;
    
    // Services
    M5StackBLEServiceContext* services_[8];
    uint8_t service_count_;
    
    // Scan results
    struct FoundDevice {
        char name[64];
        char address[32];
        int32_t rssi;
        bool connectable;
    };
    FoundDevice found_devices_[10];
    uint8_t found_count_;
    bool scanning_;
    
public:
    M5StackBLEContext()
        : mode_(BLEMode::idle)
        , initialized_(false)
        , connected_(false)
        , advertising_(false)
        , client_(nullptr)
        , scan_(nullptr)
        , server_(nullptr)
        , service_count_(0)
        , found_count_(0)
        , scanning_(false) {
        
        strncpy(local_name_, "M5Stack-BLE", sizeof(local_name_));
        local_name_[sizeof(local_name_) - 1] = '\0';
        
        for (uint8_t i = 0; i < 8; ++i) {
            services_[i] = nullptr;
        }
    }
    
    ~M5StackBLEContext() {
        end();
    }
    
    // ========================================
    // モード管理
    // ========================================
    
    bool begin_central(StringView device_name) override {
        if (initialized_) {
            end();
        }
        
        uint32_t len = (device_name.byte_length() < sizeof(local_name_) - 1) ?
                       device_name.byte_length() : sizeof(local_name_) - 1;
        for (uint32_t i = 0; i < len; ++i) {
            local_name_[i] = device_name[i];
        }
        local_name_[len] = '\0';
        
        BLEDevice::init(local_name_);
        
        client_ = BLEDevice::createClient();
        scan_ = BLEDevice::getScan();
        scan_->setActiveScan(true);
        scan_->setInterval(100);
        scan_->setWindow(99);
        
        mode_ = BLEMode::central;
        initialized_ = true;
        
        return true;
    }
    
    bool begin_peripheral(StringView device_name) override {
        if (initialized_) {
            end();
        }
        
        uint32_t len = (device_name.byte_length() < sizeof(local_name_) - 1) ?
                       device_name.byte_length() : sizeof(local_name_) - 1;
        for (uint32_t i = 0; i < len; ++i) {
            local_name_[i] = device_name[i];
        }
        local_name_[len] = '\0';
        
        BLEDevice::init(local_name_);
        
        server_ = BLEDevice::createServer();
        
        mode_ = BLEMode::peripheral;
        initialized_ = true;
        
        return true;
    }
    
    BLEMode get_mode() const override {
        return mode_;
    }
    
    void end() override {
        if (!initialized_) return;
        
        if (connected_) {
            disconnect();
        }
        
        if (advertising_) {
            stop_advertising();
        }
        
        for (uint8_t i = 0; i < service_count_; ++i) {
            delete services_[i];
            services_[i] = nullptr;
        }
        service_count_ = 0;
        
        BLEDevice::deinit(true);
        
        mode_ = BLEMode::idle;
        initialized_ = false;
        client_ = nullptr;
        scan_ = nullptr;
        server_ = nullptr;
    }
    
    // ========================================
    // Connectable実装
    // ========================================
    
    bool connect() override {
        return false;  // BLEではデバイス名またはアドレスが必要
    }
    
    void disconnect() override {
        if (!connected_) return;
        
        if (client_) {
            client_->disconnect();
        }
        
        connected_ = false;
    }
    
    bool is_connected() const override {
        if (mode_ == BLEMode::central && client_) {
            return client_->isConnected();
        } else if (mode_ == BLEMode::peripheral && server_) {
            return server_->getConnectedCount() > 0;
        }
        return false;
    }
    
    // ========================================
    // Scannable実装
    // ========================================
    
    void start_scan() override {
        if (!scan_ || mode_ != BLEMode::central) return;
        
        found_count_ = 0;
        scanning_ = true;
        
        BLEScanResults results = scan_->start(5, false);
        
        uint8_t count = results.getCount();
        found_count_ = (count < 10) ? count : 10;
        
        for (uint8_t i = 0; i < found_count_; ++i) {
            BLEAdvertisedDevice device = results.getDevice(i);
            
            // デバイス名
            if (device.haveName()) {
                strncpy(found_devices_[i].name, device.getName().c_str(), 
                       sizeof(found_devices_[i].name) - 1);
            } else {
                strncpy(found_devices_[i].name, "Unknown", 
                       sizeof(found_devices_[i].name) - 1);
            }
            found_devices_[i].name[sizeof(found_devices_[i].name) - 1] = '\0';
            
            // MACアドレス
            strncpy(found_devices_[i].address, device.getAddress().toString().c_str(),
                   sizeof(found_devices_[i].address) - 1);
            found_devices_[i].address[sizeof(found_devices_[i].address) - 1] = '\0';
            
            // RSSI
            found_devices_[i].rssi = device.getRSSI();
            
            // Connectable
            found_devices_[i].connectable = true;  // BLEはデフォルトでconnectable
        }
        
        scan_->clearResults();
        scanning_ = false;
    }
    
    void stop_scan() override {
        if (scanning_ && scan_) {
            scan_->stop();
            scanning_ = false;
        }
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
    // Central（クライアント）モード
    // ========================================
    
    bool connect_to(StringView device_name, uint32_t timeout_ms) override {
        if (mode_ != BLEMode::central || !client_) {
            return false;
        }
        
        // スキャンしてデバイスを探す
        start_scan();
        
        for (uint8_t i = 0; i < found_count_; ++i) {
            if (get_found_name(i).view() == device_name) {
                return connect_to_address(
                    StringView::from_c_string(found_devices_[i].address),
                    timeout_ms
                );
            }
        }
        
        return false;
    }
    
    bool connect_to_address(StringView address, uint32_t timeout_ms) override {
        if (mode_ != BLEMode::central || !client_) {
            return false;
        }
        
        char addr_str[32];
        uint32_t len = (address.byte_length() < sizeof(addr_str) - 1) ?
                       address.byte_length() : sizeof(addr_str) - 1;
        for (uint32_t i = 0; i < len; ++i) {
            addr_str[i] = address[i];
        }
        addr_str[len] = '\0';
        
        BLEAddress ble_addr(addr_str);
        
        if (timeout_ms > 0) {
            uint32_t start = millis();
            while (millis() - start < timeout_ms) {
                if (client_->connect(ble_addr)) {
                    connected_ = true;
                    return true;
                }
                ::delay(100);
            }
            return false;
        } else {
            if (client_->connect(ble_addr)) {
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
        
        return connect_to_address(
            StringView::from_c_string(found_devices_[index].address),
            0
        );
    }
    
    BLEService* get_service(StringView uuid) override {
        if (!client_ || !connected_) {
            return nullptr;
        }
        
        // 既存のサービスをチェック
        for (uint8_t i = 0; i < service_count_; ++i) {
            if (services_[i]->get_uuid().view() == uuid) {
                return services_[i];
            }
        }
        
        // 新しいサービスを取得
        if (service_count_ >= 8) {
            return nullptr;
        }
        
        char uuid_str[64];
        uint32_t len = (uuid.byte_length() < sizeof(uuid_str) - 1) ?
                       uuid.byte_length() : sizeof(uuid_str) - 1;
        for (uint32_t i = 0; i < len; ++i) {
            uuid_str[i] = uuid[i];
        }
        uuid_str[len] = '\0';
        
        BLERemoteService* remote_service = client_->getService(BLEUUID(uuid_str));
        
        if (!remote_service) {
            return nullptr;
        }
        
        M5StackBLEServiceContext* wrapper = new M5StackBLEServiceContext(remote_service);
        services_[service_count_++] = wrapper;
        
        return wrapper;
    }
    
    uint8_t get_service_count() const override {
        return service_count_;
    }
    
    BLEService* get_service_at(uint8_t index) override {
        if (index >= service_count_) {
            return nullptr;
        }
        return services_[index];
    }
    
    // ========================================
    // Peripheral（サーバー）モード
    // ========================================
    
    BLEService* add_service(StringView uuid) override {
        if (!server_ || mode_ != BLEMode::peripheral || service_count_ >= 8) {
            return nullptr;
        }
        
        char uuid_str[64];
        uint32_t len = (uuid.byte_length() < sizeof(uuid_str) - 1) ?
                       uuid.byte_length() : sizeof(uuid_str) - 1;
        for (uint32_t i = 0; i < len; ++i) {
            uuid_str[i] = uuid[i];
        }
        uuid_str[len] = '\0';
        
        ::BLEService* local_service = server_->createService(uuid_str);
        
        if (!local_service) {
            return nullptr;
        }
        
        M5StackBLEServiceContext* wrapper = new M5StackBLEServiceContext(local_service);
        services_[service_count_++] = wrapper;
        
        return wrapper;
    }
    
    bool start_advertising() override {
        if (!server_ || mode_ != BLEMode::peripheral) {
            return false;
        }
        
        // すべてのサービスを開始
        for (uint8_t i = 0; i < service_count_; ++i) {
            // サービスを開始（内部的に実行）
        }
        
        BLEAdvertising* advertising = BLEDevice::getAdvertising();
        
        // サービスUUIDを広告に追加
        for (uint8_t i = 0; i < service_count_; ++i) {
            FixedString<64> uuid = services_[i]->get_uuid();
            char uuid_str[64];
            for (uint32_t j = 0; j < uuid.byte_length(); ++j) {
                uuid_str[j] = uuid.view()[j];
            }
            uuid_str[uuid.byte_length()] = '\0';
            
            advertising->addServiceUUID(uuid_str);
        }
        
        advertising->setScanResponse(true);
        advertising->setMinPreferred(0x06);
        advertising->setMinPreferred(0x12);
        
        BLEDevice::startAdvertising();
        
        advertising_ = true;
        return true;
    }
    
    void stop_advertising() override {
        if (!advertising_) return;
        
        BLEDevice::stopAdvertising();
        advertising_ = false;
    }
    
    bool is_advertising() const override {
        return advertising_;
    }
    
    // ========================================
    // 共通情報
    // ========================================
    
    void set_local_name(StringView name) override {
        uint32_t len = (name.byte_length() < sizeof(local_name_) - 1) ?
                       name.byte_length() : sizeof(local_name_) - 1;
        for (uint32_t i = 0; i < len; ++i) {
            local_name_[i] = name[i];
        }
        local_name_[len] = '\0';
    }
    
    FixedString<64> get_local_name() const override {
        return FixedString<64>(local_name_);
    }
    
    FixedString<64> get_connected_device_name() const override {
        // BLEでは接続後にデバイス名を直接取得するのは難しい
        return FixedString<64>();
    }
    
    FixedString<32> get_connected_device_address() const override {
        if (client_ && connected_) {
            std::string addr = client_->getPeerAddress().toString();
            return FixedString<32>(addr.c_str());
        }
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
    
    bool is_found_connectable(uint8_t index) const override {
        if (index >= found_count_) {
            return false;
        }
        return found_devices_[index].connectable;
    }
};

// ========================================
// M5StackSystemContext::Impl
// ========================================

class M5StackSystemContext::Impl {
public:
    M5StackSerialContext serial0{0};
    M5StackSerialContext serial1{1};
    M5StackBluetoothContext bluetooth;
    M5StackWiFiContext wifi;
    M5StackBLEContext ble;  // 追加
    M5StackButtonContext button_a{39};
    M5StackButtonContext button_b{38};
    M5StackButtonContext button_c{37};
    M5StackAccelerometerContext accelerometer;
    M5StackGyroscopeContext gyroscope;
    M5StackDisplayContext display;
    bool initialized{false};
};

// ========================================
// M5StackSystemContext実装
// ========================================

M5StackSystemContext::M5StackSystemContext() {
    new (impl_buffer_) Impl();  // placement new - ヒープを使わずバッファに直接構築
}

M5StackSystemContext::~M5StackSystemContext() {
    get_impl()->~Impl();  // 明示的にデストラクタを呼び出し
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
    return PowerState::usb;  // TODO: 実装する
}

uint8_t M5StackSystemContext::get_battery_level() const {
    float voltage = M5.Power.getBatteryVoltage() / 1000.0f;
    if (voltage >= 4.1f) return 100;
    if (voltage >= 3.7f) return 50;
    if (voltage >= 3.5f) return 10;
    return 0;
}

SerialContext* M5StackSystemContext::get_serial(uint8_t port) {
    if (port == 0) return &get_impl()->serial0;
    if (port == 1) return &get_impl()->serial1;
    return nullptr;
}

BluetoothContext* M5StackSystemContext::get_bluetooth() {
    return &get_impl()->bluetooth;
}

WiFiContext* M5StackSystemContext::get_wifi() {
    return &get_impl()->wifi;
}

BLEContext* M5StackSystemContext::get_ble() {
    return &get_impl()->ble;
}

Pressable* M5StackSystemContext::get_button(uint8_t index) {
    if (index == 0) return &get_impl()->button_a;
    if (index == 1) return &get_impl()->button_b;
    if (index == 2) return &get_impl()->button_c;
    return nullptr;
}

uint8_t M5StackSystemContext::get_button_count() const {
    return 3;
}

Measurable3D* M5StackSystemContext::get_accelerometer() {
    return &get_impl()->accelerometer;
}

Measurable3D* M5StackSystemContext::get_gyroscope() {
    return &get_impl()->gyroscope;
}

Displayable* M5StackSystemContext::get_display() {
    return &get_impl()->display;
}

void M5StackSystemContext::begin() {
    if (get_impl()->initialized) return;

    M5.begin();
    get_impl()->serial0.connect();
    get_impl()->accelerometer.begin();
    get_impl()->gyroscope.begin();

    get_impl()->initialized = true;
}

void M5StackSystemContext::update() {
    M5.update();
    get_impl()->button_a.update();
    get_impl()->button_b.update();
    get_impl()->button_c.update();
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