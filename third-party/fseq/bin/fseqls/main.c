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

void printDir(const char* path, const struct FSeqDirOptions* options)
{
    struct FSeqDirEntry* entries = NULL;
    struct FSeqDirEntry* entry   = NULL;
    FSeqBool             error   = FSEQ_FALSE;

    entries = fseqDirList(path, options, &error);
    if (error)
    {
        printf("cannot read %s\n", path);
        return;
    }

    entry = entries;
    while (entry)
    {
        static char buf[FSEQ_STRING_LEN];
        fseqDirEntryToString(entry, buf, FSEQ_STRING_LEN);
        printf("%s\n", buf);
        entry = entry->next;
    }

    fseqDirListDel(entries);
}

int main(int argc, char** argv)
{
    struct FSeqDirOptions options;

    fseqDirOptionsInit(&options);

    if (argc > 1)
    {
        // List the input directories.
        for (int i = 1; i < argc; ++i)
        {
            printDir(argv[i], &options);
        }
    }
    else
    {
        // List the current directory.
        printDir(".", &options);
    }

    return 0;
}
