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

#include <djvUI/FileBrowserPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct PathWidget::Private
            {
                std::shared_ptr<Label> label;
                std::vector<FileSystem::Path> history;
                std::shared_ptr<ActionGroup> historyActionGroup;
                std::shared_ptr<Menu> historyMenu;
                std::shared_ptr<Button::Menu> historyButton;
                std::shared_ptr<Layout::Horizontal> layout;
                std::function<void(const FileSystem::Path &)> pathCallback;
            };

            void PathWidget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::PathWidget");

                _p->label = Label::create(context);
                _p->label->setTextHAlign(TextHAlign::Left);

                _p->historyActionGroup = ActionGroup::create(ButtonType::Radio);
                _p->historyMenu = Menu::create(context);
                _p->historyButton = Button::Menu::create(context);
                _p->historyButton->setIcon(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconComboBox90DPI.png"));
                _p->historyButton->setEnabled(false);

                _p->layout = Layout::Horizontal::create(context);
                _p->layout->addWidget(_p->label, Layout::RowStretch::Expand);
                _p->layout->addWidget(_p->historyButton);
                _p->layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<PathWidget>(std::dynamic_pointer_cast<PathWidget>(shared_from_this()));
                _p->historyActionGroup->setRadioCallback(
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value >= 0 && value < widget->_p->history.size() && widget->_p->pathCallback)
                        {
                            widget->_p->pathCallback(widget->_p->history[value]);
                        }
                        widget->_p->historyMenu->hide();
                    }
                });

                _p->historyMenu->setCloseCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->historyButton->setChecked(false);
                    }
                });

                _p->historyButton->setCheckedCallback(
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->historyMenu->hide();
                        if (value)
                        {
                            if (auto window = widget->getWindow().lock())
                            {
                                const auto & g = widget->getGeometry();
                                widget->_p->historyMenu->popup(window, weak, MenuType::ComboBox);
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
                _p->label->setText(path.get());
            }

            void PathWidget::setPathCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->pathCallback = value;
            }

            void PathWidget::setHistory(const std::vector<FileSystem::Path> & value)
            {
                if (value == _p->history)
                    return;
                _p->history = value;
                _p->historyActionGroup->clearActions();
                _p->historyMenu->clearActions();
                for (const auto & i : _p->history)
                {
                    auto action = Action::create();
                    action->setText(i);
                    _p->historyActionGroup->addAction(action);
                    _p->historyMenu->addAction(action);
                }
                _p->historyButton->setEnabled(_p->history.size() > 0);
            }

            void PathWidget::setHistoryIndex(size_t value)
            {
                _p->historyActionGroup->setChecked(static_cast<int>(value));
            }

            void PathWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void PathWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
