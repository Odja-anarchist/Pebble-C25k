#pragma once
#include <pebble.h>
#include "daysList.h"
#include "daysData.h"
#include "utils.h"
#include "preActivity.h"

#define NUM_OF_SECTIONS 1
#define LONG_PRESS_TIMER_PERIOD 250
#define SELECT_LONG_PRESS_THRESHOLD 700
#define DIRECTION_LONG_PRESS_THRESHOLD 400

#define LAST_COMPLETED_DAY_MENU_INDEX 99

void days_list_window_create();
void days_list_show();
void days_list_destroy();