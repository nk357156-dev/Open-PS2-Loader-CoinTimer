#include "include/coin_timer_gs.h"
#include "include/opl.h"
#include <gsKit.h>
#include <gsToolkit.h>
#include <stdio.h>
#include <string.h>

static GSGLOBAL *g_gsGlobal = NULL;

int coinTimerGSInit(GSGLOBAL *gsGlobal)
{
    if (gsGlobal == NULL) {
        LOG("COIN TIMER GS: Error - gsGlobal is NULL\\n");
        return -1;
    }
    
    g_gsGlobal = gsGlobal;
    LOG("COIN TIMER GS: Initialized with screen resolution %dx%d\\n", gsGlobal->Width, gsGlobal->Height);
    
    return 0;
}

void coinTimerGSDisplay(GSGLOBAL *gsGlobal, int x, int y, const char *time_string)
{
    if (gsGlobal == NULL || time_string == NULL) {
        return;
    }
    
    // Draw semi-transparent background box for better readability
    // Box dimensions for timer display
    int box_width = 150;
    int box_height = 60;
    int box_x = x - 20;
    int box_y = y - 10;
    
    // Draw black semi-transparent rectangle for background
    u64 color_bg = GS_SETREG_RGBA(0, 0, 0, 128);  // Black with 50% alpha
    gsKit_prim_sprite_textured(gsGlobal,
                               box_x, box_y,
                               box_x + box_width, box_y + box_height,
                               2, color_bg);
    
    // Draw yellow text for timer
    // Timer format: MM:SS in large font
    u64 color_text = GS_SETREG_RGBA(255, 255, 0, 255);  // Yellow
    
    // Use gsKit's text rendering with large font
    gsKit_print(gsGlobal, gsFont, x, y, 2, color_text, time_string);
}

void coinTimerGSCleanup(void)
{
    g_gsGlobal = NULL;
    LOG("COIN TIMER GS: Cleaned up\\n");
}
