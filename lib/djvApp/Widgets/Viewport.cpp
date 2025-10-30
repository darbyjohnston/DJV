// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp//Widgets/Viewport.h>

#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/FilesModel.h>
#include <djvApp/Models/SettingsModel.h>
#include <djvApp/Models/TimeUnitsModel.h>
#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <tlTimeline/Util.h>

#include <ftk/UI/ColorSwatch.h>
#include <ftk/UI/GridLayout.h>
#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/UI/Spacer.h>
#include <ftk/Core/Format.h>

#include <regex>

namespace djv
{
    namespace app
    {
        struct Viewport::Private
        {
            std::weak_ptr<App> app;
            bool hud = false;
            tl::file::Path path;
            OTIO_NS::RationalTime currentTime = tl::time::invalidTime;
            double fps = 0.0;
            size_t droppedFrames = 0;
            size_t videoDataSize = 0;
            ftk::ImageOptions imageOptions;
            tl::timeline::DisplayOptions displayOptions;
            ftk::Color4F colorPicker;
            tl::timeline::PlayerCacheInfo cacheInfo;
            MouseActionBinding colorPickerBinding = MouseActionBinding(1);
            MouseActionBinding frameShuttleBinding = MouseActionBinding(1, ftk::KeyModifier::Shift);

            std::shared_ptr<ftk::Label> fileNameLabel;
            std::shared_ptr<ftk::Label> timeLabel;
            std::shared_ptr<ftk::ColorSwatch> colorPickerSwatch;
            std::shared_ptr<ftk::Label> colorPickerLabel;
            std::shared_ptr<ftk::Label> cacheLabel;
            std::shared_ptr<ftk::GridLayout> hudLayout;

            std::shared_ptr<ftk::ValueObserver<OTIO_NS::RationalTime> > currentTimeObserver;
            std::shared_ptr<ftk::ListObserver<tl::timeline::VideoData> > videoDataObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::PlayerCacheInfo> > cacheObserver;
            std::shared_ptr<ftk::ValueObserver<double> > fpsObserver;
            std::shared_ptr<ftk::ValueObserver<size_t> > droppedFramesObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::CompareOptions> > compareOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::OCIOOptions> > ocioOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<ftk::Color4F> > colorPickerObserver;
            std::shared_ptr<ftk::ValueObserver<ftk::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::BackgroundOptions> > bgOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::ForegroundOptions> > fgOptionsObserver;
            std::shared_ptr<ftk::ValueObserver<ftk::ImageType> > colorBufferObserver;
            std::shared_ptr<ftk::ValueObserver<bool> > hudObserver;
            std::shared_ptr<ftk::ValueObserver<tl::timeline::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<ftk::ValueObserver<MouseSettings> > mouseSettingsObserver;

            enum class MouseMode
            {
                None,
                Shuttle,
                ColorPicker
            };
            struct MouseData
            {
                MouseMode mode = MouseMode::None;
                OTIO_NS::RationalTime shuttleStart = tl::time::invalidTime;
            };
            MouseData mouse;
        };

        void Viewport::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            tl::timelineui::Viewport::_init(context, parent);
            FTK_P();

            p.app = app;

            p.fileNameLabel = ftk::Label::create(context);
            p.fileNameLabel->setFontRole(ftk::FontRole::Mono);
            p.fileNameLabel->setMarginRole(ftk::SizeRole::MarginInside);
            p.fileNameLabel->setBackgroundRole(ftk::ColorRole::Overlay);

            p.timeLabel = ftk::Label::create(context);
            p.timeLabel->setFontRole(ftk::FontRole::Mono);
            p.timeLabel->setMarginRole(ftk::SizeRole::MarginInside);
            p.timeLabel->setBackgroundRole(ftk::ColorRole::Overlay);
            p.timeLabel->setHAlign(ftk::HAlign::Right);

            p.colorPickerSwatch = ftk::ColorSwatch::create(context);
            p.colorPickerSwatch->setSizeRole(ftk::SizeRole::MarginLarge);
            p.colorPickerLabel = ftk::Label::create(context);
            p.colorPickerLabel->setFontRole(ftk::FontRole::Mono);

            p.cacheLabel = ftk::Label::create(context);
            p.cacheLabel->setFontRole(ftk::FontRole::Mono);
            p.cacheLabel->setMarginRole(ftk::SizeRole::MarginInside);
            p.cacheLabel->setBackgroundRole(ftk::ColorRole::Overlay);
            p.cacheLabel->setHAlign(ftk::HAlign::Right);

            p.hudLayout = ftk::GridLayout::create(context, shared_from_this());
            p.hudLayout->setMarginRole(ftk::SizeRole::MarginSmall);
            p.hudLayout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.fileNameLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.fileNameLabel, 0, 0);
            p.timeLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.timeLabel, 0, 2);

            auto spacer = ftk::Spacer::create(context, ftk::Orientation::Vertical, p.hudLayout);
            spacer->setStretch(ftk::Stretch::Expanding);
            p.hudLayout->setGridPos(spacer, 1, 1);

            auto hLayout = ftk::HorizontalLayout::create(context, p.hudLayout);
            p.hudLayout->setGridPos(hLayout, 2, 0);
            hLayout->setMarginRole(ftk::SizeRole::MarginInside);
            hLayout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            hLayout->setBackgroundRole(ftk::ColorRole::Overlay);
            p.colorPickerSwatch->setParent(hLayout);
            p.colorPickerLabel->setParent(hLayout);
            p.cacheLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.cacheLabel, 2, 2);

            p.fpsObserver = ftk::ValueObserver<double>::create(
                observeFPS(),
                [this](double value)
                {
                    _p->fps = value;
                    _hudUpdate();
                });

            p.droppedFramesObserver = ftk::ValueObserver<size_t>::create(
                observeDroppedFrames(),
                [this](size_t value)
                {
                    _p->droppedFrames = value;
                    _hudUpdate();
                });

            p.compareOptionsObserver = ftk::ValueObserver<tl::timeline::CompareOptions>::create(
                app->getFilesModel()->observeCompareOptions(),
                [this](const tl::timeline::CompareOptions& value)
                {
                    setCompareOptions(value);
                });

            p.ocioOptionsObserver = ftk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                   setOCIOOptions(value);
                });

            p.lutOptionsObserver = ftk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                   setLUTOptions(value);
                });

            p.colorPickerObserver = ftk::ValueObserver<ftk::Color4F>::create(
                app->getViewportModel()->observeColorPicker(),
                [this](const ftk::Color4F& value)
                {
                    _p->colorPicker = value;
                    _hudUpdate();
                });

            p.imageOptionsObserver = ftk::ValueObserver<ftk::ImageOptions>::create(
                app->getViewportModel()->observeImageOptions(),
                [this](const ftk::ImageOptions& value)
                {
                    _p->imageOptions = value;
                    _videoDataUpdate();
                });

            p.displayOptionsObserver = ftk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->displayOptions = value;
                    _videoDataUpdate();
                });

            p.bgOptionsObserver = ftk::ValueObserver<tl::timeline::BackgroundOptions>::create(
                app->getViewportModel()->observeBackgroundOptions(),
                [this](const tl::timeline::BackgroundOptions& value)
                {
                    setBackgroundOptions(value);
                });

            p.fgOptionsObserver = ftk::ValueObserver<tl::timeline::ForegroundOptions>::create(
                app->getViewportModel()->observeForegroundOptions(),
                [this](const tl::timeline::ForegroundOptions& value)
                {
                    setForegroundOptions(value);
                });

            p.colorBufferObserver = ftk::ValueObserver<ftk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](ftk::ImageType value)
                {
                    setColorBuffer(value);
                    _hudUpdate();
                });

            p.hudObserver = ftk::ValueObserver<bool>::create(
                app->getViewportModel()->observeHUD(),
                [this](bool value)
                {
                    _p->hud = value;
                    _hudUpdate();
                });

            p.timeUnitsObserver = ftk::ValueObserver<tl::timeline::TimeUnits>::create(
                app->getTimeUnitsModel()->observeTimeUnits(),
                [this](tl::timeline::TimeUnits value)
                {
                    _hudUpdate();
                });

            p.mouseSettingsObserver = ftk::ValueObserver<MouseSettings>::create(
                app->getSettingsModel()->observeMouse(),
                [this](const MouseSettings& value)
                {
                    auto i = value.bindings.find(MouseAction::PanView);
                    setPanBinding(
                        i != value.bindings.end() ? i->second.button : 0,
                        i != value.bindings.end() ? i->second.modifier : ftk::KeyModifier::None);
                    i = value.bindings.find(MouseAction::CompareWipe);
                    setWipeBinding(
                        i != value.bindings.end() ? i->second.button : 0,
                        i != value.bindings.end() ? i->second.modifier : ftk::KeyModifier::None);
                    i = value.bindings.find(MouseAction::ColorPicker);
                    _p->colorPickerBinding = i != value.bindings.end() ? i->second : MouseActionBinding();
                    i = value.bindings.find(MouseAction::FrameShuttle);
                    _p->frameShuttleBinding = i != value.bindings.end() ? i->second : MouseActionBinding();
                });
        }

        Viewport::Viewport() :
            _p(new Private)
        {}

        Viewport::~Viewport()
        {}

        std::shared_ptr<Viewport> Viewport::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<Viewport>(new Viewport);
            out->_init(context, app, parent);
            return out;
        }

        void Viewport::setPlayer(const std::shared_ptr<tl::timeline::Player>& player)
        {
            tl::timelineui::Viewport::setPlayer(player);
            FTK_P();
            if (player)
            {
                p.path = player->getPath();

                p.currentTimeObserver = ftk::ValueObserver<OTIO_NS::RationalTime>::create(
                    player->observeCurrentTime(),
                    [this](const OTIO_NS::RationalTime& value)
                    {
                        _p->currentTime = value;
                        _hudUpdate();
                    });

                p.videoDataObserver = ftk::ListObserver<tl::timeline::VideoData>::create(
                    player->observeCurrentVideo(),
                    [this](const std::vector<tl::timeline::VideoData>& value)
                    {
                        _p->videoDataSize = value.size();
                        _videoDataUpdate();
                    });

                p.cacheObserver = ftk::ValueObserver<tl::timeline::PlayerCacheInfo>::create(
                    player->observeCacheInfo(),
                    [this](const tl::timeline::PlayerCacheInfo& value)
                    {
                        _p->cacheInfo = value;
                        _hudUpdate();
                    });
            }
            else
            {
                p.path = tl::file::Path();
                p.currentTime = tl::time::invalidTime;
                p.currentTimeObserver.reset();
                p.videoDataObserver.reset();
                p.cacheInfo = tl::timeline::PlayerCacheInfo();
                p.cacheObserver.reset();
                p.videoDataObserver.reset();
                _hudUpdate();
            }
        }

        void Viewport::setGeometry(const ftk::Box2I& value)
        {
            tl::timelineui::Viewport::setGeometry(value);
            FTK_P();
            p.hudLayout->setGeometry(value);
        }

        void Viewport::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            tl::timelineui::Viewport::sizeHintEvent(event);
            FTK_P();
            _setSizeHint(p.hudLayout->getSizeHint());
        }

        void Viewport::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            tl::timelineui::Viewport::mouseMoveEvent(event);
            FTK_P();
            switch (p.mouse.mode)
            {
            case Private::MouseMode::Shuttle:
                if (auto player = getPlayer())
                {
                    const OTIO_NS::RationalTime offset = OTIO_NS::RationalTime(
                        (event.pos.x - _getMousePressPos().x) * .05F,
                        p.mouse.shuttleStart.rate()).round();
                    const OTIO_NS::TimeRange& timeRange = player->getTimeRange();
                    OTIO_NS::RationalTime t = p.mouse.shuttleStart + offset;
                    if (t < timeRange.start_time())
                    {
                        t = timeRange.end_time_exclusive() - (timeRange.start_time() - t);
                    }
                    else if (t > timeRange.end_time_exclusive())
                    {
                        t = timeRange.start_time() + (t - timeRange.end_time_exclusive());
                    }
                    player->seek(t);
                }
                break;
            case Private::MouseMode::ColorPicker:
                if (auto app = p.app.lock())
                {
                    const ftk::Color4F color = getColorSample(event.pos);
                    app->getViewportModel()->setColorPicker(color);
                }
                break;
            default: break;
            }
        }

        void Viewport::mousePressEvent(ftk::MouseClickEvent& event)
        {
            tl::timelineui::Viewport::mousePressEvent(event);
            FTK_P();
            if (p.colorPickerBinding.button == event.button &&
                ftk::checkKeyModifier(p.colorPickerBinding.modifier, event.modifiers))
            {
                p.mouse.mode = Private::MouseMode::ColorPicker;
                if (auto app = p.app.lock())
                {
                    const ftk::Color4F color = getColorSample(event.pos);
                    app->getViewportModel()->setColorPicker(color);
                }
            }
            else if (p.frameShuttleBinding.button == event.button &&
                ftk::checkKeyModifier(p.frameShuttleBinding.modifier, event.modifiers))
            {
                p.mouse.mode = Private::MouseMode::Shuttle;
                if (auto player = getPlayer())
                {
                    player->stop();
                    p.mouse.shuttleStart = player->getCurrentTime();
                }
            }
        }

        void Viewport::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            tl::timelineui::Viewport::mouseReleaseEvent(event);
            FTK_P();
            p.mouse = Private::MouseData();
        }

        void Viewport::_videoDataUpdate()
        {
            FTK_P();
            std::vector<ftk::ImageOptions> imageOptions;
            std::vector<tl::timeline::DisplayOptions> displayOptions;
            for (size_t i = 0; i < p.videoDataSize; ++i)
            {
                imageOptions.push_back(p.imageOptions);
                displayOptions.push_back(p.displayOptions);
            }
            setImageOptions(imageOptions);
            setDisplayOptions(displayOptions);
        }

        void Viewport::_hudUpdate()
        {
            FTK_P();

            p.fileNameLabel->setText(ftk::elide(p.path.get(-1, tl::file::PathType::FileName)));

            std::string s;
            if (auto app = p.app.lock())
            {
                auto timeUnitsModel = app->getTimeUnitsModel();
                s = timeUnitsModel->getLabel(p.currentTime);
            }
            p.timeLabel->setText(ftk::Format("Time: {0}, {1} FPS, {2} dropped").
                arg(s).
                arg(p.fps, 2, 4).
                arg(p.droppedFrames));

            p.colorPickerSwatch->setColor(p.colorPicker);
            p.colorPickerLabel->setText(
                ftk::Format("Color: {0} {1} {2} {3}").
                arg(p.colorPicker.r, 2).
                arg(p.colorPicker.g, 2).
                arg(p.colorPicker.b, 2).
                arg(p.colorPicker.a, 2));

            p.cacheLabel->setText(
                ftk::Format("Cache: {0}% V, {1}% A").
                arg(static_cast<int>(p.cacheInfo.videoPercentage)).
                arg(static_cast<int>(p.cacheInfo.audioPercentage)));

            p.hudLayout->setVisible(p.hud);
        }
    }
}
