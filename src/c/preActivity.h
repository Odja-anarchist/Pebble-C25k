#pragma once
#include <pebble.h>
#include "utils.h"
#include "daysData.h"
#include "activity.h"

void pre_activity_window_create();
void pre_activity_window_show(struct day *targetDay);
void pre_activity_destroy();