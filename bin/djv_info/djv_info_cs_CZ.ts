<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="cs_CZ">
<context>
    <name>djvInfoApplication</name>
    <message>
        <location filename="djv_info.cpp" line="192"/>
        <source>-x_info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="192"/>
        <source>-xi</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="196"/>
        <source>-verbose</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="196"/>
        <source>-v</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="200"/>
        <source>-file_path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="200"/>
        <source>-fp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="204"/>
        <source>-seq</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="204"/>
        <source>-q</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="208"/>
        <source>-recurse</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="208"/>
        <source>-r</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="212"/>
        <source>-columns</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="212"/>
        <source>-c</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="234"/>
        <source>Cannot open image: &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="243"/>
        <source>djv_info

    This application provides a command line tool for displaying information about images and movies.

    Example output:

    yesterdayview.mov                    640x424:1.51 RGB U8 00:02:00:01@12
    dlad.dpx                          2048x1556:1.32 RGB U10 00:00:00:01@24
    render0001-1000.exr                                      00:00:41:16@24
        0: A,B,G,R                                     720x480:1.5 RGBA F16
        1: Ambient                                      720x480:1.5 RGB F16
        2: Diffuse                                      720x480:1.5 RGB F16
        3: Specular                                     720x480:1.5 RGB F16
        4: Z                                              720x480:1.5 L F32

    Key:

    (name)     (width)x(height):(aspect) (format) (type) (duration)@(speed)

    Layer key:

    (name)                                               (duration)@(speed)
        (layer): (name)           (width)x(height):(aspect) (format) (type)

Usage

    djv_info [image|directory]... [option]...

Options

    -x_info, -xi
        Don&apos;t show image information, only file names.
    -verbose, -v
        Show verbose image information.
    -file_path, -fp
        Show file path names.
    -seq, -q (value)
        Set file sequencing. Options = %1. Default = %2.
    -recurse, -r
        Descend into sub-directories.
    -columns, -c (value)
        Set the number of columns for formatting output. A value of zero disables formatting.
%3
Examples

    Display image information:
    &gt; djv_info image.sgi image2.sgi

    Display image sequence information:
    &gt; djv_info image.1-100.sgi

    Display information about all images within a directory:
    &gt; djv_info ~/pics
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="347"/>
        <source>%1x%2:%3 %4 %5@%6</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="357"/>
        <location filename="djv_info.cpp" line="371"/>
        <location filename="djv_info.cpp" line="389"/>
        <source>%1 %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="365"/>
        <source>%1@%2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="379"/>
        <source>    %1. %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="383"/>
        <source>%1x%2:%3 %4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="403"/>
        <source>Layer = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="404"/>
        <source>  Width = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="405"/>
        <source>  Height = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="406"/>
        <source>  Aspect = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="408"/>
        <source>  Pixel = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="412"/>
        <source>Start = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="417"/>
        <source>End = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="422"/>
        <source>Duration = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="427"/>
        <source>Speed = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="434"/>
        <source>Tag %1 = %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_info.cpp" line="467"/>
        <source>%1:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
