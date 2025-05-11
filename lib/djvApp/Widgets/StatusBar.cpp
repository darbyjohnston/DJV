// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/StatusBar.h>

#include <djvApp/Models/ToolsModel.h>
#include <djvApp/App.h>

#if defined(TLRENDER_BMD)
#include <tlDevice/BMDOutputDevice.h>
#endif // TLRENDER_BMD

#include <dtk/ui/Divider.h>
#include <dtk/ui/Icon.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/core/Context.h>
#include <dtk/core/Format.h>
#include <dtk/core/String.h>
#include <dtk/core/Timer.h>

namespace djv
{
    namespace app
    {
        struct StatusBar::Private
        {
            std::weak_ptr<App> app;

            std::shared_ptr<dtk::Label> logLabel;
            std::shared_ptr<dtk::Label> infoLabel;
#if defined(TLRENDER_BMD)
            std::shared_ptr<dtk::Icon> deviceActiveIcon;
#endif // TLRENDER_BMD
            std::shared_ptr<dtk::HorizontalLayout> layout;

            std::shared_ptr<dtk::Timer> logTimer;

            std::shared_ptr<dtk::ListObserver<dtk::LogItem> > logObserver;
            std::shared_ptr<dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> > > playerObserver;
#if defined(TLRENDER_BMD)
            std::shared_ptr<dtk::ValueObserver<bool> > bmdActiveObserver;
#endif // TLRENDER_BMD
        };

        void StatusBar::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(
                context,
                "djv::app::StatusBar",
                parent);
            DTK_P();

            setHStretch(dtk::Stretch::Expanding);
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.app = app;

            p.logLabel = dtk::Label::create(context);
            p.logLabel->setMarginRole(dtk::SizeRole::MarginSmall, dtk::SizeRole::MarginInside);
            p.logLabel->setHStretch(dtk::Stretch::Expanding);

            p.infoLabel = dtk::Label::create(context);
            p.infoLabel->setMarginRole(dtk::SizeRole::MarginSmall, dtk::SizeRole::MarginInside);

#if defined(TLRENDER_BMD)
            p.deviceActiveIcon = dtk::Icon::create(context, "Devices");
            p.deviceActiveIcon->setTooltip("Output device active");
#endif // TLRENDER_BMD

            p.layout = dtk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::None);
            p.logLabel->setParent(p.layout);
            dtk::Divider::create(context, dtk::Orientation::Horizontal, p.layout);
            p.infoLabel->setParent(p.layout);
#if defined(TLRENDER_BMD)
            dtk::Divider::create(context, dtk::Orientation::Horizontal, p.layout);
            p.deviceActiveIcon->setParent(p.layout);
#endif // TLRENDER_BMD

            _deviceUpdate(false);

            p.logTimer = dtk::Timer::create(context);

            p.logObserver = dtk::ListObserver<dtk::LogItem>::create(
                context->getLogSystem()->observeLogItems(),
                [this](const std::vector<dtk::LogItem>& value)
                {
                    _logUpdate(value);
                });

            p.playerObserver = dtk::ValueObserver<std::shared_ptr<tl::timeline::Player> >::create(
                app->observePlayer(),
                [this](const std::shared_ptr<tl::timeline::Player>& player)
                {
                    _infoUpdate(
                        player ? player->getPath() : tl::file::Path(),
                        player ? player->getIOInfo() : tl::io::Info());
                });

#if defined(TLRENDER_BMD)
            p.bmdActiveObserver = dtk::ValueObserver<bool>::create(
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
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<StatusBar>(new StatusBar);
            out->_init(context, app, parent);
            return out;
        }

        void StatusBar::setGeometry(const dtk::Box2I & value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void StatusBar::sizeHintEvent(const dtk::SizeHintEvent & event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void StatusBar::mousePressEvent(dtk::MouseClickEvent& event)
        {
            IWidget::mousePressEvent(event);
            event.accept = true;
        }

        void StatusBar::mouseReleaseEvent(dtk::MouseClickEvent& event)
        {
            IWidget::mouseReleaseEvent(event);
            DTK_P();
            event.accept = true;
            Tool tool = Tool::None;
            if (dtk::contains(p.logLabel->getGeometry(), event.pos))
            {
                tool = Tool::Messages;
            }
            else if (dtk::contains(p.infoLabel->getGeometry(), event.pos))
            {
                tool = Tool::Info;
            }
#if defined(TLRENDER_BMD)
            else if (dtk::contains(p.deviceActiveIcon->getGeometry(), event.pos))
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

        void StatusBar::_logUpdate(const std::vector<dtk::LogItem>& value)
        {
            DTK_P();
            for (const auto& i : value)
            {
                switch (i.type)
                {
                case dtk::LogType::Error:
                {
                    const std::string s = dtk::toString(i);
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
            DTK_P();
            std::vector<std::string> s;
            s.push_back(dtk::elide(path.get(-1, tl::file::PathType::FileName)));
            if (!info.video.empty())
            {
                s.push_back(std::string(
                    dtk::Format("video: {0}x{1}:{2} {3}").
                    arg(info.video[0].size.w).
                    arg(info.video[0].size.h).
                    arg(info.video[0].getAspect(), 2).
                    arg(info.video[0].type)));
            }
            if (info.audio.isValid())
            {
                s.push_back(std::string(
                    dtk::Format("audio: {0}ch {1} {2}kHz").
                    arg(info.audio.channelCount).
                    arg(info.audio.dataType).
                    arg(info.audio.sampleRate / 1000)));
            }
            const std::string text = dtk::join(s, ", ");
            p.infoLabel->setText(text);

            std::vector<std::string> t;
            t.push_back(path.get());
            if (!info.video.empty())
            {
                t.push_back(std::string(
                    dtk::Format("Video: {0}x{1}:{2} {3}").
                    arg(info.video[0].size.w).
                    arg(info.video[0].size.h).
                    arg(info.video[0].getAspect()).
                    arg(info.video[0].type)));
            }
            if (info.audio.isValid())
            {
                t.push_back(std::string(
                    dtk::Format("Audio: {0} {1} {2} {3}kHz").
                    arg(info.audio.channelCount).
                    arg(1 == info.audio.channelCount ? "channel" : "channels").
                    arg(info.audio.dataType).
                    arg(info.audio.sampleRate / 1000)));
            }
            const std::string tooltip = dtk::join(t, "\n");
            p.infoLabel->setTooltip(tooltip);
        }

        void StatusBar::_deviceUpdate(bool value)
        {
            DTK_P();
#if defined(TLRENDER_BMD)
            p.deviceActiveIcon->setBackgroundRole(value ? dtk::ColorRole::Checked : dtk::ColorRole::None);
#endif // TLRENDER_BMD  
        }
    }
}
