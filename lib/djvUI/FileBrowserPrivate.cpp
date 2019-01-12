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
#include <djvUI/ActionGroup.h>
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

                DJV_PRIVATE_PTR();
                p.label = Label::create(context);
                p.label->setTextHAlign(TextHAlign::Left);
                p.label->setTextVAlign(TextVAlign::Center);
                p.label->setMargin(Style::MetricsRole::MarginSmall);
                p.label->setBackgroundRole(Style::ColorRole::BackgroundText);

                p.historyActionGroup = ActionGroup::create(ButtonType::Radio);
                p.historyMenu = Menu::create(context);
                p.historyButton = Button::Menu::create(context);
                p.historyButton->setIcon("djvIconPopupMenu");
                p.historyButton->setEnabled(false);

                p.layout = Layout::Horizontal::create(context);
                p.layout->setSpacing(Style::MetricsRole::None);
                auto border = Layout::Border::create(context);
                border->setMargin(Style::MetricsRole::MarginSmall);
                border->addWidget(p.label);
                p.layout->addWidget(border, Layout::RowStretch::Expand);
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

                DJV_PRIVATE_PTR();
                p.actionGroup = ActionGroup::create(ButtonType::Push);

                auto itemLayout = Layout::Vertical::create(context);
                itemLayout->setSpacing(Style::MetricsRole::None);
                for (size_t i = 0; i < static_cast<size_t>(OS::DirectoryShortcut::Count); ++i)
                {
                    const auto shortcut = OS::getPath(static_cast<OS::DirectoryShortcut>(i));
                    p.shortcuts.push_back(shortcut);

                    auto action = Action::create();
                    const auto text = shortcut.getFileName();
                    action->setText(text);
                    p.actionGroup->addAction(action);

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

                p.layout = Layout::Vertical::create(context);
                p.layout->addWidget(scrollWidget, Layout::RowStretch::Expand);
                p.layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<ShorcutsWidget>(std::dynamic_pointer_cast<ShorcutsWidget>(shared_from_this()));
                p.actionGroup->setClickedCallback(
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

                DJV_PRIVATE_PTR();
                p.icon = Icon::create(context);
                p.icon->setIconColorRole(Style::ColorRole::Button);
                p.icon->setVAlign(VAlign::Bottom);

                p.imageWidget = ImageWidget::create(context);
                p.imageWidget->hide();

                switch (viewType)
                {
                case ViewType::ThumbnailsLarge:
                case ViewType::ThumbnailsSmall:
                    p.imageWidget->setVAlign(VAlign::Bottom);
                    p.textBlock = TextBlock::create(context);
                    p.textBlock->setTextHAlign(TextHAlign::Center);
                    p.layout = Layout::Vertical::create(context);
                    p.layout->setMargin(Style::MetricsRole::MarginSmall);
                    break;
                case ViewType::ListView:
                    p.icon->setIconSizeRole(Style::MetricsRole::Icon);
                    p.imageWidget->setHAlign(HAlign::Center);
                    p.imageWidget->setVAlign(VAlign::Center);
                    p.nameLabel = Label::create(context);
                    p.nameLabel->setTextHAlign(TextHAlign::Left);
                    p.layout = Layout::Horizontal::create(context);
                    break;
                default: break;
                }
                auto stackLayout = Layout::Stack::create(context);
                stackLayout->addWidget(p.icon);
                stackLayout->addWidget(p.imageWidget);
                p.layout->addWidget(stackLayout);
                if (p.textBlock)
                {
                    p.layout->addWidget(p.textBlock);
                }
                if (p.nameLabel)
                {
                    p.layout->addWidget(p.nameLabel, Layout::RowStretch::Expand);
                }
                p.layout->setParent(shared_from_this());
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

            void ItemButton::setThumbnail(const std::shared_ptr<AV::Image::Image>& value)
            {
                DJV_PRIVATE_PTR();
                p.imageWidget->setImage(value);
                p.imageWidget->show();
                p.icon->setIcon(FileSystem::Path());
            }

            void ItemButton::setText(const std::string& value)
            {
                DJV_PRIVATE_PTR();
                if (p.textBlock)
                {
                    p.textBlock->setText(value);
                }
                if (p.nameLabel)
                {
                    p.nameLabel->setText(value);
                }
            }

            void ItemButton::setTextSizeRole(Style::MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (p.textBlock)
                {
                    p.textBlock->setTextSizeRole(value);
                }
            }

            float ItemButton::getHeightForWidth(float value) const
            {
                return _p->layout->getHeightForWidth(value);
            }

            void ItemButton::_preLayoutEvent(Event::PreLayout& event)
            {
                glm::vec2 size = _p->layout->getMinimumSize();
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
                DJV_PRIVATE_PTR();
                p.icon->setIconColorRole(toggled ? Style::ColorRole::CheckedForeground : Style::ColorRole::Button);
                const Style::ColorRole colorRole = toggled ? Style::ColorRole::CheckedForeground : Style::ColorRole::Foreground;
                if (p.textBlock)
                {
                    p.textBlock->setTextColorRole(colorRole);
                }
                if (p.nameLabel)
                {
                    p.nameLabel->setTextColorRole(colorRole);
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
