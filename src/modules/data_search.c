#include "data_search.h"
#include "comm.h"

DataSearch* data_search_create(DictionaryIterator *iterator) {
  DataSearch *data_search = malloc(sizeof(DataSearch));
  data_search->error_message = NULL;
  data_search->venues = NULL;

  Tuple *result_error_tuple = dict_find(iterator, AppKeyResultError);
  if (result_error_tuple) {
    // error
    data_search->error_message = result_error_tuple->value->cstring;
    return data_search;
  }

  Tuple *venue_count_tuple = dict_find(iterator, AppKeyVenueCount);
  if (!venue_count_tuple) {
    data_search->error_message = "No result";
    return data_search;
  }

  int32_t venue_count = venue_count_tuple->value->int32;
  if (venue_count <= 0) {
    data_search->error_message = "No result";
    return data_search;
  }

  data_search->venue_count = venue_count;
  data_search->venues = malloc(venue_count * sizeof(Venue));

  for (int i = 0; i < venue_count; i++) {
    int key_id = AppKeyVenueIdPrefix + i;
    int key_name = AppKeyVenueNamePrefix + i;
    int key_distance = AppKeyVenueDistancePrefix + i;

    // id
    Tuple *venue_id_tuple = dict_find(iterator, key_id);
    char *venue_id = venue_id_tuple->value->cstring;

    // name
    Tuple *venue_name_tuple = dict_find(iterator, key_name);
    char *venue_name = venue_name_tuple->value->cstring;

    // distance
    Tuple *venue_distance_tuple = dict_find(iterator, key_distance);
    int32_t venue_distance = venue_distance_tuple->value->int32;

    data_search->venues[i] = (Venue) {
      .id = venue_id,
      .name = venue_name,
      .distance = venue_distance
    };
  }

  return data_search;
}
