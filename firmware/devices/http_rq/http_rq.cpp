#include "include/http_rq.hpp"
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "utils.hpp"

namespace
{
const logs::Logger log{"Http RQ"};
}

static int create_json_obj(sensor::Measure *ms, char *obj_string)
{
    int err = 0;
    cJSON *sample = cJSON_CreateObject();

    if (cJSON_AddNumberToObject(sample, "temp", ms->temp) == NULL)
    {
        err = -1;
        goto end;
    }

    if (cJSON_AddNumberToObject(sample, "hm", ms->hm) == NULL)
    {
        err = -1;
        goto end;
    }

    if (cJSON_AddNumberToObject(sample, "uv", ms->uv) == NULL)
    {
        err = -1;
        goto end;
    }

    if (cJSON_AddNumberToObject(sample, "air", ms->air) == NULL)
    {
        err = -1;
        goto end;
    }

    obj_string = cJSON_Print(sample);

    if (obj_string == NULL)
    {
        err = -1;
        log << "Couldn't create the json object";
    }

end:
    cJSON_Delete(sample);
    return err;
}

// TODO Actually make the request
Result http_send_sample(sensor::MeasureP sample)
{
    char *obj_string = nullptr;
    if (create_json_obj(sample, obj_string) < 0)
    {
        return Result::Err;
    }

    cJSON_free(obj_string);
    return Result::Ok;
}
