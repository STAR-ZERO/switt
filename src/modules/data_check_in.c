#include "data_check_in.h"
#include "comm.h"

DataCheckIn* data_check_in_create(DictionaryIterator *iterator) {
  DataCheckIn *data_check_in = malloc(sizeof(DataCheckIn));
  data_check_in->error_message = NULL;

  Tuple *result_error_tuple = dict_find(iterator, AppKeyResultError);
  if (result_error_tuple) {
    // error
    data_check_in->error_message = result_error_tuple->value->cstring;
  }

  return data_check_in;
}
