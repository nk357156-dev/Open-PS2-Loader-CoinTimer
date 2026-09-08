#include "include/opl.h"
#include "include/coin_timer.h"
#include "include/fntsys.h"
#include "include/renderman.h"
#include <kernel.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ps2ip.h>
#include <lwip/udp.h>

// Global timer state
static coin_timer_state_t g_coin_timer = {0, 0, 0, 1};

// UDP structures
static struct udp_pcb *g_udp_pcb = NULL;
static int g_coin_timer_running = 0;

// Maximum time in seconds (15 minutes)
#define COIN_TIMER_MAX_TIME (15 * 60)

// Timer tick frequency (PS2 runs at ~294.912 MHz)
#define TIMER_TICK_FREQ 294912000ULL

// UDP receive handler
static void coinTimerUdpRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p != NULL && p->len >= 2) {
        // Read 16-bit value (number of minutes to add)
        u16_t add_time = ((u8*)p->payload)[0] | (((u8*)p->payload)[1] << 8);
        
        if (add_time > 0 && add_time <= 60) {  // Max 60 minutes per packet
            int add_seconds = add_time * 60;
            
            // If timer is not active, start it
            if (!g_coin_timer.active) {
                g_coin_timer.active = 1;
                g_coin_timer.remaining_time = add_seconds;
                g_coin_timer.start_tick = GetTimerSystemTime(0);
                LOG("COIN TIMER: Timer started with %d minutes\\n", add_time);
            } else {
                // Add time to existing timer
                g_coin_timer.remaining_time += add_seconds;
                
                // Cap at maximum time
                if (g_coin_timer.remaining_time > COIN_TIMER_MAX_TIME) {
                    g_coin_timer.remaining_time = COIN_TIMER_MAX_TIME;
                }
                
                // Recalculate start_tick to reflect new total time
                g_coin_timer.start_tick = GetTimerSystemTime(0) - (COIN_TIMER_MAX_TIME - g_coin_timer.remaining_time);
                LOG("COIN TIMER: Added %u minutes, total time: %d seconds\\n", add_time, g_coin_timer.remaining_time);
            }
        }
    }
    
    pbuf_free(p);
}

int coinTimerInit(void)
{
    LOG("COIN TIMER: Initializing UDP server on port %d\\n", COIN_TIMER_UDP_PORT);
    
    // Create UDP PCB
    g_udp_pcb = udp_new();
    if (g_udp_pcb == NULL) {
        LOG("COIN TIMER: Failed to create UDP PCB\\n");
        return -1;
    }
    
    // Bind to port
    if (udp_bind(g_udp_pcb, IP_ADDR_ANY, COIN_TIMER_UDP_PORT) != ERR_OK) {
        LOG("COIN TIMER: Failed to bind to port %d\\n", COIN_TIMER_UDP_PORT);
        udp_remove(g_udp_pcb);
        g_udp_pcb = NULL;
        return -1;
    }
    
    // Set receive callback
    udp_recv(g_udp_pcb, coinTimerUdpRecv, NULL);
    
    g_coin_timer_running = 1;
    g_coin_timer.display_enabled = 1;
    
    LOG("COIN TIMER: UDP server started successfully\\n");
    return 0;
}

void coinTimerDeinit(void)
{
    if (g_udp_pcb != NULL) {
        udp_remove(g_udp_pcb);
        g_udp_pcb = NULL;
    }
    
    g_coin_timer_running = 0;
    g_coin_timer.active = 0;
    g_coin_timer.remaining_time = 0;
    
    LOG("COIN TIMER: UDP server stopped\\n");
}

coin_timer_state_t* coinTimerGetState(void)
{
    return &g_coin_timer;
}

void coinTimerUpdate(void)
{
    if (!g_coin_timer.active) {
        return;
    }
    
    // Get current time
    u64 current_tick = GetTimerSystemTime(0);
    u64 elapsed_ticks = current_tick - g_coin_timer.start_tick;
    
    // Convert ticks to seconds
    int elapsed_seconds = elapsed_ticks / TIMER_TICK_FREQ;
    
    // Calculate remaining time
    g_coin_timer.remaining_time = COIN_TIMER_MAX_TIME - elapsed_seconds;
    
    if (g_coin_timer.remaining_time <= 0) {
        g_coin_timer.remaining_time = 0;
        g_coin_timer.active = 0;
        LOG("COIN TIMER: Time's up!\\n");
    }
}

void coinTimerGetTimeString(char *buffer, int buflen)
{
    int minutes = g_coin_timer.remaining_time / 60;
    int seconds = g_coin_timer.remaining_time % 60;
    
    snprintf(buffer, buflen, "%02d:%02d", minutes, seconds);
}

void coinTimerDisplay(void)
{
    if (!g_coin_timer.active || !g_coin_timer.display_enabled) {
        return;
    }
    
    char time_str[16];
    coinTimerGetTimeString(time_str, sizeof(time_str));
}

void coinTimerSetDisplayEnabled(int enabled)
{
    g_coin_timer.display_enabled = enabled ? 1 : 0;
}

void coinTimerAddTime(int seconds)
{
    if (seconds > 0) {
        if (!g_coin_timer.active) {
            g_coin_timer.active = 1;
            g_coin_timer.remaining_time = seconds;
            g_coin_timer.start_tick = GetTimerSystemTime(0);
            LOG("COIN TIMER: Manual timer started with %d seconds\\n", seconds);
        } else {
            g_coin_timer.remaining_time += seconds;
            
            if (g_coin_timer.remaining_time > COIN_TIMER_MAX_TIME) {
                g_coin_timer.remaining_time = COIN_TIMER_MAX_TIME;
            }
            
            g_coin_timer.start_tick = GetTimerSystemTime(0) - (COIN_TIMER_MAX_TIME - g_coin_timer.remaining_time);
            LOG("COIN TIMER: Added %d seconds, total: %d seconds\\n", seconds, g_coin_timer.remaining_time);
        }
    }
}

void coinTimerStop(void)
{
    g_coin_timer.active = 0;
    g_coin_timer.remaining_time = 0;
    LOG("COIN TIMER: Timer stopped\\n");
}
