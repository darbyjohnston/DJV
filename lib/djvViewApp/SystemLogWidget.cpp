// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/SystemLogWidget.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/EventSystem.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>
#include <djvCore/FileIOFunc.h>
#include <djvCore/Path.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/StringFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SystemLogWidget::Private
        {
            std::vector<std::string> log;
            std::string filter;
            std::shared_ptr<UI::TextBlock> textBlock;
            std::shared_ptr<UI::ToolButton> copyButton;
            std::shared_ptr<UI::ToolButton> reloadButton;
            std::shared_ptr<UI::ToolButton> clearButton;
            std::shared_ptr<UI::SearchBox> searchBox;
        };

        void SystemLogWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SystemLogWidget");

            p.textBlock = UI::TextBlock::create(context);
            p.textBlock->setFontFamily(AV::Font::familyMono);
            p.textBlock->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.textBlock->setWordWrap(false);
            p.textBlock->setMargin(UI::MetricsRole::Margin);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.textBlock);

            p.copyButton = UI::ToolButton::create(context);
            p.copyButton->setIcon("djvIconShare");
            p.reloadButton = UI::ToolButton::create(context);
            p.reloadButton->setIcon("djvIconReload");
            p.clearButton = UI::ToolButton::create(context);
            p.clearButton->setIcon("djvIconClear");
            p.searchBox = UI::SearchBox::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->setBackgroundRole(UI::ColorRole::Background);
            layout->addChild(scrollWidget);
            layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            auto toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(p.copyButton);
            toolBar->addChild(p.reloadButton);
            toolBar->addChild(p.clearButton);
            toolBar->addChild(p.searchBox);
            layout->addChild(toolBar);
            addChild(layout);

            auto weak = std::weak_ptr<SystemLogWidget>(std::dynamic_pointer_cast<SystemLogWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.copyButton->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto eventSystem = context->getSystemT<UI::EventSystem>();
                            eventSystem->setClipboard(String::join(widget->_p->log, '\n'));
                        }
                    }
                });

            p.reloadButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->reloadLog();
                    }
                });

            p.clearButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->clearLog();
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->filter = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        SystemLogWidget::SystemLogWidget() :
            _p(new Private)
        {}

        SystemLogWidget::~SystemLogWidget()
        {}

        std::shared_ptr<SystemLogWidget> SystemLogWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<SystemLogWidget>(new SystemLogWidget);
            out->_init(context);
            return out;
        }

        void SystemLogWidget::reloadLog()
        {
            DJV_PRIVATE_PTR();
            try
            {
                p.log = FileSystem::readLines(
                    std::string(_getResourceSystem()->getPath(FileSystem::ResourcePath::LogFile)));
            }
            catch (const std::exception & e)
            {
                _log(e.what(), LogLevel::Error);
            }
            _widgetUpdate();
        }

        void SystemLogWidget::clearLog()
        {
            DJV_PRIVATE_PTR();
            if (!p.log.empty())
            {
                p.log.clear();
                _widgetUpdate();
            }
        }

        void SystemLogWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_log")));
                p.copyButton->setTooltip(_getText(DJV_TEXT("widget_log_copy_tooltip")));
                p.reloadButton->setTooltip(_getText(DJV_TEXT("widget_log_reload_tooltip")));
                p.clearButton->setTooltip(_getText(DJV_TEXT("widget_log_clear_tooltip")));
            }
        }

        void SystemLogWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> log;
            if (!p.filter.empty())
            {
                for (const auto& i : p.log)
                {
                    if (String::match(i, p.filter))
                    {
                        log.push_back(i);
                    }
                }
            }
            else
            {
                log = p.log;
            }
            p.textBlock->setText(String::join(log, '\n'));
        }

    } // namespace ViewApp
} // namespace djv

