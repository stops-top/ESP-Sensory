#pragma once

/* 
| MOD1 | 38 | 39 | 40 | 41 | GND | 3V3 |
| MOD1 | 42 | 21 | 19 | 20 | GND | 3V3 |
| MOD2 | 9  | 43 | 44 | 14 | GND | 3V3 |
| MOD2 | 10 | 11 | 13 | 12 | GND | 3V3 |
*/

/* jtag */
#define PMOD_TDI        CONFIG_BRIDGE_MOD_TDI
#define PMOD_TDO        CONFIG_BRIDGE_MOD_TDO
#define PMOD_TCK        CONFIG_BRIDGE_MOD_TCK
#define PMOD_TMS        CONFIG_BRIDGE_MOD_TMS

/* serial */
#define PMOD_RST1       CONFIG_BRIDGE_MOD_RST1
#define PMOD_BOT1       CONFIG_BRIDGE_MOD_BOOT1
#define PMOD_RXD1       CONFIG_BRIDGE_MOD_RX1
#define PMOD_TXD1       CONFIG_BRIDGE_MOD_TX1
