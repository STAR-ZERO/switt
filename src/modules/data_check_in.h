#pragma once

#include <pebble.h>

typedef struct {
  char *error_message;
} DataCheckIn;

DataCheckIn* data_check_in_create(DictionaryIterator *iterator);
