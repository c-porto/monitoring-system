#pragma once
// TODO
#include "utils.hpp"

Result http_send_sample(sensor::MeasureP sample);

static int create_json_obj(sensor::Measure *ms, char *obj_string);
