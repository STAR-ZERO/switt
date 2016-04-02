#pragma once

#include <pebble.h>

typedef struct {
  char *name;
  char *id;
  int32_t distance;
} Venue;

typedef struct {
  char *error_message;
  int32_t venue_count;
  Venue *venues;
} DataSearch;

DataSearch* data_search_create(DictionaryIterator *iterator);
