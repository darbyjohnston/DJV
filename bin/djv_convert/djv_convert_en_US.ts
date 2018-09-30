<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>djv::convert::Application</name>
    <message>
        <location filename="ConvertApplication.cpp" line="78"/>
        <source>Cannot open input: &quot;%1&quot;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="79"/>
        <source>Cannot open output: &quot;%1&quot;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="80"/>
        <source>Cannot open slate: &quot;%1&quot;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="81"/>
        <source>Cannot read input: &quot;%1&quot;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="82"/>
        <source>Cannot write output: &quot;%1&quot;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="129"/>
        <source>Timeout...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="171"/>
        <location filename="ConvertApplication.cpp" line="254"/>
        <source>%1 %2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="264"/>
        <source>Slating...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="429"/>
        <source>[%1%] Estimated = %2 (%3 Frames/Second)</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="441"/>
        <source>[100%] </source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="458"/>
        <source>Elapsed = %1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertApplication.cpp" line="470"/>
        <source>%1x%2:%3 %4 %5@%6</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>djv::convert::Context</name>
    <message>
        <location filename="ConvertContext.cpp" line="111"/>
        <source>-mirror_h</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="116"/>
        <source>-mirror_v</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="121"/>
        <source>-scale</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="127"/>
        <source>-scale_separate</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="132"/>
        <source>-resize</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="137"/>
        <source>-width</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="142"/>
        <source>-height</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="147"/>
        <source>-crop</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="152"/>
        <source>-crop_percent</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="157"/>
        <source>-channel</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="162"/>
        <source>-seq</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="163"/>
        <source>-q</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="170"/>
        <source>-layer</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="175"/>
        <source>-proxy</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="180"/>
        <source>-time</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="186"/>
        <source>-slate</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="194"/>
        <source>-timeout</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="201"/>
        <source>-pixel</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="208"/>
        <source>-speed</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="215"/>
        <source>-tag</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="223"/>
        <source>-tags_auto</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="248"/>
        <source>input</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="257"/>
        <source>output</source>
        <translation></translation>
    </message>
    <message>
        <location filename="ConvertContext.cpp" line="272"/>
        <source>djv_convert

    The djv_convert application is a command line tool for batch processing images and movies. Common uses include resizing images and converting sequences of images to a QuickTime movie.

    Here is an example of how to resize a sequence of images:

    &gt; djv_convert input.1-100.tga output.1.tga -resize 2048 1556

    Note that only the first frame of the output sequence needs to be specified, the remaining frames will be added automatically. See below for more examples.

    Note also that the djv_convert application uses OpenGL for image processing so it needs to be run on a machine with graphics resources.

Usage

    djv_convert (input) (output) [option]...

    input  - Input image, image sequence, or movie
    output - Output image, image sequence, or movie
    option - Additional options (see below)

Conversion Options

    -mirror_h
        Mirror the image horizontally.
    -mirror_v
        Mirror the image vertically.
    -scale (value)
        Scale the image width and height using a floating point value         (1.0 = 100%).
    -scale_separate (width) (height)
        Scale the image width and height separately using a floating point value (1.0 = 100%).
    -resize (width) (height)
        Resize the image width and height to the given resolution.
    -width (value)
        Resize the image width to the given resolution maintaining the aspect ratio.
    -height (value)
        Resize the image height to the given resolution maintaining the aspect ratio.
    -crop (x) (y) (width) (height)
        Crop the image.
    -crop_percent (x) (y) (width) (height)
        Crop the image using percentages.
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

    Convert an image sequence. Note that only the first frame of the output sequence needs to be specified, the remaining frames will be added automatically:

    &gt; djv_convert input.1-100.sgi output.1.tga

    Create an RLE compressed image sequence:

    &gt; djv_convert input.1-100.sgi output.1.tga -targa_compression RLE

    Convert an image sequence to a movie:

    &gt; djv_convert input.0001-0100.dpx output.m4v

    Convert a movie to an image sequence:

    &gt; djv_convert input.m4v output.1.tga

    Convert the pixel type. Note the use of the quotes around the pixel typeoption:

    &gt; djv_convert input.sgi output.sgi -pixel &quot;RGB U16&quot;

    Scale an image by half:

    &gt; djv_convert input.tga output.tga -scale 0.5

    Resize an image:

    &gt; djv_convert input.tga output.tga -resize 2048 1556

    Convert a Cineon file to a linear format using the default settings:

    &gt; djv_convert input.cin output.tga

    Convert a Cineon file to a linear format using custom film print settings (black point, white point, gamma, and soft clip):

    &gt; djv_convert input.cin output.tga -cineon_input_film_print 95 685 2.2 2
</source>
        <translation></translation>
    </message>
</context>
</TS>
