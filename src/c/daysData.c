#include "daysData.h"

static int dayIndex = 0;

void day_initialise(struct day* this, int numSteps, char* description, int steps[]) {
  this->numSteps = numSteps;
  this->steps = malloc(sizeof(struct day) * numSteps);
  this->description = description;
  for (int i = 0 ; i < numSteps ; i++) {
    this->steps[i] = (struct step) {.duration = steps[i], .isWalk = i % 2 == 1};
  }
  this->dayIndex = dayIndex;
  this->title = malloc(sizeof("Week 8"));
  this->subtitle = malloc(sizeof("Day 3"));
  snprintf(this->title, sizeof(this->title), "Week %d", (dayIndex / NUM_OF_DAYS_PER_WEEK) + 1);
  snprintf(this->subtitle, sizeof(this->subtitle),"Day %d", (dayIndex % NUM_OF_DAYS_PER_WEEK) + 1);
  dayIndex++;
}

// Week one
void setup_week_one(struct day *days) {
  int steps[] = {60,90,60,90,60,90,60,90,60,90,60,90,60,90,60,90};
  day_initialise(&days[0], 16, "8 x (60s Run, 90s Walk).", steps); 
  day_initialise(&days[1], 16, "8 x (60s Run, 90s Walk).", steps);
  day_initialise(&days[2], 16, "8 x (60s Run, 90s Walk).", steps);
}

// Week two
void setup_week_two(struct day *days) {
  int steps[] = {90,120,90,120,90,120,90,120,90,120,90,120};
  day_initialise(&days[3], 12, "6 x (90s Run, 2m Walk).", steps);
  day_initialise(&days[4], 12, "6 x (90s Run, 2m Walk).", steps);
  day_initialise(&days[5], 12, "6 x (90s Run, 2m Walk).", steps);
}

//Week three
void setup_week_three(struct day *days) {
  int steps[] = {90,90,180,180,90,90,180,180};
  day_initialise(&days[6], 8, "2x (90s Run, 90s Walk, 3m Run, 3m Walk).", steps);
  day_initialise(&days[7], 8, "2x (90s Run, 90s Walk, 3m Run, 3m Walk).", steps);
  day_initialise(&days[8], 8, "2x (90s Run, 90s Walk, 3m Run, 3m Walk).", steps);
}

//Week four
void setup_week_four(struct day *days) {
  int steps[] = {180,90,300,150,180,90,300};
  day_initialise(&days[9], 7, "3m Run, 90s Walk, 5m Run, 2.5m Walk, 3m Run, 90s Walk, 5m Run.", steps);
  day_initialise(&days[10], 7, "3m Run, 90s Walk, 5m Run, 2.5m Walk, 3m Run, 90s Walk, 5m Run.", steps);
  day_initialise(&days[11], 7, "3m Run, 90s Walk, 5m Run, 2.5m Walk, 3m Run, 90s Walk, 5m Run.", steps);
}

//Week five
void setup_week_five(struct day *days) {
  int day_one_steps[] = {300,180,300,180,300};
  day_initialise(&days[12], 5, "5m Run, 3m Walk, 5m Run, 3m Walk, 5m Run.", day_one_steps);
  int day_two_steps[] = {480,300,480};
  day_initialise(&days[13], 3, "8m Run, 5m Walk, 8m Run.", day_two_steps);
  int day_three_steps[] = {1200};
  day_initialise(&days[14], 1, "20m Run.", day_three_steps);
}

// Week six
void setup_week_six(struct day *days) {
  int day_one_steps[] = {300,180,480,180,300};
  day_initialise(&days[15], 5, "5m Run, 3m Walk, 8m Run, 3m Walk, 5m Run.", day_one_steps);
  int day_two_steps[] = {600,180,600};
  day_initialise(&days[16], 3, "10m Run, 3m Walk, 10m Run.", day_two_steps);
  int day_three_steps[] = {1320};
  day_initialise(&days[17], 1, "22m Run.", day_three_steps);
}

// Week seven
void setup_week_seven(struct day *days) {
  int steps[] = {1500};
  day_initialise(&days[18], 1, "25m Run.", steps);
  day_initialise(&days[19], 1, "25m Run.", steps);
  day_initialise(&days[20], 1, "25m Run.", steps);
}

// Week eight
void setup_week_eight(struct day *days) {
  int steps[] = {1680};
  day_initialise(&days[21], 1, "28m Run.", steps);
  day_initialise(&days[22], 1, "28m Run.", steps);
  day_initialise(&days[23], 1, "30m Run.", (int[]){1800});
}

struct day* get_days_data() {
  struct day *days = malloc(sizeof(struct day) * NUM_TOTAL_DAYS);
  setup_week_one(days);
  setup_week_two(days);
  setup_week_three(days);
  setup_week_four(days);
  setup_week_five(days);
  setup_week_six(days);
  setup_week_seven(days);
  setup_week_eight(days);
  return days;
}

void freeDays(struct day *days) {
  for(int counter = 0 ; counter<NUM_TOTAL_DAYS ; counter++) {
    struct day target_day = days[counter];
    free(target_day.steps);
    free(target_day.title);
    free(target_day.subtitle);
  }
  free(days);
}