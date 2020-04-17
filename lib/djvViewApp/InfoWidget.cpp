// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InfoWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/Bellows.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct InfoWidget::Private
        {
            AV::IO::Info info;
            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
        };

        void InfoWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::InfoWidget");

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            scrollWidget->addChild(p.layout);
            addChild(scrollWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<InfoWidget>(std::dynamic_pointer_cast<InfoWidget>(shared_from_this()));
            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value)
                            {
                                widget->_p->infoObserver = ValueObserver<AV::IO::Info>::create(
                                    value->observeInfo(),
                                    [weak](const AV::IO::Info& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->info = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->info = AV::IO::Info();
                                widget->_p->infoObserver.reset();
                                widget->_widgetUpdate();
                            }
                        }
                    });
            }
        }

        InfoWidget::InfoWidget() :
            _p(new Private)
        {}

        InfoWidget::~InfoWidget()
        {}

        std::shared_ptr<InfoWidget> InfoWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<InfoWidget>(new InfoWidget);
            out->_init(context);
            return out;
        }

        void InfoWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }

        void InfoWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            setTitle(_getText(DJV_TEXT("widget_info_title")));
            _widgetUpdate();
        }

        void InfoWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.sizeGroup->clearLabels();
                p.layout->clearChildren();

                if (!p.info.fileName.empty())
                {
                    auto label = UI::Label::create(context);
                    label->setText(p.info.fileName);
                    label->setHAlign(UI::HAlign::Left);
                    auto formLayout = UI::FormLayout::create(context);
                    formLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
                    formLayout->setSizeGroup(p.sizeGroup);
                    formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_file_name")) + ":");
                    p.layout->addChild(formLayout);
                }

                size_t j = 0;
                for (const auto& i : p.info.video)
                {
                    auto formLayout = UI::FormLayout::create(context);
                    formLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
                    formLayout->setSizeGroup(p.sizeGroup);
                    formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                    formLayout->setShadowOverlay({ UI::Side::Top });

                    auto label = UI::Label::create(context);
                    label->setText(i.info.name);
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_name")) + ":");

                    label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        ss << i.info.size;
                        ss.precision(2);
                        ss << " " << std::fixed << i.info.size.getAspectRatio();
                        label->setText(ss.str());
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_dimensions")) + ":");

                    label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        ss << i.info.type;
                        label->setText(_getText(ss.str()));
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_type")) + ":");

                    label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        ss << i.speed.toFloat();
                        label->setText(ss.str());
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_speed")) + ":");

                    label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        auto avSystem = context->getSystemT<AV::AVSystem>();
                        const Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                        ss << Time::toString(i.sequence.getSize(), i.speed, timeUnits);
                        switch (timeUnits)
                        {
                        case Time::Units::Frames:
                            ss << " " << _getText(DJV_TEXT("widget_info_frames"));
                            break;
                        default: break;
                        }
                        label->setText(ss.str());
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_duration")) + ":");

                    label = UI::Label::create(context);
                    label->setText(i.codec);
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_codec")) + ":");

                    auto bellows = UI::Bellows::create(context);
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("widget_info_video_track")) << " #" << j << ":";
                        bellows->setText(ss.str());
                    }
                    bellows->addChild(formLayout);
                    p.layout->addChild(bellows);

                    ++j;
                }

                j = 0;
                for (const auto& i : p.info.audio)
                {
                    auto formLayout = UI::FormLayout::create(context);
                    formLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
                    formLayout->setSizeGroup(p.sizeGroup);
                    formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                    formLayout->setShadowOverlay({ UI::Side::Top });

                    auto label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        ss << static_cast<int>(i.info.channelCount);
                        label->setText(ss.str());
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_channels")) + ":");

                    label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        ss << i.info.type;
                        label->setText(_getText(ss.str()));
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_type")) + ":");

                    label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        ss << i.info.sampleRate / 1000.F << _getText(DJV_TEXT("widget_info_khz"));
                        label->setText(ss.str());
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_sample_rate")) + ":");

                    label = UI::Label::create(context);
                    {
                        std::stringstream ss;
                        ss << (i.info.sampleRate > 0 ? (i.info.sampleCount / i.info.sampleRate) : 0) << " " << _getText(DJV_TEXT("widget_info_seconds"));
                        label->setText(ss.str());
                    }
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_duration")) + ":");

                    label = UI::Label::create(context);
                    label->setText(i.codec);
                    label->setHAlign(UI::HAlign::Left);
                    formLayout->addChild(label);
                    formLayout->setText(label, _getText(DJV_TEXT("widget_info_codec")) + ":");

                    auto bellows = UI::Bellows::create(context);
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("widget_info_audio_track")) << " #" << j << ":";
                        bellows->setText(ss.str());
                    }
                    bellows->addChild(formLayout);
                    p.layout->addChild(bellows);

                    ++j;
                }

                if (!p.info.tags.isEmpty())
                {
                    auto formLayout = UI::FormLayout::create(context);
                    formLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
                    formLayout->setSizeGroup(p.sizeGroup);
                    formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
                    formLayout->setShadowOverlay({ UI::Side::Top });

                    for (const auto& i : p.info.tags.getTags())
                    {
                        auto label = UI::Label::create(context);
                        label->setText(i.second);
                        label->setHAlign(UI::HAlign::Left);
                        formLayout->addChild(label);
                        std::stringstream ss;
                        ss << i.first << ":";
                        formLayout->setText(label, ss.str());
                    }

                    auto bellows = UI::Bellows::create(context);
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("widget_info_tags"));
                        bellows->setText(ss.str());
                    }
                    bellows->addChild(formLayout);
                    p.layout->addChild(bellows);
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

