#pragma once

#include <pebble.h>

#define SCREEN_PADDING_PIXELS 5
#define LAST_COMPLETED_DAY_MENU_INDEX 99

GRect get_window_bounds(Window *window);
void toggle_day_complete_state(int day, bool forceCompleted);