# Environmental Monitoring Station using STM32 Nucleo-F446RE

## Project Overview

This project is an embedded environmental monitoring system developed using the **STM32 Nucleo-F446RE** microcontroller board. The system measures **temperature** and **atmospheric pressure** using a **BMP280 sensor** through **I2C communication**.

The measured values are displayed on a **20x4 I2C LCD** and also transmitted to a computer using **UART serial communication**. A **buzzer connected to PB0** turns ON when the temperature goes above **33°C**. A Python script receives the UART data, plots live temperature and pressure graphs, and logs the sensor readings into a CSV file.

---

## Features

- Temperature measurement using BMP280
- Pressure measurement using BMP280
- I2C communication for BMP280 sensor
- I2C communication for 20x4 LCD display
- UART serial communication with PC
- Buzzer alert system using GPIO PB0
- LCD alert message when temperature exceeds 33°C
- Python live graph visualization
- CSV data logging
- Dynamic Y-axis scaling for better graph readability

---

## Required Components

| Component | Quantity |
|---|---:|
| STM32 Nucleo-F446RE | 1 |
| BMP280 Temperature and Pressure Sensor | 1 |
| 20x4 I2C LCD Display | 1 |
| Active Buzzer | 1 |
| Breadboard | 1 |
| Jumper Wires | As needed |
| USB Cable | 1 |
| Computer/Laptop | 1 |

---

## Software and Tools Used

- STM32CubeIDE
- STM32 HAL Library
- Python
- PySerial
- Matplotlib
- PuTTY
- Git
- GitHub

---

## Project Folder Structure

```text
Environmental-Monitoring-Station-STM32/
│
├── STM32_Firmware/
│   ├── Core/
│   ├── Drivers/
│   └── WEATHER_MONITORING_SYSTEM.ioc
│
├── Python_Live_Plot/
│   ├── live_bmp280_plot.py
│   └── requirements.txt
│
├── Documentation/
│   ├── circuit_diagram.png
│   ├── prototype_photo.jpg
│   ├── presentation.pptx
│   └── report.pdf
│
├── README.md
├── .gitignore
└── LICENSE
```

---

## Hardware Connections

### BMP280 Sensor Connection

| BMP280 Pin | STM32 Nucleo-F446RE |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SCL | I2C SCL |
| SDA | I2C SDA |

### 20x4 I2C LCD Connection

| LCD Pin | STM32 Nucleo-F446RE |
|---|---|
| VCC | 5V or 3.3V |
| GND | GND |
| SCL | I2C SCL |
| SDA | I2C SDA |

### Buzzer Connection

| Buzzer Pin | STM32 Nucleo-F446RE |
|---|---|
| Positive | PB0 |
| Negative | GND |

---

## STM32 Peripheral Configuration

The following peripherals are used in STM32CubeIDE:

| Peripheral | Purpose |
|---|---|
| I2C1 | Communication with BMP280 and I2C LCD |
| USART2 | UART communication with PC |
| GPIO PB0 | Buzzer output control |

### UART Settings

| Setting | Value |
|---|---|
| Baud Rate | 115200 |
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |
| Flow Control | None |

---

## STM32 Firmware Description

The STM32 firmware reads temperature and pressure data from the BMP280 sensor every 2 seconds. The measured values are displayed on the LCD and transmitted to the PC through UART.

The buzzer is controlled using GPIO pin **PB0**. When the temperature goes above **33°C**, the buzzer turns ON and the LCD displays an alert message. When the temperature goes below the threshold, the buzzer turns OFF and the LCD shows normal status.

---

## Alert Logic

```c
if (temperature > 33°C)
{
    buzzer ON;
    LCD shows alert;
}
else
{
    buzzer OFF;
    LCD shows normal status;
}
```

---

## UART Output Format

The STM32 sends data to the computer in CSV-style format:

```text
temperature,pressure,alarm
```

Example output:

```text
32.45,1009.30,OFF
33.25,1009.28,ON
```

Where:

- `temperature` is measured in degree Celsius
- `pressure` is measured in hPa
- `alarm` shows buzzer status: `ON` or `OFF`

---

## Python Live Plot

The Python script receives UART data from the STM32 board and displays live graphs for temperature and pressure.

The script also saves the readings into a CSV file for later analysis.

### Python Script Features

- Reads serial data from STM32
- Parses temperature, pressure, and alarm status
- Displays live temperature graph
- Displays live pressure graph
- Shows live buzzer/alert status
- Saves sensor readings into a CSV file
- Uses dynamic Y-axis scaling for better visualization

---

## Python Requirements

The Python script requires the following packages:

```text
pyserial
matplotlib
```

Create a file named `requirements.txt` inside the `Python_Live_Plot` folder and add:

```text
pyserial
matplotlib
```

---

## Installing Python Requirements

Open terminal inside the main project folder and run:

```bash
pip install -r Python_Live_Plot/requirements.txt
```

If you are using a virtual environment, activate the environment first, then install the requirements.

---

## Running the Python Script

Before running the Python script, check the STM32 COM port from Windows Device Manager.

Then open the Python file and update the COM port:

```python
PORT = "COM3"
```

Change `COM3` according to your computer.

Then run:

```bash
python Python_Live_Plot/live_bmp280_plot.py
```

Important: Close PuTTY before running the Python script because only one program can use the COM port at a time.

---

## How to Run the Complete Project

1. Open the STM32 firmware project in STM32CubeIDE.
2. Configure I2C, UART, and PB0 GPIO output if needed.
3. Build the STM32 project.
4. Flash the code to the STM32 Nucleo-F446RE board.
5. Connect the BMP280 sensor, I2C LCD, and buzzer according to the circuit diagram.
6. Connect the STM32 board to the computer using USB.
7. Open PuTTY or run the Python live plotting script.
8. Set the baud rate to `115200`.
9. Observe temperature, pressure, and alarm status.
10. Heat the BMP280 sensor slightly to test the buzzer alert.
11. When the temperature goes above 33°C, the buzzer turns ON and the LCD displays an alert message.

---

## Expected LCD Output

### Normal Condition

```text
Env Monitor System
Temp : 32.45 C
Press: 1009.30 hPa
Status: Normal
```

### Alert Condition

```text
Env Monitor System
Temp : 33.25 C
Press: 1009.28 hPa
ALERT: TEMP > 33C
```

---

## Expected UART Output

```text
32.45,1009.30,OFF
33.25,1009.28,ON
```

---

## Expected Python Output

The Python script displays:

- Live temperature graph
- Live pressure graph
- Minimum and maximum values
- Moving average line
- Alert message when buzzer is ON
- CSV log file with sensor readings

Example terminal output:

```text
Connected to COM3 at 115200 baud
Received: 32.45,1009.30,OFF
Received: 33.25,1009.28,ON
```

---

## CSV Log Format

The Python script creates a CSV file named:

```text
bmp280_log.csv
```

The CSV file contains:

```text
pc_time,temperature_c,pressure_hpa,alarm
```

Example:

```text
2026-07-15 22:10:05,32.45,1009.30,OFF
2026-07-15 22:10:07,33.25,1009.28,ON
```

---

## Applications

- Indoor environmental monitoring
- Weather monitoring prototype
- Temperature-based alert system
- Embedded systems laboratory project
- Sensor interfacing practice
- I2C communication demonstration
- UART communication demonstration
- Real-time data visualization
- CSV-based data logging

---

## Learning Outcomes

Through this project, the following embedded systems concepts were practiced:

- STM32 GPIO control
- I2C sensor interfacing
- I2C LCD interfacing
- UART serial communication
- BMP280 sensor data acquisition
- Temperature and pressure monitoring
- Threshold-based control system
- Buzzer alert implementation
- Python-based live data visualization
- CSV data logging
- Debugging using PuTTY and serial output

---

## Troubleshooting

### Python Graph Shows No Values

Possible solutions:

- Check the correct COM port.
- Close PuTTY before running Python.
- Make sure the baud rate is `115200`.
- Make sure STM32 is sending UART data.
- Check that `pyserial` is installed.
- Make sure the UART output format is correct.

### COM Port Not Found

Possible solutions:

- Open Windows Device Manager.
- Check under `Ports (COM & LPT)`.
- Look for `STMicroelectronics STLink Virtual COM Port`.
- Try unplugging and reconnecting the STM32 board.
- Try another USB cable.
- Avoid using a USB charging-only cable.

### LCD Not Showing Text

Possible solutions:

- Check SDA and SCL wiring.
- Check LCD power connection.
- Check LCD I2C address, usually `0x27` or `0x3F`.
- Adjust the LCD contrast screw.
- Make sure I2C is enabled in STM32CubeIDE.

### BMP280 Not Detected

Possible solutions:

- Check VCC and GND.
- Check SDA and SCL connections.
- Make sure the sensor is powered with 3.3V.
- Try BMP280 I2C address `0x76` or `0x77`.
- Check soldering or jumper wire connection.

### Buzzer Not Working

Possible solutions:

- Make sure the buzzer positive pin is connected to PB0.
- Make sure the buzzer negative pin is connected to GND.
- Make sure PB0 is configured as GPIO output.
- Use an active buzzer.
- Test the buzzer separately with simple GPIO ON/OFF code.

---

## GitHub Upload Notes

The repository should include important source code and documentation files.

Recommended files to upload:

```text
Core/
Drivers/
.ioc file
Python_Live_Plot/
Documentation/
README.md
.gitignore
LICENSE
```

Files that should not be uploaded:

```text
Debug/
Release/
.venv/
*.elf
*.o
*.map
*.bin
*.hex
*.csv
```

These unnecessary files can be ignored using the `.gitignore` file.

---

## Project Status

The project successfully measures temperature and pressure using the BMP280 sensor. The values are displayed on the 20x4 I2C LCD and transmitted to the PC through UART. The Python script receives the data, displays live graphs, and logs the readings into a CSV file. The buzzer alert system turns ON when the temperature exceeds 33°C.

---

## Conclusion

This project demonstrates a complete embedded monitoring system using STM32. It combines sensor interfacing, I2C communication, UART communication, LCD display, GPIO control, alert logic, and Python-based live data visualization. The system can be used as a basic environmental monitoring prototype and as a practical embedded systems laboratory project.

---

## Author

````
MD ABDUR RABBI  
Environmental Monitoring Station using STM32 Nucleo-F446RE
````
