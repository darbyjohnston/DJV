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

#include "fseqls.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    char** dirs = NULL;
    size_t dirsCount = 0;
    struct FSeqDirOptions options;

    fseqDirOptionsInit(&options);

    if (argc > 1)
    {
        dirsCount = argc - 1;
        dirs = (char**)malloc(dirsCount * sizeof(char**));
        for (int i = 0; i < argc - 1; ++i)
        {
            const size_t len = strlen(argv[i + 1]);
            dirs[i] = (char*)malloc((len + 1) * sizeof(char));
            memcpy(dirs[i], argv[i + 1], len);
            dirs[i][len] = 0;
        }
    }
    else
    {
        dirsCount = 1;
        dirs = (char**)malloc(dirsCount * sizeof(char**));
        dirs[0] = (char*)malloc(2 * sizeof(char));
        dirs[0][0] = '.';
        dirs[0][1] = 0;
    }

    for (size_t i = 0; i < dirsCount; ++i)
    {
        printDir(dirs[i], &options);
        free(dirs[i]);
    }

    free(dirs);

    return 0;
}
