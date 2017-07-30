#ifndef REGION_DETECT_H_
#define REGION_DETECT_H_

#include "mobj_data.h"
#include "mobj_parse.h"

typedef enum {
    COUNTRY_CODE = 0,
    REGION_CODE  = 1
} register_type;

typedef enum {
    COMPARISON = 0,
    ASSIGNMENT = 1,
    UNKNOWN = 2
} register_usage_type;

typedef struct register_access_s REGISTER_ACCESS;
struct register_access_s
{
    register_type reg;
    register_usage_type usage;
    uint32_t value;
};

int check(char* buffer, uint64_t size, REGISTER_ACCESS output[], size_t max_results);
int checkFile(const char* file, REGISTER_ACCESS output[], size_t max_results);

#endif /* REGION_DETECT_H_ */
