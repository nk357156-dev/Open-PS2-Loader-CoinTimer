#ifndef __COIN_TIMER_INTEGRATION_H
#define __COIN_TIMER_INTEGRATION_H

#include <gsKit.h>

// Initialize coin timer system
int coinTimerSystemInit(GSGLOBAL *gsGlobal);

// Deinitialize coin timer system
void coinTimerSystemDeinit(void);

// Update and display timer (call in main render loop)
void coinTimerSystemUpdate(GSGLOBAL *gsGlobal);

#endif
