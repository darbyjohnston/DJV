// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/LineEdit.h>
#include <djvUI/ListButton.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>

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
                FileSystem::Path path;
                std::vector<FileSystem::Path> history;
                std::shared_ptr<ActionGroup> historyActionGroup;
                std::shared_ptr<Menu> historyMenu;
                std::shared_ptr<Button::Menu> historyButton;
                std::shared_ptr<HorizontalLayout> buttonLayout;
                std::shared_ptr<LineEdit> lineEdit;
                std::shared_ptr<SoloLayout> soloLayout;
                std::shared_ptr<HorizontalLayout> layout;
                std::function<void(const FileSystem::Path &)> pathCallback;
                std::function<void(size_t)> historyIndexCallback;
            };

            void PathWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::PathWidget");
                setPointerEnabled(true);

                p.historyActionGroup = ActionGroup::create(ButtonType::Radio);
                p.historyMenu = Menu::create(context);
                addChild(p.historyMenu);
                p.historyButton = Button::Menu::create(UI::Button::MenuStyle::Flat, context);
                p.historyButton->setIcon("djvIconPopupMenu");
                p.historyButton->setEnabled(false);

                p.buttonLayout = HorizontalLayout::create(context);
                p.buttonLayout->setSpacing(MetricsRole::None);
                p.buttonLayout->setPointerEnabled(true);
                p.buttonLayout->installEventFilter(shared_from_this());

                p.lineEdit = LineEdit::create(context);
                p.lineEdit->setBackgroundRole(ColorRole::Trough);

                p.layout = HorizontalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                auto hLayout = HorizontalLayout::create(context);
                hLayout->setSpacing(MetricsRole::None);
                hLayout->addChild(p.historyButton);
                p.layout->addChild(hLayout);
                p.soloLayout = SoloLayout::create(context);
                p.soloLayout->addChild(p.buttonLayout);
                p.soloLayout->addChild(p.lineEdit);
                p.layout->addChild(p.soloLayout);
                p.layout->setStretch(p.soloLayout, RowStretch::Expand);
                addChild(p.layout);

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
                        widget->_p->historyMenu->close();
                    }
                });

                p.historyButton->setOpenCallback(
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value)
                            {
                                widget->_p->historyMenu->popup(widget->_p->historyButton);
                            }
                            else
                            {
                                widget->_p->historyMenu->close();
                            }
                        }
                    });

                p.historyMenu->setCloseCallback(
                    [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->historyButton->setOpen(false);
                        }
                    });

                p.lineEdit->setTextEditCallback(
                    [weak](const std::string& value, UI::TextEditReason reason)
                    {
                        if (auto widget = weak.lock())
                        {
                            switch (reason)
                            {
                            case UI::TextEditReason::Accepted:
                                if (widget->_p->pathCallback)
                                {
                                    widget->_p->pathCallback(FileSystem::Path(value));
                                }
                                break;
                            case UI::TextEditReason::LostFocus:
                                widget->_p->lineEdit->setText(widget->_p->path.get());
                                break;
                            default: break;
                            }
                        }
                    });
                p.lineEdit->setFocusCallback(
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setEdit(value);
                        }
                    });
            }

            PathWidget::PathWidget() :
                _p(new Private)
            {}

            PathWidget::~PathWidget()
            {}

            std::shared_ptr<PathWidget> PathWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<PathWidget>(new PathWidget);
                out->_init(context);
                return out;
            }

            void PathWidget::setPath(const FileSystem::Path & path)
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    if (path == p.path)
                        return;
                    p.path = path;

                    auto split = FileSystem::Path::splitDir(std::string(path));
                    std::vector<FileSystem::Path> paths;
                    while (split.size())
                    {
                        paths.push_back(FileSystem::Path(FileSystem::Path::joinDirs(split)));
                        split.pop_back();
                    }

                    p.buttonLayout->clearChildren();
                    size_t j = 0;
                    for (auto i = paths.rbegin(); i != paths.rend(); ++i, ++j)
                    {
                        if (j < paths.size() - 1)
                        {
                            auto button = ListButton::create(context);
                            button->setText(_getLabel(*i));
                            button->setForegroundColorRole(ColorRole::ForegroundDim);
                            button->setShadowOverlay({ Side::Left });

                            p.buttonLayout->addChild(button);

                            const auto path = *i;
                            auto weak = std::weak_ptr<PathWidget>(std::dynamic_pointer_cast<PathWidget>(shared_from_this()));
                            button->setClickedCallback(
                                [weak, path]
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        if (widget->_p->pathCallback)
                                        {
                                            widget->_p->pathCallback(path);
                                        }
                                    }
                                });
                        }
                        else
                        {
                            auto label = Label::create(context);
                            label->setText(_getLabel(*i));
                            label->setVAlign(VAlign::Fill);
                            label->setMargin(MetricsRole::MarginSmall);
                            label->setShadowOverlay({ Side::Left });

                            p.buttonLayout->addChild(label);
                        }
                    }

                    p.lineEdit->setText(path.get());
                }
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
                    action->setText(std::string(i));
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

            void PathWidget::setEdit(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value)
                {
                    p.lineEdit->takeTextFocus();
                }
                p.soloLayout->setCurrentWidget(
                    value ?
                    std::static_pointer_cast<Widget>(p.lineEdit) :
                    std::static_pointer_cast<Widget>(p.buttonLayout));
            }

            void PathWidget::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void PathWidget::_layoutEvent(Event::Layout & event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void PathWidget::_initEvent(Event::Init & event)
            {
                Widget::_initEvent(event);
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.historyButton->setTooltip(_getText(DJV_TEXT("file_browser_history_tooltip")));
                }
            }

            bool PathWidget::_eventFilter(const std::shared_ptr<IObject> & object, Event::Event & event)
            {
                DJV_PRIVATE_PTR();
                if (object == p.buttonLayout)
                {
                    switch (event.getEventType())
                    {
                    case Event::Type::ButtonPress:

                        event.accept();
                        setEdit(true);
                        break;
                    default: break;
                    }
                }
                return false;
            }

            std::string PathWidget::_getLabel(const Core::FileSystem::Path& value)
            {
                return (value.isRoot() || value.isServer()) ? value.get() : value.getFileName();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
