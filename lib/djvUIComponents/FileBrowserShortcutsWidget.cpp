// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::ShortcutsWidget");

                p.titleLabel = Label::create(context);
                p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
                p.titleLabel->setMargin(UI::MetricsRole::MarginSmall);

                p.addButton = ToolButton::create(context);
                p.addButton->setIcon("djvIconAdd");

                p.editButton = ToolButton::create(context);
                p.editButton->setButtonType(ButtonType::Toggle);
                p.editButton->setIcon("djvIconEdit");

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
                layout->addSeparator();
                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                p.itemLayout = GridLayout::create(context);
                p.itemLayout->setSpacing(MetricsRole::None);
                vLayout->addChild(p.itemLayout);
                layout->addChild(vLayout);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setMinimumSizeRole(MetricsRole::None);
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
                                        s = std::string(i);
                                    }
                                    button->setText(s);
                                    button->setTooltip(std::string(i));

                                    auto removeButton = ToolButton::create(context);
                                    removeButton->setIcon("djvIconClose");
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

            void ShortcutsWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->scrollWidget->getMinimumSize());
            }

            void ShortcutsWidget::_layoutEvent(Event::Layout& event)
            {
                _p->scrollWidget->setGeometry(getGeometry());
            }
            
            void ShortcutsWidget::_initEvent(Event::Init& event)
            {
                Widget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.titleLabel->setText(_getText(DJV_TEXT("file_browser_file_browser_show_shortcuts")));
                    p.addButton->setTooltip(_getText(DJV_TEXT("file_browser_add_shortcut_tooltip")));
                    p.editButton->setTooltip(_getText(DJV_TEXT("file_browser_edit_shortcuts_tooltip")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
