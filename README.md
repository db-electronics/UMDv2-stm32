# UMDv2
The Universal Mega Dumper v2 project aims to be a complete open-sourced solution for cartridge dumping and writing. It includes a very performant MCU which has a direct memory connection to a generalized databus consisting of 24 address bits and 16 data lines. Along with a dozen or so control signals, it should be sufficient to interface with most cartridge types.
## Hardware
This firmware is designed to run on [UMDv2 hardware](https://github.com/db-electronics/UMDv2-kicad).
# Tools
## IDE
This project is developped using ST's [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) which is an Eclipse-based IDE available for free on all major platforms (Debian, MacOS, Winblows).
## Programmer
An [ST-LINK/V2](https://www.st.com/content/st_com/en/products/development-tools/hardware-development-tools/hardware-development-tools-for-stm32/st-link-v2.html) JTAG programmer is required to debug and develop firmware.
# Communication Protocol
The UMDv2 enumerates over USB as a VCP (Virtual COM Port) which means it is easy to talk to the UMDv2 via any OS since COM ports are standard everywhere.
## CRC32/MPEG-2
Packets sent to the UMDv2 include a CRC32/MPEG-2 checksum. This CRC was chosen simply because the STM32 MCU used has built-in hardware which calculates this CRC very quickly. This online resource https://crccalc.com/ can be used to verify CRC32/MPEG-2 calculations.

## Send Commands / Data
When sending commands and data to the UMDv2, this format must be followed. Note that data is interpreted in little endian.
* 2 bytes - Command Word
* 2 bytes - Payload Size
* 4 bytes - CRC32/MPEG-2 (calculated on entire packet, but skip these 4 bytes obviously)
* Payload (Payload size bytes)
  * Payload can be 0 to 4K bytes

## UMDv2 Replies
The UMDv2 replies to commands in a specific structure similar to the sending format except that it omits the CRC32/MPEG-2 calculation. Receive data will follow this format:
* 2 bytes - Command acknowledge (same 2 bytes as command which is being acknowledge, error codes are also possible)
* 2 bytes - Payload Size
* Payload (Payload size bytes)
  * Payload can be 0 to 4K bytes
  
### Example 1
This byte sequence is transmitted to the UMDv2:
```
0x02 0x00 0x04 0x00 0x4C 0x51 0x8F 0x57 0x0F 0x00 0x00 0x00
```
* 0x0002 = Command
* 0x0004 = Payload size
* 0x578F514C = CRC32/MPEG-2
  * calculated on `0x02 0x00 0x04 0x00 0x0F 0x00 0x00 0x00`
* 0x0F 0x00 0x00 0x00 = Payload

The UMDv2 will respond will the following sequence:
```
0x02 0x00 0x00 0x00
```
* 0x0002 = Command being acknowleged
* 0x0000 - Payload size

### Example 2 - no payload
This byte sequence is transmitted to the UMDv2:
```
0x08 0x00 0x00 0x00 0x71 0x45 0xEE 0x3E
```
* 0x0008 = Command
* 0x0000 = Payload size
* 0x3EEE4571 = CRC32/MPEG-2
  * calculated on `0x08 0x00 0x00 0x00`

The UMDv2 will respond will the following sequence:
```
0x08 0x00 0x00 0x00
```
* 0x0002 = Command being acknowleged
* 0x0000 = Payload size

