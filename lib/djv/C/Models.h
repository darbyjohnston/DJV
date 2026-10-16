// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the DJV project.

#pragma once

//! \file Models.h
//! A C interface to DJV's models, over tlRender's C interface to the
//! engine: the color, audio and viewport models, with their settings,
//! applied to a view and a player. For Swift and any other language that
//! speaks C. Everything here is called on the main thread.
//!
//! Strings returned are the models' and stay valid until the next call
//! that changes the model they came from. Changes are counted: a caller
//! that polls compares the generation and reads again when it moved.

#include <tlRender/C/Engine.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! The models: created on an engine with a settings file, bound to the
//! view they are drawn with and the player they apply to.
typedef struct djv_models djv_models;

TL_TIMELINE_API djv_models* djv_models_create(tl_engine*, const char* settingsPath);
TL_TIMELINE_API void djv_models_destroy(djv_models*);

//! Write the settings.
TL_TIMELINE_API void djv_models_save(djv_models*);

//! The view the color and viewport options are drawn with; NULL unbinds.
TL_TIMELINE_API void djv_models_bind_view(djv_models*, tl_view*);

//! The player the audio applies to and the file the color is resolved
//! for; NULL for none.
TL_TIMELINE_API void djv_models_set_player(djv_models*, tl_player*);

//! Moves on every change to any model, from any side.
TL_TIMELINE_API uint64_t djv_models_get_generation(djv_models*);

//! \name OpenColorIO
///@{

//! Whether OpenColorIO is built in: the rest of this section does
//! nothing without it.
TL_TIMELINE_API int djv_ocio_is_available(void);
TL_TIMELINE_API int djv_ocio_is_enabled(djv_models*);
TL_TIMELINE_API void djv_ocio_set_enabled(djv_models*, int);

//! Where the configuration comes from.
typedef enum
{
    DJV_OCIO_CONFIG_BUILT_IN,
    DJV_OCIO_CONFIG_ENV,
    DJV_OCIO_CONFIG_FILE
} djv_ocio_config;
TL_TIMELINE_API djv_ocio_config djv_ocio_get_config(djv_models*);
TL_TIMELINE_API void djv_ocio_set_config(djv_models*, djv_ocio_config);
TL_TIMELINE_API const char* djv_ocio_get_file_name(djv_models*);
TL_TIMELINE_API void djv_ocio_set_file_name(djv_models*, const char*);
//! The configuration's own name.
TL_TIMELINE_API const char* djv_ocio_get_name(djv_models*);

//! The choices the configuration offers, and which is chosen.
TL_TIMELINE_API size_t djv_ocio_get_input_count(djv_models*);
TL_TIMELINE_API const char* djv_ocio_get_input(djv_models*, size_t);
TL_TIMELINE_API size_t djv_ocio_get_input_index(djv_models*);
TL_TIMELINE_API void djv_ocio_set_input_index(djv_models*, size_t);
TL_TIMELINE_API size_t djv_ocio_get_display_count(djv_models*);
TL_TIMELINE_API const char* djv_ocio_get_display(djv_models*, size_t);
TL_TIMELINE_API size_t djv_ocio_get_display_index(djv_models*);
TL_TIMELINE_API void djv_ocio_set_display_index(djv_models*, size_t);
TL_TIMELINE_API size_t djv_ocio_get_view_count(djv_models*);
TL_TIMELINE_API const char* djv_ocio_get_view(djv_models*, size_t);
TL_TIMELINE_API size_t djv_ocio_get_view_index(djv_models*);
TL_TIMELINE_API void djv_ocio_set_view_index(djv_models*, size_t);
TL_TIMELINE_API size_t djv_ocio_get_look_count(djv_models*);
TL_TIMELINE_API const char* djv_ocio_get_look(djv_models*, size_t);
TL_TIMELINE_API size_t djv_ocio_get_look_index(djv_models*);
TL_TIMELINE_API void djv_ocio_set_look_index(djv_models*, size_t);

//! The input color space resolved for the player's file and where it
//! came from, for display: "sRGB (extension)"; empty when nothing
//! resolves and the chosen input is used.
TL_TIMELINE_API const char* djv_color_get_resolved_input(djv_models*);

///@}

//! \name LUT
///@{

TL_TIMELINE_API int djv_lut_is_enabled(djv_models*);
TL_TIMELINE_API void djv_lut_set_enabled(djv_models*, int);
TL_TIMELINE_API const char* djv_lut_get_file_name(djv_models*);
TL_TIMELINE_API void djv_lut_set_file_name(djv_models*, const char*);
typedef enum
{
    DJV_LUT_ORDER_POST_CONFIG,
    DJV_LUT_ORDER_PRE_CONFIG
} djv_lut_order;
TL_TIMELINE_API djv_lut_order djv_lut_get_order(djv_models*);
TL_TIMELINE_API void djv_lut_set_order(djv_models*, djv_lut_order);
typedef enum
{
    DJV_LUT_DIRECTION_FORWARD,
    DJV_LUT_DIRECTION_INVERSE
} djv_lut_direction;
TL_TIMELINE_API djv_lut_direction djv_lut_get_direction(djv_models*);
TL_TIMELINE_API void djv_lut_set_direction(djv_models*, djv_lut_direction);

///@}

//! \name Audio
///@{

TL_TIMELINE_API float djv_audio_get_volume(djv_models*);
TL_TIMELINE_API void djv_audio_set_volume(djv_models*, float);
TL_TIMELINE_API int djv_audio_is_muted(djv_models*);
TL_TIMELINE_API void djv_audio_set_muted(djv_models*, int);
//! Seconds the audio is moved by against the video.
TL_TIMELINE_API double djv_audio_get_sync_offset(djv_models*);
TL_TIMELINE_API void djv_audio_set_sync_offset(djv_models*, double);

///@}

//! \name Viewport
///@{

//! How the picture is shown: the viewport model's display options, as
//! plain numbers.
typedef struct djv_display_options
{
    //! 0 color, 1 red, 2 green, 3 blue, 4 alpha.
    int channels;
    int mirrorX;
    int mirrorY;
    int negative;

    int colorEnabled;
    float add[3];
    float brightness[3];
    float contrast[3];
    float saturation[3];
    float hue;

    int levelsEnabled;
    float levelsInLow;
    float levelsInHigh;
    float levelsGamma;
    float levelsOutLow;
    float levelsOutHigh;

    int exposureEnabled;
    float exposure;

    int softClipEnabled;
    float softClip;
} djv_display_options;
TL_TIMELINE_API djv_display_options djv_viewport_get_display(djv_models*);
TL_TIMELINE_API void djv_viewport_set_display(djv_models*, djv_display_options);

//! How the image is sampled and blended.
typedef struct djv_image_options
{
    //! 0 from the file, 1 full range, 2 legal range.
    int videoLevels;
    //! 0 none, 1 straight, 2 premultiplied.
    int alphaBlend;
    //! 0 nearest, 1 linear, 2 high quality.
    int minify;
    int magnify;
} djv_image_options;
TL_TIMELINE_API djv_image_options djv_viewport_get_image(djv_models*);
TL_TIMELINE_API void djv_viewport_set_image(djv_models*, djv_image_options);

//! What is behind the picture.
typedef struct djv_background_options
{
    //! 0 solid, 1 checkers, 2 gradient.
    int type;
    float solid[4];
    float checkers0[4];
    float checkers1[4];
    int checkersSize;
    float gradient0[4];
    float gradient1[4];
    //! The outline around the picture.
    int outlineEnabled;
    int outlineWidth;
    float outlineColor[4];
} djv_background_options;
TL_TIMELINE_API djv_background_options djv_viewport_get_background(djv_models*);
TL_TIMELINE_API void djv_viewport_set_background(djv_models*, djv_background_options);

//! What is drawn over the picture: the grid, the center marker, the
//! indicator for a missing frame, the clipping warning.
typedef struct djv_foreground_options
{
    int gridEnabled;
    //! 0 cells of a size, 1 a count of cells.
    int gridCellMode;
    int gridCellSize;
    int gridCellCountX;
    int gridCellCountY;
    int gridLineWidth;
    float gridColor[4];
    //! 0 none, 1 pixels, 2 alphanumeric.
    int gridLabels;

    int centerMarkerEnabled;
    int centerMarkerSize;
    int centerMarkerWidth;
    float centerMarkerColor[4];

    int missingEnabled;
    int missingWidth;
    float missingColor[4];

    int clippingEnabled;
    //! 0 any channel, 1 all channels, 2 luminance.
    int clippingMode;
    float clippingLow;
    float clippingHigh;
} djv_foreground_options;
TL_TIMELINE_API djv_foreground_options djv_viewport_get_foreground(djv_models*);
TL_TIMELINE_API void djv_viewport_set_foreground(djv_models*, djv_foreground_options);

//! The aspect ratios offered, as labels, and which is chosen; the first
//! is the picture's own.
TL_TIMELINE_API size_t djv_viewport_get_aspect_ratio_count(djv_models*);
TL_TIMELINE_API const char* djv_viewport_get_aspect_ratio(djv_models*, size_t);
TL_TIMELINE_API size_t djv_viewport_get_aspect_ratio_index(djv_models*);
TL_TIMELINE_API void djv_viewport_set_aspect_ratio_index(djv_models*, size_t);

///@}

#ifdef __cplusplus
}
#endif
