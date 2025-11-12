#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

void setup() {
    SystemContext& ctx = get_system_context();
    ctx.begin();
    
    SerialCommunication* serial = ctx.get_serial(0);
    BluetoothCommunication* bt = ctx.get_bluetooth();
    Displayable* display = ctx.get_display();
    
    if (serial) {
        serial->write_line("=== Bluetooth Example ==="_sv);
    }
    
    if (display) {
        display->clear();
        display->set_cursor(0, 0);
        display->set_text_size(2);
        display->write_line("Bluetooth"_sv);
        display->set_text_size(1);
    }
    
    if (bt) {
        // ローカル名を設定
        bt->set_local_name("M5Stack-BT"_sv);
        
        if (serial) {
            serial->write("Local name: "_sv);
            serial->write_line(bt->get_local_name());
        }
        
        if (display) {
            display->write("Name: "_sv);
            display->write_line(bt->get_local_name());
            display->write_line();
            display->write_line("A: Scan"_sv);
            display->write_line("B: Wait connection"_sv);
        }
    }
}

void loop() {
    SystemContext& ctx = get_system_context();
    ctx.update();
    
    SerialCommunication* serial = ctx.get_serial(0);
    BluetoothCommunication* bt = ctx.get_bluetooth();
    Displayable* display = ctx.get_display();
    Pressable* button_a = ctx.get_button(0);
    Pressable* button_b = ctx.get_button(1);
    
    // ボタンA: スキャン
    if (button_a && button_a->was_pressed()) {
        if (display) {
            display->clear();
            display->set_cursor(0, 0);
            display->write_line("Scanning..."_sv);
        }
        
        if (bt) {
            bt->start_scan();
            ctx.delay(3000);
            bt->stop_scan();
            
            uint8_t count = bt->get_found_count();
            
            if (display) {
                display->write("Found: "_sv);
                display->write_line(count);
                display->write_line();
            }
            
            if (serial) {
                serial->write("Found "_sv);
                serial->write(count);
                serial->write_line(" devices:"_sv);
            }
            
            for (uint8_t i = 0; i < count && i < 5; ++i) {
                FixedString<64> name = bt->get_found_name(i);
                int32_t rssi = bt->get_found_signal_strength(i);
                
                if (display) {
                    display->write(i);
                    display->write(": "_sv);
                    display->write(name.view().substring(0, 15));
                    display->write_line();
                }
                
                if (serial) {
                    serial->write("  "_sv);
                    serial->write(i);
                    serial->write(": "_sv);
                    serial->write(name);
                    serial->write(" ("_sv);
                    serial->write(rssi);
                    serial->write_line(" dBm)"_sv);
                }
            }
        }
    }
    
    // ボタンB: 接続待ち
    if (button_b && button_b->was_pressed()) {
        if (display) {
            display->clear();
            display->set_cursor(0, 0);
            display->write_line("Waiting..."_sv);
        }
        
        if (serial) {
            serial->write_line("Waiting for connection..."_sv);
        }
        
        while (bt && !bt->is_connected()) {
            ctx.update();
            ctx.delay(100);
        }
        
        if (bt && bt->is_connected()) {
            if (display) {
                display->write_line("Connected!"_sv);
                display->write_line(bt->get_connected_device_name());
            }
            
            if (serial) {
                serial->write_line("Connected!"_sv);
            }
        }
    }
    
    // Bluetooth通信
    if (bt && bt->is_connected()) {
        if (bt->has_data()) {
            FixedString<256> message = bt->read_line();
            
            if (display) {
                display->write("RX: "_sv);
                display->write_line(message);
            }
            
            if (serial) {
                serial->write("RX: "_sv);
                serial->write_line(message);
            }
            
            // エコーバック
            bt->write("Echo: "_sv);
            bt->write_line(message);
        }
    }
    
    ctx.delay(10);
}