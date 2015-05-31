<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="cs_CZ">
<context>
    <name>djvConvertApplication</name>
    <message>
        <location filename="djv_convert.cpp" line="158"/>
        <source>-mirror_h</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="163"/>
        <source>-mirror_v</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="168"/>
        <source>-scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="174"/>
        <source>-scale_xy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="179"/>
        <source>-resize</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="184"/>
        <source>-width</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="189"/>
        <source>-height</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="194"/>
        <source>-crop</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="199"/>
        <source>-crop_percent</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="204"/>
        <source>-channel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="209"/>
        <source>-seq</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="210"/>
        <source>-q</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="218"/>
        <source>-layer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="223"/>
        <source>-proxy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="228"/>
        <source>-time</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="234"/>
        <source>-slate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="242"/>
        <source>-timeout</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="250"/>
        <source>-pixel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="257"/>
        <source>-speed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="264"/>
        <source>-tag</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="272"/>
        <source>-tags_auto</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="299"/>
        <source>input</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="312"/>
        <source>output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="330"/>
        <source>Cannot open input: &quot;%1&quot;</source>
        <oldsource>Cannot open image: &quot;%1&quot;</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="331"/>
        <source>Cannot open output: &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="332"/>
        <source>Cannot open slate: &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="333"/>
        <source>Cannot read input: &quot;%1&quot;</source>
        <oldsource>Cannot write image: &quot;%1&quot;</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="334"/>
        <source>Cannot write output: &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="343"/>
        <source>djv_convert

    This application provides a command line tool for processing images and movies.

Usage

    djv_convert (input) (output) [option]...

Conversion Options

    -mirror_h
    -mirror_v
        Mirror the image horizontally or vertically.
    -scale (value)
    -scale_xy (x) (y)
        Scale the image.
    -resize (width) (height)
        Resize the image.
    -width (value)
    -height (value)
        Resize the width or height of the image maintaining the aspect ratio.
    -crop (x) (y) (width) (height)
        Crop the image.
    -crop_percent (x) (y) (width) (height)
        Crop the image as a percentage.
    -channel (value)
        Show only specific image channels. Options = %1. Default = %2.
    -seq, -q (value)
        Set whether file sequencing is enabled. Options = %3. Default = %4.

Input Options

    -layer (value)
        Set the input layer.
    -proxy (value)
        Set the proxy scale. Options = %5. Default = %6.
    -time (start) (end)
        Set the start and end time.
    -slate (input) (frames)
        Set the slate.
    -timeout (value)
        Set the maximum number of seconds to wait for each input frame. Default = %7.

Output Options

    -pixel (value)
        Convert the pixel type. Options = %8.
    -speed (value)
        Set the speed. Options = %9.
    -tag (name) (value)
        Set an image tag.
    -tags_auto (value)
        Automatically generate image tags (e.g., timecode). Options = %10. Default = %11.
%12
Examples

    Convert an image:
    &gt; djv_convert input.sgi output.tga

    Convert an image sequence. Note that only the first frame of the output is specified:
    &gt; djv_convert input.1-100.sgi output.1.tga

    Create an RLE compressed image sequence:
    &gt; djv_convert input.1-100.sgi output.1.tga -targa_compression rle

    Convert an image sequence to a movie:
    &gt; djv_convert input.0001-0100.dpx output.mov

    Convert a movie to an image sequence:
    &gt; djv_convert input.mov output.1.tga

    Convert the pixel type:
    &gt; djv_convert input.sgi output.sgi -pixel &quot;rgb u16&quot;

    Scale an image by half:
    &gt; djv_convert input.tga output.tga -scale 0.5

    Resize an image:
    &gt; djv_convert input.tga output.tga -resize 2048 1556

    Convert a Cineon file to a linear format using the default settings:
    &gt; djv_convert input.cin output.tga

    Convert a Cineon file to a linear format using custom print settings (black point, white point, gamma, and soft clip):
    &gt; djv_convert input.cin output.tga -cineon_input_film_print 95 685 2.2 10
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="491"/>
        <source>Timeout...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="540"/>
        <location filename="djv_convert.cpp" line="637"/>
        <source>%1 %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="649"/>
        <source>Slating...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="866"/>
        <source>[%1%] Estimated = %2 (%3 Frames/Second)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="878"/>
        <source>[100%] </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="896"/>
        <source>Elapsed = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="djv_convert.cpp" line="906"/>
        <source>%1x%2:%3 %4 %5@%6</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
