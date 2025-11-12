// ========================================
// examples/wifi/main.cpp
// ========================================
#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

// WiFi設定
constexpr const char* WIFI_SSID = "YourSSID";
constexpr const char* WIFI_PASSWORD = "YourPassword";

void setup() {
    SystemContext& ctx = get_system_context();
    ctx.begin();
    
    SerialCommunication* serial = ctx.get_serial(0);
    WiFiCommunication* wifi = ctx.get_wifi();
    Displayable* display = ctx.get_display();
    
    if (serial) {
        serial->write_line("=== WiFi Example ==="_sv);
    }
    
    if (display) {
        display->clear();
        display->set_cursor(0, 0);
        display->set_text_size(2);
        display->write_line("WiFi"_sv);
        display->set_text_size(1);
        display->write_line();
        display->write_line("Connecting..."_sv);
    }
    
    if (wifi) {
        // WiFi接続
        if (serial) {
            serial->write("Connecting to "_sv);
            serial->write_line(StringView::from_c_string(WIFI_SSID));
        }
        
        if (wifi->connect_to(
            StringView::from_c_string(WIFI_SSID),
            StringView::from_c_string(WIFI_PASSWORD),
            10000  // 10秒タイムアウト
        )) {
            if (display) {
                display->write_line("Connected!"_sv);
                display->write_line();
                display->write("IP: "_sv);
                display->write_line(wifi->get_ip_address());
            }
            
            if (serial) {
                serial->write_line("WiFi connected!"_sv);
                serial->write("  IP: "_sv);
                serial->write_line(wifi->get_ip_address());
                serial->write("  Gateway: "_sv);
                serial->write_line(wifi->get_gateway());
                serial->write("  DNS: "_sv);
                serial->write_line(wifi->get_dns());
                serial->write("  SSID: "_sv);
                serial->write_line(wifi->get_connected_ssid());
            }
        } else {
            if (display) {
                display->write_line("Failed!"_sv);
            }
            
            if (serial) {
                serial->write_line("WiFi connection failed"_sv);
            }
        }
    }
}

void loop() {
    SystemContext& ctx = get_system_context();
    ctx.update();
    
    SerialCommunication* serial = ctx.get_serial(0);
    WiFiCommunication* wifi = ctx.get_wifi();
    Pressable* button_a = ctx.get_button(0);
    
    // ボタンA: 接続状態を表示
    if (button_a && button_a->was_pressed()) {
        if (wifi && wifi->is_connected()) {
            if (serial) {
                serial->write_line("=== WiFi Status ==="_sv);
                serial->write("  Connected: Yes"_sv);
                serial->write_line();
                serial->write("  SSID: "_sv);
                serial->write_line(wifi->get_connected_ssid());
                serial->write("  IP: "_sv);
                serial->write_line(wifi->get_ip_address());
                serial->write("  Subnet: "_sv);
                serial->write_line(wifi->get_subnet_mask());
                serial->write("  Gateway: "_sv);
                serial->write_line(wifi->get_gateway());
                serial->write("  DNS: "_sv);
                serial->write_line(wifi->get_dns());
                serial->write("  MAC: "_sv);
                serial->write_line(wifi->get_mac_address());
                serial->write_line("==================="_sv);
            }
        } else {
            if (serial) {
                serial->write_line("WiFi not connected"_sv);
            }
        }
    }
    
    ctx.delay(10);
}