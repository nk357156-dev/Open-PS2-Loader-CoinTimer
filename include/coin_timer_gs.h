#ifndef __COIN_TIMER_GS_H
#define __COIN_TIMER_GS_H

#include <tamtypes.h>
#include <gsKit.h>

// Initialize GS-based rendering for coin timer
int coinTimerGSInit(GSGLOBAL *gsGlobal);

// Display timer using GS direct rendering
void coinTimerGSDisplay(GSGLOBAL *gsGlobal, int x, int y, const char *time_string);

// Clean up GS resources
void coinTimerGSCleanup(void);

#endif
