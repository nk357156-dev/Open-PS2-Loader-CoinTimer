#include "include/opl.h"
#include "include/coin_timer.h"
#include "include/coin_timer_gs.h"
#include "include/gui.h"
#include "include/renderman.h"

// Initialize coin timer system when OPL starts
int coinTimerSystemInit(GSGLOBAL *gsGlobal)
{
    LOG("COIN TIMER SYSTEM: Initializing...\\n");
    
    // Initialize UDP server for Arduino communication
    if (coinTimerInit() < 0) {
        LOG("COIN TIMER SYSTEM: Failed to initialize UDP server\\n");
        return -1;
    }
    
    // Initialize GS rendering
    if (coinTimerGSInit(gsGlobal) < 0) {
        LOG("COIN TIMER SYSTEM: Failed to initialize GS rendering\\n");
        coinTimerDeinit();
        return -1;
    }
    
    LOG("COIN TIMER SYSTEM: Successfully initialized\\n");
    return 0;
}

// Cleanup coin timer system when OPL exits
void coinTimerSystemDeinit(void)
{
    LOG("COIN TIMER SYSTEM: Deinitializing...\\n");
    coinTimerGSCleanup();
    coinTimerDeinit();
}

// Update and display timer in main loop
void coinTimerSystemUpdate(GSGLOBAL *gsGlobal)
{
    if (gsGlobal == NULL) {
        return;
    }
    
    // Update timer state
    coinTimerUpdate();
    
    // Display timer if active
    coin_timer_state_t *state = coinTimerGetState();
    if (state->active && state->display_enabled) {
        char time_str[16];
        coinTimerGetTimeString(time_str, sizeof(time_str));
        
        // Draw using GS graphics
        coinTimerGSDisplay(gsGlobal, COIN_TIMER_DISPLAY_X, COIN_TIMER_DISPLAY_Y, time_str);
    }
}
