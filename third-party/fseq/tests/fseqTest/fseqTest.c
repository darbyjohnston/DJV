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

#include "fseqTest.h"

#include "fseq.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test()
{
    {
        struct FSeqFileNameSizes a;
        fseqFileNameSizesInit(&a);
        assert(0 == a.path);
        assert(0 == a.base);
        assert(0 == a.number);
        assert(0 == a.extension);
    }
    {
        static const char testData[][5][FSEQ_STRING_LEN_MAX] =
        {
            { "", "", "", "", "" },
            { "/", "/", "", "", "" },
            { ".", "", ".", "", "" },
            { "/.", "/", ".", "", "" },
            { "\\", "\\", "", "", "" },
            { "\\\\", "\\\\", "", "", "" },
            { "\\\\.", "\\\\", ".", "", "" },
            { "\\\\.\\", "\\\\.\\", "", "", "" },
            { "0", "", "", "0", "" },
            { "/0", "/", "", "0", "" },
            { "/a/b/c1.ext", "/a/b/", "c", "1", ".ext" },
            { "/a/b/c10.ext", "/a/b/", "c", "10", ".ext" },
            { "/a/b/c100.ext", "/a/b/", "c", "100", ".ext" },
            { "/a/b/c0100.ext", "/a/b/", "c", "0100", ".ext" },
            { "/a/b/c-0100.ext", "/a/b/", "c-", "0100", ".ext" },
            { "C:\\a\\b\\c-0100.ext", "C:\\a\\b\\", "c-", "0100", ".ext" }
        };
        static const size_t testDataSize = sizeof(testData) / sizeof(testData[0]);
        for (size_t i = 0; i < testDataSize; ++i)
        {
            struct FSeqFileNameSizes sizes;
            struct FSeqFileName fileName;

            fseqFileNameSizesInit(&sizes);
            fseqFileNameParseSizes(testData[i][0], &sizes, FSEQ_STRING_LEN_MAX);

            fseqFileNameInit(&fileName);
            fseqFileNameParse2(testData[i][0], &sizes, &fileName);

            printf("\"%s\": \"%s\" \"%s\" \"%s\" \"%s\"\n",
                testData[i][0],
                fileName.path,
                fileName.base,
                fileName.number,
                fileName.extension);

            assert(strlen(testData[i][1]) == sizes.path && 0 == memcmp(fileName.path, testData[i][1], sizes.path));
            assert(strlen(testData[i][2]) == sizes.base && 0 == memcmp(fileName.base, testData[i][2], sizes.base));
            assert(strlen(testData[i][3]) == sizes.number && 0 == memcmp(fileName.number, testData[i][3], sizes.number));
            assert(strlen(testData[i][4]) == sizes.extension && 0 == memcmp(fileName.extension, testData[i][4], sizes.extension));

            fseqFileNameDel(&fileName);
        }
    }
    {
        struct FSeqFileNameSizes a, b;
        fseqFileNameSizesInit(&a);
        fseqFileNameSizesInit(&b);
        assert(fseqFileNameSizesCompare(&a, &b) != 0);
        a.path = 1;
        assert(fseqFileNameSizesCompare(&a, &b) == 0);
    }
    {
        struct FSeqFileName a;
        fseqFileNameInit(&a);
        assert(NULL == a.path);
        assert(NULL == a.base);
        assert(NULL == a.number);
        assert(NULL == a.extension);
    }
    {
        char fileName[] = "/tmp/render.1.tif";
        struct FSeqFileName a;
        fseqFileNameInit(&a);
        fseqFileNameParse("/tmp/render.1.tif", &a);
        assert(0 == memcmp("/tmp/", a.path, strlen(a.path)));
        assert(0 == memcmp("render.", a.base, strlen(a.base)));
        assert(0 == memcmp("1", a.number, strlen(a.number)));
        assert(0 == memcmp(".tif", a.extension, strlen(a.extension)));
        fseqFileNameDel(&a);
        assert(NULL == a.path);
        assert(NULL == a.base);
        assert(NULL == a.number);
        assert(NULL == a.extension);
    }
    {
        char fileNameA[] = "/tmp/render.1.tif";
        char fileNameB[] = "/tmp/render.2.tif";
        struct FSeqFileNameSizes sizesA;
        struct FSeqFileNameSizes sizesB;
        int match = 0;
        
        fseqFileNameSizesInit(&sizesA);
        fseqFileNameParseSizes(fileNameA, &sizesA, FSEQ_STRING_LEN_MAX);
        printf("%s: %d %d %d %d\n", fileNameA, sizesA.path, sizesA.base, sizesA.number, sizesA.extension);
        assert(5 == sizesA.path);
        assert(7 == sizesA.base);
        assert(1 == sizesA.number);
        assert(4 == sizesA.extension);
        
        fseqFileNameSizesInit(&sizesB);
        fseqFileNameParseSizes(fileNameB, &sizesB, FSEQ_STRING_LEN_MAX);
        printf("%s: %d %d %d %d\n", fileNameB, sizesB.path, sizesB.base, sizesB.number, sizesB.extension);
        assert(5 == sizesB.path);
        assert(7 == sizesB.base);
        assert(1 == sizesB.number);
        assert(4 == sizesB.extension);
        
        match = fseqFileNameMatch(fileNameA, &sizesA, fileNameB, &sizesB);
        assert(match != 0);
    }
    {
        assert(0 == fseqPadding(""));
        assert(1 == fseqPadding("0"));
        assert(0 == fseqPadding("100"));
        assert(4 == fseqPadding("0100"));
    }
    {
        char fileName[] = "/tmp/render.0001.tif";
        struct FSeqFileNameSizes a;
        fseqFileNameSizesInit(&a);
        fseqFileNameParseSizes(fileName, &a, FSEQ_STRING_LEN_MAX);
        assert(4 == fseqPadding2(fileName, &a));
    }
}
