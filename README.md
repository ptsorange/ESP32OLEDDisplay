# 📱 ESP32 OLED Weather Station

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Core-orange.svg?logo=platformio&style=flat)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/ESP32-Espressif-red.svg?logo=espressif&style=flat)](https://www.espressif.com/)
[![C++](https://img.shields.io/badge/C++-17-00599C.svg?logo=c%2B%2B&style=flat)](https://isocpp.org/)

ESP32 と SSD1306 OLED ディスプレイを使用した、多機能な天気予報 & 時計ガジェットです。
OpenWeatherMap API から天気情報を取得し、NTP サーバーから正確な時刻を同期して表示します。

---

## 📑 目次

- [概要](#-概要)
- [プロジェクト構成](#-プロジェクト構成)
- [機能](#-機能)
- [要件](#-要件)
- [配線](#-配線)
- [実行方法](#-実行方法)
  - [1. セットアップ](#1-セットアップ)
  - [2. ビルドと書き込み](#2-ビルドと書き込み)

---

## 🚀 概要

このプロジェクトは、デスク上の小さな情報端末として機能します。WiFi に接続し、起動時にはアニメーションを表示。その後、現在時刻と天気情報をリアルタイムで表示します。物理ボタンを使用して、表示モードの切り替えや画面の明るさ調整が可能です。

## 📂 プロジェクト構成

このリポジトリの主要なディレクトリ構成は以下の通りです。

| ディレクトリ/ファイル | 説明 |
|--------------|------|
| **`src/main.cpp`** | アプリケーションのメインロジック。WiFi 接続、API 取得、描画処理、ボタン制御が含まれます。 |
| **`src/animation.cpp`** | 起動時の WiFi 接続アニメーションなどの描画データが含まれます。 |
| **`include/setting.h`** | **(重要)** WiFi の SSID/PASS や OpenWeatherMap の API キーなどの設定ファイルです。 |
| **`include/animation.h`** | アニメーション用のヘッダーファイルです。 |
| **`platformio.ini`** | PlatformIO のプロジェクト設定ファイル。ライブラリ依存関係などが定義されています。 |

## ✨ 機能

- ✅ **正確な時刻表示**: NTP サーバー (pool.ntp.org) と同期し、常に正確な時刻を表示します。
- ✅ **リアルタイム天気予報**: OpenWeatherMap API を使用し、設定した都市の天気、気温、湿度、気圧、風速を取得します。
- ✅ **3つの表示モード**:
  - 🏠 **HOME**: 時刻、天気、現在の気温を表示。
  - 📊 **DETAIL**: 最高/最低気温、湿度、気圧、風速などの詳細情報を表示。
  - ⚙️ **SETTING**: 接続中の SSID、IP アドレス、都市名、画面の明るさレベルを表示。
- ✅ **明るさ調整**: OLED ディスプレイの輝度を 5 段階で調整可能（SETTING モード時）。
- ✅ **接続アニメーション**: WiFi 接続待ちの間、アイコンアニメーションを表示します。

## 🛠 要件

### ハードウェア
- **ESP32 Development Board** (ESP-WROOM-32 等)
- **SSD1306 OLED Display** (128x64, I2C接続)
- **タクトスイッチ (Push Button)** x 4
- **ジャンパーワイヤ & ブレッドボード**

### ソフトウェア
- **VS Code** + **PlatformIO** 拡張機能
- **OpenWeatherMap API Key** (無料で取得可能)

## 🔌 配線

ソースコード (`src/main.cpp`) に基づくピン配置は以下の通りです。

| デバイス | ピン名 | ESP32 GPIO | 備考 |
|----------|--------|------------|------|
| **OLED** | SDA | **22** | `Wire.begin(22, 23)` |
| **OLED** | SCL | **23** | |
| **Button 1** | HOME | **12** | モード切替 (HOME / 天気更新) |
| **Button 2** | DETAIL | **14** | モード切替 (詳細表示) |
| **Button 3** | (Reserved) | **27** | 予備 / 未割り当て (画面オフ) |
| **Button 4** | SETTING | **26** | モード切替 (設定情報 / 明るさ変更) |

※ ボタンは `INPUT_PULLUP` で設定されているため、片側を GPIO、もう片側を GND に接続してください。

## 📖 実行方法

### 1. セットアップ

プロジェクトの `include` ディレクトリ内に `setting.h` ファイルを作成し、以下の内容を記述してください。

```cpp
// include/setting.h
#ifndef SETTING_H
#define SETTING_H

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* weatherAPI = "YOUR_OPENWEATHERMAP_API_KEY";
const char* city = "Tokyo"; // 天気を取得したい都市名(できなかったら"都市名,jp")

#endif
```

### 2. ビルドと書き込み

1. VS Code でこのフォルダを開きます。
2. PlatformIO のサイドバーから `Build` を実行し、ライブラリをダウンロード・コンパイルします。
3. ESP32 を PC に接続し、`Upload` を実行して書き込みます。
4. 書き込み完了後、自動的にリセットされ動作が開始します。

---

<div align="center">

Made with ☕ by ptsorange

</div>
