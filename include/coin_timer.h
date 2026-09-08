#ifndef __COIN_TIMER_H
#define __COIN_TIMER_H

#include <tamtypes.h>
#include "renderman.h"

// UDP port for receiving coin commands from Arduino
#define COIN_TIMER_UDP_PORT 9999

// Display settings
#define COIN_TIMER_DISPLAY_X 750
#define COIN_TIMER_DISPLAY_Y 40
#define COIN_TIMER_FONT_SIZE 2
#define COIN_TIMER_COLOR 0xFFFFFF00  // Yellow RGBA

// Coin timer state
typedef struct {
    int active;                    // 1 if timer is active
    int remaining_time;            // remaining time in seconds
    u64 start_tick;                // timer start time
    int display_enabled;           // 1 to display on screen
} coin_timer_state_t;

// Initialize UDP server for receiving coin commands
int coinTimerInit(void);

// Deinitialize UDP server
void coinTimerDeinit(void);

// Get current timer state
coin_timer_state_t* coinTimerGetState(void);

// Update timer (call every frame)
void coinTimerUpdate(void);

// Display timer on screen using GS graphics
void coinTimerDisplay(void);

// Get remaining time as string MM:SS
void coinTimerGetTimeString(char *buffer, int buflen);

// Enable/disable display
void coinTimerSetDisplayEnabled(int enabled);

// Manually add time to timer
void coinTimerAddTime(int seconds);

// Stop timer
void coinTimerStop(void);

#endif
