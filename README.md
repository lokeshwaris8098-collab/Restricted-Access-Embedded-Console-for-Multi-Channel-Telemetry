# Restricted Access Embedded Console for Multi-Channel Telemetry

An embedded security and telemetry monitoring system based on an **ARM7 LPC21xx microcontroller**. The project provides password-protected access to telemetry information such as **temperature and operating voltage**, using a keypad, LCD, I²C EEPROM, SPI interface, UART communication, LEDs, and motor/actuator outputs.

The system is designed to prevent unauthorized users from accessing critical telemetry data and to provide notification mechanisms when abnormal conditions are detected.

---

## 📌 Project Overview

In embedded monitoring and control systems, telemetry data can contain important operational information that should not be freely accessible.

This project implements a **restricted-access embedded console** that requires the user to authenticate before telemetry information can be accessed.

The basic workflow is:

```text
                 ┌──────────────────────┐
                 │      Power ON        │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │ Initialize Hardware  │
                 │ LCD / Keypad / I²C   │
                 │ SPI / UART           │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │ Password Entry       │
                 │ Through Keypad       │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │ Compare Password     │
                 │ With EEPROM Value    │
                 └──────────┬───────────┘
                            │
                ┌───────────┴───────────┐
                │                       │
             Correct                 Incorrect
                │                       │
                ▼                       ▼
       ┌─────────────────┐     ┌─────────────────┐
       │ Grant Access    │     │ Retry Password  │
       └────────┬────────┘     └────────┬────────┘
                │                       │
                ▼                       ▼
       ┌─────────────────┐       Maximum Attempts
       │ Select Telemetry│              │
       │ Temperature /   │              ▼
       │ Voltage         │       ┌──────────────┐
       └────────┬────────┘       │ Access Denied│
                │                └──────────────┘
                ▼
       ┌─────────────────────┐
       │ Read Telemetry via  │
       │ SPI                 │
       └──────────┬──────────┘
                  │
                  ▼
       ┌─────────────────────┐
       │ Display / Process   │
       │ Telemetry           │
       └──────────┬──────────┘
                  │
                  ▼
       ┌─────────────────────┐
       │ Notification via    │
       │ UART / GSM          │
       └─────────────────────┘
```

---

## ✨ Features

* 🔐 Password-protected telemetry access
* 🔢 6-digit numeric password entry
* ⌨️ Matrix keypad input
* 💾 Password storage using I²C EEPROM
* 🖥️ LCD-based user interface
* 🌡️ Temperature monitoring
* ⚡ Operating-voltage monitoring
* 🔌 SPI-based telemetry acquisition
* 📡 UART communication
* 📱 GSM/SMS-style alert mechanism through AT commands
* 🚨 Temperature threshold monitoring
* 💡 LED-based status indication
* ⚙️ Motor/actuator control during abnormal temperature conditions
* 🛡️ Restricted access with limited password attempts
* 📦 Precompiled `.hex` firmware included in the repository

---

## 🧰 Hardware Requirements

The source code is written for the **LPC21xx ARM7 microcontroller family** and uses the LPC21xx device header.

### Main Components

| Component             | Purpose                              |
| --------------------- | ------------------------------------ |
| LPC21xx ARM7 MCU      | Main controller                      |
| 16×2 LCD              | User interface and telemetry display |
| Matrix Keypad         | Password and menu input              |
| I²C EEPROM            | Password storage                     |
| SPI peripheral/device | Telemetry acquisition                |
| GSM module            | SMS/remote notification              |
| LEDs                  | Temperature/status indication        |
| Motor/Actuator        | High-temperature response            |
| Power supply          | System power                         |
| UART interface        | GSM/serial communication             |

---

## 🔌 Interfaces Used

### I²C

I²C is used for communication with the external EEPROM.

The firmware writes and reads a six-character password from EEPROM using the EEPROM address:

```text
0x50
```

The current implementation writes the default password:

```text
123456
```

The password is subsequently read back from EEPROM and compared against the keypad input.

> **Security note:** The current source stores the password as plaintext. For a production security system, the password should not be stored in plaintext.

---

### SPI

SPI is used to acquire telemetry values.

The firmware reads two telemetry channels:

```c
f1 = spi_read(0);
f2 = spi_read(1);
```

The first channel is treated as temperature, while the second is treated as operating voltage.

---

### UART

UART0 is used for serial communication and GSM modem control.

The UART implementation configures the LPC21xx UART using the standard UART registers and transmits/receives individual bytes.

The main application uses AT commands such as:

```text
AT
AT+CMGF=1
AT+CMGS="..."
```

to send notification messages through a connected GSM modem.

---

### Keypad

The keypad is used for:

1. Password entry
2. Telemetry selection

During authentication, keypad digits are converted to characters and displayed as `*` on the LCD so that the password is not shown directly.

---

### LCD

The LCD provides the primary user interface.

At startup, the system displays messages similar to:

```text
Restricted Access
Embedded Console
```

followed by:

```text
For MultiChannel
Telemetry
```

The LCD is subsequently used to display password prompts, authentication status, telemetry values, and operating states.

---

## 🔐 Authentication System

The authentication mechanism works as follows:

1. The microcontroller initializes the I²C interface.
2. A six-character password is stored/read from EEPROM.
3. The user is prompted to enter a password using the keypad.
4. Each entered digit is displayed as `*`.
5. The entered six-digit value is compared with the EEPROM value.
6. If the values match, access is granted.
7. If the password is incorrect, the system displays an error message and allows another attempt.
8. The current implementation uses a maximum of three password-entry cycles.

### Default Password

The current source contains:

```text
123456
```

as the EEPROM password value.

**Change this before deploying the system.**

---

## 📊 Telemetry Channels

After successful authentication, the user can select telemetry information.

The current application presents:

```text
0:temp/1:volt
```

This corresponds to:

| Key | Channel     | Description                              |
| --- | ----------- | ---------------------------------------- |
| `0` | Temperature | Reads temperature through SPI            |
| `1` | Voltage     | Reads operating-voltage data through SPI |

The source reads the corresponding SPI channels using:

```c
spi_read(0);
spi_read(1);
```

The voltage display/notification path currently uses a scaled value (`f2/10`) in the UART output path.

---

## 🌡️ Temperature Monitoring

The temperature channel contains threshold-based logic.

The current firmware evaluates the temperature approximately as follows:

```text
Temperature < 10
        │
        ▼
Low temperature state
        │
        ▼
LED indication

10 ≤ Temperature < 20
        │
        ▼
Intermediate state
        │
        ▼
LED indication

20 ≤ Temperature < 30
        │
        ▼
Higher temperature state
        │
        ▼
LED indication

Temperature ≥ 30
        │
        ▼
High-temperature condition
        │
        ├── Motor/actuator activated
        │
        └── Notification sent
```

The source switches different LED combinations based on the temperature range. When the temperature exceeds the highest threshold, the firmware activates motor outputs and sends notification messages through the UART/GSM path.

---

## 🚨 Alert and Notification System

When an abnormal temperature condition is detected, the firmware communicates with a GSM modem using UART AT commands.

The general sequence is:

```text
AT
AT+CMGF=1
AT+CMGS="<destination>"
<message>
CTRL+Z
```

The firmware uses ASCII `26` as the `CTRL+Z` character to terminate an SMS message.

The current source contains a hard-coded phone number.

**Do not use the hard-coded number in a public or production deployment. Replace it with a configurable destination.**

---

## 💡 LED Indication

Three LEDs are defined in the main application:

```c
#define led1 1<<18
#define led2 1<<19
#define led3 1<<20
```

The LEDs are controlled according to the temperature range.

Conceptually:

| Temperature Condition | LED Status            |
| --------------------- | --------------------- |
| Low                   | LED 1 indication      |
| Moderate              | LED 1 + LED 2         |
| High                  | LED 1 + LED 2 + LED 3 |
| Critical              | LEDs + motor/actuator |

The exact electrical behavior depends on the hardware wiring and whether the LEDs are active-high or active-low.

---

## ⚙️ Motor / Actuator Control

The firmware defines two motor control outputs:

```c
#define motor1 1<<21
#define motor2 1<<22
```

During a high-temperature condition, the application changes these outputs to activate the connected motor/actuator.

This can be used for applications such as:

* Cooling fan control
* Emergency actuator
* Ventilation
* Thermal protection
* Alarm mechanism

> The motor must not be connected directly to an MCU GPIO pin. Use an appropriate transistor/MOSFET driver, relay driver, or motor-driver IC with suitable protection.

---

# 📁 Repository Structure

The current repository contains the following main files:

```text
Restricted-Access-Embedded-Console-for-Multi-Channel-Telemetry/
│
├── Main.c
├── decleration.h
├── delay.h
├── keypad_i2c_define.h
├── spi.h
├── uart.c
├── uart.hex
│
└── README.md
```

### `Main.c`

Main application firmware.

Responsibilities include:

* Hardware initialization
* LCD interface
* Password authentication
* Keypad input
* EEPROM password handling
* SPI telemetry reading
* Temperature monitoring
* Voltage monitoring
* LED control
* Motor control
* GSM notification

The current file is approximately 474 lines and contains the main application logic.

---

### `decleration.h`

Contains function declarations for the keypad, LCD, I²C, and related functions.

Examples include:

```c
unsigned int keypad_scan(void);

void lcd_init(void);
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_str(unsigned char*);
void lcd_integer(int);

void i2c_init(void);
void i2c_page_write(...);
void i2c_seq_read(...);
```

---

### `delay.h`

Provides delay functionality used throughout the embedded application.

The main program uses functions such as:

```c
delay_ms(...)
```

for LCD timing, user-interface delays, UART/GSM timing, and control operations.

---

### `keypad_i2c_define.h`

Contains keypad and I²C-related definitions and implementations used by the main program.

The project uses these routines for:

* Keypad scanning
* LCD control
* I²C communication
* EEPROM access

---

### `spi.h`

Contains the SPI implementation used by the application.

The main application calls:

```c
Init_SPI0();
```

and:

```c
spi_read(0);
spi_read(1);
```

to initialize and retrieve telemetry data.

---

### `uart.c`

Provides a standalone UART example/implementation.

The UART configuration uses LPC21xx UART registers, including:

```c
PINSEL0
U0LCR
U0DLL
U0DLM
U0LSR
U0THR
U0RBR
```

The included implementation provides transmit and receive functions.

---

### `uart.hex`

Precompiled HEX firmware/output file included in the repository.

This can be programmed into a compatible target when the HEX file corresponds to the intended firmware/build configuration.

---

# 🛠️ Software Requirements

You need an ARM7/LPC21xx-compatible embedded C development environment capable of compiling code that uses:

```c
#include <lpc21xx.h>
```

A typical development setup may include:

* Keil µVision / ARM toolchain
* LPC21xx device support package
* Flash programmer
* UART terminal
* Suitable ISP/JTAG programming hardware

The exact compiler/project settings are not currently included in the repository, so they may need to be recreated for your specific LPC21xx target.

---

# 🚀 Getting Started

## 1. Clone the Repository

```bash
git clone https://github.com/lokeshwaris8098-collab/Restricted-Access-Embedded-Console-for-Multi-Channel-Telemetry.git
```

Enter the project directory:

```bash
cd Restricted-Access-Embedded-Console-for-Multi-Channel-Telemetry
```

---

## 2. Review the Hardware Configuration

Before compiling or programming the firmware, verify:

* MCU model
* Clock frequency
* LCD connections
* Keypad connections
* I²C EEPROM connections
* SPI connections
* UART pins
* GSM module interface
* LED GPIO connections
* Motor-driver connections

The source is specifically written around LPC21xx hardware registers, so the selected MCU must be compatible with the definitions used by the project.

---

## 3. Configure the Password

The current implementation writes:

```c
"123456"
```

to the EEPROM before reading it back.

For a real application, replace this with a configurable initialization process rather than embedding the password directly into the firmware.

---

## 4. Configure GSM Notification

The current application sends messages using AT commands and contains a hard-coded destination number.

Before deployment:

1. Replace the destination number.
2. Verify GSM module baud rate.
3. Verify SIM availability.
4. Verify network registration.
5. Test `AT`.
6. Test SMS mode.
7. Test SMS sending.
8. Confirm the modem uses the UART pins configured by the firmware.

---

## 5. Build the Firmware

Create an LPC21xx embedded C project and add:

```text
Main.c
decleration.h
delay.h
keypad_i2c_define.h
spi.h
uart.c
```

Configure the correct MCU device and startup code.

Then compile the project and generate the required HEX file.

---

## 6. Program the Microcontroller

Use an appropriate LPC21xx programming method to flash the generated HEX file.

After programming:

1. Power on the board.
2. Verify the LCD startup messages.
3. Verify keypad input.
4. Verify EEPROM access.
5. Test authentication.
6. Test temperature telemetry.
7. Test voltage telemetry.
8. Test LEDs.
9. Test actuator control.
10. Test GSM notification.

---

# 🧪 Testing Procedure

## Test 1 — Power-On

Expected behavior:

```text
Restricted Access
Embedded Console
```

followed by:

```text
For MultiChannel
Telemetry
```

The firmware displays these startup messages before beginning authentication.

---

## Test 2 — Correct Password

Enter:

```text
123456
```

Expected result:

```text
Correct Password
```

The system then proceeds to the telemetry selection stage.

---

## Test 3 — Incorrect Password

Enter an incorrect six-digit password.

Expected result:

```text
Wrong Password
Try Again
```

The current implementation allows multiple attempts and tracks failed attempts using an internal counter.

---

## Test 4 — Temperature Channel

Select:

```text
0
```

The firmware reads:

```c
spi_read(0)
```

and treats the resulting value as temperature.

Verify:

* LCD temperature display
* LED status
* High-temperature response
* GSM alert

---

## Test 5 — Voltage Channel

Select:

```text
1
```

The firmware reads:

```c
spi_read(1)
```

and processes the result as operating voltage.

---

## Test 6 — High Temperature

Apply a simulated temperature above the configured maximum threshold.

Expected behavior includes:

* LEDs change state
* Motor/actuator is activated
* GSM notification is generated

The current high-temperature threshold is approximately:

```text
30
```

according to the application logic.

---

# 🔒 Security Considerations

This project demonstrates access-controlled embedded telemetry, but the current implementation should **not be considered production-grade security** without additional hardening.

### Current limitations

The source currently includes:

* A plaintext/default password
* Hard-coded SMS destination
* No cryptographic password hashing
* No secure credential provisioning
* No account/user management
* No lockout delay/backoff suitable for hostile environments
* No secure boot mechanism
* No encrypted telemetry transport

### Recommended Improvements

For production deployment:

* Store a password hash rather than plaintext credentials.
* Use a secure provisioning procedure.
* Add configurable credentials.
* Add exponential lockout/backoff after failed attempts.
* Avoid hard-coded phone numbers.
* Protect firmware against unauthorized extraction.
* Use secure firmware update mechanisms.
* Add watchdog protection.
* Validate all external sensor values.
* Add bounds checking to telemetry processing.
* Protect the GSM interface against unexpected responses.
* Add tamper detection where required.

---

# ⚠️ Important Safety Notes

### Motor Control

Do not connect motors directly to LPC21xx GPIO pins.

Use:

```text
MCU GPIO
   │
   ▼
MOSFET / Motor Driver
   │
   ▼
Motor
```

with an appropriate power supply and protection circuitry.

### GSM

GSM modules can draw significant transient current. Use a suitable regulated supply and proper decoupling.

### Sensors

Make sure the voltage and signal levels of the telemetry source are compatible with the SPI interface and external circuitry.

### EEPROM

Verify EEPROM voltage levels and I²C pull-up requirements before connecting it to the MCU.

---

# 🧩 System Architecture

```text
                   ┌──────────────────────┐
                   │     LPC21xx MCU      │
                   │                      │
                   │  Authentication      │
                   │  Telemetry Logic     │
                   │  Alert Processing     │
                   └───────┬───────┬──────┘
                           │       │
             ┌─────────────┘       └──────────────┐
             │                                     │
             ▼                                     ▼
      ┌─────────────┐                      ┌─────────────┐
      │   Keypad    │                      │    LCD      │
      │             │                      │             │
      │ User Input  │                      │ User Output │
      └─────────────┘                      └─────────────┘
             │
             │ I²C
             ▼
      ┌─────────────┐
      │ EEPROM      │
      │ Password    │
      │ Storage     │
      └─────────────┘

             SPI
              │
              ▼
      ┌─────────────────┐
      │ Telemetry Source│
      │                 │
      │ CH0 Temperature │
      │ CH1 Voltage     │
      └─────────────────┘

              │
              │ UART
              ▼
      ┌─────────────────┐
      │   GSM Module    │
      │                 │
      │ SMS Notification│
      └─────────────────┘

              │
              ▼
      ┌─────────────────┐
      │ Remote Operator │
      │ / Alert Receiver│
      └─────────────────┘

       GPIO │       │ GPIO
            ▼       ▼
         ┌────┐  ┌─────────┐
         │LEDs│  │ Actuator│
         └────┘  └─────────┘
```

---

# 📋 Functional Summary

| Function             | Interface  | Purpose                      |
| -------------------- | ---------- | ---------------------------- |
| Password input       | Keypad     | Authenticate user            |
| Password storage     | I²C        | Store/retrieve credential    |
| User interface       | LCD        | Display status/data          |
| Temperature          | SPI CH0    | Monitor temperature          |
| Voltage              | SPI CH1    | Monitor operating voltage    |
| Remote notification  | UART/GSM   | Send alerts                  |
| Status indication    | GPIO/LED   | Indicate operating condition |
| Thermal response     | GPIO/Motor | Activate actuator            |
| Serial communication | UART       | Communicate with modem       |

---

# 🔄 Application Flow

```text
START
  │
  ▼
Initialize I²C
  │
  ├── Initialize LCD
  │
  ├── Initialize SPI
  │
  └── Initialize UART
  │
  ▼
Display System Information
  │
  ▼
Read Password From EEPROM
  │
  ▼
Request Password
  │
  ▼
Read Keypad
  │
  ▼
Compare Password
  │
  ├───────────────┐
  │               │
MATCH           NO MATCH
  │               │
  ▼               ▼
Grant Access    Retry
  │               │
  │               └── Maximum Attempts
  │                       │
  │                       ▼
  │                  Access Restricted
  │
  ▼
Telemetry Selection
  │
  ├──────────────┐
  │              │
  ▼              ▼
Temp           Voltage
  │              │
  ▼              ▼
SPI CH0        SPI CH1
  │              │
  ▼              ▼
Display        Process
  │              │
  ▼              ▼
Threshold      Notification
Processing     if required
  │
  ▼
LED / Motor / GSM Response
```

---

# 🐛 Known Limitations

The current repository is a compact embedded firmware project rather than a complete production-ready software package.

Some areas that may require further development include:

* Formal build/project configuration
* Hardware schematic
* Pin mapping documentation
* Detailed sensor specifications
* EEPROM configuration procedure
* Secure password management
* Configurable GSM destination
* Improved GSM response/error handling
* Robust telemetry validation
* Production-grade fault handling
* Automated tests
* Detailed API/module documentation

The GitHub repository currently has a single commit and no releases, issues, or pull requests, so this README documents the functionality observable in the current source rather than claiming features that are not present.

---

# 🔮 Future Enhancements

Possible future improvements include:

### Security

* Password hashing
* Secure credential provisioning
* Multi-user authentication
* User roles
* Failed-attempt lockout
* Tamper detection
* Secure boot

### Telemetry

* More sensor channels
* Data logging
* Min/max telemetry values
* Historical telemetry
* Graphical display
* Configurable thresholds

### Communication

* GSM status feedback
* Configurable phone numbers
* Multiple alert recipients
* MQTT support
* Wi-Fi/Ethernet connectivity
* Remote monitoring dashboard

### Reliability

* Watchdog timer
* Sensor failure detection
* GSM failure detection
* EEPROM error detection
* SPI communication validation
* Brownout recovery

### User Interface

* Menu-driven LCD interface
* Configuration menu
* Password change feature
* Alert configuration
* Sensor calibration

---

# 📚 Learning Objectives

This project can be used as a practical example for learning:

* ARM7 embedded C programming
* LPC21xx microcontroller programming
* GPIO programming
* LCD interfacing
* Keypad interfacing
* I²C communication
* EEPROM interfacing
* SPI communication
* UART communication
* GSM modem interfacing
* Sensor/telemetry monitoring
* Embedded access control
* Threshold-based control systems
* Embedded alert systems

---

# 🤝 Contributing

Contributions are welcome.

A typical contribution workflow is:

```bash
git clone https://github.com/lokeshwaris8098-collab/Restricted-Access-Embedded-Console-for-Multi-Channel-Telemetry.git

cd Restricted-Access-Embedded-Console-for-Multi-Channel-Telemetry

git checkout -b feature/my-improvement
```

Make your changes, test them on the target hardware, and commit them:

```bash
git add .
git commit -m "Add improved telemetry handling"
git push origin feature/my-improvement
```

Then open a pull request on GitHub.

---

# 📄 License

No explicit license is currently provided in the repository.

Until a license is added by the repository owner, users should **not assume that the source code is freely licensed for redistribution, modification, or commercial use**.

If you intend to make this an open-source project, consider adding an appropriate license such as:

* MIT
* Apache-2.0
* GPL-3.0

---

# 👤 Author

**Lokeshwari S**

GitHub repository:

[Restricted Access Embedded Console for Multi-Channel Telemetry](https://github.com/lokeshwaris8098-collab/Restricted-Access-Embedded-Console-for-Multi-Channel-Telemetry?utm_source=chatgpt.com)

---

# ⭐ Project Highlights

The project combines several important embedded-system concepts into a single application:

```text
             SECURITY
                │
                ▼
       ┌─────────────────┐
       │ Password Access │
       └────────┬────────┘
                │
                ▼
        ┌───────────────┐
        │ LPC21xx ARM7  │
        │ Microcontroller│
        └───────┬───────┘
                │
      ┌─────────┼─────────┐
      │         │         │
      ▼         ▼         ▼
     I²C       SPI       UART
      │         │         │
      ▼         ▼         ▼
   EEPROM    Sensors    GSM
      │         │         │
      │         ▼         ▼
      │    Telemetry    Alerts
      │
      ▼
  Credentials

                │
                ▼
       ┌─────────────────┐
       │ LCD + Keypad    │
       │ User Interface  │
       └─────────────────┘

                │
                ▼
       ┌─────────────────┐
       │ LEDs + Actuator │
       │ Safety Response │
       └─────────────────┘
```

---

## Conclusion

**Restricted Access Embedded Console for Multi-Channel Telemetry** is an ARM7-based embedded monitoring system that combines authentication, telemetry acquisition, local display, threshold-based control, and remote notification.

The current implementation demonstrates how an LPC21xx microcontroller can integrate **keypad input, LCD output, I²C EEPROM storage, SPI telemetry, UART/GSM communication, LEDs, and actuator control** into a single restricted-access console.

For academic demonstrations and embedded-systems learning, the project provides a useful foundation for exploring secure access control and real-time telemetry monitoring. For production use, the authentication, credential storage, communication, and fault-handling mechanisms should be strengthened before deployment.
