#pragma once

#include <pebble.h>

#define NUM_TOTAL_DAYS 24

struct step {
  int duration;
  bool isWalk;
};

struct day {
  int numSteps;
  struct step *steps;
  char *description;
  int dayIndex;
};

struct day* get_days_data();
void freeDays(struct day *days);