#include "region_detect.h"

#include <stdio.h>
#include <string.h>


int main(int argc, const char *argv[]) {
    MOBJ_OBJECTS *mobj = NULL;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    REGISTER_ACCESS result[10];
    int c = checkFile(argv[argc-1], result, 10);
    if(c == -1) {
       printf("Failed to read/parse file!\n");
       return 1;
    }

    for(int i = 0; i < c; i++) {
        if(result[i].usage == UNKNOWN) {
            printf("Unknown access type of country and/or region register!\n");
        } else if(result[i].reg == REGION_CODE) {
            printf("Region: %c\n", result[i].value - 1 + 'A');
        } else {
            printf("Country: %c%c\n", result[i].value >> 8 & 0xFF, result[i].value & 0xFF);
        }
    }
    return 0;
}
