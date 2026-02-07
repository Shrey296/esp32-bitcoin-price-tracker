# ESP32 Bitcoin Price Tracker (TFT Display)

This project is a standalone Bitcoin price tracker built using an ESP32 DevKit V1 and a 1.8-inch SPI TFT display. It fetches the live Bitcoin price in USD from the CoinGecko API and displays it on the screen along with the current time. Once programmed, the device runs independently and does not require a computer to stay connected.

This was my first complete ESP32 project involving hardware interfacing, WiFi networking, HTTPS communication, and display handling. The objective was not to build something perfect, but to learn by building something real.

---

## Project Background

I started this project to understand how the ESP32 works beyond simple LED or serial examples. I specifically wanted to learn how WiFi connections are handled, how HTTPS APIs behave on microcontrollers, how SPI-based TFT displays are wired and configured, and how a device can continue running after power is removed and restored.

Very early on, it became clear that this project would involve far more debugging and experimentation than expected, especially because the display came without a proper datasheet.

---

## Hardware Overview

The hardware consists of an ESP32 DevKit V1 board and a 1.8-inch SPI TFT display with a resolution of 128×160 pixels. The display was purchased locally by asking for a “128×160 SPI TFT”, without any additional documentation about the driver or pin behavior.

Because the ESP32 DevKit uses pin labels like **D18, D23, D5**, etc., all wiring and configuration were done using these pin names rather than raw GPIO numbering.

---

## Initial Wiring Plan and Issues Faced

Initially, the plan was to connect all TFT pins, including RESET and LED (backlight), to ESP32 digital pins so that everything could be controlled through software. This seemed logical at first and matched many online examples.

However, shown symptoms included the screen powering on but staying completely blank. The ESP32 code was clearly running, WiFi was connecting, and serial output looked correct, but the display showed no visible output apart from a faint backlight.

The lack of a datasheet made it difficult to immediately identify the issue. After extensive testing, it became clear that the display did not require software-controlled reset and that driving the RESET pin via an ESP32 pin was actually preventing the display from initializing correctly.

---

## Final Working Wiring Configuration

The wiring was revised after experimentation, and the final configuration proved to be stable and reliable.

The TFT display is powered directly from the ESP32’s 3.3V rail. The RESET pin of the display is permanently tied to 3.3V, ensuring the display is never held in reset. The LED (backlight) pin is also tied directly to 3.3V so that the backlight remains on at all times.

SPI communication uses the ESP32’s default SPI pins, referenced using the board’s Dx-style pin names:

- SCK is connected to **D18**
- SDA (MOSI) is connected to **D23**
- CS is connected to **D5**
- DC (AO) is connected to **D2**
- RESET is connected to **3.3V**
- LED is connected to **3.3V**

Multiple connections share the 3.3V pin, which is safe in this configuration and well within the ESP32’s current limits.

This wiring change was the turning point where the display finally began responding correctly.

---

## Software and Libraries Used

The project was developed using the Arduino IDE. The TFT display is driven using the TFT_eSPI library, which required manual configuration for the specific display controller. JSON parsing is handled using ArduinoJson, and network communication is done using the ESP32’s built-in WiFi and HTTPClient libraries. Time synchronization is achieved using NTP via `time.h`.

The TFT library required careful setup, including disabling software reset and tuning SPI frequency to ensure reliable communication.

---

## API Integration and Networking Challenges

The Bitcoin price data is fetched from the CoinGecko API using the following endpoint:

CoinGecko was chosen as the data source because it provides a simple and reliable API suitable for microcontroller-based projects. This project uses a cryptocurrency price API endpoint, which can be obtained directly from CoinGecko’s official API documentation. To adapt or extend this project can refer to CoinGecko’s official website for available endpoints and usage details: https://www.coingecko.com/en/api

Several networking issues were encountered during development. Initial attempts resulted in HTTP error codes such as `-1`, which were caused by TLS certificate verification failures. This was resolved by using `WiFiClientSecure` with certificate verification disabled.

Another issue involved malformed or chunked HTTP responses, which caused JSON parsing to fail. This was solved by manually extracting the JSON body from the HTTP response string before passing it to ArduinoJson.

Serial Monitor debugging played a crucial role in identifying and fixing these issues.

---

## Display Behavior and Customization

Over multiple iterations, the display output was refined to improve clarity and usability. On startup, the screen displays WiFi connection messages such as “Connecting WiFi” and “Trying…”. Once connected, a confirmation message is shown.

During normal operation, the screen displays the Bitcoin price in USD along with the current time obtained via NTP. The text colors and font sizes were customized to make the price stand out clearly while keeping timestamps readable.

WiFi and HTTP errors are handled gracefully. If the WiFi connection drops, a red error screen is shown and the ESP32 automatically attempts to reconnect. If the API request fails, an error message is displayed on the screen.

---

## Power and Persistence

The program is stored in the ESP32’s flash memory. This means the device can be unplugged from the computer and powered again using a phone charger or power bank without losing the code. On every power-up, the program starts automatically.

This makes the project fully standalone and suitable for continuous operation.

---

## Limitations and Learning Outcome

This project is not perfect. It is not optimized for low power consumption, HTTPS certificate validation is disabled for simplicity, and the user interface can still be improved. However, it successfully achieves its core goal.

More importantly, it provided hands-on experience with real-world embedded system challenges, including undocumented hardware behavior, library configuration issues, network reliability problems, and debugging without clear error messages.

This project represents a starting point, not a final product shows how learning in embedded systems often happens: by building, breaking, and fixing.

---

## Final Note

This is my first complete ESP32 project. It is far from perfect, but it works, it taught me a lot, and it motivated me to continue exploring embedded systems.
And that, honestly, is how you start.

---

**Author:** Shrey Verma  
**Platform:** ESP32 (Arduino)
