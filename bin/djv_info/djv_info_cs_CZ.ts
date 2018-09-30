<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="cs_CZ">
<context>
    <name>djv::info::Application</name>
    <message>
        <location filename="InfoApplication.cpp" line="170"/>
        <source>Cannot open image: &quot;%1&quot;</source>
        <translation type="unfinished">Nelze otevřít obrázek: &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="186"/>
        <source>%1x%2:%3 %4 %5@%6</source>
        <translation type="unfinished">%1x%2:%3 %4 %5@%6</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="196"/>
        <location filename="InfoApplication.cpp" line="208"/>
        <location filename="InfoApplication.cpp" line="227"/>
        <source>%1 %2</source>
        <translation type="unfinished">%1 %2</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="203"/>
        <source>%1@%2</source>
        <translation type="unfinished">%1@%2</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="215"/>
        <source>    %1. %2</source>
        <translation type="unfinished">    %1. %2</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="221"/>
        <source>%1x%2:%3 %4</source>
        <translation type="unfinished">%1x%2:%3 %4</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="239"/>
        <source>Layer = %1</source>
        <translation type="unfinished">Vrstva = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="241"/>
        <source>  Width = %1</source>
        <translation type="unfinished">  Šířka = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="243"/>
        <source>  Height = %1</source>
        <translation type="unfinished">  Výška = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="245"/>
        <source>  Aspect = %1</source>
        <translation type="unfinished">  Poměr = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="249"/>
        <source>  Pixel = %1</source>
        <translation type="unfinished">  Pixel = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="252"/>
        <source>Start = %1</source>
        <translation type="unfinished">  Začátek = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="256"/>
        <source>End = %1</source>
        <translation type="unfinished">  Konec = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="260"/>
        <source>Duration = %1</source>
        <translation type="unfinished">Doba trvání = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="264"/>
        <source>Speed = %1</source>
        <translation type="unfinished">Rychlost = %1</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="269"/>
        <source>Tag %1 = %2</source>
        <translation type="unfinished">Značka %1 = %2</translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="297"/>
        <source>%1:</source>
        <translation type="unfinished">%1:</translation>
    </message>
</context>
<context>
    <name>djv::info::Context</name>
    <message>
        <location filename="InfoContext.cpp" line="112"/>
        <source>-x_info</source>
        <translation type="unfinished">-x_info</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="113"/>
        <source>-xi</source>
        <translation type="unfinished">-xi</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="118"/>
        <source>-verbose</source>
        <translation type="unfinished">-verbose</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="119"/>
        <source>-v</source>
        <translation type="unfinished">-v</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="124"/>
        <source>-file_path</source>
        <translation type="unfinished">-file_path</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="125"/>
        <source>-fp</source>
        <translation type="unfinished">-fp</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="130"/>
        <source>-seq</source>
        <translation type="unfinished">-seq</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="131"/>
        <source>-q</source>
        <translation type="unfinished">-q</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="136"/>
        <source>-recurse</source>
        <translation type="unfinished">-recurse</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="137"/>
        <source>-r</source>
        <translation type="unfinished">-r</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="142"/>
        <source>-columns</source>
        <translation type="unfinished">-columns</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="143"/>
        <source>-c</source>
        <translation type="unfinished">-c</translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="165"/>
        <source>djv_info

    The djv_info application is a command line tool for displaying information about images and movies.

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

    image     - One or more images, image sequences, or movies
    directory - One or more directories
    option    - Additional options (see below)

    If no images or directories are given then the current directory will be used.

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
<context>
    <name>djvInfoApplication</name>
    <message>
        <source>Cannot open image: &quot;%1&quot;</source>
        <translation type="vanished">Nelze otevřít obrázek: &quot;%1&quot;</translation>
    </message>
    <message>
        <source>%1x%2:%3 %4 %5@%6</source>
        <translation type="vanished">%1x%2:%3 %4 %5@%6</translation>
    </message>
    <message>
        <source>%1 %2</source>
        <translation type="vanished">%1 %2</translation>
    </message>
    <message>
        <source>%1@%2</source>
        <translation type="vanished">%1@%2</translation>
    </message>
    <message>
        <source>    %1. %2</source>
        <translation type="vanished">    %1. %2</translation>
    </message>
    <message>
        <source>%1x%2:%3 %4</source>
        <translation type="vanished">%1x%2:%3 %4</translation>
    </message>
    <message>
        <source>Layer = %1</source>
        <translation type="vanished">Vrstva = %1</translation>
    </message>
    <message>
        <source>  Width = %1</source>
        <translation type="vanished">  Šířka = %1</translation>
    </message>
    <message>
        <source>  Height = %1</source>
        <translation type="vanished">  Výška = %1</translation>
    </message>
    <message>
        <source>  Aspect = %1</source>
        <translation type="vanished">  Poměr = %1</translation>
    </message>
    <message>
        <source>  Pixel = %1</source>
        <translation type="vanished">  Pixel = %1</translation>
    </message>
    <message>
        <source>Start = %1</source>
        <translation type="vanished">  Začátek = %1</translation>
    </message>
    <message>
        <source>End = %1</source>
        <translation type="vanished">  Konec = %1</translation>
    </message>
    <message>
        <source>Duration = %1</source>
        <translation type="vanished">Doba trvání = %1</translation>
    </message>
    <message>
        <source>Speed = %1</source>
        <translation type="vanished">Rychlost = %1</translation>
    </message>
    <message>
        <source>Tag %1 = %2</source>
        <translation type="vanished">Značka %1 = %2</translation>
    </message>
    <message>
        <source>%1:</source>
        <translation type="vanished">%1:</translation>
    </message>
</context>
<context>
    <name>djvInfoContext</name>
    <message>
        <source>-x_info</source>
        <translation type="vanished">-x_info</translation>
    </message>
    <message>
        <source>-xi</source>
        <translation type="vanished">-xi</translation>
    </message>
    <message>
        <source>-verbose</source>
        <translation type="vanished">-verbose</translation>
    </message>
    <message>
        <source>-v</source>
        <translation type="vanished">-v</translation>
    </message>
    <message>
        <source>-file_path</source>
        <translation type="vanished">-file_path</translation>
    </message>
    <message>
        <source>-fp</source>
        <translation type="vanished">-fp</translation>
    </message>
    <message>
        <source>-seq</source>
        <translation type="vanished">-seq</translation>
    </message>
    <message>
        <source>-q</source>
        <translation type="vanished">-q</translation>
    </message>
    <message>
        <source>-recurse</source>
        <translation type="vanished">-recurse</translation>
    </message>
    <message>
        <source>-r</source>
        <translation type="vanished">-r</translation>
    </message>
    <message>
        <source>-columns</source>
        <translation type="vanished">-columns</translation>
    </message>
    <message>
        <source>-c</source>
        <translation type="vanished">-c</translation>
    </message>
</context>
</TS>
