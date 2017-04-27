#include "utils.h"

GRect get_window_bounds(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  bounds.origin.y = STATUS_BAR_LAYER_HEIGHT;
  bounds.size.w = bounds.size.w - ACTION_BAR_WIDTH;
  bounds.size.h = bounds.size.h - STATUS_BAR_LAYER_HEIGHT;
  return bounds;
}

void toggle_day_complete_state(int day, bool forceCompleted) {
  if(forceCompleted) {
    persist_write_bool(day, true);
    persist_write_int(LAST_COMPLETED_DAY_MENU_INDEX, day);
  } else {
    bool day_was_completed = persist_read_bool(day);
    persist_write_bool(day, !day_was_completed);
    if(!day_was_completed) {
      persist_write_int(LAST_COMPLETED_DAY_MENU_INDEX, day);
    }
  }
}