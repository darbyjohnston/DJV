//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Border.h>
#include <djvUI/Icon.h>
#include <djvUI/LineEditBase.h>
#include <djvUI/ListButton.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct PathWidget::Private
            {
                std::shared_ptr<LineEditBase> lineEditBase;
                std::vector<FileSystem::Path> history;
                std::shared_ptr<ActionGroup> historyActionGroup;
                std::shared_ptr<Menu> historyMenu;
                std::shared_ptr<Button::Menu> historyButton;
                std::shared_ptr<HorizontalLayout> layout;
                std::function<void(const FileSystem::Path &)> pathCallback;
                std::function<void(size_t)> historyIndexCallback;
            };

            void PathWidget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::PathWidget");

                DJV_PRIVATE_PTR();
                p.lineEditBase = LineEditBase::create(context);

                p.historyActionGroup = ActionGroup::create(ButtonType::Radio);
                p.historyMenu = Menu::create(context);
                p.historyButton = Button::Menu::create(context);
                p.historyButton->setIcon("djvIconPopupMenu");
                p.historyButton->setEnabled(false);

                p.layout = HorizontalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                auto border = Border::create(context);
                border->setMargin(Layout::Margin(MetricsRole::None, MetricsRole::MarginSmall, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
                border->setVAlign(VAlign::Center);
                border->addWidget(p.lineEditBase);
                p.layout->addWidget(border, RowStretch::Expand);
                p.layout->addWidget(p.historyButton);
                p.layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<PathWidget>(std::dynamic_pointer_cast<PathWidget>(shared_from_this()));
                p.historyActionGroup->setRadioCallback(
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->historyIndexCallback)
                        {
                            widget->_p->historyIndexCallback(static_cast<size_t>(value));
                        }
                        widget->_p->historyMenu->hide();
                    }
                });

                p.historyMenu->setCloseCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->historyButton->setChecked(false);
                    }
                });

                p.historyButton->setCheckedCallback(
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->historyMenu->hide();
                        if (value)
                        {
                            if (auto window = widget->getWindow().lock())
                            {
                                widget->_p->historyMenu->popup(window, weak);
                            }
                        }
                    }
                });
            }

            PathWidget::PathWidget() :
                _p(new Private)
            {}

            PathWidget::~PathWidget()
            {}

            std::shared_ptr<PathWidget> PathWidget::create(Context * context)
            {
                auto out = std::shared_ptr<PathWidget>(new PathWidget);
                out->_init(context);
                return out;
            }

            void PathWidget::setPath(const FileSystem::Path & path)
            {
                _p->lineEditBase->setText(path.get());
            }

            void PathWidget::setPathCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->pathCallback = value;
            }

            void PathWidget::setHistory(const std::vector<FileSystem::Path> & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.history)
                    return;
                p.history = value;
                p.historyActionGroup->clearActions();
                p.historyMenu->clearActions();
                for (const auto & i : p.history)
                {
                    auto action = Action::create();
                    action->setText(i);
                    p.historyActionGroup->addAction(action);
                    p.historyMenu->addAction(action);
                }
                p.historyButton->setEnabled(p.history.size() > 1);
            }

            void PathWidget::setHistoryIndex(size_t value)
            {
                _p->historyActionGroup->setChecked(static_cast<int>(value));
            }

            void PathWidget::setHistoryIndexCallback(const std::function<void(size_t)> & value)
            {
                _p->historyIndexCallback = value;
            }

            void PathWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void PathWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void PathWidget::_localeEvent(Core::Event::Locale &)
            {
                DJV_PRIVATE_PTR();
                p.historyButton->setTooltip(_getText(DJV_TEXT("History Tooltip")));
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
