/*
 * This file is part of libbluray
 * Copyright (C) 2010  hpi1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "region_detect.h"

#include "mobj_data.h"
#include "mobj_parse.h"
#include "file/file.h"

#include <stdio.h>
#include <string.h>

static int _scan_objects(MOBJ_OBJECTS *objects, REGISTER_ACCESS output[], size_t max_results)
{
    int c = 0;
    
    for(int o = 0; o < objects->num_objects; o++) {
        MOBJ_OBJECT *obj = &objects->objects[o];
        for(int pc = 0; pc < obj->num_cmds; pc++) {
            if(c == max_results) {
                printf("Quitting as to not exceed max results");
                return max_results;
            }
            
            MOBJ_CMD *cmd = &obj->cmds[pc];
            if(cmd->insn.op_cnt != 2) {
                continue;
            }

            if(!cmd->insn.imm_op1) {
                if(cmd->dst & 0x80000000) {
                    int psr = cmd->dst & 0x7f;
                    if(psr == 19) {
                        // Country check
                        output[c++] = (REGISTER_ACCESS) {COUNTRY_CODE, COMPARISON, cmd->src};
                    } else if(psr == 20) {
                        // Region check
                        output[c++] = (REGISTER_ACCESS) {REGION_CODE, COMPARISON, cmd->src};
                    }
                }
            }

            if(!cmd->insn.imm_op2) {
                if(cmd->src & 0x80000000) {
                    int psr = cmd->src & 0x7f;
                    if(psr == 19) {
                        // Country check
                        int country = obj->cmds[pc + 2].src;
                        output[c++] = (REGISTER_ACCESS) {COUNTRY_CODE, ASSIGNMENT, country};
                    } else if(psr == 20) {
                        // Region check
                        output[c++] = (REGISTER_ACCESS) {REGION_CODE, ASSIGNMENT, -1};
                    }
                }
            }
        }
    }

    return c;
}

int check(char* buffer, uint64_t size, REGISTER_ACCESS output[], size_t max_results) {
    MOBJ_OBJECTS *mobj = NULL;
    printf("%llu\n", size);
    
    BD_FILE_H *fp;
    fp = file_from_buffer(buffer, size);
    if (!fp) {
        printf("Failed to open\n");
        return -1;
    }
    mobj = mobj_parse(fp);
    if (!mobj) {
        printf("Failed to parse\n");
        return -1;
    }

    int res = _scan_objects(mobj, output, max_results);
    mobj_free(&mobj);
    return res;
}

int checkFile(const char* file, REGISTER_ACCESS output[], size_t max_results) {
    MOBJ_OBJECTS *mobj = NULL;

    BD_FILE_H *fp;
    fp = file_open(file, "rb");
    if (!fp) {
        printf("Failed to open\n");
        return -1;
    }
    mobj = mobj_parse(fp);
    if (!mobj) {
        printf("Failed to parse\n");
        return -1;
    }

    int res = _scan_objects(mobj, output, max_results);
    mobj_free(&mobj);
    return res;
}
