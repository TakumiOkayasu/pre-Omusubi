#pragma once

namespace omusubi {

// 前方宣言
// TODO: 実際の出力デバイスContextクラスを作成後、ここに追加
// class DisplayContext;
// class LEDContext;
// class SpeakerContext;

/**
 * @brief 出力デバイスのコンテキスト
 */
class OutputContext {
public:
    OutputContext() = default;
    virtual ~OutputContext() = default;
    OutputContext(const OutputContext&) = delete;
    OutputContext& operator=(const OutputContext&) = delete;
    OutputContext(OutputContext&&) = delete;
    OutputContext& operator=(OutputContext&&) = delete;

    // TODO: 出力デバイスのgetterメソッドをここに追加
    // virtual DisplayContext* get_display_context() = 0;
    // virtual LEDContext* get_led_context() = 0;
    // virtual SpeakerContext* get_speaker_context() = 0;
};

}  // namespace omusubi
