# RISC-V core industrial-grade multi-channel ADC microcontroller CH32X315
EN | [中文](README_zh.md)



### Overview

CH32X315 is a multi-channel ADC microcontroller designed based on the Tibetan barley RISC-V core. The core supports 480MHz zero-wait operation and supports single-precision floating-point instructions and bit manipulation instructions. CH32X315 has four sets of high-speed 12-bit ADC conversion units built-in, providing 48 direct inputs
The channel supports scanning mode and can be expanded to 8 times the number of channels with an analog switch chip, which can automatically switch. It is equipped with a built-in USB 3.0 ultra-high-speed controller and PHY, a USB 2.0 high-speed controller and PHY, as well as a Type-C/PD controller and PHY. It supports USB 3.2 Gen1 and USBSS Device
Equipment functions, USBHS Host and USBHS Device functions, Type-C and PDUSB fast charging functions; provides rich peripheral resources such as DMA controller, ARGB single-wire RGB driver, multiple sets of timers, 4 sets of USART serial ports, 2 sets of I2C interfaces, 3 sets of SPI, etc.
CH32X305 has the USB 3.0 module removed from CH32X315.

### System Block Diagram
![Alt](image/frame.png)
### Features

- Qingke 32-bit RISC-V3F core, with multiple instruction set combinations
- Fast programmable interrupt controller + hardware interrupt stack
- Level 2 interrupt nesting
- Supports single-precision floating-point instructions and bit manipulation instructions
- The highest frequency of the kernel is 480MHz, and the highest frequency of the system is 313MHz
- In performance mode, the kernel supports 625MHz, and the system supports 400MHz

- 64KB volatile data storage area SRAM
- 480KB program storage area CodeFlash
(192KB zero-wait application area + 288KB non-zero-wait data area)
- 28KB system bootloader storage area for BootLoader
- 128B user-defined information storage area

- System power supply VDD range: 2.8~3.6V
- Low power consumption modes: sleep, stop

- Built-in factory-calibrated 20MHz RC oscillator
- Built-in RC oscillator with a frequency of approximately 40kHz
- High-speed oscillator supports external 5~32MHz crystal
- Power-on/off reset, programmable voltage monitor

- One 16-bit advanced timer, supporting dead zone control and emergency braking
Vehicle, providing complementary PWM output for motor control
- 2 general-purpose 16-bit timers and 1 general-purpose 32-bit timer, providing input capture
Capture/output comparison/PWM/pulse counting and incremental encoder input
- 2 watchdog timers (independent and window type)
- System time base timer: 32-bit counter

- Real-Time Clock (RTC): 32-bit independent timer

- 11 DMA channels, supporting ring buffer management

- 4 sets of 12-bit analog-to-digital converters (ADCs):
- Analog input range: VREF- to VREF+
- 4*12=48 external signal channels + 1 internal signal channel
- Sampling rate up to 5Msps, supporting multiple ADC conversion modes
- Supports scanning mode and automatic switching with external extensions
- Support for early channel switching to facilitate stable signal sampling
- Supports 4 sets of ADC cascading, equivalent to a 20Msps sampling rate

- 4 sets of USART serial ports: support LIN

- 2 sets of I2C interfaces

- 3 sets of SPI interfaces

- 5Gbps ultra-high-speed USB 3.0 controller and PHY
- Supports ultra-high-speed Device mode
- High-speed integrated design, with a measured speed of 450 Mbytes per second

- 480Mbps high-speed USB 2.0 controller and PHY:
- Supports high-speed/full-speed Host and Device modes
- Supports 1024-byte data packets

- USB PD and Type-C controller and PHY:
- Supports DRP, Sink, and Source applications, as well as PDUSB
- Supports PD3.2 and EPR, and supports 100W or 240W fast charging

- Cascadable and addressable single-wire RGB (ARGB)

- Fast GPIO port:
- 64 I/O ports, mapped to 16 external interrupts

- CRC calculation unit, 96-bit unique chip ID

- Supports both single-line and dual-line debugging modes

- Packaging type: LQFP, QFN
