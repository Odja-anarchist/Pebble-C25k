#include "main.h"

static void init() {
  days_list_window_create();
  pre_activity_window_create();
  activity_window_create();
  days_list_show();
}

static void deinit() {
  days_list_destroy();
  pre_activity_destroy();
  activity_window_destroy();
}

int main(void) {
  locale_init();
  init();
  app_event_loop();
  deinit();
}