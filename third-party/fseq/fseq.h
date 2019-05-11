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

#ifndef _FSEQ_H
#define _FSEQ_H

#include <stddef.h>

//! \mainpage
//! FSeq is a C library for working with file sequences commonly used in the
//! VFX and film industry.
//!
//! Parsing a file name:
//! \code
//! struct FSeqFileName fileName;
//! fseqFileNameInit(&fileName);
//! fseqFileNameParse("/tmp/render0100.exr", &fileName);
//! printf("%s, %s, %s, %s\n", fileName.path, fileName.base, fileName.number, fileName.extension);
//! fseqFileNameDel(&fileName);
//! \endcode
//!
//! Output:
//! \code
//! /tmp/, render, 0100, .exr
//! \endcode
//!
//! List the contents of a directory with file sequences:
//! \code
//! struct FSeqDirEntry* entries = NULL;
//! struct FSeqDirEntry* entry = NULL;
//! entries = fseqDirList("/tmp", NULL);
//! entry = entries;
//! while (entry)
//! {
//!     printf("%s%s%s\n", entry->fileName.base, entry->fileName.number, entry->fileName.extension);
//!     entry = entry->next;
//! }
//! fseqDirListDel(entries);
//! \endcode
//!
//! Output:
//! \code
//! render.rgba.1-100.tif
//! render.normals.1-100.tif
//! render.z.1-100.tif
//! \endcode

//! \addtogroup Util
//! @{

//! The default maximum string length.
#define FSEQ_STRING_LEN_MAX 4096

//! Get the minimum of two values.
#define FSEQ_MIN(A, B) (A < B ? A : B)

//! Get the maximum of two values.
#define FSEQ_MAX(A, B) (A > B ? A : B)

//! @}

//! \addtogroup FileNames File Names
//! @{

//! This struct provides file name component sizes.
struct FSeqFileNameSizes
{
	unsigned short path;
	unsigned short base;
	unsigned short number;
	unsigned short extension;
};

//! Initialize a FSeqFileNameSizes struct.
void fseqFileNameSizesInit(struct FSeqFileNameSizes*);

//! Parse the file name component sizes.
//! \arg fileName - The file name to be parsed
//! \arg sizes - The parsed sizes
//! \arg max - The maximum length of the file name
//! \return The length of the file name
unsigned short fseqFileNameParseSizes(
    const char*               fileName,
    struct FSeqFileNameSizes* sizes,
    size_t                    max);

//! Compare FSeqFileNameSizes structs.
//! \return 1 if the structs are equal, otherwise 0
int fseqFileNameSizesCompare(const struct FSeqFileNameSizes*, const struct FSeqFileNameSizes*);

//! This sructure provides file name components.
struct FSeqFileName
{
    char* path;
    char* base;
    char* number;
    char* extension;
};

//! Initialize a FSeqFileName struct.
void fseqFileNameInit(struct FSeqFileName*);

//! Delete a FSeqFileName struct.
void fseqFileNameDel(struct FSeqFileName*);

//! Parse the file name components.
void fseqFileNameParse(const char*, struct FSeqFileName*);

//! Parse the file name components.
void fseqFileNameParse2(const char*, const struct FSeqFileNameSizes*, struct FSeqFileName*);

//! Test whether two file names are part of the same sequence (all components are equal
//! except for the number).
//! \return 1 if there is a match, otherwise 0
int fseqFileNameMatch(
    const char*,
    const struct FSeqFileNameSizes*,
    const char*,
    const struct FSeqFileNameSizes*);

//! Return the number of digits if the number is padded with zeroes.
int fseqPadding(const char*);

//! Return the number of digits if the number is padded with zeroes.
int fseqPadding2(const char*, const struct FSeqFileNameSizes*);

//! @}

//! \addtogroup Directories
//! @{

//! This struct provides a directory entry.
struct FSeqDirEntry
{
    struct FSeqFileName fileName;
    int frameMin;
    int frameMax;
    char framePadding;
    struct FSeqDirEntry* next;
};

//! Initialize a FSeqDirEntry struct.
void fseqDirEntryInit(struct FSeqDirEntry*);

//! Delete a FSeqDirEntry struct.
void fseqDirEntryDel(struct FSeqDirEntry*);

//! This struct provides directory listing options.
struct FSeqDirOptions
{
    char dotAndDotDotDirs;
    char dotFiles;
    char sequence;
};

//! Initialize a fseqDirOptionsInit struct.
void fseqDirOptionsInit(struct FSeqDirOptions*);

//! List the contents of a directory.
//! \arg path - The directory path
//! \arg options - The directory listing options, may be NULL
//! \return A list of directory entries
struct FSeqDirEntry* fseqDirList(const char* path, const struct FSeqDirOptions* options);

//! Delete a directory list.
void fseqDirListDel(struct FSeqDirEntry*);

//! @}

#endif // _FSEQ_H
