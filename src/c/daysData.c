#include "daysData.h"

static int dayIndex = 0;

static void day_initialise(struct day* this, int numSteps, char* description, int steps[]) {
  this->numSteps = numSteps;
  this->steps = malloc(sizeof(struct day) * numSteps);
  this->description = description;
  for (int i = 0 ; i < numSteps ; i++) {
    this->steps[i] = (struct step) {.duration = steps[i], .isWalk = i % 2 == 1};
  }
  this->dayIndex = dayIndex;
  this->title = malloc(strlen(_("Week %d")));
  this->subtitle = malloc(strlen(_("Day %d")));
  snprintf(this->title, SIZE_OF_WEEK_STRING, _("Week %d"), (dayIndex / NUM_OF_DAYS_PER_WEEK) + 1);
  snprintf(this->subtitle, SIZE_OF_DAY_STRING,_("Day %d"), (dayIndex % NUM_OF_DAYS_PER_WEEK) + 1);
  dayIndex++;
}

static void setup_copy_week(struct day *days, int steps[], char *description, int numSteps, int week_number) {
  for(int i = 0 ; i < NUM_OF_DAYS_PER_WEEK ; i++) {
    int dayIndex = i + ((week_number - 1) * 3);
    day_initialise(&days[dayIndex], numSteps, description, steps); 
  }
}

// Week one
static void setup_week_one(struct day *days) {
  int steps[] = {60,90,60,90,60,90,60,90,60,90,60,90,60,90,60,90};
  char *description = _("8 x (60s Run, 90s Walk).");
  setup_copy_week(days, steps, description, 16, 1);
}

// Week two
static void setup_week_two(struct day *days) {
  int steps[] = {90,120,90,120,90,120,90,120,90,120,90,120};
  char *description = _("6 x (90s Run, 2m Walk).");
  setup_copy_week(days, steps, description, 12, 2);
}

//Week three
static void setup_week_three(struct day *days) {
  int steps[] = {90,90,180,180,90,90,180,180};
  char *description = _("2x (90s Run, 90s Walk, 3m Run, 3m Walk).");
  setup_copy_week(days, steps, description, 8, 3);
}

//Week four
static void setup_week_four(struct day *days) {
  int steps[] = {180,90,300,150,180,90,300};
  char *description = _("3m Run, 90s Walk, 5m Run, 2.5m Walk, 3m Run, 90s Walk, 5m Run.");
  setup_copy_week(days, steps, description, 7, 4);
}

//Week five
static void setup_week_five(struct day *days) {
  int day_one_steps[] = {300,180,300,180,300};
  day_initialise(&days[12], 5, "5m Run, 3m Walk, 5m Run, 3m Walk, 5m Run.", day_one_steps);
  int day_two_steps[] = {480,300,480};
  day_initialise(&days[13], 3, "8m Run, 5m Walk, 8m Run.", day_two_steps);
  int day_three_steps[] = {1200};
  day_initialise(&days[14], 1, "20m Run.", day_three_steps);
}

// Week six
static void setup_week_six(struct day *days) {
  int day_one_steps[] = {300,180,480,180,300};
  day_initialise(&days[15], 5, _("5m Run, 3m Walk, 8m Run, 3m Walk, 5m Run."), day_one_steps);
  int day_two_steps[] = {600,180,600};
  day_initialise(&days[16], 3, _("10m Run, 3m Walk, 10m Run."), day_two_steps);
  int day_three_steps[] = {1320};
  day_initialise(&days[17], 1, _("22m Run."), day_three_steps);
}

// Week seven
static void setup_week_seven(struct day *days) {
  int steps[] = {1500};
  char *description = _("25m Run.");
  setup_copy_week(days, steps, description, 1, 7);
}

// Week eight
static void setup_week_eight(struct day *days) {
  int steps[] = {1680};
  day_initialise(&days[21], 1, _("28m Run."), steps);
  day_initialise(&days[22], 1, _("28m Run."), steps);
  day_initialise(&days[23], 1, _("30m Run."), (int[]){1800});
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