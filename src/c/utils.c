#include "utils.h"

GRect get_window_bounds(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  bounds.origin.y = STATUS_BAR_LAYER_HEIGHT;
  bounds.size.w = bounds.size.w - ACTION_BAR_WIDTH;
  bounds.size.h = bounds.size.h - STATUS_BAR_LAYER_HEIGHT;
  return bounds;
}