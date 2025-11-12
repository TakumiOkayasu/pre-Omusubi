#pragma once

/**
 * @file mcu_config.h
 * @brief MCU（マイコン）固有の設定定義
 *
 * デバイスやプラットフォームに応じた設定値やマクロを定義します。
 * コンパイル時に各種制約や機能を調整できます。
 */

// メモリ制約のあるMCU向けの設定
#ifndef OMUSUBI_MAX_STRING_LENGTH
#define OMUSUBI_MAX_STRING_LENGTH 256
#endif

#ifndef OMUSUBI_MAX_BUFFER_SIZE
#define OMUSUBI_MAX_BUFFER_SIZE 1024
#endif

// デバッグビルドの判定
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
#define OMUSUBI_DEBUG 1
#else
#define OMUSUBI_DEBUG 0
#endif

// プラットフォーム判定
#if defined(ESP32) || defined(ESP8266)
#define OMUSUBI_PLATFORM_ESP 1
#elif defined(ARDUINO)
#define OMUSUBI_PLATFORM_ARDUINO 1
#else
#define OMUSUBI_PLATFORM_GENERIC 1
#endif
