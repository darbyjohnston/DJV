//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include "fseq.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <sys/types.h>
#include <dirent.h>
#endif

void fseqFileNameSizesInit(struct FSeqFileNameSizes* value)
{
    value->path = 0;
    value->base = 0;
    value->number = 0;
    value->extension = 0;
}

#define _IS_PATH_SEPARATOR(V) \
	('/' == V || \
     '\\' == V)
#define _IS_DOT(V) \
	('.' == V)
#define _IS_NUMBER(V) \
	('0' == V || \
     '1' == V || \
     '2' == V || \
     '3' == V || \
     '4' == V || \
     '5' == V || \
     '6' == V || \
     '7' == V || \
     '8' == V || \
     '9' == V || \
     '#' == V)
#define _IS_NUMBER_SEPARATOR(V) \
	('-' == V)

unsigned short fseqFileNameParseSizes(const char* in, struct FSeqFileNameSizes* out, size_t max)
{
    unsigned short len = 0;
    int lastPathSeparator = -1;
    int lastDot = -1;
    int start = 0;
    int end = 0;
    const char* p = in;

    // Iterate over the characters to find the last path separator,
    // the last dot, and the end of the string.
    for (; *p && len < max; ++p, ++len)
    {
        if (_IS_PATH_SEPARATOR(*p))
        {
            lastPathSeparator = (int)(p - in);
        }
        else if (_IS_DOT(*p))
        {
            lastDot = (int)(p - in);
        }
    }

    if (len > 0)
    {
        // Extract the path.
        if (lastPathSeparator != -1)
        {
            start = lastPathSeparator + 1;
            out->path = (unsigned short)(start);
        }

        // Extract the extension.
        end = len - 1;
        if (lastDot > 0 && lastDot > lastPathSeparator + 1)
        {
            out->extension = (unsigned short)(end - lastDot + 1);
            end = lastDot - 1;
        }

        // Extract the number.
        p = in + end;
        if (_IS_NUMBER(*p))
        {
            while (p > in && (_IS_NUMBER(*(p - 1)) || _IS_NUMBER_SEPARATOR(*(p - 1))))
                --p;
            if (_IS_NUMBER_SEPARATOR(*p))
                ++p;
            out->number = (unsigned short)(end - (p - in) + 1);
            end = (int)(p - in - 1);
        }

        // Whatever is leftover is the base.
        out->base = end - start + 1;
    }
    return len;
}

int fseqFileNameSizesCompare(const struct FSeqFileNameSizes* a, const struct FSeqFileNameSizes* b)
{
    return
        a->path == b->path &&
        a->base == b->base &&
        a->number == b->number &&
        a->extension == b->extension;
}

void fseqFileNameInit(struct FSeqFileName* value)
{
    value->path = NULL;
    value->base = NULL;
    value->number = NULL;
    value->extension = NULL;
}

void fseqFileNameDel(struct FSeqFileName* value)
{
    free(value->path);
    free(value->base);
    free(value->number);
    free(value->extension);
    value->path = NULL;
    value->base = NULL;
    value->number = NULL;
    value->extension = NULL;
}

void fseqFileNameParse(const char* value, struct FSeqFileName* out)
{
    struct FSeqFileNameSizes sizes;
    fseqFileNameSizesInit(&sizes);
    fseqFileNameParse2(value, &sizes, out);
}

void fseqFileNameParse2(const char* value, const struct FSeqFileNameSizes* sizes, struct FSeqFileName* out)
{
    out->path = (char*)malloc(sizes->path + 1);
    memcpy(out->path, value, sizes->path);
    out->path[sizes->path] = 0;

    out->base = (char*)malloc(sizes->base + 1);
    memcpy(out->base, value + sizes->path, sizes->base);
    out->base[sizes->base] = 0;

    out->number = (char*)malloc(sizes->number + 1);
    memcpy(out->number, value + sizes->path + sizes->base, sizes->number);
    out->number[sizes->number] = 0;

    out->extension = (char*)malloc(sizes->extension + 1);
    memcpy(out->extension, value + sizes->path + sizes->base + sizes->number, sizes->extension);
    out->extension[sizes->extension] = 0;
}

int fseqFileNameMatch(
    const char* a,
    const struct FSeqFileNameSizes* as,
    const char* b,
    const struct FSeqFileNameSizes* bs)
{
    return
        as->path == bs->path &&
        as->base == bs->base &&
        as->number &&
        bs->number &&
        as->extension == bs->extension &&
        0 == memcmp(a, b, as->path) &&
        0 == memcmp(a + as->path, b + bs->path, as->base) &&
        0 == memcmp(a + as->path + as->base + as->number, b + bs->path + bs->base + bs->number, as->extension);
}

int fseqPadding(const char* value)
{
    return '0' == value[0] ? (int)strlen(value) : 0;
}

int fseqPadding2(const char* value, const struct FSeqFileNameSizes* sizes)
{
    return sizes->number > 0 ? ('0' == *(value + sizes->path + sizes->base) ? sizes->number : 0) : 0;
}

void fseqDirEntryInit(struct FSeqDirEntry* value)
{
    fseqFileNameInit(&value->fileName);
    value->frameMin = -1;
    value->frameMax = -1;
    value->framePadding = 0;
    value->next = NULL;
}

void fseqDirEntryDel(struct FSeqDirEntry* value)
{
    fseqFileNameDel(&value->fileName);
    value->next = NULL;
}

void fseqDirOptionsInit(struct FSeqDirOptions* value)
{
    value->dotAndDotDotDirs = 0;
    value->dotFiles = 0;
    value->sequence = 1;
}

struct _DirEntry
{
    char* fileName;
    struct FSeqFileNameSizes sizes;
    int frameMin;
    int frameMax;
    char framePadding;
    struct _DirEntry* next;
};

static struct _DirEntry* _dirEntryCreate(const char* fileName, size_t fileNameLen, const struct FSeqFileNameSizes* sizes)
{
    struct _DirEntry* out = (struct _DirEntry*)malloc(sizeof(struct _DirEntry));
    out->fileName = (char*)malloc((fileNameLen + 1) * sizeof(char));
    memcpy(out->fileName, fileName, fileNameLen);
    out->fileName[fileNameLen] = 0;
    out->sizes = *sizes;
    if (sizes->number)
    {
        static char buf[FSEQ_STRING_LEN_MAX];
        memcpy(buf, fileName + sizes->path + sizes->base, sizes->number);
        buf[sizes->number] = 0;
        out->frameMin = out->frameMax = atoi(buf);
    }
    else
    {
        out->frameMin = -1;
        out->frameMax = -1;
    }
    out->framePadding = fseqPadding2(fileName, sizes);
    out->next = NULL;
    return out;
}

static void _dirEntryDel(struct _DirEntry* value)
{
    free(value->fileName);
    value->fileName = NULL;
    value->next = NULL;
}

#define _IS_DOT_DIR(V, LEN) (1 == LEN && '.' == V[0])
#define _IS_DOT_DOT_DIR(V, LEN) (2 == LEN && '.' == V[0] && '.' == V[1])

struct FSeqDirEntry* fseqDirList(const char* path, const struct FSeqDirOptions* options)
{
    struct FSeqDirEntry* out = NULL;
    struct FSeqDirEntry* entry = NULL;
    const size_t len = strlen(path);
    struct _DirEntry* _entries = NULL;
    struct _DirEntry* _entry = NULL;
    struct FSeqDirOptions _options;

    if (!options)
    {
        fseqDirOptionsInit(&_options);
        options = &_options;
    }

#if defined(WIN32)

    char* glob = NULL;

    glob = (char*)malloc(len + 3 * sizeof(char));
    memcpy(glob, path, len);
    glob[len] = '\\';
    glob[len + 1] = '*';
    glob[len + 2] = 0;

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(glob, &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            struct FSeqFileNameSizes sizes;
            unsigned short fileNameLen = 0;
            char filter = 0;

            fseqFileNameSizesInit(&sizes);
            fileNameLen = fseqFileNameParseSizes(ffd.cFileName, &sizes, FSEQ_STRING_LEN_MAX);

            if (!options->dotAndDotDotDirs && _IS_DOT_DIR(ffd.cFileName, fileNameLen))
            {
                filter = 1;
            }
            else if (!options->dotAndDotDotDirs && _IS_DOT_DOT_DIR(ffd.cFileName, fileNameLen))
            {
                filter = 1;
            }
            else if (!options->dotFiles && sizes.base)
            {
                filter = '.' == *(ffd.cFileName + sizes.path);
            }

            if (!filter)
            {
                if (!_entries)
                {
                    _entries = _dirEntryCreate(ffd.cFileName, fileNameLen, &sizes);
                }
                else
                {
                    _entry = _entries;
                    while (_entry)
                    {
                        if (options->sequence && fseqFileNameMatch(ffd.cFileName, &sizes, _entry->fileName, &_entry->sizes))
                        {
                            static char buf[FSEQ_STRING_LEN_MAX];
                            memcpy(buf, ffd.cFileName + sizes.path + sizes.base, sizes.number);
                            buf[sizes.number] = 0;
                            const int number = atoi(buf);
                            _entry->frameMin = FSEQ_MIN(_entry->frameMin, number);
                            _entry->frameMax = FSEQ_MAX(_entry->frameMax, number);
                            _entry->framePadding = FSEQ_MAX(_entry->framePadding, fseqPadding2(ffd.cFileName, &sizes));
                            break;
                        }
                        _entry = _entry->next;
                    }
                    if (!_entry)
                    {
                        _entry = _entries;
                        while (_entry->next)
                        {
                            _entry = _entry->next;
                        }
                        _entry->next = _dirEntryCreate(ffd.cFileName, fileNameLen, &sizes);
                    }
                }
            }

        } while (FindNextFile(hFind, &ffd) != 0);
        FindClose(hFind);
    }

#else

    DIR* dir = NULL;
    struct dirent* de = NULL;

    dir = opendir(path);
    if (!dir)
    {
        printf("cannot open %s\n", path);
        return NULL;
    }

    while ((de = readdir(dir)))
    {
        struct FSeqFileNameSizes sizes;
        unsigned short fileNameLen = 0;

        fseqFileNameSizesInit(&sizes);
        fileNameLen = fseqFileNameParseSizes(de->d_name, &sizes, FSEQ_STRING_LEN_MAX);

        if (!_entries)
        {
            _entries = _dirEntryCreate(de->d_name, fileNameLen, &sizes);
        }
        else
        {
            _entry = _entries;
            while (_entry)
            {
                if (fseqFileNameMatch(de->d_name, &sizes, _entry->fileName, &_entry->sizes))
                {
                    static char buf[FSEQ_STRING_LEN_MAX];
                    memcpy(buf, de->d_name + sizes.path + sizes.base, sizes.number);
                    buf[sizes.number] = 0;
                    const int number = atoi(buf);
                    _entry->frameMin = FSEQ_MIN(_entry->frameMin, number);
                    _entry->frameMax = FSEQ_MAX(_entry->frameMax, number);
                    _entry->framePadding = FSEQ_MAX(_entry->framePadding, fseqPadding2(de->d_name, &sizes));
                    break;
                }
                _entry = _entry->next;
            }
            if (!_entry)
            {
                _entry = _entries;
                while (_entry->next)
                {
                    _entry = _entry->next;
                }
                _entry->next = _dirEntryCreate(de->d_name, fileNameLen, &sizes);
            }
        }
    }

    closedir(dir);

#endif

    _entry = _entries;
    while (_entry)
    {
        struct _DirEntry* tmp = _entry;
        if (!out)
        {
            out = (struct FSeqDirEntry*)malloc(sizeof(struct FSeqDirEntry));
            fseqDirEntryInit(out);
            fseqFileNameParse2(_entry->fileName, &_entry->sizes, &out->fileName);
            out->frameMin = _entry->frameMin;
            out->frameMax = _entry->frameMax;
            out->framePadding = _entry->framePadding;
            entry = out;
        }
        else
        {
            entry->next = (struct FSeqDirEntry*)malloc(sizeof(struct FSeqDirEntry));
            fseqDirEntryInit(entry->next);
            fseqFileNameParse2(_entry->fileName, &_entry->sizes, &entry->next->fileName);
            entry->next->frameMin = _entry->frameMin;
            entry->next->frameMax = _entry->frameMax;
            entry->next->framePadding = _entry->framePadding;
            entry = entry->next;
        }
        _entry = _entry->next;
        _dirEntryDel(tmp);
        free(tmp);
    }

    return out;
}

void fseqDirListDel(struct FSeqDirEntry* value)
{
    while (value)
    {
        struct FSeqDirEntry* tmp = value;
        value = value->next;
        fseqDirEntryDel(tmp);
        free(tmp);
    }
}
