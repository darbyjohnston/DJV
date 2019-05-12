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

#ifndef FSEQ_H
#define FSEQ_H

#include <stddef.h>
#include <stdint.h>

// Boolean type.
typedef char FSeqBool;
#define FSEQ_TRUE 1
#define FSEQ_FALSE 0

// The default string length.
#define FSEQ_STRING_LEN 4096

// Get the minimum or maximum of two values.
#define FSEQ_MIN(A, B) (A < B ? A : B)
#define FSEQ_MAX(A, B) (A > B ? A : B)

// This struct provides file name component sizes.
struct FSeqFileNameSizes
{
	unsigned short path;
	unsigned short base;
	unsigned short number;
	unsigned short extension;
};
void fseqFileNameSizesInit(struct FSeqFileNameSizes*);

// Parse the file name component sizes.
// Args:
// * fileName - The file name to be parsed
// * sizes - The parsed sizes
// * max - The maximum length of the file name
// Returns:
// * The length of the file name
unsigned short fseqFileNameParseSizes(
    const char*               fileName,
    struct FSeqFileNameSizes* sizes,
    size_t                    max);

// Compare FSeqFileNameSizes structs.
FSeqBool fseqFileNameSizesCompare(
    const struct FSeqFileNameSizes*,
    const struct FSeqFileNameSizes*);

// This sruct provides file name components.
struct FSeqFileName
{
    char* path;
    char* base;
    char* number;
    char* extension;
};
void fseqFileNameInit(struct FSeqFileName*);
void fseqFileNameDel(struct FSeqFileName*);

// Split a file name into components.
void fseqFileNameSplit(const char*, struct FSeqFileName*, size_t max);

// Split a file name into components.
FSeqBool fseqFileNameSplit2(
    const char*,
    const struct FSeqFileNameSizes*,
    struct FSeqFileName*);

// Test whether two file names are part of the same sequence (all components
// are equal except for the number).
FSeqBool fseqFileNameMatch(
    const char*,
    const struct FSeqFileNameSizes*,
    const char*,
    const struct FSeqFileNameSizes*);

// This struct provides a directory entry.
struct FSeqDirEntry
{
    struct FSeqFileName  fileName;
    int64_t              frameMin;
    int64_t              frameMax;
    FSeqBool             hasFramePadding;
    char                 framePadding;
    struct FSeqDirEntry* next;
};
void fseqDirEntryInit(struct FSeqDirEntry*);
void fseqDirEntryDel(struct FSeqDirEntry*);

// Convert a directory entry to a string.
// Args:
// * entry - The directory entry
// * out - The output string
// * path - Whether to include the path
// * max - The maximum output string length
void fseqDirEntryToString(
    const struct FSeqDirEntry* entry,
    char*                      out,
    FSeqBool                   path,
    size_t                     max);

// This struct provides directory listing options.
struct FSeqDirOptions
{
    FSeqBool dotAndDotDotDirs;
    FSeqBool dotFiles;
    FSeqBool sequence;
};
void fseqDirOptionsInit(struct FSeqDirOptions*);

// List the contents of a directory. Use fseqDirListDel() to delete the list.
// Args:
// * path - The directory path
// * options - The directory listing options, may also pass NULL instead
// * error - Whether any erros occurred, may also pass NULL instead
// Returns:
// * A list of directory entries
struct FSeqDirEntry* fseqDirList(
    const char*                  path,
    const struct FSeqDirOptions* options,
    FSeqBool*                    error);

// Delete a directory list.
void fseqDirListDel(struct FSeqDirEntry*);

#endif // FSEQ_H
