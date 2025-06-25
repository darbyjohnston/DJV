// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/StatusBar.h>

#include <djvApp/Models/ToolsModel.h>
#include <djvApp/App.h>

#if defined(TLRENDER_BMD)
#include <tlDevice/BMDOutputDevice.h>
#endif // TLRENDER_BMD

#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/Icon.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/core/Context.h>
#include <feather-tk/core/Format.h>
#include <feather-tk/core/String.h>
#include <feather-tk/core/Timer.h>

namespace djv
{
    namespace app
    {
        struct StatusBar::Private
        {
            std::weak_ptr<App> app;

            std::shared_ptr<feather_tk::Label> logLabel;
            std::shared_ptr<feather_tk::Label> infoLabel;
#if defined(TLRENDER_BMD)
            std::shared_ptr<feather_tk::Icon> deviceActiveIcon;
#endif // TLRENDER_BMD
            std::shared_ptr<feather_tk::HorizontalLayout> layout;

            std::shared_ptr<feather_tk::Timer> logTimer;

            std::shared_ptr<feather_tk::ListObserver<feather_tk::LogItem> > logObserver;
            std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
#if defined(TLRENDER_BMD)
            std::shared_ptr<feather_tk::ValueObserver<bool> > bmdActiveObserver;
#endif // TLRENDER_BMD
        };

        void StatusBar::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(
                context,
                "djv::app::StatusBar",
                parent);
            FEATHER_TK_P();

            setHStretch(feather_tk::Stretch::Expanding);
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.app = app;

            p.logLabel = feather_tk::Label::create(context);
            p.logLabel->setMarginRole(feather_tk::SizeRole::MarginSmall, feather_tk::SizeRole::MarginInside);
            p.logLabel->setHStretch(feather_tk::Stretch::Expanding);

            p.infoLabel = feather_tk::Label::create(context);
            p.infoLabel->setMarginRole(feather_tk::SizeRole::MarginSmall, feather_tk::SizeRole::MarginInside);

#if defined(TLRENDER_BMD)
            p.deviceActiveIcon = feather_tk::Icon::create(context, "Devices");
            p.deviceActiveIcon->setTooltip("Output device active");
#endif // TLRENDER_BMD

            p.layout = feather_tk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(feather_tk::SizeRole::None);
            p.logLabel->setParent(p.layout);
            feather_tk::Divider::create(context, feather_tk::Orientation::Horizontal, p.layout);
            p.infoLabel->setParent(p.layout);
#if defined(TLRENDER_BMD)
            feather_tk::Divider::create(context, feather_tk::Orientation::Horizontal, p.layout);
            p.deviceActiveIcon->setParent(p.layout);
#endif // TLRENDER_BMD

            _deviceUpdate(false);

            p.logTimer = feather_tk::Timer::create(context);

            p.logObserver = feather_tk::ListObserver<feather_tk::LogItem>::create(
                context->getLogSystem()->observeLogItems(),
                [this](const std::vector<feather_tk::LogItem>& value)
                {
                    _logUpdate(value);
                });

            p.playerObserver = feather_tk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& player)
                {
                    _infoUpdate(
                        player ? player->getPath() : tl::file::Path(),
                        player ? player->getIOInfo() : tl::io::Info());
                });

#if defined(TLRENDER_BMD)
            p.bmdActiveObserver = feather_tk::ValueObserver<bool>::create(
                app->getBMDOutputDevice()->observeActive(),
                [this](bool value)
                {
                    _deviceUpdate(value);
                });
#endif // TLRENDER_BMD
        }

        StatusBar::StatusBar() :
            _p(new Private)
        {}

        StatusBar::~StatusBar()
        {}

        std::shared_ptr<StatusBar> StatusBar::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<StatusBar>(new StatusBar);
            out->_init(context, app, parent);
            return out;
        }

        void StatusBar::setGeometry(const feather_tk::Box2I & value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void StatusBar::sizeHintEvent(const feather_tk::SizeHintEvent & event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void StatusBar::mousePressEvent(feather_tk::MouseClickEvent& event)
        {
            IWidget::mousePressEvent(event);
            event.accept = true;
        }

        void StatusBar::mouseReleaseEvent(feather_tk::MouseClickEvent& event)
        {
            IWidget::mouseReleaseEvent(event);
            FEATHER_TK_P();
            event.accept = true;
            Tool tool = Tool::None;
            if (feather_tk::contains(p.logLabel->getGeometry(), event.pos))
            {
                tool = Tool::Messages;
            }
            else if (feather_tk::contains(p.infoLabel->getGeometry(), event.pos))
            {
                tool = Tool::Info;
            }
#if defined(TLRENDER_BMD)
            else if (feather_tk::contains(p.deviceActiveIcon->getGeometry(), event.pos))
            {
                tool = Tool::Devices;
            }
#endif // TLRENDER_BMD
            if (tool != Tool::None)
            {
                if (auto app = p.app.lock())
                {
                    auto toolsModel = app->getToolsModel();
                    const Tool active = toolsModel->getActiveTool();
                    toolsModel->setActiveTool(tool != active ? tool : Tool::None);
                }
            }
        }

        void StatusBar::_logUpdate(const std::vector<feather_tk::LogItem>& value)
        {
            FEATHER_TK_P();
            for (const auto& i : value)
            {
                switch (i.type)
                {
                case feather_tk::LogType::Error:
                {
                    const std::string s = feather_tk::toString(i);
                    p.logLabel->setText(s);
                    p.logLabel->setTooltip(s);
                    p.logTimer->start(
                        std::chrono::seconds(5),
                        [this]
                        {
                            _p->logLabel->setText(std::string());
                            _p->logLabel->setTooltip(std::string());
                        });
                    break;
                }
                default: break;
                }
            }
        }

        void StatusBar::_infoUpdate(const tl::file::Path& path, const tl::io::Info& info)
        {
            FEATHER_TK_P();
            std::vector<std::string> s;
            s.push_back(feather_tk::elide(path.get(-1, tl::file::PathType::FileName)));
            if (!info.video.empty())
            {
                s.push_back(std::string(
                    feather_tk::Format("video: {0}x{1}:{2} {3}").
                    arg(info.video[0].size.w).
                    arg(info.video[0].size.h).
                    arg(info.video[0].getAspect(), 2).
                    arg(info.video[0].type)));
            }
            if (info.audio.isValid())
            {
                s.push_back(std::string(
                    feather_tk::Format("audio: {0}ch {1} {2}kHz").
                    arg(info.audio.channelCount).
                    arg(info.audio.dataType).
                    arg(info.audio.sampleRate / 1000)));
            }
            const std::string text = feather_tk::join(s, ", ");
            p.infoLabel->setText(text);

            std::vector<std::string> t;
            t.push_back(path.get());
            if (!info.video.empty())
            {
                t.push_back(std::string(
                    feather_tk::Format("Video: {0}x{1}:{2} {3}").
                    arg(info.video[0].size.w).
                    arg(info.video[0].size.h).
                    arg(info.video[0].getAspect()).
                    arg(info.video[0].type)));
            }
            if (info.audio.isValid())
            {
                t.push_back(std::string(
                    feather_tk::Format("Audio: {0} {1} {2} {3}kHz").
                    arg(info.audio.channelCount).
                    arg(1 == info.audio.channelCount ? "channel" : "channels").
                    arg(info.audio.dataType).
                    arg(info.audio.sampleRate / 1000)));
            }
            const std::string tooltip = feather_tk::join(t, "\n");
            p.infoLabel->setTooltip(tooltip);
        }

        void StatusBar::_deviceUpdate(bool value)
        {
            FEATHER_TK_P();
#if defined(TLRENDER_BMD)
            p.deviceActiveIcon->setBackgroundRole(value ? feather_tk::ColorRole::Checked : feather_tk::ColorRole::None);
#endif // TLRENDER_BMD  
        }
    }
}
