// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Label.h>
#include <djvUI/LineEdit.h>
#include <djvUI/ListButton.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>

#include <djvSystem/FileInfo.h>
#include <djvSystem/PathFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace FileBrowser
        {
            struct PathWidget::Private
            {
                System::File::Path path;
                std::vector<System::File::Path> history;
                std::shared_ptr<UI::ActionGroup> historyActionGroup;
                std::shared_ptr<UI::Menu> historyMenu;
                std::shared_ptr<UI::Button::Menu> historyButton;
                std::shared_ptr<UI::HorizontalLayout> buttonLayout;
                std::shared_ptr<UI::Text::LineEdit> lineEdit;
                std::shared_ptr<UI::SoloLayout> soloLayout;
                std::shared_ptr<UI::HorizontalLayout> layout;
                std::function<void(const System::File::Path &)> pathCallback;
                std::function<void(size_t)> historyIndexCallback;
            };

            void PathWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::FileBrowser::PathWidget");
                setPointerEnabled(true);

                p.historyActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
                p.historyMenu = UI::Menu::create(context);
                p.historyMenu->setMinimumSizeRole(UI::MetricsRole::None);
                addChild(p.historyMenu);
                p.historyButton = UI::Button::Menu::create(UI::MenuButtonStyle::Flat, context);
                p.historyButton->setIcon("djvIconPopupMenu");
                p.historyButton->setEnabled(false);

                p.buttonLayout = UI::HorizontalLayout::create(context);
                p.buttonLayout->setSpacing(UI::MetricsRole::None);
                p.buttonLayout->setPointerEnabled(true);
                p.buttonLayout->installEventFilter(shared_from_this());

                p.lineEdit = UI::Text::LineEdit::create(context);
                p.lineEdit->setBackgroundRole(UI::ColorRole::Trough);

                p.layout = UI::HorizontalLayout::create(context);
                p.layout->setSpacing(UI::MetricsRole::None);
                auto hLayout = UI::HorizontalLayout::create(context);
                hLayout->setSpacing(UI::MetricsRole::None);
                hLayout->addChild(p.historyButton);
                p.layout->addChild(hLayout);
                p.soloLayout = UI::SoloLayout::create(context);
                p.soloLayout->addChild(p.buttonLayout);
                p.soloLayout->addChild(p.lineEdit);
                p.layout->addChild(p.soloLayout);
                p.layout->setStretch(p.soloLayout);
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
                                    widget->_p->pathCallback(System::File::Path(value));
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

            std::shared_ptr<PathWidget> PathWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<PathWidget>(new PathWidget);
                out->_init(context);
                return out;
            }

            void PathWidget::setPath(const System::File::Path & path)
            {
                DJV_PRIVATE_PTR();
                if (auto context = getContext().lock())
                {
                    if (path == p.path)
                        return;
                    p.path = path;

                    auto split = System::File::splitDir(std::string(path));
                    std::vector<System::File::Path> paths;
                    while (split.size())
                    {
                        paths.push_back(System::File::Path(System::File::joinDirs(split)));
                        split.pop_back();
                    }

                    p.buttonLayout->clearChildren();
                    size_t j = 0;
                    for (auto i = paths.rbegin(); i != paths.rend(); ++i, ++j)
                    {
                        if (j < paths.size() - 1)
                        {
                            auto button = UI::ListButton::create(context);
                            button->setText(_getLabel(*i));
                            button->setForegroundColorRole(UI::ColorRole::ForegroundDim);
                            button->setShadowOverlay({ UI::Side::Left });

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
                            auto label = UI::Text::Label::create(context);
                            label->setText(_getLabel(*i));
                            label->setVAlign(UI::VAlign::Fill);
                            label->setMargin(UI::MetricsRole::MarginSmall);
                            label->setShadowOverlay({ UI::Side::Left });

                            p.buttonLayout->addChild(label);
                        }
                    }

                    p.lineEdit->setText(path.get());
                }
            }

            void PathWidget::setPathCallback(const std::function<void(const System::File::Path &)> & value)
            {
                _p->pathCallback = value;
            }

            void PathWidget::setHistory(const std::vector<System::File::Path> & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.history)
                    return;
                p.history = value;
                std::vector<std::shared_ptr<UI::Action> > actions;
                p.historyMenu->clearActions();
                for (const auto & i : p.history)
                {
                    auto action = UI::Action::create();
                    action->setText(std::string(i));
                    actions.push_back(action);
                    p.historyMenu->addAction(action);
                }
                p.historyActionGroup->setActions(actions);
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

            void PathWidget::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void PathWidget::_layoutEvent(System::Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void PathWidget::_initEvent(System::Event::Init & event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.historyButton->setTooltip(_getText(DJV_TEXT("file_browser_history_tooltip")));
                }
            }

            bool PathWidget::_eventFilter(const std::shared_ptr<System::IObject> & object, System::Event::Event & event)
            {
                DJV_PRIVATE_PTR();
                if (object == p.buttonLayout)
                {
                    switch (event.getEventType())
                    {
                    case System::Event::Type::ButtonPress:

                        event.accept();
                        setEdit(true);
                        break;
                    default: break;
                    }
                }
                return false;
            }

            std::string PathWidget::_getLabel(const System::File::Path& value)
            {
                return (value.isRoot() || value.isServer()) ? value.get() : value.getFileName();
            }

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv
