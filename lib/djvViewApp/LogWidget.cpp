// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/LogWidget.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/EventSystem.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileIOFunc.h>
#include <djvSystem/Path.h>
#include <djvSystem/ResourceSystem.h>

#include <djvCore/StringFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct LogWidget::Private
        {
            std::vector<std::string> log;
            std::string filter;
            std::shared_ptr<UI::Text::Block> textBlock;
        };

        void LogWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::LogWidget");

            p.textBlock = UI::Text::Block::create(context);
            p.textBlock->setFontFamily(Render2D::Font::familyMono);
            p.textBlock->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.textBlock->setWordWrap(false);
            p.textBlock->setMargin(UI::MetricsRole::Margin);
            addChild(p.textBlock);
        }

        LogWidget::LogWidget() :
            _p(new Private)
        {}

        LogWidget::~LogWidget()
        {}

        std::shared_ptr<LogWidget> LogWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<LogWidget>(new LogWidget);
            out->_init(context);
            return out;
        }

        void LogWidget::copyLog()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto eventSystem = context->getSystemT<UI::EventSystem>();
                eventSystem->setClipboard(String::join(p.log, '\n'));
            }
        }

        void LogWidget::reloadLog()
        {
            DJV_PRIVATE_PTR();
            try
            {
                p.log = System::File::readLines(
                    std::string(_getResourceSystem()->getPath(System::File::ResourcePath::LogFile)));
            }
            catch (const std::exception & e)
            {
                _log(e.what(), System::LogLevel::Error);
            }
            _widgetUpdate();
        }

        void LogWidget::clearLog()
        {
            DJV_PRIVATE_PTR();
            if (!p.log.empty())
            {
                p.log.clear();
                _widgetUpdate();
            }
        }

        void LogWidget::setFilter(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.filter)
                return;
            p.filter = value;
            _widgetUpdate();
        }

        float LogWidget::getHeightForWidth(float value) const
        {
            return _p->textBlock->getHeightForWidth(value);
        }

        void LogWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->textBlock->getMinimumSize());
        }

        void LogWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->textBlock->setGeometry(getGeometry());
        }

        void LogWidget::_widgetUpdate()
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

        struct LogToolBar::Private
        {
            std::shared_ptr<UI::ToolButton> copyButton;
            std::shared_ptr<UI::ToolButton> reloadButton;
            std::shared_ptr<UI::ToolButton> clearButton;
            std::shared_ptr<UIComponents::SearchBox> searchBox;
            std::function<void(void)> copyCallback;
            std::function<void(void)> reloadCallback;
            std::function<void(void)> clearCallback;
            std::function<void(const std::string&)> filterCallback;
        };

        void LogToolBar::_init(const std::shared_ptr<System::Context>& context)
        {
            ToolBar::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::LogToolBar");

            p.copyButton = UI::ToolButton::create(context);
            p.copyButton->setIcon("djvIconShare");
            p.reloadButton = UI::ToolButton::create(context);
            p.reloadButton->setIcon("djvIconReload");
            p.clearButton = UI::ToolButton::create(context);
            p.clearButton->setIcon("djvIconClear");
            p.searchBox = UIComponents::SearchBox::create(context);

            addExpander();
            addChild(p.copyButton);
            addChild(p.reloadButton);
            addChild(p.clearButton);
            addChild(p.searchBox);

            auto weak = std::weak_ptr<LogToolBar>(std::dynamic_pointer_cast<LogToolBar>(shared_from_this()));
            p.copyButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->copyCallback)
                        {
                            widget->_p->copyCallback();
                        }
                    }
                });

            p.reloadButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->reloadCallback)
                        {
                            widget->_p->reloadCallback();
                        }
                    }
                });

            p.clearButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->clearCallback)
                        {
                            widget->_p->clearCallback();
                        }
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->filterCallback)
                        {
                            widget->_p->filterCallback(value);
                        }
                    }
                });
        }

        LogToolBar::LogToolBar() :
            _p(new Private)
        {}

        LogToolBar::~LogToolBar()
        {}

        std::shared_ptr<LogToolBar> LogToolBar::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<LogToolBar>(new LogToolBar);
            out->_init(context);
            return out;
        }

        void LogToolBar::setCopyCallback(const std::function<void(void)>& value)
        {
            _p->copyCallback = value;
        }

        void LogToolBar::setReloadCallback(const std::function<void(void)>& value)
        {
            _p->reloadCallback = value;
        }

        void LogToolBar::setClearCallback(const std::function<void(void)>& value)
        {
            _p->clearCallback = value;
        }

        void LogToolBar::setFilterCallback(const std::function<void(const std::string&)>& value)
        {
            _p->filterCallback = value;
        }

        void LogToolBar::_initEvent(System::Event::Init& event)
        {
            ToolBar::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.copyButton->setTooltip(_getText(DJV_TEXT("widget_log_copy_tooltip")));
                p.reloadButton->setTooltip(_getText(DJV_TEXT("widget_log_reload_tooltip")));
                p.clearButton->setTooltip(_getText(DJV_TEXT("widget_log_clear_tooltip")));
            }
        }

    } // namespace ViewApp
} // namespace djv

