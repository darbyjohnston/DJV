// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/Util.h>

#include <djvViewLib/Context.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/ImageIO.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>
#include <djvCore/FileInfo.h>
#include <djvCore/VectorUtil.h>

#include <QApplication>
#include <QDir>

namespace djv
{
    namespace ViewLib
    {
        Util::~Util()
        {}

        const QStringList & Util::viewMaxLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Unlimited") <<
                qApp->translate("djv::ViewLib::Util", "25% of screen") <<
                qApp->translate("djv::ViewLib::Util", "50% of screen") <<
                qApp->translate("djv::ViewLib::Util", "75% of screen") <<
                qApp->translate("djv::ViewLib::Util", "User specified");
            DJV_ASSERT(data.count() == VIEW_MAX_COUNT);
            return data;
        }

        const QStringList & Util::toolBarLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Tool Bars") <<
                qApp->translate("djv::ViewLib::Util", "Playback Bar") <<
                qApp->translate("djv::ViewLib::Util", "Information Bar");
            DJV_ASSERT(data.count() == TOOL_BAR_COUNT);
            return data;
        }

        const QStringList & Util::gridLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "None") <<
                qApp->translate("djv::ViewLib::Util", "1x1") <<
                qApp->translate("djv::ViewLib::Util", "10x10") <<
                qApp->translate("djv::ViewLib::Util", "100x100");
            DJV_ASSERT(data.count() == GRID_COUNT);
            return data;
        }

        const QStringList & Util::hudInfoLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "File Name") <<
                qApp->translate("djv::ViewLib::Util", "Layer") <<
                qApp->translate("djv::ViewLib::Util", "Size") <<
                qApp->translate("djv::ViewLib::Util", "Proxy") <<
                qApp->translate("djv::ViewLib::Util", "Pixel") <<
                qApp->translate("djv::ViewLib::Util", "Tags") <<
                qApp->translate("djv::ViewLib::Util", "Playback Frame") <<
                qApp->translate("djv::ViewLib::Util", "Playback Speed");
            DJV_ASSERT(data.count() == HUD_COUNT);
            return data;
        }

        const QStringList & Util::hudBackgroundLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "None") <<
                qApp->translate("djv::ViewLib::Util", "Solid") <<
                qApp->translate("djv::ViewLib::Util", "Shadow");
            DJV_ASSERT(data.count() == HUD_BACKGROUND_COUNT);
            return data;
        }

        const QStringList & Util::imageScaleLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "None") <<
                qApp->translate("djv::ViewLib::Util", "16.9") <<
                qApp->translate("djv::ViewLib::Util", "1.0") <<
                qApp->translate("djv::ViewLib::Util", "1.33") <<
                qApp->translate("djv::ViewLib::Util", "1.78") <<
                qApp->translate("djv::ViewLib::Util", "1.85") <<
                qApp->translate("djv::ViewLib::Util", "2.0") <<
                qApp->translate("djv::ViewLib::Util", "2.35") <<
                qApp->translate("djv::ViewLib::Util", "2.39") <<
                qApp->translate("djv::ViewLib::Util", "2.40") <<
                qApp->translate("djv::ViewLib::Util", "1:1") <<
                qApp->translate("djv::ViewLib::Util", "2:1") <<
                qApp->translate("djv::ViewLib::Util", "3:2") <<
                qApp->translate("djv::ViewLib::Util", "4:3") <<
                qApp->translate("djv::ViewLib::Util", "5:3") <<
                qApp->translate("djv::ViewLib::Util", "5:4");
            DJV_ASSERT(data.count() == IMAGE_SCALE_COUNT);
            return data;
        }

        glm::vec2 Util::imageScale(IMAGE_SCALE in, const glm::ivec2 & size)
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

        const QStringList & Util::imageRotateLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "0") <<
                qApp->translate("djv::ViewLib::Util", "90") <<
                qApp->translate("djv::ViewLib::Util", "180") <<
                qApp->translate("djv::ViewLib::Util", "270");
            DJV_ASSERT(data.count() == IMAGE_ROTATE_COUNT);
            return data;
        }

        float Util::imageRotate(IMAGE_ROTATE in)
        {
            static const float data[] = { 0, 90, 180, 270 };
            return data[in];
        }

        void Util::loadLut(
            const Core::FileInfo & fileInfo,
            Graphics::PixelData &  lut,
            const QPointer<Context> & context)
        {
            if (fileInfo.fileName().isEmpty())
                return;
            //DJV_DEBUG("Util::loadLut");
            //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
            Core::FileInfo fileInfoTmp(fileInfo);
            if (fileInfoTmp.isSequenceValid())
            {
                fileInfoTmp.setType(Core::FileInfo::SEQUENCE);
            }
            try
            {
                Graphics::ImageIOInfo info;
                QScopedPointer<Graphics::ImageLoad> load(context->imageIOFactory()->load(fileInfoTmp, info));
                Graphics::Image image;
                load->read(image);
                lut = image;
                //DJV_DEBUG_PRINT("lut = " << lut);
            }
            catch (Core::Error error)
            {
                error.add(
                    errorLabels()[ERROR_OPEN_LUT].
                    arg(QDir::toNativeSeparators(fileInfoTmp)));
                throw error;
            }
        }

        const QStringList & Util::playbackLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Reverse") <<
                qApp->translate("djv::ViewLib::Util", "Stop") <<
                qApp->translate("djv::ViewLib::Util", "Forward");
            DJV_ASSERT(data.count() == PLAYBACK_COUNT);
            return data;
        }

        const QStringList & Util::frameLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Start") <<
                qApp->translate("djv::ViewLib::Util", "Start Absolute") <<
                qApp->translate("djv::ViewLib::Util", "Previous") <<
                qApp->translate("djv::ViewLib::Util", "Previous X10") <<
                qApp->translate("djv::ViewLib::Util", "Previous X100") <<
                qApp->translate("djv::ViewLib::Util", "Next") <<
                qApp->translate("djv::ViewLib::Util", "Next X10") <<
                qApp->translate("djv::ViewLib::Util", "Next X100") <<
                qApp->translate("djv::ViewLib::Util", "End") <<
                qApp->translate("djv::ViewLib::Util", "End Absolute");
            DJV_ASSERT(data.count() == FRAME_COUNT);
            return data;
        }

        const QStringList & Util::loopLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Once") <<
                qApp->translate("djv::ViewLib::Util", "Repeat") <<
                qApp->translate("djv::ViewLib::Util", "PingPong");
            DJV_ASSERT(data.count() == LOOP_COUNT);
            return data;
        }

        const QStringList & Util::inOutLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Enable &In/Out Points") <<
                qApp->translate("djv::ViewLib::Util", "&Mark In Point") <<
                qApp->translate("djv::ViewLib::Util", "Mar&k Out Point") <<
                qApp->translate("djv::ViewLib::Util", "Reset In Point") <<
                qApp->translate("djv::ViewLib::Util", "Reset Out Point");
            DJV_ASSERT(data.count() == IN_OUT_COUNT);
            return data;
        }

        const QStringList & Util::layoutLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Default") <<
                qApp->translate("djv::ViewLib::Util", "Left") <<
                qApp->translate("djv::ViewLib::Util", "Center") <<
                qApp->translate("djv::ViewLib::Util", "Minimal");
            DJV_ASSERT(data.count() == LAYOUT_COUNT);
            return data;
        }

        const QStringList & Util::toolLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Magnify") <<
                qApp->translate("djv::ViewLib::Util", "Color Picker") <<
                qApp->translate("djv::ViewLib::Util", "Histogram") <<
                qApp->translate("djv::ViewLib::Util", "Information");
            DJV_ASSERT(data.count() == TOOL_COUNT);
            return data;
        }

        const QStringList & Util::histogramLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "256") <<
                qApp->translate("djv::ViewLib::Util", "1024") <<
                qApp->translate("djv::ViewLib::Util", "2048") <<
                qApp->translate("djv::ViewLib::Util", "4096");
            DJV_ASSERT(data.count() == HISTOGRAM_COUNT);
            return data;
        }

        const int Util::histogramSize(HISTOGRAM histogram)
        {
            static const QList<int> data = QList<int>() <<
                256 <<
                1024 <<
                2048 <<
                4096;
            DJV_ASSERT(data.count() == HISTOGRAM_COUNT);
            return data[histogram];
        }

        const QStringList & Util::shortcutLabels()
        {
            static const QStringList data = QStringList() <<

                qApp->translate("djv::ViewLib::Util", "Exit") <<

                qApp->translate("djv::ViewLib::Util", "File Open") <<
                qApp->translate("djv::ViewLib::Util", "File Reload") <<
                qApp->translate("djv::ViewLib::Util", "File Reload Frame") <<
                qApp->translate("djv::ViewLib::Util", "File Save") <<
                qApp->translate("djv::ViewLib::Util", "File Save Frame") <<
                qApp->translate("djv::ViewLib::Util", "File Close") <<
                qApp->translate("djv::ViewLib::Util", "File Layer Default") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 1") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 2") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 3") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 4") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 5") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 6") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 7") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 8") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 9") <<
                qApp->translate("djv::ViewLib::Util", "File Layer 10") <<
                qApp->translate("djv::ViewLib::Util", "File Layer Previous") <<
                qApp->translate("djv::ViewLib::Util", "File Layer Next") <<
                qApp->translate("djv::ViewLib::Util", "File Proxy None") <<
                qApp->translate("djv::ViewLib::Util", "File Proxy 1/2") <<
                qApp->translate("djv::ViewLib::Util", "File Proxy 1/4") <<
                qApp->translate("djv::ViewLib::Util", "File Proxy 1/8") <<

                qApp->translate("djv::ViewLib::Util", "Window New") <<
                qApp->translate("djv::ViewLib::Util", "Window Copy") <<
                qApp->translate("djv::ViewLib::Util", "Window Close") <<
                qApp->translate("djv::ViewLib::Util", "Window Fit") <<
                qApp->translate("djv::ViewLib::Util", "Window Full Screen") <<
                qApp->translate("djv::ViewLib::Util", "Window Show Controls") <<
                qApp->translate("djv::ViewLib::Util", "Window Show Tool Bars") <<
                qApp->translate("djv::ViewLib::Util", "Window Show Playback") <<
                qApp->translate("djv::ViewLib::Util", "Window Show Information") <<

                qApp->translate("djv::ViewLib::Util", "View Left") <<
                qApp->translate("djv::ViewLib::Util", "View Right") <<
                qApp->translate("djv::ViewLib::Util", "View Up") <<
                qApp->translate("djv::ViewLib::Util", "View Down") <<
                qApp->translate("djv::ViewLib::Util", "View Center") <<
                qApp->translate("djv::ViewLib::Util", "View Zoom In") <<
                qApp->translate("djv::ViewLib::Util", "View Zoom Out") <<
                qApp->translate("djv::ViewLib::Util", "View Zoom Reset") <<
                qApp->translate("djv::ViewLib::Util", "View Reset") <<
                qApp->translate("djv::ViewLib::Util", "View Fit") <<
                qApp->translate("djv::ViewLib::Util", "View HUD") <<

                qApp->translate("djv::ViewLib::Util", "Image Show Frame Store") <<
                qApp->translate("djv::ViewLib::Util", "Image Load Frame Store") <<
                qApp->translate("djv::ViewLib::Util", "Image Mirror Horizontal") <<
                qApp->translate("djv::ViewLib::Util", "Image Mirror Vertical") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale None") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 16:9") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 1.0") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 1.33") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 1.78") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 1.85") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 2.0") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 2.35") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 2.39") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 2.40") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 1:1") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 2:1") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 3:2") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 4:3") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 5:3") <<
                qApp->translate("djv::ViewLib::Util", "Image Scale 5:4") <<
                qApp->translate("djv::ViewLib::Util", "Image Rotate 0") <<
                qApp->translate("djv::ViewLib::Util", "Image Rotate 90") <<
                qApp->translate("djv::ViewLib::Util", "Image Rotate 180") <<
                qApp->translate("djv::ViewLib::Util", "Image Rotate 270") <<
                qApp->translate("djv::ViewLib::Util", "Image Color Profile") <<
                qApp->translate("djv::ViewLib::Util", "Image Channel Red") <<
                qApp->translate("djv::ViewLib::Util", "Image Channel Green") <<
                qApp->translate("djv::ViewLib::Util", "Image Channel Blue") <<
                qApp->translate("djv::ViewLib::Util", "Image Channel Alpha") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile Reset") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 1") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 2") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 3") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 4") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 5") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 6") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 7") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 8") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 9") <<
                qApp->translate("djv::ViewLib::Util", "Image Display Profile 10") <<

                qApp->translate("djv::ViewLib::Util", "Playback Reverse") <<
                qApp->translate("djv::ViewLib::Util", "Playback Stop") <<
                qApp->translate("djv::ViewLib::Util", "Playback Forward") <<
                qApp->translate("djv::ViewLib::Util", "Playback Toggle") <<
                qApp->translate("djv::ViewLib::Util", "Playback Loop") <<
                qApp->translate("djv::ViewLib::Util", "Playback Start") <<
                qApp->translate("djv::ViewLib::Util", "Playback Start Absolute") <<
                qApp->translate("djv::ViewLib::Util", "Playback Previous") <<
                qApp->translate("djv::ViewLib::Util", "Playback Previous 10") <<
                qApp->translate("djv::ViewLib::Util", "Playback Previous 100") <<
                qApp->translate("djv::ViewLib::Util", "Playback Next") <<
                qApp->translate("djv::ViewLib::Util", "Playback Next 10") <<
                qApp->translate("djv::ViewLib::Util", "Playback Next 100") <<
                qApp->translate("djv::ViewLib::Util", "Playback End") <<
                qApp->translate("djv::ViewLib::Util", "Playback End Absolute") <<
                qApp->translate("djv::ViewLib::Util", "Playback In/Out Points") <<
                qApp->translate("djv::ViewLib::Util", "Playback In Point Mark") <<
                qApp->translate("djv::ViewLib::Util", "Playback In Point Reset") <<
                qApp->translate("djv::ViewLib::Util", "Playback Out Point Mark") <<
                qApp->translate("djv::ViewLib::Util", "Playback Out Point Reset") <<

                qApp->translate("djv::ViewLib::Util", "Tool Magnify") <<
                qApp->translate("djv::ViewLib::Util", "Tool Color Picker") <<
                qApp->translate("djv::ViewLib::Util", "Tool Histogram") <<
                qApp->translate("djv::ViewLib::Util", "Tool Information");
            DJV_ASSERT(data.count() == SHORTCUT_COUNT);
            return data;
        }

        const QStringList & Util::mouseWheelLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "View Zoom") <<
                qApp->translate("djv::ViewLib::Util", "Playback Shuttle") <<
                qApp->translate("djv::ViewLib::Util", "Playback Speed");
            DJV_ASSERT(data.count() == MOUSE_WHEEL_COUNT);
            return data;
        }

        const QStringList & Util::zoomFactorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "10%") <<
                qApp->translate("djv::ViewLib::Util", "50%") <<
                qApp->translate("djv::ViewLib::Util", "100%");
            DJV_ASSERT(data.count() == ZOOM_FACTOR_COUNT);
            return data;
        }

        float Util::zoomFactor(ZOOM_FACTOR zoomFactor)
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

        const QStringList & Util::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::ViewLib::Util", "Cannot open image: %1") <<
                qApp->translate("djv::ViewLib::Util", "Cannot read image: %1") <<
                qApp->translate("djv::ViewLib::Util", "Cannot write image: %1") <<
                qApp->translate("djv::ViewLib::Util", "Cannot open LUT: %1") <<
                qApp->translate("djv::ViewLib::Util", "Cannot pick color") <<
                qApp->translate("djv::ViewLib::Util", "Cannot compute histogram") <<
                qApp->translate("djv::ViewLib::Util", "Cannot magnify");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

    } // namespace ViewLib

    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::VIEW_MAX, ViewLib::Util::viewMaxLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::TOOL_BAR, ViewLib::Util::toolBarLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::GRID, ViewLib::Util::gridLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::HUD_BACKGROUND, ViewLib::Util::hudBackgroundLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::IMAGE_SCALE, ViewLib::Util::imageScaleLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::IMAGE_ROTATE, ViewLib::Util::imageRotateLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::PLAYBACK, ViewLib::Util::playbackLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::FRAME, ViewLib::Util::frameLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::LOOP, ViewLib::Util::loopLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::LAYOUT, ViewLib::Util::layoutLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::TOOL, ViewLib::Util::toolLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::HISTOGRAM, ViewLib::Util::histogramLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::MOUSE_WHEEL, ViewLib::Util::mouseWheelLabels());
    _DJV_STRING_OPERATOR_LABEL(ViewLib::Util::ZOOM_FACTOR, ViewLib::Util::zoomFactorLabels());

} // namespace djv
