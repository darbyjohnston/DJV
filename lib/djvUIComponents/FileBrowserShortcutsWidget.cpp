//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUI/ButtonGroup.h>
#include <djvUI/GridLayout.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct ShortcutsWidget::Private
            {
                FileSystem::Path path;
                bool edit = false;
                std::shared_ptr<Label> titleLabel;
                std::shared_ptr<ToolButton> addButton;
                std::shared_ptr<ToolButton> editButton;
                std::shared_ptr<ButtonGroup> removeButtonGroup;
                std::shared_ptr<GridLayout> itemLayout;
                std::shared_ptr<ScrollWidget> scrollWidget;
                std::function<void(const FileSystem::Path&)> callback;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsObserver;
            };

            void ShortcutsWidget::_init(const std::shared_ptr<ShortcutsModel> & model, const std::shared_ptr<Context>& context)
            {
                UI::Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::ShortcutsWidget");

                p.titleLabel = Label::create(context);
                p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
                p.titleLabel->setMargin(UI::MetricsRole::MarginSmall);

                p.addButton = ToolButton::create(context);
                p.addButton->setIcon("djvIconAddSmall");

                p.editButton = ToolButton::create(context);
                p.editButton->setButtonType(ButtonType::Toggle);
                p.editButton->setIcon("djvIconEditSmall");

                p.removeButtonGroup = ButtonGroup::create(ButtonType::Push);
                
                auto layout = VerticalLayout::create(context);
                layout->setSpacing(MetricsRole::None);
                auto hLayout = HorizontalLayout::create(context);
                hLayout->setBackgroundRole(UI::ColorRole::Trough);
                hLayout->setSpacing(MetricsRole::None);
                hLayout->addChild(p.titleLabel);
                hLayout->addExpander();
                hLayout->addChild(p.addButton);
                hLayout->addChild(p.editButton);
                layout->addChild(hLayout);
                auto vLayout = VerticalLayout::create(context);
                vLayout->setMargin(MetricsRole::MarginSmall);
                vLayout->setSpacing(MetricsRole::SpacingSmall);
                p.itemLayout = GridLayout::create(context);
                p.itemLayout->setSpacing(MetricsRole::None);
                vLayout->addChild(p.itemLayout);
                layout->addChild(vLayout);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setMinimumSizeRole(MetricsRole::Menu);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addChild(layout);
                addChild(p.scrollWidget);

                auto weak = std::weak_ptr<ShortcutsWidget>(std::dynamic_pointer_cast<ShortcutsWidget>(shared_from_this()));
                p.addButton->setClickedCallback(
                    [weak, model]
                    {
                        if (auto widget = weak.lock())
                        {
                            model->addShortcut(widget->_p->path);
                        }
                    });

                p.editButton->setCheckedCallback(
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->edit = value;
                            for (const auto& i : widget->_p->removeButtonGroup->getButtons())
                            {
                                i->setVisible(value);
                            }
                        }
                    });
                
                p.removeButtonGroup->setPushCallback(
                    [model](int value)
                    {
                        model->removeShortcut(value);
                    });

                auto contextWeak = std::weak_ptr<Context>(context);
                p.shortcutsObserver = ListObserver<FileSystem::Path>::create(
                    model->observeShortcuts(),
                    [weak, contextWeak](const std::vector<FileSystem::Path> & value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->removeButtonGroup->clearButtons();
                                widget->_p->itemLayout->clearChildren();
                                size_t j = 0;
                                for (const auto& i : value)
                                {
                                    auto button = ListButton::create(context);
                                    std::string s = i.getFileName();
                                    if (s.empty())
                                    {
                                        s = i;
                                    }
                                    button->setText(s);
                                    button->setInsideMargin(MetricsRole::Margin);
                                    button->setTooltip(i);

                                    auto removeButton = ToolButton::create(context);
                                    removeButton->setIcon("djvIconCloseSmall");
                                    removeButton->setVisible(widget->_p->edit);
                                    widget->_p->removeButtonGroup->addButton(removeButton);

                                    widget->_p->itemLayout->addChild(button);
                                    widget->_p->itemLayout->setGridPos(button, 0, j);
                                    widget->_p->itemLayout->setStretch(button, GridStretch::Horizontal);
                                    widget->_p->itemLayout->addChild(removeButton);
                                    widget->_p->itemLayout->setGridPos(removeButton, 1, j);

                                    const auto path = i;
                                    button->setClickedCallback(
                                        [weak, path]
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                if (widget->_p->callback)
                                                {
                                                    widget->_p->callback(path);
                                                }
                                            }
                                        });

                                    ++j;
                                }
                            }
                        }
                    });
            }

            ShortcutsWidget::ShortcutsWidget() :
                _p(new Private)
            {}

            ShortcutsWidget::~ShortcutsWidget()
            {}

            std::shared_ptr<ShortcutsWidget> ShortcutsWidget::create(const std::shared_ptr<ShortcutsModel> & model, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ShortcutsWidget>(new ShortcutsWidget);
                out->_init(model, context);
                return out;
            }

            void ShortcutsWidget::setPath(const FileSystem::Path& value)
            {
                _p->path = value;
            }

            void ShortcutsWidget::setCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->callback = value;
            }

            void ShortcutsWidget::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->scrollWidget->getMinimumSize());
            }

            void ShortcutsWidget::_layoutEvent(Event::Layout & event)
            {
                _p->scrollWidget->setGeometry(getGeometry());
            }
            
            void ShortcutsWidget::_localeEvent(Event::Locale&)
            {
                DJV_PRIVATE_PTR();
                p.titleLabel->setText(_getText(DJV_TEXT("Shortcuts")));
                p.addButton->setTooltip(_getText(DJV_TEXT("Add shortcut tooltip")));
                p.editButton->setTooltip(_getText(DJV_TEXT("Edit shortcuts tooltip")));
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
