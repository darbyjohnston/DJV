//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#pragma once

#include <djvViewLib/ViewLib.h>

#include <djvCore/Error.h>
#include <djvCore/StringUtil.h>
#include <djvCore/Vector.h>

#include <QMetaType>

#if defined(DJV_WINDOWS)
//! \todo Windows defines the macro "FILE_OPEN".
#undef FILE_OPEN
#endif

namespace djv
{
    namespace AV
    {
        class Color;
    
    } // namespace Color

    namespace ViewLib
    {
        //! This class provides enumerations.
        class Enum
        {
            Q_GADGET

        public:
            virtual ~Enum() = 0;

            //! This enumeration provides the maximum view size.
            enum VIEW_MAX
            {
                VIEW_MAX_NONE,
                VIEW_MAX_25,
                VIEW_MAX_50,
                VIEW_MAX_75,
                VIEW_MAX_USER,

                VIEW_MAX_COUNT
            };
            Q_ENUM(VIEW_MAX);

            //! Get the maximum view size labels.
            static const QStringList & viewMaxLabels();

            //! This enumeration provides the full screen user interface options.
            enum FULL_SCREEN_UI
            {
                FULL_SCREEN_UI_HIDE,
                FULL_SCREEN_UI_SHOW,
                FULL_SCREEN_UI_DETACH,

                FULL_SCREEN_UI_COUNT
            };
            Q_ENUM(FULL_SCREEN_UI);

            //! Get the full screen user interface option labels.
            static const QStringList & fullScreenUILabels();

            //! This enumeration provides the user interface components.
            enum UI_COMPONENT
            {
                UI_FILE_TOOL_BAR,
                UI_WINDOW_TOOL_BAR,
                UI_VIEW_TOOL_BAR,
                UI_IMAGE_TOOL_BAR,
                UI_TOOLS_TOOL_BAR,
                UI_ANNOTATE_TOOL_BAR,
                UI_PLAYBACK_CONTROLS,
                UI_STATUS_BAR,

                UI_COMPONENT_COUNT
            };
            Q_ENUM(UI_COMPONENT);

            //! Get the user interface component labels.
            static const QStringList & uiComponentLabels();

            //! This enumeration provides the grid overlays.
            enum GRID
            {
                GRID_NONE,
                GRID_1x1,
                GRID_10x10,
                GRID_100x100,

                GRID_COUNT
            };
            Q_ENUM(GRID);

            //! Get the grid overlay labels.
            static const QStringList & gridLabels();

            //! This enumeration provides the HUD information.
            enum HUD
            {
                HUD_FILE_NAME,
                HUD_LAYER,
                HUD_SIZE,
                HUD_PROXY,
                HUD_PIXEL,
                HUD_TAG,
                HUD_FRAME,
                HUD_SPEED,

                HUD_COUNT
            };
            Q_ENUM(HUD);

            //! Get the HUD information labels.
            static const QStringList & hudInfoLabels();

            //! This enumeration provides the HUD backgrounds.
            enum HUD_BACKGROUND
            {
                HUD_BACKGROUND_NONE,
                HUD_BACKGROUND_SOLID,
                HUD_BACKGROUND_SHADOW,

                HUD_BACKGROUND_COUNT
            };
            Q_ENUM(HUD_BACKGROUND);

            //! Get the HUD background labels.
            static const QStringList & hudBackgroundLabels();

            //! This enumeration provides the image scales.
            enum IMAGE_SCALE
            {
                IMAGE_SCALE_NONE,
                IMAGE_SCALE_16_9,
                IMAGE_SCALE_1_0,
                IMAGE_SCALE_1_33,
                IMAGE_SCALE_1_78,
                IMAGE_SCALE_1_85,
                IMAGE_SCALE_2_0,
                IMAGE_SCALE_2_35,
                IMAGE_SCALE_2_39,
                IMAGE_SCALE_2_40,
                IMAGE_SCALE_1_1,
                IMAGE_SCALE_2_1,
                IMAGE_SCALE_3_2,
                IMAGE_SCALE_4_3,
                IMAGE_SCALE_5_3,
                IMAGE_SCALE_5_4,

                IMAGE_SCALE_COUNT
            };
            Q_ENUM(IMAGE_SCALE);

            //! Get image scale labels.
            static const QStringList & imageScaleLabels();

            //! Calculate an image scale.
            static glm::vec2 imageScale(IMAGE_SCALE, const glm::ivec2 &);

            //! This enumeration provides the image rotation.
            enum IMAGE_ROTATE
            {
                IMAGE_ROTATE_0,
                IMAGE_ROTATE_90,
                IMAGE_ROTATE_180,
                IMAGE_ROTATE_270,

                IMAGE_ROTATE_COUNT
            };
            Q_ENUM(IMAGE_ROTATE);

            //! Get image rotate labels.
            static const QStringList & imageRotateLabels();

            //! Get the image rotation in degrees.
            static float imageRotate(IMAGE_ROTATE);

            //! This enumeration provides the playback modes.
            enum PLAYBACK
            {
                FORWARD,
                STOP,
                REVERSE,

                PLAYBACK_COUNT
            };
            Q_ENUM(PLAYBACK);

            //! Get the playback labels.
            static const QStringList & playbackLabels();

            //! This enumeration provides the frame controls.
            enum FRAME
            {
                FRAME_START,
                FRAME_END,
                FRAME_NEXT,
                FRAME_NEXT_10,
                FRAME_NEXT_100,
                FRAME_PREV,
                FRAME_PREV_10,
                FRAME_PREV_100,

                FRAME_COUNT
            };
            Q_ENUM(FRAME);

            //! Get the frame labels.
            static const QStringList & frameLabels();

            //! This enumeration provides the loop modes.
            enum LOOP
            {
                LOOP_ONCE,
                LOOP_REPEAT,
                LOOP_PING_PONG,

                LOOP_COUNT
            };
            Q_ENUM(LOOP);

            //! Get the loop mode labels.
            static const QStringList & loopLabels();

            //! This enumeration provides the in/out controls.
            enum IN_OUT
            {
                IN_OUT_ENABLE,
                MARK_IN,
                MARK_OUT,
                RESET_IN,
                RESET_OUT,

                IN_OUT_COUNT
            };
            Q_ENUM(IN_OUT);

            //! Get the in/out labels.
            static const QStringList & inOutLabels();

            //! This enumeration provides the playback controls layouts.
            enum LAYOUT
            {
                LAYOUT_DEFAULT,
                LAYOUT_LEFT,
                LAYOUT_CENTER,
                LAYOUT_MINIMAL,

                LAYOUT_COUNT
            };
            Q_ENUM(LAYOUT);

            //! Get the playback bar layout labels.
            static const QStringList & layoutLabels();

            //! This enumeration provides the tools.
            enum TOOL
            {
                TOOL_MAGNIFY,
                TOOL_COLOR_PICKER,
                TOOL_HISTOGRAM,
                TOOL_INFO,

                TOOL_COUNT
            };

            //! Get the tool labels.
            static const QStringList & toolLabels();

            //! This enumeration provides the histogram sizes.
            enum HISTOGRAM
            {
                HISTOGRAM_256,
                HISTOGRAM_1024,
                HISTOGRAM_2048,
                HISTOGRAM_4096,

                HISTOGRAM_COUNT
            };
            Q_ENUM(HISTOGRAM);

            //! Get the histogram size labels.
            static const QStringList & histogramLabels();

            //! Get a histogram size.
            static const int histogramSize(HISTOGRAM);

            //! This enumeration provides the annotation primitives.
            enum ANNOTATE_PRIMITIVE
            {
                ANNOTATE_PEN,
                ANNOTATE_RECTANGLE,
                ANNOTATE_ELLIPSE,

                ANNOTATE_PRIMITIVE_COUNT
            };
            Q_ENUM(ANNOTATE_PRIMITIVE);

            //! Get the annotation primitive labels.
            static const QStringList & annotatePrimitiveLabels();

            //! This enumeration provides the keyboard shortcuts.
            enum SHORTCUT
            {
                SHORTCUT_EXIT,

                SHORTCUT_FILE_OPEN,
                SHORTCUT_FILE_RELOAD,
                SHORTCUT_FILE_RELOAD_FRAME,
                SHORTCUT_FILE_EXPORT_SEQUENCE,
                SHORTCUT_FILE_EXPORT_FRAME,
                SHORTCUT_FILE_CLOSE,
                SHORTCUT_FILE_LAYER_DEFAULT,
                SHORTCUT_FILE_LAYER_1,
                SHORTCUT_FILE_LAYER_2,
                SHORTCUT_FILE_LAYER_3,
                SHORTCUT_FILE_LAYER_4,
                SHORTCUT_FILE_LAYER_5,
                SHORTCUT_FILE_LAYER_6,
                SHORTCUT_FILE_LAYER_7,
                SHORTCUT_FILE_LAYER_8,
                SHORTCUT_FILE_LAYER_9,
                SHORTCUT_FILE_LAYER_10,
                SHORTCUT_FILE_LAYER_NEXT,
                SHORTCUT_FILE_LAYER_PREV,
                SHORTCUT_FILE_PROXY_NONE,
                SHORTCUT_FILE_PROXY_1_2,
                SHORTCUT_FILE_PROXY_1_4,
                SHORTCUT_FILE_PROXY_1_8,

                SHORTCUT_WINDOW_NEW,
                SHORTCUT_WINDOW_DUPLICATE,
                SHORTCUT_WINDOW_CLOSE,
                SHORTCUT_WINDOW_FIT,
                SHORTCUT_WINDOW_FULL_SCREEN,
                SHORTCUT_WINDOW_UI_VISIBLE,
                SHORTCUT_WINDOW_FILE_TOOL_BAR_VISIBLE,
                SHORTCUT_WINDOW_WINDOW_TOOL_BAR_VISIBLE,
                SHORTCUT_WINDOW_VIEW_TOOL_BAR_VISIBLE,
                SHORTCUT_WINDOW_IMAGE_TOOL_BAR_VISIBLE,
                SHORTCUT_WINDOW_TOOLS_TOOL_BAR_VISIBLE,
                SHORTCUT_WINDOW_PLAYBACK_VISIBLE,
                SHORTCUT_WINDOW_INFO_VISIBLE,
                SHORTCUT_WINDOW_DETACH_CONTROLS,

                SHORTCUT_VIEW_LEFT,
                SHORTCUT_VIEW_RIGHT,
                SHORTCUT_VIEW_UP,
                SHORTCUT_VIEW_DOWN,
                SHORTCUT_VIEW_CENTER,
                SHORTCUT_VIEW_ZOOM_IN,
                SHORTCUT_VIEW_ZOOM_OUT,
                SHORTCUT_VIEW_ZOOM_RESET,
                SHORTCUT_VIEW_RESET,
                SHORTCUT_VIEW_FIT,
                SHORTCUT_VIEW_HUD,

                SHORTCUT_IMAGE_SHOW_FRAME_STORE,
                SHORTCUT_IMAGE_SET_FRAME_STORE,
                SHORTCUT_IMAGE_MIRROR_HORIZONTAL,
                SHORTCUT_IMAGE_MIRROR_VERTICAL,
                SHORTCUT_IMAGE_SCALE_NONE,
                SHORTCUT_IMAGE_SCALE_16_9,
                SHORTCUT_IMAGE_SCALE_1_0,
                SHORTCUT_IMAGE_SCALE_1_33,
                SHORTCUT_IMAGE_SCALE_1_78,
                SHORTCUT_IMAGE_SCALE_1_85,
                SHORTCUT_IMAGE_SCALE_2_0,
                SHORTCUT_IMAGE_SCALE_2_35,
                SHORTCUT_IMAGE_SCALE_2_39,
                SHORTCUT_IMAGE_SCALE_2_40,
                SHORTCUT_IMAGE_SCALE_1_1,
                SHORTCUT_IMAGE_SCALE_2_1,
                SHORTCUT_IMAGE_SCALE_3_2,
                SHORTCUT_IMAGE_SCALE_4_3,
                SHORTCUT_IMAGE_SCALE_5_3,
                SHORTCUT_IMAGE_SCALE_5_4,
                SHORTCUT_IMAGE_ROTATE_0,
                SHORTCUT_IMAGE_ROTATE_90,
                SHORTCUT_IMAGE_ROTATE_180,
                SHORTCUT_IMAGE_ROTATE_270,
                SHORTCUT_IMAGE_PREMULTIPLIED_ALPHA,
                SHORTCUT_IMAGE_COLOR_PROFILE,
                SHORTCUT_IMAGE_CHANNEL_DEFAULT,
                SHORTCUT_IMAGE_CHANNEL_RED,
                SHORTCUT_IMAGE_CHANNEL_GREEN,
                SHORTCUT_IMAGE_CHANNEL_BLUE,
                SHORTCUT_IMAGE_CHANNEL_ALPHA,
                SHORTCUT_IMAGE_DISPLAY_PROFILE,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_RESET,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_1,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_2,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_3,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_4,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_5,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_6,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_7,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_8,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_9,
                SHORTCUT_IMAGE_DISPLAY_PROFILE_10,

                SHORTCUT_PLAYBACK_FORWARD,
                SHORTCUT_PLAYBACK_STOP,
                SHORTCUT_PLAYBACK_REVERSE,
                SHORTCUT_PLAYBACK_TOGGLE,
                SHORTCUT_PLAYBACK_LOOP,
                SHORTCUT_PLAYBACK_START,
                SHORTCUT_PLAYBACK_END,
                SHORTCUT_PLAYBACK_NEXT,
                SHORTCUT_PLAYBACK_NEXT_10,
                SHORTCUT_PLAYBACK_NEXT_100,
                SHORTCUT_PLAYBACK_PREV,
                SHORTCUT_PLAYBACK_PREV_10,
                SHORTCUT_PLAYBACK_PREV_100,
                SHORTCUT_PLAYBACK_IN_OUT_POINTS,
                SHORTCUT_PLAYBACK_MARK_IN,
                SHORTCUT_PLAYBACK_RESET_IN,
                SHORTCUT_PLAYBACK_MARK_OUT,
                SHORTCUT_PLAYBACK_RESET_OUT,

                SHORTCUT_TOOL_MAGNIFY,
                SHORTCUT_TOOL_COLOR_PICKER,
                SHORTCUT_TOOL_HISTOGRAM,
                SHORTCUT_TOOL_INFO,

                SHORTCUT_ANNOTATE_TOOL,

                SHORTCUT_HELP_WHATS_THIS,

                SHORTCUT_COUNT
            };
            Q_ENUM(SHORTCUT);

            //! Get the shortcut labels.
            static const QStringList & shortcutLabels();

            //! This enumeration provides the keyboard modifiers.
            enum KEYBOARD_MODIFIER
            {
                KEYBOARD_MODIFIER_NONE,
                KEYBOARD_MODIFIER_SHIFT,
                KEYBOARD_MODIFIER_CTRL,
                KEYBOARD_MODIFIER_ALT,

                KEYBOARD_MODIFIER_COUNT
            };
            Q_ENUM(KEYBOARD_MODIFIER);

            //! Get the keyboard modifier labels.
            static const QStringList & keyboardModifierLabels();

            //! Get the Qt keyboard modifiers.
            static const QVector<Qt::KeyboardModifier> & qtKeyboardModifiers();

            //! This enumeration provides the mouse buttons.
            enum MOUSE_BUTTON
            {
                MOUSE_BUTTON_NONE,
                MOUSE_BUTTON_LEFT,
                MOUSE_BUTTON_MIDDLE,
                MOUSE_BUTTON_RIGHT,

                MOUSE_BUTTON_COUNT
            };
            Q_ENUM(MOUSE_BUTTON);

            //! Get the mouse button labels.
            static const QStringList & mouseButtonLabels();

            //! Get the Qt moouse buttons.
            static const QVector<Qt::MouseButton> & qtMouseButtons();

            //! This enumeration provides the mouse button actions.
            enum MOUSE_BUTTON_ACTION
            {
                MOUSE_BUTTON_ACTION_NONE,
                MOUSE_BUTTON_ACTION_PICK,
                MOUSE_BUTTON_ACTION_VIEW_MOVE,
                MOUSE_BUTTON_ACTION_VIEW_ZOOM_IN,
                MOUSE_BUTTON_ACTION_VIEW_ZOOM_OUT,
                MOUSE_BUTTON_ACTION_PLAYBACK_SHUTTLE,
                MOUSE_BUTTON_ACTION_CONTEXT_MENU,

                MOUSE_BUTTON_ACTION_COUNT
            };
            Q_ENUM(MOUSE_BUTTON_ACTION);

            //! Get the mouse button action labels.
            static const QStringList & mouseButtonActionLabels();

            //! This enumeration provides the mouse wheel actions.
            enum MOUSE_WHEEL_ACTION
            {
                MOUSE_WHEEL_ACTION_NONE,
                MOUSE_WHEEL_ACTION_VIEW_ZOOM,
                MOUSE_WHEEL_ACTION_PLAYBACK_SHUTTLE,
                MOUSE_WHEEL_ACTION_PLAYBACK_SPEED,

                MOUSE_WHEEL_ACTION_COUNT
            };
            Q_ENUM(MOUSE_WHEEL_ACTION);

            //! Get the mouse wheel action labels.
            static const QStringList & mouseWheelActionLabels();

            //! This enumeration provides the mouse wheel zoom factors.
            enum ZOOM_FACTOR
            {
                ZOOM_FACTOR_10,
                ZOOM_FACTOR_50,
                ZOOM_FACTOR_100,

                ZOOM_FACTOR_COUNT
            };
            Q_ENUM(ZOOM_FACTOR);

            //! Get the mouse wheel zoom factor labels.
            static const QStringList & zoomFactorLabels();

            //! Get the mouse wheel zoom factor.
            static float zoomFactor(ZOOM_FACTOR);

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_OPEN_IMAGE,
                ERROR_READ_IMAGE,
                ERROR_WRITE_IMAGE,
                ERROR_OPEN_LUT,
                ERROR_PICK_COLOR,
                ERROR_HISTOGRAM,
                ERROR_MAGNIFY,

                ERROR_COUNT
            };
            Q_ENUM(ERROR);

            //! Get the error code labels.
            static const QStringList & errorLabels();
        };

    } // namespace ViewLib

    DJV_STRING_OPERATOR(ViewLib::Enum::VIEW_MAX);
    DJV_STRING_OPERATOR(ViewLib::Enum::FULL_SCREEN_UI);
    DJV_STRING_OPERATOR(ViewLib::Enum::UI_COMPONENT);
    DJV_STRING_OPERATOR(ViewLib::Enum::GRID);
    DJV_STRING_OPERATOR(ViewLib::Enum::HUD);
    DJV_STRING_OPERATOR(ViewLib::Enum::HUD_BACKGROUND);
    DJV_STRING_OPERATOR(ViewLib::Enum::IMAGE_SCALE);
    DJV_STRING_OPERATOR(ViewLib::Enum::IMAGE_ROTATE);
    DJV_STRING_OPERATOR(ViewLib::Enum::PLAYBACK);
    DJV_STRING_OPERATOR(ViewLib::Enum::FRAME);
    DJV_STRING_OPERATOR(ViewLib::Enum::LOOP);
    DJV_STRING_OPERATOR(ViewLib::Enum::LAYOUT);
    DJV_STRING_OPERATOR(ViewLib::Enum::TOOL);
    DJV_STRING_OPERATOR(ViewLib::Enum::HISTOGRAM);
    DJV_STRING_OPERATOR(ViewLib::Enum::ANNOTATE_PRIMITIVE);
    DJV_STRING_OPERATOR(ViewLib::Enum::SHORTCUT);
    DJV_STRING_OPERATOR(ViewLib::Enum::KEYBOARD_MODIFIER);
    DJV_STRING_OPERATOR(ViewLib::Enum::MOUSE_BUTTON);
    DJV_STRING_OPERATOR(ViewLib::Enum::MOUSE_BUTTON_ACTION);
    DJV_STRING_OPERATOR(ViewLib::Enum::MOUSE_WHEEL_ACTION);
    DJV_STRING_OPERATOR(ViewLib::Enum::ZOOM_FACTOR);
    QStringList & operator >> (QStringList &, QMap<ViewLib::Enum::UI_COMPONENT, bool> &);
    QStringList & operator >> (QStringList &, QMap<ViewLib::Enum::HUD, bool> &);
    QStringList & operator >> (QStringList &, QMap<ViewLib::Enum::TOOL, bool> &);
    QStringList & operator << (QStringList &, const QMap<ViewLib::Enum::UI_COMPONENT, bool> &);
    QStringList & operator << (QStringList &, const QMap<ViewLib::Enum::HUD, bool> &);
    QStringList & operator << (QStringList &, const QMap<ViewLib::Enum::TOOL, bool> &);

} // namespace djv

