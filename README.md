RS485 Relay & Temperature Control with ATmega32

This project demonstrates controlling relays and monitoring temperature using an ATmega32 microcontroller over RS485 communication.
Commands are sent from a PC (via USB-to-RS485 dongle) to the microcontroller, which can toggle relays and return temperature readings from an LM35 sensor.

📦 Hardware Used

ATmega32 Microcontroller

MAX485 transceiver (RS485 interface)

USB to RS485 dongle (for PC connection)

LM35 temperature sensor

Relays (connected to PORTB pins)

12V/5V power supply

⚡ Features

RS485 communication at 9600 baud.

Supports the following commands:

relay1 on → Turns ON Relay 1

relay1 off → Turns OFF Relay 1

relay2 on → Turns ON Relay 2

relay2 off → Turns OFF Relay 2

temp? → Reads temperature from LM35 and returns value in °C

Safe RS485 driver enable/disable handling to avoid bus conflicts.

Interrupt-based UART receive.

ADC-based LM35 temperature reading.

🛠️ How It Works

PC sends commands over RS485 (via terminal or custom software).

ATmega32 receives the command through USART interrupt.

Depending on the command:

Relays toggle ON/OFF.

Temperature is read from LM35 using ADC and sent back.

Responses are sent back to the PC, confirming the action.
💻 Code Written using CodeVisionAVR.
📽️ Video YouTube video link: https://youtu.be/Go4oI4Oo0fw
https://youtube.com/shorts/swqP5hhhvvU?feature=share

