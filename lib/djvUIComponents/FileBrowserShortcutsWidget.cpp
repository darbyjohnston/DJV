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

#include <djvUI/Action.h>
#include <djvUI/GroupBox.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct ShortcutsWidget::Private
            {
                std::shared_ptr<VerticalLayout> layout;
                std::function<void(const FileSystem::Path &)> callback;
                std::shared_ptr<ListObserver<FileSystem::Path> > shortcutsObserver;
            };

            void ShortcutsWidget::_init(const std::shared_ptr<ShortcutsModel> & model, Context * context)
            {
                UI::Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::ShortcutsWidget");

                p.layout = VerticalLayout::create(context);
                auto itemLayout = VerticalLayout::create(context);
                itemLayout->setSpacing(MetricsRole::None);
                p.layout->addChild(itemLayout);
                p.layout->setStretch(itemLayout, RowStretch::Expand);
                addChild(p.layout);

                auto weak = std::weak_ptr<ShortcutsWidget>(std::dynamic_pointer_cast<ShortcutsWidget>(shared_from_this()));
                p.shortcutsObserver = ListObserver<FileSystem::Path>::create(
                    model->observeShortcuts(),
                    [weak, itemLayout, context](const std::vector<FileSystem::Path> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        itemLayout->clearChildren();
                        for (const auto & i : value)
                        {
                            auto button = ListButton::create(context);
                            button->setText(i.getFileName());
                            itemLayout->addChild(button);

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
                        }
                    }
                });
            }

            ShortcutsWidget::ShortcutsWidget() :
                _p(new Private)
            {}

            ShortcutsWidget::~ShortcutsWidget()
            {}

            std::shared_ptr<ShortcutsWidget> ShortcutsWidget::create(const std::shared_ptr<ShortcutsModel> & model, Context * context)
            {
                auto out = std::shared_ptr<ShortcutsWidget>(new ShortcutsWidget);
                out->_init(model, context);
                return out;
            }

            void ShortcutsWidget::setCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->callback = value;
            }

            void ShortcutsWidget::_preLayoutEvent(Event::PreLayout & event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void ShortcutsWidget::_layoutEvent(Event::Layout & event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
