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
#include <stdint.h>

//! \mainpage
//! FSeq is a C library for working with numbered sequences of files. File
//! sequences are commonly used in the VFX and film industry to represent
//! individual frames of an animation or film clip.
//!
//! For example the files:
//! - render.0001.exr
//! - render.0002.exr
//! - render.0003.exr
//! - render.0004.exr
//! - render.0005.exr
//!
//! Can be represented by the file sequence:
//! - render.0001-0005.exr
//!
//! This makes browsing much easier when you have hundreds or thousands of
//! files.
//!
//! Example splitting a file name into components:
//! \code
//! struct FSeqFileName fileName;
//! fseqFileNameInit(&fileName);
//! fseqFileNameSplit("/tmp/render0100.exr", &fileName);
//! printf("%s, %s, %s, %s\n", fileName.path, fileName.base, fileName.number, fileName.extension);
//! fseqFileNameDel(&fileName);
//! \endcode
//!
//! Output:
//! \code
//! /tmp/, render, 0100, .exr
//! \endcode
//!
//! Example listing the contents of a directory:
//! \code
//! struct FSeqDirEntry* entries = fseqDirList("/tmp/", NULL, NULL);
//! struct FSeqDirEntry* entry = entries;
//! while (entry)
//! {
//!     static char buf[FSEQ_STRING_LEN];
//!     fseqDirEntryToString(entry, buf, FSEQ_FALSE, FSEQ_STRING_LEN);
//!     printf("%s\n", buf);
//!     entry = entry->next;
//! }
//! fseqDirListDel(entries);
//! \endcode
//!
//! Output:
//! \code
//! render.rgba.0001-0100.tif
//! render.normals.0001-0100.tif
//! render.z.0001-0100.tif
//! \endcode
//!
//! Listing a directory with 100K files on a Raspberry Pi 3 B+:
//! \code
//! pi@raspberrypi:~/dev/fseq-Release $ time ./fseqls ~/Desktop/Seq/Big/
//! shot51_scene95_normals.00916-10915.tif
//! shot97_scene29_normals.00771-10770.tif
//! shot19_scene33_rgba.00768-10767.tif
//! shot21_scene51_rgba.00839-10838.tif
//! shot24_scene13_normals.00804-10803.tif
//! shot29_scene63_rgba.00524-10523.tif
//! shot40_scene12_normals.00108-10107.tif
//! shot71_scene14_rgba.00606-10605.tif
//! shot39_scene78_rgba.00798-10797.tif
//! shot55_scene47_normals.00628-10627.tif
//! 
//! real    0m0.315s
//! user    0m0.142s
//! sys     0m0.171s
//! \endcode

//------------------------------------------------------------------------------
//! \addtogroup Util
//! @{

//! Boolean type.
typedef char FSeqBool;

//! Boolean true value.
#define FSEQ_TRUE 1

//! Boolean false value.
#define FSEQ_FALSE 0

//! The default string length.
#define FSEQ_STRING_LEN 4096

//! Get the minimum of two values.
#define FSEQ_MIN(A, B) (A < B ? A : B)

//! Get the maximum of two values.
#define FSEQ_MAX(A, B) (A > B ? A : B)

//! @}

//------------------------------------------------------------------------------
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
FSeqBool fseqFileNameSizesCompare(
    const struct FSeqFileNameSizes*,
    const struct FSeqFileNameSizes*);

//! This sruct provides file name components.
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

//! Split a file name into components.
void fseqFileNameSplit(const char*, struct FSeqFileName*, size_t max);

//! Split a file name into components.
FSeqBool fseqFileNameSplit2(
    const char*,
    const struct FSeqFileNameSizes*,
    struct FSeqFileName*);

//! Test whether two file names are part of the same sequence (all components
//! are equal except for the number).
FSeqBool fseqFileNameMatch(
    const char*,
    const struct FSeqFileNameSizes*,
    const char*,
    const struct FSeqFileNameSizes*);

//! @}

//------------------------------------------------------------------------------
//! \addtogroup Directories
//! @{

//! This struct provides a directory entry.
struct FSeqDirEntry
{
    struct FSeqFileName  fileName;
    int64_t              frameMin;
    int64_t              frameMax;
    FSeqBool             hasFramePadding;
    char                 framePadding;
    struct FSeqDirEntry* next;
};

//! Initialize a FSeqDirEntry struct.
void fseqDirEntryInit(struct FSeqDirEntry*);

//! Delete a FSeqDirEntry struct.
void fseqDirEntryDel(struct FSeqDirEntry*);

//! Convert a directory entry to a string.
//! \arg entry - The directory entry
//! \arg out - The output string
//! \arg path - Whether to include the path
//! \arg max - The maximum output string length
void fseqDirEntryToString(
    const struct FSeqDirEntry* entry,
    char*                      out,
    FSeqBool                   path,
    size_t                     max);

//! This struct provides directory listing options.
struct FSeqDirOptions
{
    FSeqBool dotAndDotDotDirs;
    FSeqBool dotFiles;
    FSeqBool sequence;
};

//! Initialize a fseqDirOptionsInit struct.
void fseqDirOptionsInit(struct FSeqDirOptions*);

//! List the contents of a directory. Use fseqDirListDel() to delete the list.
//! \arg path - The directory path
//! \arg options - The directory listing options, may also pass NULL instead
//! \arg error - Whether any erros occurred, may also pass NULL instead
//! \return A list of directory entries
struct FSeqDirEntry* fseqDirList(
    const char*                  path,
    const struct FSeqDirOptions* options,
    FSeqBool*                    error);

//! Delete a directory list.
void fseqDirListDel(struct FSeqDirEntry*);

//! @}

#endif // _FSEQ_H
