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

#include <djvViewLib/Enum.h>

#include <djvCore/Assert.h>
#include <djvCore/VectorUtil.h>

#include <QApplication>

namespace djv
{
    namespace ViewLib
    {
        Enum::~Enum()
        {}

        const QStringList & Enum::viewMaxLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Unlimited") <<
                qApp->translate("djv::ViewLib::Enum", "25% of screen") <<
                qApp->translate("djv::ViewLib::Enum", "50% of screen") <<
                qApp->translate("djv::ViewLib::Enum", "75% of screen") <<
                qApp->translate("djv::ViewLib::Enum", "User specified");
            DJV_ASSERT(data.count() == VIEW_MAX_COUNT);
            return data;
        }

        const QStringList & Enum::fullScreenUILabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Hide") <<
                qApp->translate("djv::ViewLib::Enum", "Show") <<
                qApp->translate("djv::ViewLib::Enum", "Detach");
            DJV_ASSERT(data.count() == FULL_SCREEN_UI_COUNT);
            return data;
        }

        const QStringList & Enum::uiComponentLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "File Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Window Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "View Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Image Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Tools Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Playback Controls") <<
                qApp->translate("djv::ViewLib::Enum", "Status Bar");
            DJV_ASSERT(data.count() == UI_COMPONENT_COUNT);
            return data;
        }

        const QStringList & Enum::gridLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "None") <<
                qApp->translate("djv::ViewLib::Enum", "1x1") <<
                qApp->translate("djv::ViewLib::Enum", "10x10") <<
                qApp->translate("djv::ViewLib::Enum", "100x100");
            DJV_ASSERT(data.count() == GRID_COUNT);
            return data;
        }

        const QStringList & Enum::hudInfoLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "File Name") <<
                qApp->translate("djv::ViewLib::Enum", "Layer") <<
                qApp->translate("djv::ViewLib::Enum", "Size") <<
                qApp->translate("djv::ViewLib::Enum", "Proxy") <<
                qApp->translate("djv::ViewLib::Enum", "Pixel") <<
                qApp->translate("djv::ViewLib::Enum", "Tags") <<
                qApp->translate("djv::ViewLib::Enum", "Playback Frame") <<
                qApp->translate("djv::ViewLib::Enum", "Playback Speed");
            DJV_ASSERT(data.count() == HUD_COUNT);
            return data;
        }

        const QStringList & Enum::hudBackgroundLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "None") <<
                qApp->translate("djv::ViewLib::Enum", "Solid") <<
                qApp->translate("djv::ViewLib::Enum", "Shadow");
            DJV_ASSERT(data.count() == HUD_BACKGROUND_COUNT);
            return data;
        }

        const QStringList & Enum::imageScaleLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "None") <<
                qApp->translate("djv::ViewLib::Enum", "16.9") <<
                qApp->translate("djv::ViewLib::Enum", "1.0") <<
                qApp->translate("djv::ViewLib::Enum", "1.33") <<
                qApp->translate("djv::ViewLib::Enum", "1.78") <<
                qApp->translate("djv::ViewLib::Enum", "1.85") <<
                qApp->translate("djv::ViewLib::Enum", "2.0") <<
                qApp->translate("djv::ViewLib::Enum", "2.35") <<
                qApp->translate("djv::ViewLib::Enum", "2.39") <<
                qApp->translate("djv::ViewLib::Enum", "2.40") <<
                qApp->translate("djv::ViewLib::Enum", "1:1") <<
                qApp->translate("djv::ViewLib::Enum", "2:1") <<
                qApp->translate("djv::ViewLib::Enum", "3:2") <<
                qApp->translate("djv::ViewLib::Enum", "4:3") <<
                qApp->translate("djv::ViewLib::Enum", "5:3") <<
                qApp->translate("djv::ViewLib::Enum", "5:4");
            DJV_ASSERT(data.count() == IMAGE_SCALE_COUNT);
            return data;
        }

        glm::vec2 Enum::imageScale(IMAGE_SCALE in, const glm::ivec2 & size)
        {
            static const float data[] =
            {
                1.f,
                16.f / 9.f,
                1.f,
                1.33f,
                1.78f,
                1.85f,
                2.f,
                2.35f,
                2.39f,
                2.40f,
                1.f,
                2.f,
                3.f / 2.f,
                4.f / 3.f,
                5.f / 3.f,
                5.f / 4.f
            };
            static const int dataCount = sizeof(data) / sizeof(data[0]);
            DJV_ASSERT(dataCount == IMAGE_SCALE_COUNT);
            glm::vec2 out(1.f, 1.f);
            switch (in)
            {
            case IMAGE_SCALE_NONE: break;
            default:
                out.x = data[in] / Core::VectorUtil::aspect(size);
                break;
            }
            return out;
        }

        const QStringList & Enum::imageRotateLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "0") <<
                qApp->translate("djv::ViewLib::Enum", "90") <<
                qApp->translate("djv::ViewLib::Enum", "180") <<
                qApp->translate("djv::ViewLib::Enum", "270");
            DJV_ASSERT(data.count() == IMAGE_ROTATE_COUNT);
            return data;
        }

        float Enum::imageRotate(IMAGE_ROTATE in)
        {
            static const float data[] = { 0, 90, 180, 270 };
            return data[in];
        }

        const QStringList & Enum::playbackLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Forward") <<
                qApp->translate("djv::ViewLib::Enum", "Stop") <<
                qApp->translate("djv::ViewLib::Enum", "Reverse");
            DJV_ASSERT(data.count() == PLAYBACK_COUNT);
            return data;
        }

        const QStringList & Enum::frameLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Start") <<
                qApp->translate("djv::ViewLib::Enum", "End") <<
                qApp->translate("djv::ViewLib::Enum", "Next Frame") <<
                qApp->translate("djv::ViewLib::Enum", "Next Frame X10") <<
                qApp->translate("djv::ViewLib::Enum", "Next Frame X100") <<
                qApp->translate("djv::ViewLib::Enum", "Previous Frame") <<
                qApp->translate("djv::ViewLib::Enum", "Previous Frame X10") <<
                qApp->translate("djv::ViewLib::Enum", "Previous Frame X100");
            DJV_ASSERT(data.count() == FRAME_COUNT);
            return data;
        }

        const QStringList & Enum::loopLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Once") <<
                qApp->translate("djv::ViewLib::Enum", "Repeat") <<
                qApp->translate("djv::ViewLib::Enum", "Ping Pong");
            DJV_ASSERT(data.count() == LOOP_COUNT);
            return data;
        }

        const QStringList & Enum::inOutLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "&In/Out Points") <<
                qApp->translate("djv::ViewLib::Enum", "&Mark In Point") <<
                qApp->translate("djv::ViewLib::Enum", "Mar&k Out Point") <<
                qApp->translate("djv::ViewLib::Enum", "Reset In Point") <<
                qApp->translate("djv::ViewLib::Enum", "Reset Out Point");
            DJV_ASSERT(data.count() == IN_OUT_COUNT);
            return data;
        }

        const QStringList & Enum::layoutLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Default") <<
                qApp->translate("djv::ViewLib::Enum", "Left") <<
                qApp->translate("djv::ViewLib::Enum", "Center") <<
                qApp->translate("djv::ViewLib::Enum", "Minimal");
            DJV_ASSERT(data.count() == LAYOUT_COUNT);
            return data;
        }

        const QStringList & Enum::toolLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Magnify") <<
                qApp->translate("djv::ViewLib::Enum", "Color Picker") <<
                qApp->translate("djv::ViewLib::Enum", "Histogram") <<
                qApp->translate("djv::ViewLib::Enum", "Information");
            DJV_ASSERT(data.count() == TOOL_COUNT);
            return data;
        }

        const QStringList & Enum::histogramLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "256") <<
                qApp->translate("djv::ViewLib::Enum", "1024") <<
                qApp->translate("djv::ViewLib::Enum", "2048") <<
                qApp->translate("djv::ViewLib::Enum", "4096");
            DJV_ASSERT(data.count() == HISTOGRAM_COUNT);
            return data;
        }

        const int Enum::histogramSize(HISTOGRAM histogram)
        {
            static const QList<int> data = QList<int>() <<
                256 <<
                1024 <<
                2048 <<
                4096;
            DJV_ASSERT(data.count() == HISTOGRAM_COUNT);
            return data[histogram];
        }

        const QStringList & Enum::shortcutLabels()
        {
            static const QStringList data = QStringList() <<

                qApp->translate("djv::ViewLib::Enum", "Exit") <<

                qApp->translate("djv::ViewLib::Enum", "File/Open") <<
                qApp->translate("djv::ViewLib::Enum", "File/Reload") <<
                qApp->translate("djv::ViewLib::Enum", "File/Reload Frame") <<
                qApp->translate("djv::ViewLib::Enum", "File/Export Sequence") <<
                qApp->translate("djv::ViewLib::Enum", "File/Export Frame") <<
                qApp->translate("djv::ViewLib::Enum", "File/Close") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer Default") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 1") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 2") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 3") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 4") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 5") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 6") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 7") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 8") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 9") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer 10") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer Previous") <<
                qApp->translate("djv::ViewLib::Enum", "File/Layer Next") <<
                qApp->translate("djv::ViewLib::Enum", "File/Proxy None") <<
                qApp->translate("djv::ViewLib::Enum", "File/Proxy 1/2") <<
                qApp->translate("djv::ViewLib::Enum", "File/Proxy 1/4") <<
                qApp->translate("djv::ViewLib::Enum", "File/Proxy 1/8") <<

                qApp->translate("djv::ViewLib::Enum", "Window/New") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Duplicate") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Close") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Fit") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Full Screen") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show UI") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show File Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show Window Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show View Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show Image Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show Tools Tool Bar") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show Playback") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Show Information") <<
                qApp->translate("djv::ViewLib::Enum", "Window/Detach Controls") <<

                qApp->translate("djv::ViewLib::Enum", "View/Left") <<
                qApp->translate("djv::ViewLib::Enum", "View/Right") <<
                qApp->translate("djv::ViewLib::Enum", "View/Up") <<
                qApp->translate("djv::ViewLib::Enum", "View/Down") <<
                qApp->translate("djv::ViewLib::Enum", "View/Center") <<
                qApp->translate("djv::ViewLib::Enum", "View/Zoom In") <<
                qApp->translate("djv::ViewLib::Enum", "View/Zoom Out") <<
                qApp->translate("djv::ViewLib::Enum", "View/Zoom Reset") <<
                qApp->translate("djv::ViewLib::Enum", "View/Reset") <<
                qApp->translate("djv::ViewLib::Enum", "View/Fit") <<
                qApp->translate("djv::ViewLib::Enum", "View/HUD") <<

                qApp->translate("djv::ViewLib::Enum", "Image/Show Frame Store") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Set Frame Store") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Mirror Horizontal") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Mirror Vertical") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale None") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 16:9") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 1.0") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 1.33") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 1.78") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 1.85") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 2.0") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 2.35") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 2.39") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 2.40") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 1:1") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 2:1") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 3:2") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 4:3") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 5:3") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Scale 5:4") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Rotate 0") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Rotate 90") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Rotate 180") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Rotate 270") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Premultiplied Alpha") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Color Profile") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Channel Default") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Channel Red") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Channel Green") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Channel Blue") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Channel Alpha") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile Editor") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile Reset") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 1") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 2") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 3") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 4") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 5") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 6") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 7") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 8") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 9") <<
                qApp->translate("djv::ViewLib::Enum", "Image/Display Profile 10") <<

                qApp->translate("djv::ViewLib::Enum", "Playback/Forward") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Stop") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Reverse") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Toggle") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Loop") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Start") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/End") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Next") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Next 10") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Next 100") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Previous") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Previous 10") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Previous 100") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/In/Out Points") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/In Point Mark") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/In Point Reset") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Out Point Mark") <<
                qApp->translate("djv::ViewLib::Enum", "Playback/Out Point Reset") <<

                qApp->translate("djv::ViewLib::Enum", "Tool/Magnify") <<
                qApp->translate("djv::ViewLib::Enum", "Tool/Color Picker") <<
                qApp->translate("djv::ViewLib::Enum", "Tool/Histogram") <<
                qApp->translate("djv::ViewLib::Enum", "Tool/Information") <<
            
                qApp->translate("djv::ViewLib::Enum", "Help/What's This?");
            DJV_ASSERT(data.count() == SHORTCUT_COUNT);
            return data;
        }

        const QStringList & Enum::keyboardModifierLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "None") <<
                qApp->translate("djv::ViewLib::Enum", "Shift") <<
                qApp->translate("djv::ViewLib::Enum", "Ctrl") <<
                qApp->translate("djv::ViewLib::Enum", "Alt");
            DJV_ASSERT(data.count() == KEYBOARD_MODIFIER_COUNT);
            return data;
        }

        const QVector<Qt::KeyboardModifier> & Enum::qtKeyboardModifiers()
        {
            static const auto data = QVector<Qt::KeyboardModifier>() <<
                Qt::NoModifier <<
                Qt::ShiftModifier <<
                Qt::ControlModifier <<
                Qt::AltModifier;
            DJV_ASSERT(data.count() == KEYBOARD_MODIFIER_COUNT);
            return data;
        }

        const QStringList & Enum::mouseButtonLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "None") <<
                qApp->translate("djv::ViewLib::Enum", "Left") <<
                qApp->translate("djv::ViewLib::Enum", "Middle") <<
                qApp->translate("djv::ViewLib::Enum", "Right");
            DJV_ASSERT(data.count() == MOUSE_BUTTON_COUNT);
            return data;
        }

        const QVector<Qt::MouseButton> & Enum::qtMouseButtons()
        {
            static const auto data = QVector<Qt::MouseButton>() <<
                Qt::NoButton <<
                Qt::LeftButton <<
                Qt::MiddleButton <<
                Qt::RightButton;
                DJV_ASSERT(data.count() == MOUSE_BUTTON_COUNT);
            return data;
        }

        const QStringList & Enum::mouseButtonActionLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "None") <<
                qApp->translate("djv::ViewLib::Enum", "Color Pick") <<
                qApp->translate("djv::ViewLib::Enum", "View Move") <<
                qApp->translate("djv::ViewLib::Enum", "View Zoom In") <<
                qApp->translate("djv::ViewLib::Enum", "View Zoom Out") <<
                qApp->translate("djv::ViewLib::Enum", "Playback Shuttle") <<
                qApp->translate("djv::ViewLib::Enum", "Context Menu");
            DJV_ASSERT(data.count() == MOUSE_BUTTON_ACTION_COUNT);
            return data;
        }

        const QStringList & Enum::mouseWheelActionLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "None") <<
                qApp->translate("djv::ViewLib::Enum", "View Zoom") <<
                qApp->translate("djv::ViewLib::Enum", "Playback Shuttle") <<
                qApp->translate("djv::ViewLib::Enum", "Playback Speed");
            DJV_ASSERT(data.count() == MOUSE_WHEEL_ACTION_COUNT);
            return data;
        }

        const QStringList & Enum::zoomFactorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "10%") <<
                qApp->translate("djv::ViewLib::Enum", "50%") <<
                qApp->translate("djv::ViewLib::Enum", "100%");
            DJV_ASSERT(data.count() == ZOOM_FACTOR_COUNT);
            return data;
        }

        float Enum::zoomFactor(ZOOM_FACTOR zoomFactor)
        {
            static const float data[] =
            {
                1.1f,
                1.5f,
                2.f
            };
            static const int dataCount = sizeof(data) / sizeof(data[0]);
            DJV_ASSERT(dataCount == ZOOM_FACTOR_COUNT);
            return data[zoomFactor];
        }

        const QStringList & Enum::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Enum", "Cannot open image: %1") <<
                qApp->translate("djv::ViewLib::Enum", "Cannot read image: %1") <<
                qApp->translate("djv::ViewLib::Enum", "Cannot write image: %1") <<
                qApp->translate("djv::ViewLib::Enum", "Cannot open LUT: %1") <<
                qApp->translate("djv::ViewLib::Enum", "Cannot pick color") <<
                qApp->translate("djv::ViewLib::Enum", "Cannot compute histogram") <<
                qApp->translate("djv::ViewLib::Enum", "Cannot magnify");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

    } // namespace ViewLib

    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::VIEW_MAX, ViewLib::Enum::viewMaxLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::FULL_SCREEN_UI, ViewLib::Enum::fullScreenUILabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::UI_COMPONENT, ViewLib::Enum::uiComponentLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::GRID, ViewLib::Enum::gridLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::HUD, ViewLib::Enum::hudInfoLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::HUD_BACKGROUND, ViewLib::Enum::hudBackgroundLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::IMAGE_SCALE, ViewLib::Enum::imageScaleLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::IMAGE_ROTATE, ViewLib::Enum::imageRotateLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::PLAYBACK, ViewLib::Enum::playbackLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::FRAME, ViewLib::Enum::frameLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::LOOP, ViewLib::Enum::loopLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::LAYOUT, ViewLib::Enum::layoutLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::TOOL, ViewLib::Enum::toolLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::HISTOGRAM, ViewLib::Enum::histogramLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::KEYBOARD_MODIFIER, ViewLib::Enum::keyboardModifierLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::MOUSE_BUTTON, ViewLib::Enum::mouseButtonLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::MOUSE_BUTTON_ACTION, ViewLib::Enum::mouseButtonActionLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::MOUSE_WHEEL_ACTION, ViewLib::Enum::mouseWheelActionLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Enum::ZOOM_FACTOR, ViewLib::Enum::zoomFactorLabels());

    QStringList & operator >> (QStringList & in, QMap<ViewLib::Enum::UI_COMPONENT, bool> & out)
    {
        while (in.count())
        {
            auto key = static_cast<ViewLib::Enum::UI_COMPONENT>(0);
            in >> key;
            if (in.count())
            {
                bool value = false;
                in >> value;
                out[key] = value;
            }
        }
        return in;
    }

    QStringList & operator >> (QStringList & in, QMap<ViewLib::Enum::HUD, bool> & out)
    {
        while (in.count())
        {
            auto key = static_cast<ViewLib::Enum::HUD>(0);
            in >> key;
            if (in.count())
            {
                bool value = false;
                in >> value;
                out[key] = value;
            }
        }
        return in;
    }

    QStringList & operator >> (QStringList & in, QMap<ViewLib::Enum::TOOL, bool> & out)
    {
        while (in.count())
        {
            auto key = static_cast<ViewLib::Enum::TOOL>(0);
            in >> key;
            if (in.count())
            {
                bool value = false;
                in >> value;
                out[key] = value;
            }
        }
        return in;
    }

    QStringList & operator << (QStringList & out, const QMap<ViewLib::Enum::UI_COMPONENT, bool> & in)
    {
        Q_FOREACH(auto key, in.keys())
        {
            out << key << in[key];
        }
        return out;
    }

    QStringList & operator << (QStringList & out, const QMap<ViewLib::Enum::HUD, bool> & in)
    {
        Q_FOREACH(auto key, in.keys())
        {
            out << key << in[key];
        }
        return out;
    }

    QStringList & operator << (QStringList & out, const QMap<ViewLib::Enum::TOOL, bool> & in)
    {
        Q_FOREACH(auto key, in.keys())
        {
            out << key << in[key];
        }
        return out;
    }

} // namespace djv
