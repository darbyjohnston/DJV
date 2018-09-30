<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>djv::ls::Application</name>
    <message>
        <location filename="LsApplication.cpp" line="106"/>
        <location filename="LsApplication.cpp" line="145"/>
        <source>Cannot open: &quot;%1&quot;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="200"/>
        <source>%1 %2 %3 %4</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="202"/>
        <source>%1 %2 %3 %4 %5</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="232"/>
        <source>%1 %2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="258"/>
        <source>%1:</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>djv::ls::Context</name>
    <message>
        <location filename="LsContext.cpp" line="136"/>
        <source>-x_info</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="137"/>
        <source>-xi</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="142"/>
        <source>-file_path</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="143"/>
        <source>-fp</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="148"/>
        <source>-seq</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="149"/>
        <source>-q</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="154"/>
        <source>-recurse</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="155"/>
        <source>-r</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="160"/>
        <source>-hidden</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="165"/>
        <source>-columns</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="166"/>
        <source>-c</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="173"/>
        <source>-sort</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="174"/>
        <source>-s</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="179"/>
        <source>-reverse_sort</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="180"/>
        <source>-rs</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="185"/>
        <source>-x_sort_dirs</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="186"/>
        <source>-xsd</source>
        <translation></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="208"/>
        <source>djv_ls

    The djv_ls application is a command line tool for listing directories with file sequences.

    Example output:

    el_cerrito_bart.1k.tiff   File 2.23MB darby rw Mon Jun 12 21:21:55 2006
    richmond_train.2k.tiff    File 8.86MB darby rw Mon Jun 12 21:21:58 2006
    fishpond.1-749.png       Seq 293.17MB darby rw Thu Aug 17 16:47:43 2006

    Key:

    (name)                        (type) (size) (user) (permissions) (time)

    File types:                   Permissions:

    File - Regular file           r - Readable
    Seq  - File sequence          w - Writable
    Dir  - Directory              x - Executable

Usage

    djv_ls [file|directory]... [option]...

    file      - One or more files or image sequences
    directory - One or more directories
    option    - Additional options (see below)

    If no files or directories are given then the current directory will be used.

Options

    -x_info, -xi
        Don&apos;t show information, only file names.
    -file_path, -fp
        Show file path names.
    -seq, -q (value)
        Set file sequencing. Options = %1. Default = %2.
    -recurse, -r
        Descend into sub-directories.
    -hidden
        Show hidden files.
    -columns, -c (value)
        Set the number of columns for formatting output. A value of zero disables formatting.

Sorting Options

    -sort, -s (value)
        Set the sorting. Options = %3. Default = %4.
    -reverse_sort, -rs
        Reverse the sorting order.
    -x_sort_dirs, -xsd
        Don&apos;t sort directories first.
%5
Examples

    List the current directory:

    &gt; djv_ls

    List specific directories:

    &gt; djv_ls ~/movies ~/pictures

    Sort by time with the most recent first:

    &gt; djv_ls -sort time -reverse_sort
</source>
        <translation></translation>
    </message>
</context>
</TS>
