#pragma once

#include <pebble.h>

#include "modules/comm.h"
#include "modules/data_search.h"
#include "modules/data_check_in.h"

void main_window_push();

void main_window_deinit();

// search result
void main_window_set_data_search(DataSearch *data_search);

// check in result
void main_window_set_data_check_in(DataCheckIn *data_check_in);
