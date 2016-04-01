#include <stdio.h>
#include <jansson.h>

int main()
{
    json_t *json;

    json = json_pack("{s:s}", "result", "www");

    if (json) {
        printf("%s\n", json_dumps(json, 0));
        json_decref(json);
    }
    return 0;
}
