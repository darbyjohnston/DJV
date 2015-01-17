<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="cs_CZ">
<context>
    <name>djvInfoApplication</name>
    <message>
        <source>-x_info</source>
        <translation>-x_info</translation>
    </message>
    <message>
        <source>-xi</source>
        <translation>-xi</translation>
    </message>
    <message>
        <source>-verbose</source>
        <translation>-verbose</translation>
    </message>
    <message>
        <source>-v</source>
        <translation>-v</translation>
    </message>
    <message>
        <source>-file_path</source>
        <translation>-file_path</translation>
    </message>
    <message>
        <source>-fp</source>
        <translation>-fp</translation>
    </message>
    <message>
        <source>-seq</source>
        <translation>-seq</translation>
    </message>
    <message>
        <source>-q</source>
        <translation>-q</translation>
    </message>
    <message>
        <source>-recurse</source>
        <translation>-recurse</translation>
    </message>
    <message>
        <source>-r</source>
        <translation>-r</translation>
    </message>
    <message>
        <source>-columns</source>
        <translation>-columns</translation>
    </message>
    <message>
        <source>-c</source>
        <translation>-c</translation>
    </message>
    <message>
        <source>Input</source>
        <translation type="obsolete">Vstup</translation>
    </message>
    <message>
        <source>Cannot open directory: &quot;%1&quot;</source>
        <translation type="obsolete">Nelze otevřít adresář: &quot;%1&quot;</translation>
    </message>
    <message>
        <source>%1x%2:%3 %4 %5@%6</source>
        <translation>%1x%2:%3 %4 %5@%6</translation>
    </message>
    <message>
        <source>%1 %2</source>
        <translation>%1 %2</translation>
    </message>
    <message>
        <source>%1@%2</source>
        <translation>%1@%2</translation>
    </message>
    <message>
        <source>    %1. %2</source>
        <translation>    %1. %2</translation>
    </message>
    <message>
        <source>%1x%2:%3 %4</source>
        <translation>%1x%2:%3 %4</translation>
    </message>
    <message>
        <source>Layer = %1</source>
        <translation>Vrstva = %1</translation>
    </message>
    <message>
        <source>  Width = %1</source>
        <translation>  Šířka = %1</translation>
    </message>
    <message>
        <source>  Height = %1</source>
        <translation>  Výška = %1</translation>
    </message>
    <message>
        <source>  Aspect = %1</source>
        <translation>  Poměr = %1</translation>
    </message>
    <message>
        <source>  Pixel = %1</source>
        <translation>  Pixel = %1</translation>
    </message>
    <message>
        <source>Start = %1</source>
        <translation>  Začátek = %1</translation>
    </message>
    <message>
        <source>End = %1</source>
        <translation>  Konec = %1</translation>
    </message>
    <message>
        <source>Duration = %1</source>
        <translation>Doba trvání = %1</translation>
    </message>
    <message>
        <source>Speed = %1</source>
        <translation>Rychlost = %1</translation>
    </message>
    <message>
        <source>Tag %1 = %2</source>
        <translation>Značka %1 = %2</translation>
    </message>
    <message>
        <source>%1:</source>
        <translation>%1:</translation>
    </message>
    <message>
        <source>Cannot open image: &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
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
</context>
</TS>
