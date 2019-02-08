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

#include <djvAV/IO.h>

#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct ShortcutsWidget::Private
            {
                std::vector<FileSystem::Path> shortcuts;
                std::shared_ptr<ActionGroup> actionGroup;
                std::shared_ptr<VerticalLayout> layout;
                std::function<void(const FileSystem::Path &)> shortcutCallback;
            };

            void ShortcutsWidget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::ShortcutsWidget");

                DJV_PRIVATE_PTR();
                p.actionGroup = ActionGroup::create(ButtonType::Push);

                auto itemLayout = VerticalLayout::create(context);
                itemLayout->setSpacing(MetricsRole::None);
                for (size_t i = 0; i < static_cast<size_t>(OS::DirectoryShortcut::Count); ++i)
                {
                    const auto shortcut = OS::getPath(static_cast<OS::DirectoryShortcut>(i));
                    p.shortcuts.push_back(shortcut);
                }
                p.shortcuts.push_back(FileSystem::Path("//MAGURO/darby"));
                for (const auto & i : p.shortcuts)
                {
                    auto action = Action::create();
                    const auto text = i.getFileName();
                    action->setText(text);
                    p.actionGroup->addAction(action);

                    auto button = ListButton::create(context);
                    button->setText(text);
                    itemLayout->addWidget(button);

                    button->setClickedCallback(
                        [action]
                    {
                        action->doClicked();
                    });
                }
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->addWidget(itemLayout);

                p.layout = VerticalLayout::create(context);
                p.layout->addWidget(scrollWidget, RowStretch::Expand);
                p.layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<ShortcutsWidget>(std::dynamic_pointer_cast<ShortcutsWidget>(shared_from_this()));
                p.actionGroup->setPushCallback(
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

			ShortcutsWidget::ShortcutsWidget() :
                _p(new Private)
            {}

			ShortcutsWidget::~ShortcutsWidget()
            {}

            std::shared_ptr<ShortcutsWidget> ShortcutsWidget::create(Context * context)
            {
                auto out = std::shared_ptr<ShortcutsWidget>(new ShortcutsWidget);
                out->_init(context);
                return out;
            }

            void ShortcutsWidget::setShortcutCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->shortcutCallback = value;
            }

            void ShortcutsWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void ShortcutsWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
