// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InfoWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/Action.h>
#include <djvUI/Bellows.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolBar.h>

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
            bool bellowsState = true;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::vector<std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::ToolBar> toolBar;
            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;

            std::shared_ptr<UI::TextBlock> createTextBlock(const std::shared_ptr<Context>& context)
            {
                auto textBlock = UI::TextBlock::create(context);
                textBlock->setFontFamily(AV::Font::familyMono);
                textBlock->setMargin(UI::MetricsRole::MarginSmall);
                return textBlock;
            }

            std::shared_ptr<UI::FormLayout> createFormLayout(const std::shared_ptr<Context>& context)
            {
                auto formLayout = UI::FormLayout::create(context);
                formLayout->setFontFamily(AV::Font::familyMono);
                formLayout->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);
                formLayout->setLabelVAlign(UI::VAlign::Top);
                formLayout->setLabelSizeGroup(sizeGroup);
                formLayout->setSpacing(UI::MetricsRole::None);
                return formLayout;
            }
        };

        void InfoWidget::_init(const std::shared_ptr<Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::InfoWidget");

            p.actions["ExpandAll"] = UI::Action::create();
            p.actions["ExpandAll"]->setIcon("djvIconArrowSmallDown");
            p.actions["CollapseAll"] = UI::Action::create();
            p.actions["CollapseAll"]->setIcon("djvIconArrowSmallRight");

            p.toolBar = UI::ToolBar::create(context);
            p.toolBar->addAction(p.actions["ExpandAll"]);
            p.toolBar->addAction(p.actions["CollapseAll"]);

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            scrollWidget->addChild(p.layout);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(scrollWidget);
            vLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            vLayout->addChild(p.toolBar);
            addChild(vLayout);

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

            p.actionObservers["ExpandAll"] = ValueObserver<bool>::create(
                p.actions["ExpandAll"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->bellowsState = true;
                        widget->_expandAll();
                    }
                }
            });

            p.actionObservers["CollapseAll"] = ValueObserver<bool>::create(
                p.actions["CollapseAll"]->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->bellowsState = false;
                        widget->_collapseAll();
                    }
                }
            });
        }

        InfoWidget::InfoWidget() :
            _p(new Private)
        {}

        InfoWidget::~InfoWidget()
        {}

        std::shared_ptr<InfoWidget> InfoWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<InfoWidget>(new InfoWidget);
            out->_init(context);
            return out;
        }

        bool InfoWidget::getBellowsState() const
        {
            return _p->bellowsState;
        }

        void InfoWidget::setBellowsState(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.bellowsState)
                return;
            p.bellowsState = value;
            if (p.bellowsState)
            {
                _expandAll();
            }
            else
            {
                _collapseAll();
            }
        }

        void InfoWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }

        void InfoWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();

            setTitle(_getText(DJV_TEXT("widget_info_title")));

            p.actions["ExpandAll"]->setTooltip(_getText(DJV_TEXT("widget_info_expand_all_tooltip")));
            p.actions["CollapseAll"]->setTooltip(_getText(DJV_TEXT("widget_info_collapse_all_tooltip")));

            _widgetUpdate();
        }

        void InfoWidget::_expandAll()
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : p.bellows)
            {
                i->open();
            }
        }

        void InfoWidget::_collapseAll()
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : p.bellows)
            {
                i->close();
            }
        }

        void InfoWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.bellows.clear();
                p.sizeGroup->clearLabels();
                p.layout->clearChildren();

                if (!p.info.fileName.empty())
                {
                    auto textBlock = p.createTextBlock(context);
                    textBlock->setText(p.info.fileName);
                    auto formLayout = p.createFormLayout(context);
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_file_name")) + ":");

                    auto bellows = UI::Bellows::create(context);
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("widget_info_general"));
                        bellows->setText(ss.str());
                    }
                    bellows->addChild(formLayout);
                    p.bellows.push_back(bellows);
                    p.layout->addChild(bellows);
                }

                size_t j = 0;
                for (const auto& i : p.info.video)
                {
                    auto formLayout = p.createFormLayout(context);

                    auto textBlock = p.createTextBlock(context);
                    {
                        std::stringstream ss;
                        ss << i.info.size.w << "x" << i.info.size.h;
                        ss.precision(2);
                        ss << ":" << std::fixed << i.info.size.getAspectRatio();
                        textBlock->setText(ss.str());
                    }
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_dimensions")) + ":");

                    textBlock = p.createTextBlock(context);
                    {
                        std::stringstream ss;
                        ss << i.info.type;
                        textBlock->setText(_getText(ss.str()));
                    }
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_type")) + ":");

                    if (i.sequence.getSize() > 0)
                    {
                        textBlock = p.createTextBlock(context);
                        {
                            std::stringstream ss;
                            ss.precision(2);
                            ss << std::fixed << i.speed.toFloat();
                            textBlock->setText(ss.str());
                        }
                        formLayout->addChild(textBlock);
                        formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_speed")) + ":");

                        textBlock = p.createTextBlock(context);
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
                            textBlock->setText(ss.str());
                        }
                        formLayout->addChild(textBlock);
                        formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_duration")) + ":");
                    }

                    if (!i.codec.empty())
                    {
                        textBlock = p.createTextBlock(context);
                        textBlock->setText(i.codec);
                        formLayout->addChild(textBlock);
                        formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_codec")) + ":");
                    }

                    auto bellows = UI::Bellows::create(context);
                    bellows->setText(i.info.name);
                    bellows->addChild(formLayout);
                    p.bellows.push_back(bellows);
                    p.layout->addChild(bellows);

                    ++j;
                }

                j = 0;
                for (const auto& i : p.info.audio)
                {
                    auto formLayout = p.createFormLayout(context);

                    auto textBlock = p.createTextBlock(context);
                    {
                        std::stringstream ss;
                        ss << static_cast<int>(i.info.channelCount);
                        textBlock->setText(ss.str());
                    }
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_channels")) + ":");

                    textBlock = p.createTextBlock(context);
                    {
                        std::stringstream ss;
                        ss << i.info.type;
                        textBlock->setText(_getText(ss.str()));
                    }
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_type")) + ":");

                    textBlock = p.createTextBlock(context);
                    {
                        std::stringstream ss;
                        ss << i.info.sampleRate / 1000.F << _getText(DJV_TEXT("widget_info_khz"));
                        textBlock->setText(ss.str());
                    }
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_sample_rate")) + ":");

                    textBlock = p.createTextBlock(context);
                    {
                        std::stringstream ss;
                        ss << (i.info.sampleRate > 0 ? (i.info.sampleCount / i.info.sampleRate) : 0) << " " << _getText(DJV_TEXT("widget_info_seconds"));
                        textBlock->setText(ss.str());
                    }
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_duration")) + ":");

                    if (!i.codec.empty())
                    {
                        textBlock = p.createTextBlock(context);
                        textBlock->setText(i.codec);
                        formLayout->addChild(textBlock);
                        formLayout->setText(textBlock, _getText(DJV_TEXT("widget_info_codec")) + ":");
                    }

                    auto bellows = UI::Bellows::create(context);
                    bellows->setText(i.info.name);
                    bellows->addChild(formLayout);
                    p.bellows.push_back(bellows);
                    p.layout->addChild(bellows);

                    ++j;
                }

                if (!p.info.tags.isEmpty())
                {
                    auto formLayout = p.createFormLayout(context);

                    for (const auto& i : p.info.tags.getTags())
                    {
                        auto textBlock = p.createTextBlock(context);
                        textBlock->setText(i.second);
                        formLayout->addChild(textBlock);
                        std::stringstream ss;
                        ss << i.first << ":";
                        formLayout->setText(textBlock, ss.str());
                    }

                    auto bellows = UI::Bellows::create(context);
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("widget_info_tags"));
                        bellows->setText(ss.str());
                    }
                    bellows->addChild(formLayout);
                    p.bellows.push_back(bellows);
                    p.layout->addChild(bellows);
                }

                if (p.bellowsState)
                {
                    _expandAll();
                }
                else
                {
                    _collapseAll();
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

