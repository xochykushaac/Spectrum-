# Spectrum

## Интерактивная акустическая система с динамической светомузыкальной мини-сценой-визуализатором
## Interactive Audio System with a Dynamic Light-and-Music Mini-Stage Visualizer

Третьякова Елена Б01-511

Давыдова Дарья Б01-502

## Аудиотракт/Audio pipeline

Система получает аудио с внешнего устройства по Bluetooth A2DP.
Исходный PCM-аудиосигнал кодируется в поток SBC на передающем устройстве, передается на ESP32 и там декодируется обратно в PCM-отсчеты, после декодирования PCM-аудио разделяется на две параллельные ветви:
- **Ветка воспроизведения** — PCM-отсчеты передаются по I2S на усилитель класса D. Усилитель формирует электрический выходной сигнал, который подается на динамик и в результате создает звук.
- **Ветка анализа** — те же PCM-отсчеты обрабатываются на ESP32 для извлечения информации из музыки. Полученные признаки затем используются для управления адресной светодиодной лентой, лазерами, стробоскопами, генератором тумана и другими эффектами.
  
Общая схема прохождения данных показана ниже.

---

The system receives audio from an external device over Bluetooth A2DP.
The original PCM audio is encoded into an SBC stream on the transmitting device, transmitted to the ESP32, and decoded back into PCM audio samples on the ESP32, after decoding, the PCM audio is split into two parallel processing paths:
- **Playback path** — PCM samples are transferred over I2S to the Class-D audio amplifier. The amplifier generates the electrical output that drives the speaker and produces sound.
- **Analysis path** — the same PCM samples are processed on the ESP32 to extract information from the music. These features are then used to control the LED strip, lasers, strobes, fog machine, and other effects.

The overall data flow is shown below.

![Audio pipeline](assets/structure-graph-common.svg)
