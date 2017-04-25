#pragma once
#include <pebble.h>
#include "utils.h"
#include "daysData.h"

#define S_ACTIVITY_STEP_LAYER_HEIGHT 47
#define S_ACTIVITY_CLOCK_LAYER_HEIGHT 56
#define S_ACTIVITY_DIRECTIONS_LAYER_HEIGHT 24;

#define WARMUP_STEP_DURATION 300
#define COOLDOWN_STEP_DURATION 300

void activity_window_create();
void activity_window_show(struct day *targetDay);
void activity_window_destroy();