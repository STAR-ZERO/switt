#pragma once

#include <pebble.h>

#include "../windows/main_window.h"

typedef enum {
  AppKeyMessageType = 0,
  AppKeyResultError,
  AppKeyVenueCount,
  AppKeyCheckInId,
  AppKeyVenueIdPrefix = 100,
  AppKeyVenueNamePrefix = 200,
  AppKeyVenueDistancePrefix = 300
} AppKey;

void comm_init();

void comm_deinit();

void comm_check_in(char *venue_id);
