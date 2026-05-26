# 🌡️ Temperature Monitoring System

An Arduino-based temperature monitoring system designed to display two main operational parameters on a **P10 LED Matrix (1x1)** panel:

* **SV (Set Value):** The static standard/target value (Default: 25°C).
* **PV (Process Value):** The actual real-time temperature read by the sensor.

This project offers **two sensor variants** that can be selected based on your field deployment needs: **Non-Contact (Infrared)** or **Contact-Type (Waterproof Probe)**.

---

## ✨ Key Features

* **Flicker-Free Display:** The P10 display only refreshes (clears and redraws) the specific PV area when a physical value change occurs. This eliminates screen flickering and optimizes microcontroller processing load.
* **Smart Data Filtering:** Equipped with a validation algorithm to filter out invalid spike readings caused by inductive electrical noise.
* **Fail-Safe Memory Hold:** If the sensor detects a temperature outside the safe operational range (`2°C` to `60°C`), the system automatically holds and displays the last valid state stored in memory.
* **Hardware Error Indicator:** The system will not freeze or lock up if a sensor issue occurs. Instead, it displays a specific error code on the P10 panel as a visual cue for operators.

---

## 🔌 Sensor Variants

This repository provides two source code choices based on the integrated sensor type:

### 🔹 Variant 1: Non-Contact Infrared (MLX90614)
* **Characteristics:** Measures temperature from a distance using infrared radiation without physical contact.
* **Special Implementation:** The I2C communication clock rate is manually downclocked to `50 kHz` (Noise Mitigation) to ensure stable data transmission over longer cable runs.
* **Error Indicator:** Displays `ERR-MLX` on the panel if the sensor is not detected during boot.

### 🔹 Variant 2: Waterproof Probe (DS18B20)
* **Characteristics:** Utilizes a submersible probe sensor based on the OneWire protocol. Ideal for fluid temperature tracking.
* **Special Implementation:** Implements physical wire disconnection handling via `DEVICE_DISCONNECTED_C`.
* **Error Indicator:** Displays `??` in real-time if the sensor cable gets detached or severed during operation.

---

## 🛠️ Hardware Requirements

| Component | MLX90614 Variant | DS18B20 Variant |
| :--- | :--- | :--- |
| **Microcontroller** | Arduino Uno / Nano / Mega | Arduino Uno / Nano / Mega |
| **Display** | P10 LED Matrix Panel (1x1) | P10 LED Matrix Panel (1x1) |
| **Sensor** | MLX90614 (Gy-906) | DS18B20 Waterproof Probe |
| **Additional Components**| 2x 10k Ohm Resistors | 1x 4.7k Ohm Resistor (Data Pull-up) |
| **Protocol** | I2C (SDA, SCL) | OneWire (Digital Pin 4) |

---

## 🗺️ P10 Display Layout

```text
+-------------------------------+
|  SV                  PV       |  <- Row 1: Parameter Labels
|  25                  --       |  <- Row 2: Numeric Values / Status
+-------------------------------+
