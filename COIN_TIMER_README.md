# Arduino Coin Timer for OPL (Open PS2 Loader)

## Overview

This modification adds support for an Arduino-based coin acceptor system that communicates with OPL via UDP over Ethernet. When coins are detected, a countdown timer appears on the PS2 screen (both in OPL menu and during gameplay) using the Graphics Synthesizer (GS) for rendering.

## Features

- ✅ UDP-based communication between Arduino and PS2
- ✅ Real-time timer display using GS graphics
- ✅ Timer overlay visible in both OPL menu and during gameplay
- ✅ Configurable coin value (minutes per coin)
- ✅ Multiple coins accumulate time (up to 15 minutes max)
- ✅ Automatic time decrement
- ✅ W5500 Ethernet module support

## Hardware Requirements

### Arduino Side:
- Arduino board (Nano, Mega, Uno, or compatible)
- W5500 Ethernet module (SPI interface)
- Coin acceptor or impulse detector
- 5V power supply for Arduino
- Network cable (Ethernet)

### PS2 Side:
- PS2 with network adapter
- OPL with this coin timer modification
- Same network as Arduino

## Configuration

Edit `arduino/coin_timer_w5500.ino`:

```cpp
IPAddress ps2_ip(192, 168, 1, 100);  // Change to your PS2's IP
const int MINUTES_PER_COIN = 5;      // Minutes to add per coin
```

## Building OPL

1. Clone: `git clone https://github.com/PROSTOgrustniy/Open-PS2-Loader-CoinTimer.git`
2. Add coin timer files to compilation
3. Call `coinTimerSystemInit(gsGlobal)` during OPL startup
4. Call `coinTimerSystemUpdate(gsGlobal)` in main render loop

## Integration Points

### In src/gui.c:

```c
// During initialization
coinTimerSystemInit(gsGlobal);

// In main loop
coinTimerSystemUpdate(gsGlobal);
```

## Timer Display

- Location: Top-right corner (750x40)
- Format: MM:SS (yellow text on black background)
- Visible: In menu and during gameplay

## Protocol

- Port: 9999 (UDP)
- Payload: 2 bytes (little-endian unsigned 16-bit = minutes)
- Range: 1-60 minutes per packet

## Troubleshooting

1. Check Arduino Ethernet connection
2. Verify PS2 IP address matches config
3. Ensure network is working
4. Check coin sensor threshold
