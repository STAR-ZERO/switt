#include "comm.h"
#include "data_search.h"
#include "data_check_in.h"

// receive message type
typedef enum {
  messageTypeSearchVenue = 0,
  messageTypeCheckIn = 1
} MessageType;

static DataSearch *s_data_search = NULL;
static DataCheckIn *s_data_check_in = NULL;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

  Tuple *message_type_tuple = dict_find(iterator, AppKeyMessageType);
  if (!message_type_tuple) {
    return;
  }

  int32_t message_type = message_type_tuple->value->int32;
  if (message_type == messageTypeSearchVenue) {
    // search result
    s_data_search = data_search_create(iterator);
    main_window_set_data_search(s_data_search);

  } else if (message_type == messageTypeCheckIn) {
    // checkin result
    s_data_check_in = data_check_in_create(iterator);
    main_window_set_data_check_in(s_data_check_in);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void comm_init() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void comm_deinit() {
  APP_LOG(APP_LOG_LEVEL_INFO, "comm_deinit");

  if (s_data_search) {
    if (s_data_search->venues) {
      free(s_data_search->venues);
    }
    free(s_data_search);
  }
  if (s_data_check_in) {
    free(s_data_check_in);
  }
}

// check in
void comm_check_in(char *venue_id) {
  DictionaryIterator *iterator;
  if (app_message_outbox_begin(&iterator) != APP_MSG_OK) {
    // Error creating outbound message
    return;
  }

  dict_write_cstring(iterator, AppKeyCheckInId, venue_id);
  dict_write_end(iterator);

  // send to js
  app_message_outbox_send();
}
