/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  Obliter0n
 * Copyright (C) 2009-2010  John Stebbins
 * Copyright (C) 2010-2015  Petri Hintukainen
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

#include "file.h"
#include "util/macro.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h> // remove(), SEEK_*
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __ANDROID__
# undef  lseek
# define lseek lseek64
# undef  off_t
# define off_t off64_t
#endif

typedef struct buffer_file_s BFILE;
struct buffer_file_s
{
    char* buffer;
    int64_t offset;
    int64_t size;
};

static void _file_close(BD_FILE_H *file)
{
    X_FREE(file);
}

static int64_t _file_seek(BD_FILE_H *file, int64_t offset, int32_t origin)
{
    BFILE* bfile = (BFILE*)file->internal;
    if(origin == SEEK_SET) {
        bfile->offset = offset;
    } else if(origin == SEEK_CUR) {
        bfile->offset += offset;
    } else if(origin == SEEK_END) {
        bfile->offset = bfile->size + offset;
    } else {
        BD_DEBUG(DBG_FILE, "Unsupported seeking whence\n");
        return -1;
    }
    return bfile->offset;
}

static int64_t _file_tell(BD_FILE_H *file)
{
    return _file_seek(file, 0, SEEK_CUR);
}

static int64_t _file_read(BD_FILE_H *file, uint8_t *buf, int64_t size)
{
    if (size <= 0 || size >= BD_MAX_SSIZE) {
        BD_DEBUG(DBG_FILE | DBG_CRIT, "Ignoring invalid read of size %"PRId64" (%p)\n", size, (void*)file);
        return 0;
    }

    BFILE* bfile = (BFILE*)file->internal;
    if(bfile->offset >= bfile->size) {
        return 0;
    }

    int64_t capped_size = size;
    if(bfile->offset + size > bfile->size) {
        capped_size = bfile->size - bfile->offset;
    }

    // HACK Unsafe copy
    memcpy(buf, bfile->buffer + bfile->offset, capped_size);
    bfile->offset += capped_size;
    return capped_size;
}

BD_FILE_H *file_from_buffer(char* buffer, int64_t size) {
    BD_FILE_H *file;

    file = calloc(1, sizeof(BD_FILE_H));
    if (!file) {
        BD_DEBUG(DBG_FILE, "Error opening file (out of memory)\n");
        return NULL;
    }

    file->close = _file_close;
    file->seek  = _file_seek;
    file->read  = _file_read;
    file->tell  = _file_tell;
    
    BFILE* bfile = malloc(sizeof(BFILE));
    bfile->buffer = buffer;
    bfile->offset = 0;
    bfile->size = size;
    file->internal = (void*)(intptr_t)bfile;
    return file;
}

static BD_FILE_H *_file_open(const char* filename, const char *cmode) {
    return NULL;
}


int64_t file_size(BD_FILE_H *fp)
{
    int64_t pos    = file_tell(fp);
    int64_t res1   = file_seek(fp, 0, SEEK_END);
    int64_t length = file_tell(fp);
    int64_t res2   = file_seek(fp, pos, SEEK_SET);

    if (res1 < 0 || res2 < 0 || pos < 0 || length < 0) {
        return -1;
    }

    return length;
}


BD_FILE_H* (*file_open)(const char* filename, const char *mode) = _file_open;

BD_FILE_OPEN file_open_default(void)
{
    return _file_open;
}

int file_unlink(const char *file)
{
    return remove(file);
}

int file_path_exists(const char *path)
{
    struct stat s;
    return stat(path, &s);
}

int file_mkdir(const char *dir)
{
    return mkdir(dir, S_IRWXU);
}

BD_FILE_OPEN bd_register_file(BD_FILE_OPEN p)
{
    BD_FILE_OPEN old = file_open;
    file_open = p;
    return old;
}
