#include "preActivity.h"

static Window *s_pre_activity_window;
static TextLayer *s_pre_activity_text;
static StatusBarLayer *s_pre_activity_status_bar;
static ActionBarLayer *s_pre_activity_action_bar;
static GBitmap *s_pre_activity_menu_arrow_right;
static struct day *day;

static void pre_activity_load_status_bar(Window *window) {
  s_pre_activity_status_bar = status_bar_layer_create();
  layer_add_child(window_get_root_layer(window), status_bar_layer_get_layer(s_pre_activity_status_bar));
}

static void pre_activity_menu_select_callback(ClickRecognizerRef recognizer, void *context) {
  activity_window_show(day);
  window_stack_remove(s_pre_activity_window, false);
}

static void pre_activity_click_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, pre_activity_menu_select_callback);
}

static void pre_activity_load_action_bar(Window *window) {
  s_pre_activity_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_pre_activity_action_bar, window);
  action_bar_layer_set_click_config_provider(s_pre_activity_action_bar, pre_activity_click_provider);
  action_bar_layer_set_icon_animated(s_pre_activity_action_bar, BUTTON_ID_SELECT, s_pre_activity_menu_arrow_right, true);
}

static void pre_activity_load_activity_text(Window *window) {
  GRect bounds = get_window_bounds(window);
  s_pre_activity_text = text_layer_create(bounds);
  text_layer_set_text(s_pre_activity_text, day->description);
  text_layer_set_font(s_pre_activity_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_pre_activity_text));
}

static void pre_activity_load_menu_bitmaps() {
  s_pre_activity_menu_arrow_right = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_ARROW_RIGHT);
}

static void pre_activity_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading pre-activity window");
  pre_activity_load_menu_bitmaps();
  pre_activity_load_action_bar(window);
  pre_activity_load_status_bar(window);
  pre_activity_load_activity_text(window);
}

static void pre_activity_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloading pre-activity window");
  text_layer_destroy(s_pre_activity_text);
  status_bar_layer_destroy(s_pre_activity_status_bar);
  action_bar_layer_destroy(s_pre_activity_action_bar);
  gbitmap_destroy(s_pre_activity_menu_arrow_right);
}

void pre_activity_window_create() {
  s_pre_activity_window = window_create();
  window_set_window_handlers(s_pre_activity_window, (WindowHandlers) {
    .load = pre_activity_window_load,
    .unload = pre_activity_window_unload,
  });
}

void pre_activity_window_show(struct day *targetDay) {
  day = targetDay;
  window_stack_push(s_pre_activity_window, false);
}

void pre_activity_destroy(){
  window_destroy(s_pre_activity_window);
}
