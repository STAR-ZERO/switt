#include "main_window.h"

// UI
static Window *s_window;
static StatusBarLayer *s_status_bar;
static TextLayer *s_background_layer;
static TextLayer *s_message_layer;
static TextLayer *s_venue_name_layer;
static TextLayer *s_venue_distance_layer;

// button enable flag
static bool s_enable_button = false;

// search result
static DataSearch *s_data_search = NULL;
static int s_venue_count = 0;
static int s_venue_index = 0;
// char format distance
static char *s_distance = NULL;

// UI Height
static const int HEIGHT_BACKGROUD = 100;
static const int HEIGHT_TEXT_LAYER = 38;

// set vertical center to text layer
static void text_layer_set_vertical_center(TextLayer *text_layer) {
  GRect parent_frame = layer_get_frame(text_layer_get_layer(s_background_layer));
  GRect frame = layer_get_frame(text_layer_get_layer(text_layer));
  GSize content_size = text_layer_get_content_size(text_layer);

  int x = frame.origin.x;
  int y = (parent_frame.size.h / 2) - (content_size.h / 2);
  y = y - 4; // adjustment
  int w = frame.size.w;
  int h = HEIGHT_TEXT_LAYER;
  layer_set_frame(text_layer_get_layer(text_layer), GRect(x, y, w, h));
}

// show venue info
static void show_venue_info() {
  Venue venue = s_data_search->venues[s_venue_index];

  // name
  text_layer_set_text(s_venue_name_layer, venue.name);
  text_layer_set_vertical_center(s_venue_name_layer);

  // distance
  if (s_distance) {
    free(s_distance);
  }
  s_distance = malloc(sizeof(char) * 24);
  snprintf(s_distance, 24, "%dM", (int)venue.distance);
  text_layer_set_text(s_venue_distance_layer, s_distance);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Click Select");

  if (!s_enable_button) {
    return;
  }

  s_enable_button = false;

  text_layer_set_text(s_message_layer, "Processing...");

  layer_set_hidden(text_layer_get_layer(s_message_layer), false);
  layer_set_hidden(text_layer_get_layer(s_venue_name_layer), true);
  layer_set_hidden(text_layer_get_layer(s_venue_distance_layer), true);

  Venue venue = s_data_search->venues[s_venue_index];
  comm_check_in(venue.id);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Click Up");

  if (!s_enable_button) {
    return;
  }

  s_venue_index--;
  if (0 > s_venue_index) {
    s_venue_index = s_venue_count - 1;
  }
  show_venue_info();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Click Down");

  if (!s_enable_button) {
    return;
  }

  s_venue_index++;
  if (s_venue_count <= s_venue_index) {
    s_venue_index = 0;
  }
  show_venue_info();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load() {
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);

  // status bar
  s_status_bar = status_bar_layer_create();
  status_bar_layer_set_colors(s_status_bar, GColorClear, GColorBlack);
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));

  // background
  s_background_layer = text_layer_create(GRect(0, (bounds.size.h / 2) - (HEIGHT_BACKGROUD / 2), bounds.size.w, HEIGHT_BACKGROUD));
  text_layer_set_background_color(s_background_layer, PBL_IF_COLOR_ELSE(GColorMediumSpringGreen, GColorClear));
  layer_add_child(window_layer, text_layer_get_layer(s_background_layer));

  // message
  s_message_layer = text_layer_create(GRect(8, (HEIGHT_BACKGROUD / 2) - (HEIGHT_TEXT_LAYER / 2), bounds.size.w - 16, HEIGHT_TEXT_LAYER));
  text_layer_set_text(s_message_layer, "Loading...");
  text_layer_set_background_color(s_message_layer, GColorClear);
  text_layer_set_font(s_message_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_message_layer, GTextAlignmentCenter);
  layer_add_child(text_layer_get_layer(s_background_layer), text_layer_get_layer(s_message_layer));
  text_layer_set_vertical_center(s_message_layer);

  // venue name
  s_venue_name_layer = text_layer_create(GRect(8, (HEIGHT_BACKGROUD / 2) - (HEIGHT_TEXT_LAYER / 2), bounds.size.w - 16, HEIGHT_TEXT_LAYER));
  text_layer_set_background_color(s_venue_name_layer, GColorClear);
  text_layer_set_font(s_venue_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_venue_name_layer, GTextAlignmentCenter);
  layer_add_child(text_layer_get_layer(s_background_layer), text_layer_get_layer(s_venue_name_layer));
  layer_set_hidden(text_layer_get_layer(s_venue_name_layer), true);

  // distance
  s_venue_distance_layer = text_layer_create(GRect(8, (HEIGHT_BACKGROUD / 2) + (HEIGHT_TEXT_LAYER / 2) + 4, bounds.size.w - 16, 18));
  text_layer_set_background_color(s_venue_distance_layer, GColorClear);
  text_layer_set_font(s_venue_distance_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_venue_distance_layer, GTextAlignmentCenter);
  layer_add_child(text_layer_get_layer(s_background_layer), text_layer_get_layer(s_venue_distance_layer));
  layer_set_hidden(text_layer_get_layer(s_venue_distance_layer), true);

  comm_init();
}

static void window_unload() {
  status_bar_layer_destroy(s_status_bar);
  text_layer_destroy(s_background_layer);
  text_layer_destroy(s_message_layer);
  text_layer_destroy(s_venue_name_layer);
  text_layer_destroy(s_venue_distance_layer);
}

void main_window_push() {

  s_venue_count = 0;

  if (!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
    window_stack_push(s_window, true);
  }
}

void main_window_deinit() {
  if (s_distance) {
    free(s_distance);
  }

  window_destroy(s_window);
}

// search result
void main_window_set_data_search(DataSearch *data_search) {
  APP_LOG(APP_LOG_LEVEL_INFO, "main_window_set_venue_result");

  if (data_search->error_message) {
    // error
    text_layer_set_text(s_message_layer, data_search->error_message);
    text_layer_set_vertical_center(s_message_layer);
    return;
  }

  // set hidden
  layer_set_hidden(text_layer_get_layer(s_message_layer), true);
  layer_set_hidden(text_layer_get_layer(s_venue_name_layer), false);
  layer_set_hidden(text_layer_get_layer(s_venue_distance_layer), false);

  // store
  s_data_search = data_search;
  s_venue_count = s_data_search->venue_count;
  s_venue_index = 0;

  s_enable_button = true;

  show_venue_info();
}

// check in result
void main_window_set_data_check_in(DataCheckIn *data_check_in) {
  APP_LOG(APP_LOG_LEVEL_INFO, "main_window_set_check_in_result");

  if (data_check_in->error_message) {
    // error
    text_layer_set_text(s_message_layer, data_check_in->error_message);
    text_layer_set_vertical_center(s_message_layer);
  } else {
    // success
    text_layer_set_text(s_message_layer, "\U0001F389 Checked In \U0001F389");
    text_layer_set_vertical_center(s_message_layer);
  }
}
