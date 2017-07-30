/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  Obliter0n
 * Copyright (C) 2009-2010  John Stebbins
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

#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>
#include <stddef.h>

/*
 * file access
 */

typedef struct bd_file_s BD_FILE_H;
struct bd_file_s
{
    void* internal;
    void    (*close) (BD_FILE_H *file);
    int64_t (*seek)  (BD_FILE_H *file, int64_t offset, int32_t origin);
    int64_t (*tell)  (BD_FILE_H *file);
    int     (*eof)   (BD_FILE_H *file);
    int64_t (*read)  (BD_FILE_H *file, uint8_t *buf, int64_t size);
};

/*
 * directory access
 */

typedef BD_FILE_H* (*BD_FILE_OPEN)(const char* filename, const char *mode);


static inline void file_close(BD_FILE_H *fp)
{
    fp->close(fp);
}

static inline int64_t file_tell(BD_FILE_H *fp)
{
    return fp->tell(fp);
}

static inline int64_t file_seek(BD_FILE_H *fp, int64_t offset, int32_t origin)
{
    return fp->seek(fp, offset, origin);
}

static inline size_t file_read(BD_FILE_H *fp, uint8_t *buf, size_t size)
{
    return (size_t)fp->read(fp, buf, (int64_t)size);
}

int64_t file_size(BD_FILE_H *fp);

extern BD_FILE_H* (*file_open)(const char* filename, const char *mode);

BD_FILE_H* file_from_buffer(char* buffer, int64_t size);

BD_FILE_OPEN file_open_default(void);

#endif /* FILE_H_ */
