/*
 * global.c
 *
 * Created: 2021. 03. 08. 20:50:38
 *  Author: Jedocs
 */ 

#include <global.h>
#include <atmel_start.h>

struct report event_report;
struct session session_data;
struct menu menu_structure;
struct session_temp_values session_temp;

bool ACK_received;
bool NAK_received;
uint16_t response_beep_length = 0;
uint16_t sync_beep_length = 0;
uint16_t usb_disconnected = 0;
