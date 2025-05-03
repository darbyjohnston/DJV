// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp//Widgets/Viewport.h>

#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/FilesModel.h>
#include <djvApp/Models/TimeUnitsModel.h>
#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <tlTimeline/Util.h>

#include <dtk/ui/ColorSwatch.h>
#include <dtk/ui/GridLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Spacer.h>
#include <dtk/core/Format.h>

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
            dtk::ImageOptions imageOptions;
            tl::timeline::DisplayOptions displayOptions;
            dtk::Color4F colorPicker;
            tl::timeline::PlayerCacheInfo cacheInfo;
            dtk::KeyModifier colorPickerModifier = dtk::KeyModifier::None;
            dtk::KeyModifier frameShuttleModifier = dtk::KeyModifier::Shift;

            std::shared_ptr<dtk::Label> fileNameLabel;
            std::shared_ptr<dtk::Label> timeLabel;
            std::shared_ptr<dtk::ColorSwatch> colorPickerSwatch;
            std::shared_ptr<dtk::Label> colorPickerLabel;
            std::shared_ptr<dtk::Label> cacheLabel;
            std::shared_ptr<dtk::GridLayout> hudLayout;

            std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > currentTimeObserver;
            std::shared_ptr<dtk::ListObserver<tl::timeline::VideoData> > videoDataObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::PlayerCacheInfo> > cacheObserver;
            std::shared_ptr<dtk::ValueObserver<double> > fpsObserver;
            std::shared_ptr<dtk::ValueObserver<size_t> > droppedFramesObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::CompareOptions> > compareOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::OCIOOptions> > ocioOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<dtk::Color4F> > colorPickerObserver;
            std::shared_ptr<dtk::ValueObserver<dtk::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::BackgroundOptions> > bgOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::ForegroundOptions> > fgOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<dtk::ImageType> > colorBufferObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > hudObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::TimeUnits> > timeUnitsObserver;
            std::shared_ptr<dtk::ValueObserver<MouseSettings> > mouseSettingsObserver;

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
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            tl::timelineui::Viewport::_init(context, parent);
            DTK_P();

            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.app = app;

            p.fileNameLabel = dtk::Label::create(context);
            p.fileNameLabel->setFontRole(dtk::FontRole::Mono);
            p.fileNameLabel->setMarginRole(dtk::SizeRole::MarginInside);
            p.fileNameLabel->setBackgroundRole(dtk::ColorRole::Overlay);

            p.timeLabel = dtk::Label::create(context);
            p.timeLabel->setFontRole(dtk::FontRole::Mono);
            p.timeLabel->setMarginRole(dtk::SizeRole::MarginInside);
            p.timeLabel->setBackgroundRole(dtk::ColorRole::Overlay);
            p.timeLabel->setHAlign(dtk::HAlign::Right);

            p.colorPickerSwatch = dtk::ColorSwatch::create(context);
            p.colorPickerSwatch->setSizeRole(dtk::SizeRole::MarginLarge);
            p.colorPickerLabel = dtk::Label::create(context);
            p.colorPickerLabel->setFontRole(dtk::FontRole::Mono);

            p.cacheLabel = dtk::Label::create(context);
            p.cacheLabel->setFontRole(dtk::FontRole::Mono);
            p.cacheLabel->setMarginRole(dtk::SizeRole::MarginInside);
            p.cacheLabel->setBackgroundRole(dtk::ColorRole::Overlay);
            p.cacheLabel->setHAlign(dtk::HAlign::Right);

            p.hudLayout = dtk::GridLayout::create(context, shared_from_this());
            p.hudLayout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.hudLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.fileNameLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.fileNameLabel, 0, 0);
            p.timeLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.timeLabel, 0, 2);

            auto spacer = dtk::Spacer::create(context, dtk::Orientation::Vertical, p.hudLayout);
            spacer->setStretch(dtk::Stretch::Expanding);
            p.hudLayout->setGridPos(spacer, 1, 1);

            auto hLayout = dtk::HorizontalLayout::create(context, p.hudLayout);
            p.hudLayout->setGridPos(hLayout, 2, 0);
            hLayout->setMarginRole(dtk::SizeRole::MarginInside);
            hLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            hLayout->setBackgroundRole(dtk::ColorRole::Overlay);
            p.colorPickerSwatch->setParent(hLayout);
            p.colorPickerLabel->setParent(hLayout);
            p.cacheLabel->setParent(p.hudLayout);
            p.hudLayout->setGridPos(p.cacheLabel, 2, 2);

            p.fpsObserver = dtk::ValueObserver<double>::create(
                observeFPS(),
                [this](double value)
                {
                    _p->fps = value;
                    _hudUpdate();
                });

            p.droppedFramesObserver = dtk::ValueObserver<size_t>::create(
                observeDroppedFrames(),
                [this](size_t value)
                {
                    _p->droppedFrames = value;
                    _hudUpdate();
                });

            p.compareOptionsObserver = dtk::ValueObserver<tl::timeline::CompareOptions>::create(
                app->getFilesModel()->observeCompareOptions(),
                [this](const tl::timeline::CompareOptions& value)
                {
                    setCompareOptions(value);
                });

            p.ocioOptionsObserver = dtk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                   setOCIOOptions(value);
                });

            p.lutOptionsObserver = dtk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                   setLUTOptions(value);
                });

            p.colorPickerObserver = dtk::ValueObserver<dtk::Color4F>::create(
                app->getViewportModel()->observeColorPicker(),
                [this](const dtk::Color4F& value)
                {
                    _p->colorPicker = value;
                    _hudUpdate();
                });

            p.imageOptionsObserver = dtk::ValueObserver<dtk::ImageOptions>::create(
                app->getViewportModel()->observeImageOptions(),
                [this](const dtk::ImageOptions& value)
                {
                    _p->imageOptions = value;
                    _videoDataUpdate();
                });

            p.displayOptionsObserver = dtk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->displayOptions = value;
                    _videoDataUpdate();
                });

            p.bgOptionsObserver = dtk::ValueObserver<tl::timeline::BackgroundOptions>::create(
                app->getViewportModel()->observeBackgroundOptions(),
                [this](const tl::timeline::BackgroundOptions& value)
                {
                    setBackgroundOptions(value);
                });

            p.fgOptionsObserver = dtk::ValueObserver<tl::timeline::ForegroundOptions>::create(
                app->getViewportModel()->observeForegroundOptions(),
                [this](const tl::timeline::ForegroundOptions& value)
                {
                    setForegroundOptions(value);
                });

            p.colorBufferObserver = dtk::ValueObserver<dtk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](dtk::ImageType value)
                {
                    setColorBuffer(value);
                    _hudUpdate();
                });

            p.hudObserver = dtk::ValueObserver<bool>::create(
                app->getViewportModel()->observeHUD(),
                [this](bool value)
                {
                    _p->hud = value;
                    _hudUpdate();
                });

            p.timeUnitsObserver = dtk::ValueObserver<tl::timeline::TimeUnits>::create(
                app->getTimeUnitsModel()->observeTimeUnits(),
                [this](tl::timeline::TimeUnits value)
                {
                    _hudUpdate();
                });

            p.mouseSettingsObserver = dtk::ValueObserver<MouseSettings>::create(
                app->getSettingsModel()->observeMouse(),
                [this](const MouseSettings& value)
                {
                    auto i = value.actions.find(MouseAction::PanView);
                    setPanModifier(i != value.actions.end() ? i->second : dtk::KeyModifier::None);
                    i = value.actions.find(MouseAction::CompareWipe);
                    setWipeModifier(i != value.actions.end() ? i->second : dtk::KeyModifier::None);
                    i = value.actions.find(MouseAction::ColorPicker);
                    _p->colorPickerModifier = i != value.actions.end() ? i->second : dtk::KeyModifier::None;
                    i = value.actions.find(MouseAction::FrameShuttle);
                    _p->frameShuttleModifier = i != value.actions.end() ? i->second : dtk::KeyModifier::None;
                });
        }

        Viewport::Viewport() :
            _p(new Private)
        {}

        Viewport::~Viewport()
        {}

        std::shared_ptr<Viewport> Viewport::create(
            const std::shared_ptr<dtk::Context>& context,
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
            DTK_P();
            if (player)
            {
                p.path = player->getPath();

                p.currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
                    player->observeCurrentTime(),
                    [this](const OTIO_NS::RationalTime& value)
                    {
                        _p->currentTime = value;
                        _hudUpdate();
                    });

                p.videoDataObserver = dtk::ListObserver<tl::timeline::VideoData>::create(
                    player->observeCurrentVideo(),
                    [this](const std::vector<tl::timeline::VideoData>& value)
                    {
                        _p->videoDataSize = value.size();
                        _videoDataUpdate();
                    });

                p.cacheObserver = dtk::ValueObserver<tl::timeline::PlayerCacheInfo>::create(
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

        void Viewport::setGeometry(const dtk::Box2I& value)
        {
            tl::timelineui::Viewport::setGeometry(value);
            DTK_P();
            p.hudLayout->setGeometry(value);
        }

        void Viewport::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            tl::timelineui::Viewport::sizeHintEvent(event);
            DTK_P();
            _setSizeHint(p.hudLayout->getSizeHint());
        }

        void Viewport::mouseMoveEvent(dtk::MouseMoveEvent& event)
        {
            tl::timelineui::Viewport::mouseMoveEvent(event);
            DTK_P();
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
                    const dtk::Color4F color = getColorSample(event.pos);
                    app->getViewportModel()->setColorPicker(color);
                }
                break;
            default: break;
            }
        }

        void Viewport::mousePressEvent(dtk::MouseClickEvent& event)
        {
            tl::timelineui::Viewport::mousePressEvent(event);
            DTK_P();
            takeKeyFocus();
            if (0 == event.button &&
                dtk::checkKeyModifier(p.colorPickerModifier, event.modifiers))
            {
                p.mouse.mode = Private::MouseMode::ColorPicker;
                if (auto app = p.app.lock())
                {
                    const dtk::Color4F color = getColorSample(event.pos);
                    app->getViewportModel()->setColorPicker(color);
                }
            }
            else if (0 == event.button &&
                dtk::checkKeyModifier(p.frameShuttleModifier, event.modifiers))
            {
                p.mouse.mode = Private::MouseMode::Shuttle;
                if (auto player = getPlayer())
                {
                    player->stop();
                    p.mouse.shuttleStart = player->getCurrentTime();
                }
            }
        }

        void Viewport::mouseReleaseEvent(dtk::MouseClickEvent& event)
        {
            tl::timelineui::Viewport::mouseReleaseEvent(event);
            DTK_P();
            p.mouse = Private::MouseData();
        }

        void Viewport::_videoDataUpdate()
        {
            DTK_P();
            std::vector<dtk::ImageOptions> imageOptions;
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
            DTK_P();

            p.fileNameLabel->setText(dtk::elide(p.path.get(-1, tl::file::PathType::FileName)));

            std::string s;
            if (auto app = p.app.lock())
            {
                auto timeUnitsModel = app->getTimeUnitsModel();
                s = timeUnitsModel->getLabel(p.currentTime);
            }
            p.timeLabel->setText(dtk::Format("{0}, {1} FPS, {2} dropped").
                arg(s).
                arg(p.fps, 2, 4).
                arg(p.droppedFrames));

            p.colorPickerSwatch->setColor(p.colorPicker);
            p.colorPickerLabel->setText(
                dtk::Format("Color: {0} {1} {2} {3}").
                arg(p.colorPicker.r, 2).
                arg(p.colorPicker.g, 2).
                arg(p.colorPicker.b, 2).
                arg(p.colorPicker.a, 2));

            p.cacheLabel->setText(
                dtk::Format("Cache: {0}% V, {1}% A").
                arg(p.cacheInfo.videoPercentage, 2, 5).
                arg(p.cacheInfo.audioPercentage, 2, 5));

            p.hudLayout->setVisible(p.hud);
        }
    }
}
