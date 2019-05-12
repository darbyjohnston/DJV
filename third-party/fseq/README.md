FSeq
====
FSeq is a C library for working with numbered sequences of files. File
sequences are commonly used in the VFX and film industry to represent
individual frames of an animation or film clip. For example the files:
* render.0001.exr
* render.0002.exr
* render.0003.exr
* render.0004.exr
* render.0005.exr

Can be represented by the file sequence:
* render.0001-0005.exr

This makes browsing much easier when you have hundreds or thousands of
files.

FSeq compiles on Linux, OS X, and Windows. A CMakeLists.txt is provided,
but the library can be trivially included in other build systems since it
consists of a single .h and .c file.

FSeq is open source software distributed under a BSD style license. See
the file LICENSE.txt for details.

Example splitting a file name into components:
    
	#include <fseq/fseq.h>
    ...
    struct FSeqFileName fileName;
    fseqFileNameInit(&fileName);
    fseqFileNameSplit("/tmp/render0100.exr", &fileName);
    printf("%s, %s, %s, %s\n", fileName.path, fileName.base, fileName.number, fileName.extension);
    fseqFileNameDel(&fileName);

Output:

    /tmp/, render, 0100, .exr

Example listing the contents of a directory:

    #include <fseq/fseq.h>
    ...
    struct FSeqDirEntry* entries = fseqDirList("/tmp/", NULL, NULL);
    struct FSeqDirEntry* entry = entries;
    while (entry)
    {
        static char buf[FSEQ_STRING_LEN];
        fseqDirEntryToString(entry, buf, FSEQ_FALSE, FSEQ_STRING_LEN);
        printf("%s\n", buf);
        entry = entry->next;
    }
    fseqDirListDel(entries);

Output:

    render.rgba.0001-0100.tif
    render.normals.0001-0100.tif
    render.z.0001-0100.tif

Listing a directory with 100K files on a Raspberry Pi 3 B+:

    pi@raspberrypi:~/dev/fseq-Release $ time ./fseqls ~/Desktop/Seq/Big/
    shot51_scene95_normals.00916-10915.tif
    shot97_scene29_normals.00771-10770.tif
    shot19_scene33_rgba.00768-10767.tif
    shot21_scene51_rgba.00839-10838.tif
    shot24_scene13_normals.00804-10803.tif
    shot29_scene63_rgba.00524-10523.tif
    shot40_scene12_normals.00108-10107.tif
    shot71_scene14_rgba.00606-10605.tif
    shot39_scene78_rgba.00798-10797.tif
    shot55_scene47_normals.00628-10627.tif

    real    0m0.315s
    user    0m0.142s
    sys     0m0.171s
