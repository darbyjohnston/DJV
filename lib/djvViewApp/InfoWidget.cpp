// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InfoWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUIComponents/SearchBox.h>

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
            std::string filter;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::vector<std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::SearchBox> searchBox;
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

            p.searchBox = UI::SearchBox::create(context);

            auto toolBar = UI::ToolBar::create(context);
            toolBar->setBackgroundRole(UI::ColorRole::Background);
            toolBar->addAction(p.actions["ExpandAll"]);
            toolBar->addAction(p.actions["CollapseAll"]);
            toolBar->addChild(p.searchBox);
            toolBar->setStretch(p.searchBox, UI::RowStretch::Expand);

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
            vLayout->addSeparator();
            vLayout->addChild(toolBar);
            addChild(vLayout);

            _widgetUpdate();

            auto weak = std::weak_ptr<InfoWidget>(std::dynamic_pointer_cast<InfoWidget>(shared_from_this()));
            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->setBellowsState(true);
                    widget->_p->filter = value;
                    widget->_widgetUpdate();
                }
            });

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
                        widget->setBellowsState(true);
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
                        widget->setBellowsState(false);
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
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_info_title")));

                p.actions["ExpandAll"]->setTooltip(_getText(DJV_TEXT("widget_info_expand_all_tooltip")));
                p.actions["CollapseAll"]->setTooltip(_getText(DJV_TEXT("widget_info_collapse_all_tooltip")));

                _widgetUpdate();
            }
        }

        std::string InfoWidget::_text(int value) const
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        std::string InfoWidget::_text(const Math::Rational& value) const
        {
            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << value.toFloat();
            return ss.str();
        }

        std::string InfoWidget::_text(const Frame::Sequence& sequence, const Math::Rational& speed) const
        {
            std::stringstream ss;
            if (auto context = getContext().lock())
            {
                auto avSystem = context->getSystemT<AV::AVSystem>();
                const Time::Units timeUnits = avSystem->observeTimeUnits()->get();
                ss << Time::toString(sequence.getFrameCount(), speed, timeUnits);
                switch (timeUnits)
                {
                case Time::Units::Frames:
                    ss << " " << _getText(DJV_TEXT("widget_info_frames"));
                    break;
                default: break;
                }
            }
            return ss.str();
        }

        std::string InfoWidget::_text(AV::Image::Type value) const
        {
            std::stringstream ss;
            ss << value;
            return _getText(ss.str());
        }

        std::string InfoWidget::_text(const AV::Image::Size& value) const
        {
            std::stringstream ss;
            ss << value.w << "x" << value.h;
            ss.precision(2);
            ss << ":" << std::fixed << value.getAspectRatio();
            return ss.str();
        }

        std::string InfoWidget::_text(AV::Audio::Type value) const
        {
            std::stringstream ss;
            ss << value;
            return _getText(ss.str());
        }

        std::string InfoWidget::_textSampleRate(size_t value) const
        {
            std::stringstream ss;
            ss << value / 1000.F << _getText(DJV_TEXT("widget_info_khz"));
            return ss.str();
        }

        std::string InfoWidget::_textDuration(size_t sampleCount, size_t sampleRate) const
        {
            std::stringstream ss;
            ss << (sampleRate > 0 ? (sampleCount / sampleRate) : 0) << " " << _getText(DJV_TEXT("widget_info_seconds"));
            return ss.str();
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

                const std::string fileNameLabel = _getText(DJV_TEXT("widget_info_file_name"));
                const bool fileNameMatch = !p.info.fileName.empty() &&
                    (String::match(fileNameLabel, p.filter) || String::match(p.info.fileName, p.filter));
                if (fileNameMatch)
                {
                    auto textBlock = p.createTextBlock(context);
                    textBlock->setText(p.info.fileName);
                    auto formLayout = p.createFormLayout(context);
                    formLayout->addChild(textBlock);
                    formLayout->setText(textBlock, fileNameLabel + ":");

                    auto bellows = UI::Bellows::create(context);
                    bellows->setText(_getText(DJV_TEXT("widget_info_general")));
                    bellows->addChild(formLayout);
                    p.bellows.push_back(bellows);
                    p.layout->addChild(bellows);
                }

                size_t j = 0;
                for (const auto& i : p.info.video)
                {
                    const std::string sizeLabel = _getText(DJV_TEXT("widget_info_dimensions"));
                    const std::string sizeText = _text(i.info.size);
                    const std::string typeLabel = _getText(DJV_TEXT("widget_info_type"));
                    const std::string typeText = _text(i.info.type);
                    std::string speedLabel;
                    std::string speedText;
                    std::string durationLabel;
                    std::string durationText;
                    if (i.sequence.getFrameCount() > 0)
                    {
                        speedLabel = _getText(DJV_TEXT("widget_info_speed"));
                        speedText = _text(i.speed);
                        durationLabel = _getText(DJV_TEXT("widget_info_duration"));
                        durationText = _text(i.sequence, i.speed);
                    }
                    const std::string codecLabel = _getText(DJV_TEXT("widget_info_codec"));

                    const bool sizeMatch = String::match(sizeLabel, p.filter) || String::match(sizeText, p.filter);
                    const bool typeMatch = String::match(typeLabel, p.filter) || String::match(typeText, p.filter);
                    bool speedMatch = false;
                    bool durationMatch = false;
                    if (i.sequence.getFrameCount() > 0)
                    {
                        speedMatch |= String::match(speedLabel, p.filter) || String::match(speedText, p.filter);
                        durationMatch |= String::match(durationLabel, p.filter) || String::match(durationText, p.filter);
                    }
                    const bool codecMatch = !i.codec.empty() &&
                        (String::match(codecLabel, p.filter) || String::match(i.codec, p.filter));

                    if (sizeMatch || typeMatch || speedMatch || durationMatch || codecMatch)
                    {
                        auto formLayout = p.createFormLayout(context);
                        if (sizeMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(sizeText);
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, sizeLabel + ":");
                        }
                        if (typeMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(typeText);
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, typeLabel + ":");
                        }
                        if (i.sequence.getFrameCount() > 0)
                        {
                            if (speedMatch)
                            {
                                auto textBlock = p.createTextBlock(context);
                                textBlock->setText(speedText);
                                formLayout->addChild(textBlock);
                                formLayout->setText(textBlock, speedLabel + ":");
                            }
                            if (durationMatch)
                            {
                                auto textBlock = p.createTextBlock(context);
                                textBlock->setText(durationText);
                                formLayout->addChild(textBlock);
                                formLayout->setText(textBlock, durationLabel + ":");
                            }
                        }
                        if (codecMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(i.codec);
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, codecLabel + ":");
                        }
                        auto bellows = UI::Bellows::create(context);
                        bellows->setText(i.info.name);
                        bellows->addChild(formLayout);
                        p.bellows.push_back(bellows);
                        p.layout->addChild(bellows);
                    }

                    ++j;
                }

                j = 0;
                for (const auto& i : p.info.audio)
                {
                    const std::string channelLabel = _getText(DJV_TEXT("widget_info_channels"));
                    const std::string channelText = _text(i.info.channelCount);
                    const std::string typeLabel = _getText(DJV_TEXT("widget_info_type"));
                    const std::string typeText = _text(i.info.type);
                    const std::string sampleRateLabel = _getText(DJV_TEXT("widget_info_sample_rate"));
                    const std::string sampleRateText = _textSampleRate(i.info.sampleRate);
                    const std::string durationLabel = _getText(DJV_TEXT("widget_info_duration"));
                    const std::string durationText = _textDuration(i.info.sampleCount, i.info.sampleRate);
                    const std::string codecLabel = _getText(DJV_TEXT("widget_info_codec"));

                    const bool channelMatch = String::match(channelLabel, p.filter) || String::match(channelText, p.filter);
                    const bool typeMatch = String::match(typeLabel, p.filter) || String::match(typeText, p.filter);
                    const bool sampleRateMatch = String::match(sampleRateLabel, p.filter) || String::match(sampleRateText, p.filter);
                    const bool durationMatch = String::match(durationLabel, p.filter) || String::match(durationText, p.filter);
                    const bool codecMatch = !i.codec.empty() &&
                        (String::match(codecLabel, p.filter) || String::match(i.codec, p.filter));

                    if (channelMatch || typeMatch || sampleRateMatch || durationMatch || codecMatch)
                    {
                        auto formLayout = p.createFormLayout(context);
                        if (channelMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(channelText);
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, channelLabel + ":");
                        }
                        if (typeMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(_getText(typeText));
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, typeLabel + ":");
                        }
                        if (sampleRateMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(sampleRateText);
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, sampleRateLabel + ":");
                        }
                        if (durationMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(durationText);
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, durationLabel + ":");
                        }
                        if (codecMatch)
                        {
                            auto textBlock = p.createTextBlock(context);
                            textBlock->setText(i.codec);
                            formLayout->addChild(textBlock);
                            formLayout->setText(textBlock, codecLabel + ":");
                        }
                        auto bellows = UI::Bellows::create(context);
                        bellows->setText(i.info.name);
                        bellows->addChild(formLayout);
                        p.bellows.push_back(bellows);
                        p.layout->addChild(bellows);
                    }

                    ++j;
                }

                if (!p.info.tags.isEmpty())
                {
                    bool match = false;
                    for (const auto& i : p.info.tags.getTags())
                    {
                        match |= String::match(i.first, p.filter);
                        match |= String::match(i.second, p.filter);
                    }
                    if (match)
                    {
                        auto formLayout = p.createFormLayout(context);
                        for (const auto& i : p.info.tags.getTags())
                        {
                            if (String::match(i.first, p.filter) || String::match(i.second, p.filter))
                            {
                                auto textBlock = p.createTextBlock(context);
                                textBlock->setText(i.second);
                                formLayout->addChild(textBlock);
                                std::stringstream ss;
                                ss << i.first << ":";
                                formLayout->setText(textBlock, ss.str());
                            }
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

