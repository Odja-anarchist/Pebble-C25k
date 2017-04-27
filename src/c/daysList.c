#include "daysList.h"

// ----------------------------
// Main Days List
// ----------------------------
static Window *s_days_list_window;
static StatusBarLayer *s_days_list_status_bar;
static ActionBarLayer *s_days_list_action_bar;
static MenuLayer *s_days_list_menu_layer;
static GBitmap *s_menu_icon_tick;
static GBitmap *s_menu_icon_cross;
static GBitmap *s_menu_icon_tick_invert;
static GBitmap *s_menu_icon_cross_invert;
static GBitmap *s_main_list_menu_arrow_right;
static GBitmap *s_main_list_menu_arrow_up;
static GBitmap *s_main_list_menu_arrow_down;

static struct day *days;
static AppTimer *timer;

static void days_list_load_status_bar(Window *window) {
  s_days_list_status_bar = status_bar_layer_create();
  layer_add_child(window_get_root_layer(window), status_bar_layer_get_layer(s_days_list_status_bar));
}

// Clickprovider callbacks

static void days_list_select_callback(ClickRecognizerRef recognizer, void* context) {
  MenuIndex selected = menu_layer_get_selected_index(s_days_list_menu_layer);
  pre_activity_window_show(&days[selected.row]);
}

static void days_list_long_select_callback(ClickRecognizerRef recognizer, void* context) {
  MenuIndex selected = menu_layer_get_selected_index(s_days_list_menu_layer);
  vibes_short_pulse();
  toggle_day_complete_state(selected.row, false);
  menu_layer_reload_data(s_days_list_menu_layer);
}

static void days_list_up_callback(ClickRecognizerRef recognizer, void* context) {
  menu_layer_set_selected_next(s_days_list_menu_layer, true, MenuRowAlignCenter, true);
}

static void days_list_down_callback(ClickRecognizerRef recognizer, void* contex) {
  menu_layer_set_selected_next(s_days_list_menu_layer, false, MenuRowAlignCenter, true);
}

static void up_timer_listener(void *data) {
  menu_layer_set_selected_next(s_days_list_menu_layer, true, MenuRowAlignCenter, true);
  timer = app_timer_register(LONG_PRESS_TIMER_PERIOD, up_timer_listener, NULL);
}

static void down_timer_listener(void *data) {
  menu_layer_set_selected_next(s_days_list_menu_layer, false, MenuRowAlignCenter, true);
  timer = app_timer_register(LONG_PRESS_TIMER_PERIOD, down_timer_listener, NULL);
}

static void clear_timer() {
  if(timer != NULL) {
    app_timer_cancel(timer);
    timer = NULL;
  }
}

static void days_list_long_up_callback(ClickRecognizerRef recognizer, void* context) {
  clear_timer();
}

static void days_list_up_long_down_callback(ClickRecognizerRef recognizer, void* context) {
  menu_layer_set_selected_next(s_days_list_menu_layer, true, MenuRowAlignCenter, true);
  timer = app_timer_register(LONG_PRESS_TIMER_PERIOD, up_timer_listener, NULL);
}

static void days_list_down_long_down_callback(ClickRecognizerRef recognizer, void* context) {
  menu_layer_set_selected_next(s_days_list_menu_layer, false, MenuRowAlignCenter, true);
  timer = app_timer_register(LONG_PRESS_TIMER_PERIOD, down_timer_listener, NULL);
}

// End clickprovider callbacks

static void days_list_new_ccp(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, days_list_select_callback);
  window_long_click_subscribe(BUTTON_ID_SELECT, SELECT_LONG_PRESS_THRESHOLD, days_list_long_select_callback, NULL);
  window_single_click_subscribe(BUTTON_ID_UP, days_list_up_callback);
  window_long_click_subscribe(BUTTON_ID_UP, DIRECTION_LONG_PRESS_THRESHOLD, days_list_up_long_down_callback, days_list_long_up_callback);
  window_single_click_subscribe(BUTTON_ID_DOWN, days_list_down_callback);
  window_long_click_subscribe(BUTTON_ID_DOWN, DIRECTION_LONG_PRESS_THRESHOLD, days_list_down_long_down_callback, days_list_long_up_callback);
}

static void days_list_load_action_bar(Window *window) {
  s_days_list_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_days_list_action_bar, window);
  action_bar_layer_set_click_config_provider(s_days_list_action_bar, days_list_new_ccp);
  action_bar_layer_set_icon_animated(s_days_list_action_bar, BUTTON_ID_SELECT, s_main_list_menu_arrow_right, true);
  action_bar_layer_set_icon_animated(s_days_list_action_bar, BUTTON_ID_UP, s_main_list_menu_arrow_up, true);
  action_bar_layer_set_icon_animated(s_days_list_action_bar, BUTTON_ID_DOWN, s_main_list_menu_arrow_down, true);
}

// Menu Callbacks

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_OF_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_TOTAL_DAYS;
}

static GBitmap* get_menu_row_icon(bool row_is_highlighted, bool row_is_completed) {
  if(row_is_highlighted) {
    return row_is_completed ? s_menu_icon_tick_invert : s_menu_icon_cross_invert;
  } else {
    return row_is_completed ? s_menu_icon_tick : s_menu_icon_cross;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  struct day targetDay = days[cell_index->row];
  bool row_is_highlighted = menu_cell_layer_is_highlighted(cell_layer);
  bool row_is_completed = persist_read_bool(targetDay.dayIndex);
  menu_cell_basic_draw(ctx, cell_layer, targetDay.title, targetDay.subtitle, get_menu_row_icon(row_is_highlighted, row_is_completed));
}

// End Menu callbacks

static void days_list_load_menu(Window *window) {
  GRect bounds = get_window_bounds(window);
  s_days_list_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_days_list_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
  });
  menu_layer_set_click_config_onto_window(s_days_list_menu_layer, window);
  layer_add_child(window_get_root_layer(window), menu_layer_get_layer(s_days_list_menu_layer));
}

static void days_list_load_bitmaps() {
  s_menu_icon_tick = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_TICK);
  s_menu_icon_cross = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_CROSS);
  s_menu_icon_tick_invert = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_TICK_INVERT);
  s_menu_icon_cross_invert = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_CROSS_INVERT);
  s_main_list_menu_arrow_right = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_ARROW_RIGHT);
  s_main_list_menu_arrow_down = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_ARROW_DOWN);
  s_main_list_menu_arrow_up = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_ARROW_UP);
}

static void days_list_unload_bitmaps() {
  gbitmap_destroy(s_menu_icon_tick);
  gbitmap_destroy(s_menu_icon_cross);
  gbitmap_destroy(s_menu_icon_tick_invert);
  gbitmap_destroy(s_menu_icon_cross_invert);
  gbitmap_destroy(s_main_list_menu_arrow_right);
  gbitmap_destroy(s_main_list_menu_arrow_down);
  gbitmap_destroy(s_main_list_menu_arrow_up);
}

static void days_list_set_selected_to_last_completed() {
  int selected = persist_read_int(LAST_COMPLETED_DAY_MENU_INDEX);
  struct MenuIndex targetIndex = {
    .section = 0,
    .row = selected
  };
  menu_layer_set_selected_index(s_days_list_menu_layer, targetIndex, MenuRowAlignCenter, false);
}

static void days_list_window_load(Window *window) {
  days_list_load_bitmaps();
  days_list_load_status_bar(window);
  days_list_load_menu(window);
  days_list_load_action_bar(window);
  days_list_set_selected_to_last_completed();
}

static void days_list_window_unload(Window *window) {
  action_bar_layer_remove_from_window(s_days_list_action_bar);
  action_bar_layer_destroy(s_days_list_action_bar);
  status_bar_layer_destroy(s_days_list_status_bar);
  menu_layer_destroy(s_days_list_menu_layer);
  days_list_unload_bitmaps();
}

static void days_list_window_appear(Window *window) {
  menu_layer_reload_data(s_days_list_menu_layer);
}

void days_list_window_create() {
  days = get_days_data();
  s_days_list_window = window_create();
  window_set_window_handlers(s_days_list_window, (WindowHandlers) {
    .load = days_list_window_load,
    .unload = days_list_window_unload,
    .appear = days_list_window_appear,
  });
}

void days_list_show() {
  window_stack_push(s_days_list_window, false);
}

void days_list_destroy() {
  window_destroy(s_days_list_window);
  freeDays(days);
}