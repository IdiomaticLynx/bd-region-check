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

static void _file_close(BD_FILE_H *file)
{
    if (file) {
        if (close((int)(intptr_t)file->internal)) {
            BD_DEBUG(DBG_CRIT | DBG_FILE, "Error closing POSIX file (%p)\n", (void*)file);
        }

        BD_DEBUG(DBG_FILE, "Closed POSIX file (%p)\n", (void*)file);

        X_FREE(file);
    }
}

static int64_t _file_seek(BD_FILE_H *file, int64_t offset, int32_t origin)
{
    off_t result = lseek((int)(intptr_t)file->internal, offset, origin);
    if (result == (off_t)-1) {
        BD_DEBUG(DBG_FILE, "lseek() failed (%p)\n", (void*)file);
        return -1;
    }
    return (int64_t)result;
}

static int64_t _file_tell(BD_FILE_H *file)
{
    return _file_seek(file, 0, SEEK_CUR);
}

static int64_t _file_read(BD_FILE_H *file, uint8_t *buf, int64_t size)
{
    ssize_t got, result;

    if (size <= 0 || size >= BD_MAX_SSIZE) {
        BD_DEBUG(DBG_FILE | DBG_CRIT, "Ignoring invalid read of size %"PRId64" (%p)\n", size, (void*)file);
        return 0;
    }

    for (got = 0; got < (ssize_t)size; got += result) {
        result = read((int)(intptr_t)file->internal, buf + got, size - got);
        if (result < 0) {
            if (errno != EINTR) {
                BD_DEBUG(DBG_FILE, "read() failed (%p)\n", (void*)file);
                break;
            }
            result = 0;
        } else if (result == 0) {
            // hit EOF.
            break;
        }
    }
    return (int64_t)got;
}

static BD_FILE_H *_file_open(const char* filename, const char *cmode)
{
    BD_FILE_H *file;
    int fd    = -1;
    int flags = 0;
    int mode  = 0;

    if (strchr(cmode, 'w')) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode  = S_IRUSR | S_IWUSR;
    } else {
        flags = O_RDONLY;
    }

#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif
#ifdef O_BINARY
    flags |= O_BINARY;
#endif

    if ((fd = open(filename, flags, mode)) < 0) {
        BD_DEBUG(DBG_FILE, "Error opening file %s\n", filename);
        return NULL;
    }

    file = calloc(1, sizeof(BD_FILE_H));
    if (!file) {
        close(fd);
        BD_DEBUG(DBG_FILE, "Error opening file %s (out of memory)\n", filename);
        return NULL;
    }

    file->close = _file_close;
    file->seek  = _file_seek;
    file->read  = _file_read;
    file->tell  = _file_tell;

    file->internal = (void*)(intptr_t)fd;

    BD_DEBUG(DBG_FILE, "Opened POSIX file %s (%p)\n", filename, (void*)file);
    return file;
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

BD_FILE_H *file_from_buffer(char* buffer, int64_t size) {
    return NULL;
}

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
