#include "activity.h"

static Window *s_activity_window;
static StatusBarLayer *s_activity_status_bar;
static ActionBarLayer *s_activity_action_bar;
static GBitmap *s_activity_menu_play;
static GBitmap *s_activity_menu_next;
static GBitmap *s_activity_menu_previous;
static GBitmap *s_activity_menu_pause;
static TextLayer *s_activity_step_layer;
static TextLayer *s_activity_clock_layer;
static TextLayer *s_activity_directions_layer;

static int currentStepIndex = 0;
static int timeSpentInCurrentStep = 0;
static bool isPaused = false;
static struct day *day;
static AppTimer *timer;
  
static bool isCurrentlyInWarmupStep() {
  return currentStepIndex == 0;
}

static bool isCurrentlyInCooldownStep() {
  int totalSteps = day->numSteps + 3;
  return currentStepIndex == totalSteps - 2;
}

static bool isCurrentlyInCompleteStep() {
  int totalSteps = day->numSteps + 3;
  return currentStepIndex == totalSteps -1;
}

static bool isWithinTimeFrameToJumpToPreviousStep() {
  return timeSpentInCurrentStep < 10;
}

static void activity_load_status_bar(Window *window) {
  s_activity_status_bar = status_bar_layer_create();
  layer_add_child(window_get_root_layer(window), status_bar_layer_get_layer(s_activity_status_bar));
}

static void activity_load_bitmaps() {
  s_activity_menu_play = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_PLAY);
  s_activity_menu_next = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_NEXT);
  s_activity_menu_previous = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_PREVIOUS);
  s_activity_menu_pause = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_PAUSE);
}

static void activity_load_step_text_layer(Window *window) {
  GRect bounds = get_window_bounds(window);
  bounds.size.h = S_ACTIVITY_STEP_LAYER_HEIGHT;
  s_activity_step_layer = text_layer_create(bounds);
  text_layer_set_text_alignment(s_activity_step_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_activity_step_layer));
  text_layer_set_font(s_activity_step_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
}

static void activity_load_clock_text_layer(Window *window) {
  GRect bounds = get_window_bounds(window);
  bounds.origin.y = bounds.origin.y + S_ACTIVITY_STEP_LAYER_HEIGHT;
  bounds.size.h = S_ACTIVITY_CLOCK_LAYER_HEIGHT;
  s_activity_clock_layer = text_layer_create(bounds);
  text_layer_set_text_alignment(s_activity_clock_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_activity_clock_layer));
  text_layer_set_font(s_activity_clock_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
}

static void activity_load_directions_text_layer(Window *window) {
  GRect bounds = get_window_bounds(window);
  bounds.origin.y = bounds.origin.y + S_ACTIVITY_STEP_LAYER_HEIGHT + S_ACTIVITY_CLOCK_LAYER_HEIGHT;
  bounds.size.h = S_ACTIVITY_DIRECTIONS_LAYER_HEIGHT;
  s_activity_directions_layer = text_layer_create(bounds);
  text_layer_set_text_alignment(s_activity_directions_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_activity_directions_layer));
  text_layer_set_font(s_activity_directions_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
}

static void check_for_day_complete() {
  if(isCurrentlyInCompleteStep()) {
      persist_write_bool(day->dayIndex, true);
  }
}

static void redraw_text_layers() {
  static char stepBuffer[] = "Step: 99/99";
  snprintf(stepBuffer, sizeof(stepBuffer), "Step: %02d/%02d", currentStepIndex + 1, day->numSteps + 3);
  text_layer_set_text(s_activity_step_layer, stepBuffer);
  
  int currentStepLength = 0;
  if(isCurrentlyInWarmupStep()) {
    text_layer_set_text(s_activity_directions_layer, "WARMUP");
    currentStepLength = WARMUP_STEP_DURATION;
  } else if(isCurrentlyInCooldownStep()) {
    text_layer_set_text(s_activity_directions_layer, "COOLDOWN");
    currentStepLength = COOLDOWN_STEP_DURATION;
  } else if(isCurrentlyInCompleteStep()) {
    text_layer_set_text(s_activity_directions_layer, "COMPLETE!");
  } else {
    struct step currentStep = day->steps[currentStepIndex-1];
    text_layer_set_text(s_activity_directions_layer, currentStep.isWalk ? "WALK" : "RUN");
    currentStepLength = currentStep.duration;
  }
  int timeLeftInCurrentStep = currentStepLength - timeSpentInCurrentStep;
  int mins = timeLeftInCurrentStep / 60;
  int seconds = timeLeftInCurrentStep % 60;
  static char clockBuffer[] = "99:99";
  snprintf(clockBuffer, sizeof(clockBuffer), "%02d:%02d", mins, seconds);
  text_layer_set_text(s_activity_clock_layer, clockBuffer);
}

static void activity_load_main_layers(Window *window) {
  activity_load_step_text_layer(window);
  activity_load_clock_text_layer(window);
  activity_load_directions_text_layer(window);
  redraw_text_layers();
}

static void setup_pause_play_icons() {
  action_bar_layer_set_icon_animated(s_activity_action_bar, BUTTON_ID_SELECT, isPaused ? s_activity_menu_play : s_activity_menu_pause, true);
}

static void activity_menu_select_callback(ClickRecognizerRef recognizer, void *context) {
  isPaused = !isPaused;
  setup_pause_play_icons();
}

static void activity_menu_up_callback(ClickRecognizerRef recognizer, void *context) {
  if (isWithinTimeFrameToJumpToPreviousStep() && !isCurrentlyInWarmupStep()) {
    currentStepIndex--;
  }
  timeSpentInCurrentStep = 0;
  redraw_text_layers();
}

static void activity_menu_down_callback(ClickRecognizerRef recognizer, void *context) {
  if(!isCurrentlyInCompleteStep()) {
      currentStepIndex++;
      timeSpentInCurrentStep = 0;
      check_for_day_complete();
  }
  redraw_text_layers();
}

static void activity_click_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, activity_menu_select_callback);
  window_single_click_subscribe(BUTTON_ID_UP, activity_menu_up_callback);
  window_single_click_subscribe(BUTTON_ID_DOWN, activity_menu_down_callback);
}

static void activity_load_action_bar(Window *window) {
  s_activity_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_activity_action_bar, window);
  action_bar_layer_set_click_config_provider(s_activity_action_bar, activity_click_provider);
  setup_pause_play_icons();
  action_bar_layer_set_icon_animated(s_activity_action_bar, BUTTON_ID_UP, s_activity_menu_previous, true);
  action_bar_layer_set_icon_animated(s_activity_action_bar, BUTTON_ID_DOWN, s_activity_menu_next, true);
}

static void second_tick_handler(struct tm *tick_time, TimeUnits units_changed){
  if(!isCurrentlyInCompleteStep()) {
    if(!isPaused) {
      timeSpentInCurrentStep++;
    }
    int currentStepLength = 0;
    if(isCurrentlyInWarmupStep()) {
      currentStepLength = WARMUP_STEP_DURATION;
    } else if(isCurrentlyInCooldownStep()) {
      currentStepLength = COOLDOWN_STEP_DURATION;
    } else {
      currentStepLength = day->steps[currentStepIndex-1].duration;
    }
    
    if (timeSpentInCurrentStep > currentStepLength) {
      timeSpentInCurrentStep = 0;
      currentStepIndex++;
      check_for_day_complete();
    }
  }
  redraw_text_layers();
}

static void activity_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading activity window");
  currentStepIndex = 0;
  timeSpentInCurrentStep = 0;
  isPaused = false;
  activity_load_bitmaps();
  activity_load_status_bar(window);
  activity_load_action_bar(window);
  activity_load_main_layers(window);
  tick_timer_service_subscribe(SECOND_UNIT, second_tick_handler);
}

static void activity_widow_unload() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloading activity window");
  tick_timer_service_unsubscribe();
  status_bar_layer_destroy(s_activity_status_bar);
  action_bar_layer_destroy(s_activity_action_bar);
  gbitmap_destroy(s_activity_menu_play);
  gbitmap_destroy(s_activity_menu_next);
  gbitmap_destroy(s_activity_menu_previous);
  gbitmap_destroy(s_activity_menu_pause);
  text_layer_destroy(s_activity_step_layer);
  text_layer_destroy(s_activity_clock_layer);
  text_layer_destroy(s_activity_directions_layer);
}

void activity_window_create() {
  s_activity_window = window_create();
  window_set_window_handlers(s_activity_window, (WindowHandlers) {
    .load = activity_window_load,
    .unload = activity_widow_unload,
  });
}

void activity_window_show(struct day *targetDay){
  day = targetDay;
  window_stack_push(s_activity_window, false);
}

void activity_window_destroy() {
  window_destroy(s_activity_window);
}