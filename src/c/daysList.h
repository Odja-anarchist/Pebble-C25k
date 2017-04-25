#pragma once
#include <pebble.h>
#include "daysList.h"
#include "daysData.h"
#include "utils.h"
#include "preActivity.h"

#define NUM_OF_SECTIONS 1

void days_list_window_create();
void days_list_show();
void days_list_destroy();