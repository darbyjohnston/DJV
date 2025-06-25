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

#include <feather-tk/ui/ColorSwatch.h>
#include <feather-tk/ui/GridLayout.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/Spacer.h>
#include <feather-tk/core/Format.h>

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
            feather_tk::ImageOptions imageOptions;
            tl::timeline::DisplayOptions displayOptions;
            feather_tk::Color4F colorPicker;
            tl::timeline::PlayerCacheInfo cacheInfo;
            feather_tk::KeyModifier colorPickerModifier = feather_tk::KeyModifier::None;
            feather_tk::KeyModifier frameShuttleModifier = feather_tk::KeyModifier::Shift;

            std::shared_ptr<feather_tk::Label> fileNameLabel;
            std::shared_ptr<feather_tk::Label> timeLabel;
            std::shared_ptr<feather_tk::ColorSwatch> colorPickerSwatch;
            std::shared_ptr<feather_tk::Label> colorPickerLabel;
            std::shared_ptr<feather_tk::Label> cacheLabel;
            std::shared_ptr<feather_tk::GridLayout> hudLayout;

            std::shared_ptr<feather_tk::ValueObserver<OTIO_NS::RationalTime> > currentTimeObserver;
            std::shared_ptr<feather_tk::ListObserver<tl::timeline::VideoData> > videoDataObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::PlayerCacheInfo> > cacheObserver;
            std::shared_ptr<feather_tk::ValueObserver<double> > fpsObserver;
            std::shared_ptr<feather_tk::ValueObserver<size_t> > droppedFramesObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::CompareOptions> > compareOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::OCIOOptions> > ocioOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::Color4F> > colorPickerObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::BackgroundOptions> > bgOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::ForegroundOptions> > fgOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::ImageType> > colorBufferObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > hudObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<feather_tk::ValueObserver<MouseSettings> > mouseSettingsObserver;

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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            tl::timelineui::Viewport::_init(context, parent);
            FEATHER_TK_P();

            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.app = app;

            p.fileNameLabel = feather_tk::Label::create(context);
            p.fileNameLabel->setFontRole(feather_tk::FontRole::Mono);
            p.fileNameLabel->setMarginRole(feather_tk::SizeRole::MarginInside);
            p.fileNameLabel->setBackgroundRole(feather_tk::ColorRole::Overlay);

            p.timeLabel = feather_tk::Label::create(context);
            p.timeLabel->setFontRole(feather_tk::FontRole::Mono);
            p.timeLabel->setMarginRole(feather_tk::SizeRole::MarginInside);
            p.timeLabel->setBackgroundRole(feather_tk::ColorRole::Overlay);
            p.timeLabel->setHAlign(feather_tk::HAlign::Right);

            p.colorPickerSwatch = feather_tk::ColorSwatch::create(context);
            p.colorPickerSwatch->setSizeRole(feather_tk::SizeRole::MarginLarge);
            p.colorPickerLabel = feather_tk::Label::create(context);
            p.colorPickerLabel->setFontRole(feather_tk::FontRole::Mono);

            p.cacheLabel = feather_tk::Label::create(context);
            p.cacheLabel->setFontRole(feather_tk::FontRole::Mono);
            p.cacheLabel->setMarginRole(feather_tk::SizeRole::MarginInside);
            p.cacheLabel->setBackgroundRole(feather_tk::ColorRole::Overlay);
            p.cacheLabel->setHAlign(feather_tk::HAlign::Right);

            p.hudLayout = feather_tk::GridLayout::create(context, shared_from_this());
            p.hudLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            p.hudLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.fileNameLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.fileNameLabel, 0, 0);
            p.timeLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.timeLabel, 0, 2);

            auto spacer = feather_tk::Spacer::create(context, feather_tk::Orientation::Vertical, p.hudLayout);
            spacer->setStretch(feather_tk::Stretch::Expanding);
            p.hudLayout->setGridPos(spacer, 1, 1);

            auto hLayout = feather_tk::HorizontalLayout::create(context, p.hudLayout);
            p.hudLayout->setGridPos(hLayout, 2, 0);
            hLayout->setMarginRole(feather_tk::SizeRole::MarginInside);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            hLayout->setBackgroundRole(feather_tk::ColorRole::Overlay);
            p.colorPickerSwatch->setParent(hLayout);
            p.colorPickerLabel->setParent(hLayout);
            p.cacheLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.cacheLabel, 2, 2);

            p.fpsObserver = feather_tk::ValueObserver<double>::create(
                observeFPS(),
                [this](double value)
                {
                    _p->fps = value;
                    _hudUpdate();
                });

            p.droppedFramesObserver = feather_tk::ValueObserver<size_t>::create(
                observeDroppedFrames(),
                [this](size_t value)
                {
                    _p->droppedFrames = value;
                    _hudUpdate();
                });

            p.compareOptionsObserver = feather_tk::ValueObserver<tl::timeline::CompareOptions>::create(
                app->getFilesModel()->observeCompareOptions(),
                [this](const tl::timeline::CompareOptions& value)
                {
                    setCompareOptions(value);
                });

            p.ocioOptionsObserver = feather_tk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                   setOCIOOptions(value);
                });

            p.lutOptionsObserver = feather_tk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                   setLUTOptions(value);
                });

            p.colorPickerObserver = feather_tk::ValueObserver<feather_tk::Color4F>::create(
                app->getViewportModel()->observeColorPicker(),
                [this](const feather_tk::Color4F& value)
                {
                    _p->colorPicker = value;
                    _hudUpdate();
                });

            p.imageOptionsObserver = feather_tk::ValueObserver<feather_tk::ImageOptions>::create(
                app->getViewportModel()->observeImageOptions(),
                [this](const feather_tk::ImageOptions& value)
                {
                    _p->imageOptions = value;
                    _videoDataUpdate();
                });

            p.displayOptionsObserver = feather_tk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->displayOptions = value;
                    _videoDataUpdate();
                });

            p.bgOptionsObserver = feather_tk::ValueObserver<tl::timeline::BackgroundOptions>::create(
                app->getViewportModel()->observeBackgroundOptions(),
                [this](const tl::timeline::BackgroundOptions& value)
                {
                    setBackgroundOptions(value);
                });

            p.fgOptionsObserver = feather_tk::ValueObserver<tl::timeline::ForegroundOptions>::create(
                app->getViewportModel()->observeForegroundOptions(),
                [this](const tl::timeline::ForegroundOptions& value)
                {
                    setForegroundOptions(value);
                });

            p.colorBufferObserver = feather_tk::ValueObserver<feather_tk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](feather_tk::ImageType value)
                {
                    setColorBuffer(value);
                    _hudUpdate();
                });

            p.hudObserver = feather_tk::ValueObserver<bool>::create(
                app->getViewportModel()->observeHUD(),
                [this](bool value)
                {
                    _p->hud = value;
                    _hudUpdate();
                });

            p.timeUnitsObserver = feather_tk::ValueObserver<tl::timeline::TimeUnits>::create(
                app->getTimeUnitsModel()->observeTimeUnits(),
                [this](tl::timeline::TimeUnits value)
                {
                    _hudUpdate();
                });

            p.mouseSettingsObserver = feather_tk::ValueObserver<MouseSettings>::create(
                app->getSettingsModel()->observeMouse(),
                [this](const MouseSettings& value)
                {
                    auto i = value.actions.find(MouseAction::PanView);
                    setPanModifier(i != value.actions.end() ? i->second : feather_tk::KeyModifier::None);
                    i = value.actions.find(MouseAction::CompareWipe);
                    setWipeModifier(i != value.actions.end() ? i->second : feather_tk::KeyModifier::None);
                    i = value.actions.find(MouseAction::ColorPicker);
                    _p->colorPickerModifier = i != value.actions.end() ? i->second : feather_tk::KeyModifier::None;
                    i = value.actions.find(MouseAction::FrameShuttle);
                    _p->frameShuttleModifier = i != value.actions.end() ? i->second : feather_tk::KeyModifier::None;
                });
        }

        Viewport::Viewport() :
            _p(new Private)
        {}

        Viewport::~Viewport()
        {}

        std::shared_ptr<Viewport> Viewport::create(
            const std::shared_ptr<feather_tk::Context>& context,
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
            FEATHER_TK_P();
            if (player)
            {
                p.path = player->getPath();

                p.currentTimeObserver = feather_tk::ValueObserver<OTIO_NS::RationalTime>::create(
                    player->observeCurrentTime(),
                    [this](const OTIO_NS::RationalTime& value)
                    {
                        _p->currentTime = value;
                        _hudUpdate();
                    });

                p.videoDataObserver = feather_tk::ListObserver<tl::timeline::VideoData>::create(
                    player->observeCurrentVideo(),
                    [this](const std::vector<tl::timeline::VideoData>& value)
                    {
                        _p->videoDataSize = value.size();
                        _videoDataUpdate();
                    });

                p.cacheObserver = feather_tk::ValueObserver<tl::timeline::PlayerCacheInfo>::create(
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

        void Viewport::setGeometry(const feather_tk::Box2I& value)
        {
            tl::timelineui::Viewport::setGeometry(value);
            FEATHER_TK_P();
            p.hudLayout->setGeometry(value);
        }

        void Viewport::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            tl::timelineui::Viewport::sizeHintEvent(event);
            FEATHER_TK_P();
            _setSizeHint(p.hudLayout->getSizeHint());
        }

        void Viewport::mouseMoveEvent(feather_tk::MouseMoveEvent& event)
        {
            tl::timelineui::Viewport::mouseMoveEvent(event);
            FEATHER_TK_P();
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
                    const feather_tk::Color4F color = getColorSample(event.pos);
                    app->getViewportModel()->setColorPicker(color);
                }
                break;
            default: break;
            }
        }

        void Viewport::mousePressEvent(feather_tk::MouseClickEvent& event)
        {
            tl::timelineui::Viewport::mousePressEvent(event);
            FEATHER_TK_P();
            takeKeyFocus();
            if (0 == event.button &&
                feather_tk::checkKeyModifier(p.colorPickerModifier, event.modifiers))
            {
                p.mouse.mode = Private::MouseMode::ColorPicker;
                if (auto app = p.app.lock())
                {
                    const feather_tk::Color4F color = getColorSample(event.pos);
                    app->getViewportModel()->setColorPicker(color);
                }
            }
            else if (0 == event.button &&
                feather_tk::checkKeyModifier(p.frameShuttleModifier, event.modifiers))
            {
                p.mouse.mode = Private::MouseMode::Shuttle;
                if (auto player = getPlayer())
                {
                    player->stop();
                    p.mouse.shuttleStart = player->getCurrentTime();
                }
            }
        }

        void Viewport::mouseReleaseEvent(feather_tk::MouseClickEvent& event)
        {
            tl::timelineui::Viewport::mouseReleaseEvent(event);
            FEATHER_TK_P();
            p.mouse = Private::MouseData();
        }

        void Viewport::_videoDataUpdate()
        {
            FEATHER_TK_P();
            std::vector<feather_tk::ImageOptions> imageOptions;
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
            FEATHER_TK_P();

            p.fileNameLabel->setText(feather_tk::elide(p.path.get(-1, tl::file::PathType::FileName)));

            std::string s;
            if (auto app = p.app.lock())
            {
                auto timeUnitsModel = app->getTimeUnitsModel();
                s = timeUnitsModel->getLabel(p.currentTime);
            }
            p.timeLabel->setText(feather_tk::Format("{0}, {1} FPS, {2} dropped").
                arg(s).
                arg(p.fps, 2, 4).
                arg(p.droppedFrames));

            p.colorPickerSwatch->setColor(p.colorPicker);
            p.colorPickerLabel->setText(
                feather_tk::Format("Color: {0} {1} {2} {3}").
                arg(p.colorPicker.r, 2).
                arg(p.colorPicker.g, 2).
                arg(p.colorPicker.b, 2).
                arg(p.colorPicker.a, 2));

            p.cacheLabel->setText(
                feather_tk::Format("Cache: {0}% V, {1}% A").
                arg(p.cacheInfo.videoPercentage, 2, 5).
                arg(p.cacheInfo.audioPercentage, 2, 5));

            p.hudLayout->setVisible(p.hud);
        }
    }
}
