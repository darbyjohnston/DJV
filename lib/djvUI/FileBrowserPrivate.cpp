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
#include <djvUI/Border.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/StackLayout.h>
#include <djvUI/TextBlock.h>

#include <djvAV/Image.h>

#include <djvCore/OS.h>

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
                std::function<void(size_t)> historyIndexCallback;
            };

            void PathWidget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::PathWidget");

                _p->label = Label::create(context);
                _p->label->setTextHAlign(TextHAlign::Left);
                _p->label->setTextVAlign(TextVAlign::Center);
                _p->label->setMargin(Style::MetricsRole::MarginSmall);
                _p->label->setBackgroundRole(Style::ColorRole::BackgroundScroll);

                _p->historyActionGroup = ActionGroup::create(ButtonType::Radio);
                _p->historyMenu = Menu::create(context);
                _p->historyButton = Button::Menu::create(context);
                _p->historyButton->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconPopupMenu90DPI.png"));
                _p->historyButton->setEnabled(false);

                _p->layout = Layout::Horizontal::create(context);
                _p->layout->setSpacing(Style::MetricsRole::None);
                auto border = Layout::Border::create(context);
                border->setMargin(Style::MetricsRole::MarginSmall);
                border->addWidget(_p->label);
                _p->layout->addWidget(border, Layout::RowStretch::Expand);
                _p->layout->addWidget(_p->historyButton);
                _p->layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<PathWidget>(std::dynamic_pointer_cast<PathWidget>(shared_from_this()));
                _p->historyActionGroup->setRadioCallback(
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
                _p->historyButton->setEnabled(_p->history.size() > 1);
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

            struct ShorcutsWidget::Private
            {
                std::vector<FileSystem::Path> shortcuts;
                std::shared_ptr<ActionGroup> actionGroup;
                std::shared_ptr<Layout::Vertical> layout;
                std::function<void(const FileSystem::Path &)> shortcutCallback;
            };

            void ShorcutsWidget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::ShorcutsWidget");

                _p->actionGroup = ActionGroup::create(ButtonType::Push);

                auto itemLayout = Layout::Vertical::create(context);
                itemLayout->setSpacing(Style::MetricsRole::None);
                for (size_t i = 0; i < static_cast<size_t>(OS::DirectoryShortcut::Count); ++i)
                {
                    const auto shortcut = OS::getPath(static_cast<OS::DirectoryShortcut>(i));
                    _p->shortcuts.push_back(shortcut);

                    auto action = Action::create();
                    const auto text = shortcut.getFileName();
                    action->setText(text);
                    _p->actionGroup->addAction(action);

                    auto button = Button::List::create(context);
                    button->setText(text);
                    button->setTextHAlign(TextHAlign::Left);
                    itemLayout->addWidget(button);

                    button->setClickedCallback(
                        [action]
                    {
                        action->doClicked();
                    });
                }
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->addWidget(itemLayout);

                _p->layout = Layout::Vertical::create(context);
                _p->layout->addWidget(scrollWidget, Layout::RowStretch::Expand);
                _p->layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<ShorcutsWidget>(std::dynamic_pointer_cast<ShorcutsWidget>(shared_from_this()));
                _p->actionGroup->setClickedCallback(
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value >= 0 && value < widget->_p->shortcuts.size() && widget->_p->shortcutCallback)
                        {
                            widget->_p->shortcutCallback(widget->_p->shortcuts[value]);
                        }
                    }
                });
            }

            ShorcutsWidget::ShorcutsWidget() :
                _p(new Private)
            {}

            ShorcutsWidget::~ShorcutsWidget()
            {}

            std::shared_ptr<ShorcutsWidget> ShorcutsWidget::create(Context * context)
            {
                auto out = std::shared_ptr<ShorcutsWidget>(new ShorcutsWidget);
                out->_init(context);
                return out;
            }

            void ShorcutsWidget::setShortcutCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->shortcutCallback = value;
            }

            void ShorcutsWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void ShorcutsWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            struct ItemButton::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<ImageWidget> imageWidget;
                std::shared_ptr<TextBlock> textBlock;
                std::shared_ptr<Label> nameLabel;
                std::shared_ptr<Layout::Row> layout;
            };

            void ItemButton::_init(ViewType viewType, Context * context)
            {
                IButton::_init(context);

                setClassName("djv::UI::FileBrowser::ItemButton");

                _p->icon = Icon::create(context);
                _p->icon->setIconColorRole(Style::ColorRole::Button);
                _p->icon->setVAlign(VAlign::Bottom);

                _p->imageWidget = ImageWidget::create(context);
                _p->imageWidget->hide();

                switch (viewType)
                {
                case ViewType::ThumbnailsLarge:
                case ViewType::ThumbnailsSmall:
                    _p->imageWidget->setVAlign(VAlign::Bottom);
                    _p->textBlock = TextBlock::create(context);
                    _p->textBlock->setTextHAlign(TextHAlign::Center);
                    _p->layout = Layout::Vertical::create(context);
                    _p->layout->setMargin(Style::MetricsRole::Margin);
                    break;
                case ViewType::ListView:
                    _p->icon->setIconSizeRole(Style::MetricsRole::Icon);
                    _p->imageWidget->setHAlign(HAlign::Center);
                    _p->imageWidget->setVAlign(VAlign::Center);
                    _p->nameLabel = Label::create(context);
                    _p->nameLabel->setTextHAlign(TextHAlign::Left);
                    _p->layout = Layout::Horizontal::create(context);
                    break;
                }
                auto stackLayout = Layout::Stack::create(context);
                stackLayout->addWidget(_p->icon);
                stackLayout->addWidget(_p->imageWidget);
                _p->layout->addWidget(stackLayout);
                if (_p->textBlock)
                {
                    _p->layout->addWidget(_p->textBlock);
                }
                if (_p->nameLabel)
                {
                    _p->layout->addWidget(_p->nameLabel, Layout::RowStretch::Expand);
                }
                _p->layout->setParent(shared_from_this());
            }

            ItemButton::ItemButton() :
                _p(new Private)
            {}

            ItemButton::~ItemButton()
            {}

            std::shared_ptr<ItemButton> ItemButton::create(ViewType viewType, Context * context)
            {
                auto out = std::shared_ptr<ItemButton>(new ItemButton);
                out->_init(viewType, context);
                return out;
            }

            void ItemButton::setIcon(const FileSystem::Path& path)
            {
                _p->icon->setIcon(path);
            }

            void ItemButton::setIconSizeRole(Style::MetricsRole value)
            {
                _p->icon->setIconSizeRole(value);
            }

            void ItemButton::setThumbnail(const std::shared_ptr<AV::Image::Image>& value, UID uid)
            {
                _p->imageWidget->setImage(value, uid);
                _p->imageWidget->show();
            }

            void ItemButton::setText(const std::string& value)
            {
                if (_p->textBlock)
                {
                    _p->textBlock->setText(value);
                }
                if (_p->nameLabel)
                {
                    _p->nameLabel->setText(value);
                }
            }

            float ItemButton::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void ItemButton::_preLayoutEvent(Event::PreLayout& event)
            {
                const auto size = _p->layout->getMinimumSize();
                _setMinimumSize(size);
            }

            void ItemButton::_layoutEvent(Event::Layout&)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void ItemButton::_updateEvent(Event::Update& event)
            {
                IButton::_updateEvent(event);
                const bool toggled = _isToggled();
                _p->icon->setIconColorRole(toggled ? Style::ColorRole::CheckedForeground : Style::ColorRole::Button);
                const Style::ColorRole colorRole = toggled ? Style::ColorRole::CheckedForeground : Style::ColorRole::Foreground;
                if (_p->textBlock)
                {
                    _p->textBlock->setTextColorRole(colorRole);
                }
                if (_p->nameLabel)
                {
                    _p->nameLabel->setTextColorRole(colorRole);
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
