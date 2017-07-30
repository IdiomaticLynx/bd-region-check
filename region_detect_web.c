#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emscripten/emscripten.h>

#include "region_detect.h"

int main(int argc, char ** argv) {
    printf("Initialized\n");
}

int EMSCRIPTEN_KEEPALIVE analyze_movie_objects(char* buffer, uint64_t size) {
    REGISTER_ACCESS result[10];
    
    int c = check(buffer, size, result, 10);
    if(c == -1) {
        EM_ASM_({
            registerFailure();
        }, 0);
    }
    for(int i = 0; i < c; i++) {
        printf("R: %d, T: %d, V: %d\n", result[i].reg, result[i].usage, result[i].value);
        if(result[i].usage == UNKNOWN) {
            EM_ASM_({
                registerUncertainty();
            }, 0);
        } else if(result[i].reg == REGION_CODE) {
            EM_ASM_({
                registerRegionCode($0);
            }, result[i].value);
        } else {
            EM_ASM_({
                registerCountryCode($0);
            }, result[i].value);
        }
    }
    return c;
}
