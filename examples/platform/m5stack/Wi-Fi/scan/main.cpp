#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

void setup() {
    SystemContext& ctx = get_system_context();
    ctx.begin();
    
    SerialCommunication* serial = ctx.get_serial(0);
    WiFiCommunication* wifi = ctx.get_wifi();
    Displayable* display = ctx.get_display();
    
    if (serial) {
        serial->write_line("=== WiFi Scanner ==="_sv);
    }
    
    if (display) {
        display->clear();
        display->set_cursor(0, 0);
        display->set_text_size(2);
        display->write_line("WiFi Scan"_sv);
        display->set_text_size(1);
        display->write_line();
        display->write_line("Press A to scan"_sv);
    }
}

void loop() {
    SystemContext& ctx = get_system_context();
    ctx.update();
    
    SerialCommunication* serial = ctx.get_serial(0);
    WiFiCommunication* wifi = ctx.get_wifi();
    Displayable* display = ctx.get_display();
    Pressable* button_a = ctx.get_button(0);
    
    // ボタンA: スキャン
    if (button_a && button_a->was_pressed()) {
        if (display) {
            display->clear();
            display->set_cursor(0, 0);
            display->write_line("Scanning..."_sv);
        }
        
        if (serial) {
            serial->write_line("Scanning WiFi networks..."_sv);
        }
        
        if (wifi) {
            wifi->start_scan();
            ctx.delay(3000);
            wifi->stop_scan();
            
            uint8_t count = wifi->get_found_count();
            
            if (display) {
                display->clear();
                display->set_cursor(0, 0);
                display->write("Found: "_sv);
                display->write_line(count);
                display->write_line();
            }
            
            if (serial) {
                serial->write("Found "_sv);
                serial->write(count);
                serial->write_line(" networks:"_sv);
            }
            
            for (uint8_t i = 0; i < count && i < 10; ++i) {
                FixedString<64> ssid = wifi->get_found_name(i);
                
                if (display && i < 5) {
                    display->write(i);
                    display->write(": "_sv);
                    display->write(ssid.view().substring(0, 18));
                    display->write_line();
                }
                
                if (serial) {
                    serial->write("  "_sv);
                    serial->write(i);
                    serial->write(": "_sv);
                    serial->write_line(ssid);
                }
            }
        }
    }
    
    ctx.delay(10);
}