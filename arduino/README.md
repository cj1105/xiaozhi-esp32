# Arduino 版本迁移骨架

- 目录结构遵循 Arduino/PlatformIO，提供最小可运行骨架以逐步移植原 ESP-IDF 功能。
- 使用 Application/Board/Display 等核心模块抽象，便于按板型与外设扩展。
- 默认目标为 ESP32‑S3 DevKitC，通过 PlatformIO 构建，可在 Arduino IDE 中导入 `arduino` 目录。

## 快速开始
- 安装 PlatformIO 或 Arduino IDE（配置 ESP32 Arduino Core）。
- 使用 PlatformIO: `platformio run -d arduino`，烧写为 `platformio run -t upload -d arduino`。
- 使用 Arduino IDE: 打开 `arduino/src/main.ino`，选择合适的开发板与端口，编译并烧写。

## 后续移植建议
- 显示：将 LVGL 与具体面板驱动移至 `Display` 实现。
- 音频：在 `Board` 中对接 I2S 与编解码器驱动，复用唤醒词与音频服务。
- 网络：在 `WiFiService` 与 `Protocols` 中对接 Wi‑Fi、MQTT/WebSocket。
- 资源：将原有 assets 的加载逻辑按需移植至 Arduino 环境。

