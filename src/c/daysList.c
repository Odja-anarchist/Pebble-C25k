#include "daysList.h"

// ----------------------------
// Main Days List
// ----------------------------
static Window *s_days_list_window;
static SimpleMenuLayer *s_simple_menu_layer;
static StatusBarLayer *s_days_list_status_bar;
static SimpleMenuSection s_menu_sections[NUM_OF_SECTIONS];
static SimpleMenuItem s_menu_items[NUM_TOTAL_DAYS];
static TextLayer *s_days_list_header_layer;
static GBitmap *s_menu_icon_tick;
static GBitmap *s_menu_icon_cross;
static GBitmap *s_menu_icon_tick_invert;
static GBitmap *s_menu_icon_cross_invert;
static GBitmap *s_main_list_menu_arrow_right;
static GBitmap *s_main_list_menu_arrow_up;
static GBitmap *s_main_list_menu_arrow_down;
static ActionBarLayer *s_days_list_action_bar;

static struct day *days;
static int selectedDay;

static void days_list_menu_select_callback(ClickRecognizerRef recognizer, void *context) {
  pre_activity_window_show(&days[selectedDay]);
}

static void days_list_menu_up_callback(ClickRecognizerRef recognizer, void *context) {
  if (selectedDay > 0) {
      s_menu_items[selectedDay].icon = persist_read_bool(selectedDay) ? s_menu_icon_tick : s_menu_icon_cross,
      selectedDay = selectedDay - 1;
      s_menu_items[selectedDay].icon = persist_read_bool(selectedDay) ? s_menu_icon_tick_invert : s_menu_icon_cross_invert;
  }
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
  simple_menu_layer_set_selected_index(s_simple_menu_layer, selectedDay, true);
}

static void days_list_menu_down_callback(ClickRecognizerRef recognizer, void *context) {
  if (selectedDay < NUM_TOTAL_DAYS - 1) {
      s_menu_items[selectedDay].icon = persist_read_bool(selectedDay) ? s_menu_icon_tick : s_menu_icon_cross,
      selectedDay = selectedDay + 1;
      s_menu_items[selectedDay].icon = persist_read_bool(selectedDay) ? s_menu_icon_tick_invert : s_menu_icon_cross_invert;
  }
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
  simple_menu_layer_set_selected_index(s_simple_menu_layer, selectedDay, true);
}

static void days_list_long_press_handler(ClickRecognizerRef recognizer, void *context) {
  bool rowHasBeenCompleted = persist_read_bool(selectedDay);
  vibes_short_pulse();
  persist_write_bool(selectedDay, !rowHasBeenCompleted);
  s_menu_items[selectedDay].icon = !rowHasBeenCompleted ? s_menu_icon_tick_invert : s_menu_icon_cross_invert;
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
  vibes_short_pulse();
}

static void days_list_new_ccp(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, days_list_menu_down_callback);
  window_single_click_subscribe(BUTTON_ID_UP, days_list_menu_up_callback);
  window_single_click_subscribe(BUTTON_ID_SELECT, days_list_menu_select_callback);
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, days_list_long_press_handler, NULL);
}

static void days_list_create_menu_items() {
  int num_a_items;
  char titleBuf[] = "Week 8";
  char dayBuf[] = "Day 3";
  for( num_a_items = 0; num_a_items < NUM_TOTAL_DAYS; num_a_items = num_a_items + 1 ) {
    char *title = malloc(sizeof(titleBuf));
    char *subtitle = malloc(sizeof(dayBuf));
    snprintf(title, sizeof(titleBuf), "Week %d", (num_a_items / NUM_OF_DAYS_PER_WEEK) + 1);
    snprintf(subtitle, sizeof(dayBuf),"Day %d", (num_a_items % NUM_OF_DAYS_PER_WEEK) + 1);
    s_menu_items[num_a_items] = (SimpleMenuItem) {
      .title = title,
      .subtitle = subtitle,
      .icon = persist_read_bool(num_a_items) ? s_menu_icon_tick : s_menu_icon_cross,
    };
  }
}

static void days_list_load_bitmaps(){
  s_menu_icon_tick = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_TICK);
  s_menu_icon_cross = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_CROSS);
  s_menu_icon_tick_invert = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_TICK_INVERT);
  s_menu_icon_cross_invert = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_CROSS_INVERT);
  s_main_list_menu_arrow_right = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_ARROW_RIGHT);
  s_main_list_menu_arrow_down = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_ARROW_DOWN);
  s_main_list_menu_arrow_up = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_ARROW_UP);
}

static void days_list_load_action_bar(Window *window) {
  s_days_list_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_days_list_action_bar, window);
  action_bar_layer_set_click_config_provider(s_days_list_action_bar, days_list_new_ccp);
  action_bar_layer_set_icon_animated(s_days_list_action_bar, BUTTON_ID_SELECT, s_main_list_menu_arrow_right, true);
  action_bar_layer_set_icon_animated(s_days_list_action_bar, BUTTON_ID_UP, s_main_list_menu_arrow_up, true);
  action_bar_layer_set_icon_animated(s_days_list_action_bar, BUTTON_ID_DOWN, s_main_list_menu_arrow_down, true);
}

static void days_list_load_status_bar(Window *window) {
  s_days_list_status_bar = status_bar_layer_create();
  layer_add_child(window_get_root_layer(window), status_bar_layer_get_layer(s_days_list_status_bar));
}

static void days_list_load_header_layer(Window *window) {
  GRect bounds = get_window_bounds(window);
  bounds.size.h = S_DAYS_LIST_HEADER_HEIGHT;
  bounds.origin.y = bounds.origin.y - 5;
  s_days_list_header_layer = text_layer_create(bounds);
  text_layer_set_text_alignment(s_days_list_header_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_days_list_header_layer));
  text_layer_set_font(s_days_list_header_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(s_days_list_header_layer, "C25K");
}

static void days_list_load_days_list(Window *window) {
  GRect bounds = get_window_bounds(window);
  bounds.origin.y = bounds.origin.y + S_DAYS_LIST_HEADER_HEIGHT;
  bounds.size.h = bounds.size.h - S_DAYS_LIST_HEADER_HEIGHT;
  s_menu_sections[0] = (SimpleMenuSection) {
     .num_items = NUM_TOTAL_DAYS,
     .items = s_menu_items,
  };
  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_OF_SECTIONS, NULL);
  layer_add_child(window_get_root_layer(window), simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void days_list_window_load(Window *window) {
  days_list_load_bitmaps();
  days_list_create_menu_items();
  days_list_load_header_layer(window);
  days_list_load_status_bar(window);
  days_list_load_days_list(window);
  days_list_load_action_bar(window);
}

static void days_list_window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
  text_layer_destroy(s_days_list_header_layer);
  gbitmap_destroy(s_menu_icon_tick);
  gbitmap_destroy(s_menu_icon_cross);
  gbitmap_destroy(s_menu_icon_tick_invert);
  gbitmap_destroy(s_menu_icon_cross_invert);
  gbitmap_destroy(s_main_list_menu_arrow_right);
  gbitmap_destroy(s_main_list_menu_arrow_down);
  gbitmap_destroy(s_main_list_menu_arrow_up);
  action_bar_layer_destroy(s_days_list_action_bar);
}

static void days_list_window_appear(Window *window) {
  bool rowHasBeenCompleted = persist_read_bool(selectedDay);
  s_menu_items[selectedDay].icon = rowHasBeenCompleted ? s_menu_icon_tick_invert : s_menu_icon_cross_invert;
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void load_days_data() {
  days = get_days_data();
}

void days_list_window_create() {
  load_days_data();
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