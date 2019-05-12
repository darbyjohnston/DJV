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

#if defined(WIN32)
#define FSEQ_SNPRINTF sprintf_s
FILE* _fopen(const char* fileName, const char* mode)
{
    FILE* out = NULL;
    fopen_s(&out, fileName, mode);
    return out;
}
#else
#define FSEQ_SNPRINTF snprintf
FILE* _fopen(const char* fileName, const char* mode)
{
    return fopen(fileName, mode);
}
#endif

static const char fileNames[][FSEQ_STRING_LEN] =
{
    "shot%d_scene%d_normals",
    "shot%d_scene%d_rgba",
    "shot%d_scene%d_z"
};
static const size_t fileNamesSize = sizeof(fileNames) / sizeof(fileNames[0]);

int randInt(int max)
{
    return (int)(max * (rand() / (float)RAND_MAX));
}

int main(int argc, char** argv)
{
    int fileCount = 0;
    int seqCount  = 0;

    if (argc != 4)
    {
        printf("usage: fseqCreateRandom (directory) (total file count) (sequence count)\n");
        return 1;
    }
    fileCount = atoi(argv[2]);
    seqCount  = atoi(argv[3]);

    for (size_t i = 0; i < seqCount; ++i)
    {
        static char seq[FSEQ_STRING_LEN];
        int frame = 0;
        FSEQ_SNPRINTF(seq, FSEQ_STRING_LEN, fileNames[randInt((int)fileNamesSize - 1)], randInt(100), randInt(100));
        frame = randInt(1000);
        for (size_t j = 0; j < fileCount / seqCount; ++j, ++frame)
        {
            static char buf[FSEQ_STRING_LEN];
            FILE* f = NULL;
            FSEQ_SNPRINTF(buf, FSEQ_STRING_LEN, "%s/%s.%d.tif", argv[1], seq, frame);
            f = _fopen(buf, "w");
            if (!f)
            {
                printf("cannot create %s\n", buf);
                return 1;
            }
            fclose(f);
        }
    }
    return 0;
}
