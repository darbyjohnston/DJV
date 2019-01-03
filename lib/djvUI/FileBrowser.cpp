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

#include <djvUI/FileBrowser.h>

#include <djvUI/FileBrowserPrivate.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/ToolButton.h>

#include <djvCore/DirectoryModel.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct Widget::Private
            {
                std::shared_ptr<FileSystem::DirectoryModel> directoryModel;
                std::vector<FileSystem::FileInfo> fileInfoList;
                std::shared_ptr<Layout::Vertical> itemLayout;
                std::shared_ptr<Layout::Vertical> layout;
                std::shared_ptr<ValueObserver<FileSystem::Path> > pathObserver;
                std::shared_ptr<ListObserver<FileSystem::FileInfo> > fileInfoObserver;
                std::shared_ptr<ValueObserver<bool> > hasUpObserver;
                std::shared_ptr<ListObserver<FileSystem::Path> > historyObserver;
                std::shared_ptr<ValueObserver<size_t> > historyIndexObserver;
                std::shared_ptr<ValueObserver<bool> > hasBackObserver;
                std::shared_ptr<ValueObserver<bool> > hasForwardObserver;
            };

            void Widget::_init(Context * context)
            {
                UI::Widget::_init(context);

                setClassName("djv::UI::FileBrowser::Widget");

                _p->directoryModel = FileSystem::DirectoryModel::create(context);

                auto upButton = Button::Tool::create(context);
                upButton->setIcon(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconArrowUp90DPI.png"));
                auto backButton = Button::Tool::create(context);
                backButton->setIcon(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconArrowLeft90DPI.png"));
                auto forwardButton = Button::Tool::create(context);
                forwardButton->setIcon(context->getPath(Core::FileSystem::ResourcePath::IconsDirectory, "djvIconArrowRight90DPI.png"));

                auto pathWidget = PathWidget::create(context);

                auto topToolLayout = Layout::Horizontal::create(context);
                topToolLayout->setSpacing(Style::MetricsRole::None);
                topToolLayout->addWidget(upButton);
                topToolLayout->addWidget(backButton);
                topToolLayout->addWidget(forwardButton);
                topToolLayout->addWidget(pathWidget, Layout::RowStretch::Expand);

                _p->itemLayout = Layout::Vertical::create(context);
                _p->itemLayout->setSpacing(Style::MetricsRole::None);

                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->addWidget(_p->itemLayout);

                _p->layout = Layout::Vertical::create(context);
                _p->layout->setSpacing(Style::MetricsRole::None);
                _p->layout->addWidget(topToolLayout);
                _p->layout->addWidget(scrollWidget, Layout::RowStretch::Expand);
                _p->layout->setParent(shared_from_this());

                auto weak = std::weak_ptr<Widget>(std::dynamic_pointer_cast<Widget>(shared_from_this()));
                upButton->setClickedCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->cdUp();
                    }
                });
                backButton->setClickedCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->goBack();
                    }
                });
                forwardButton->setClickedCallback(
                    [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->goForward();
                    }
                });

                pathWidget->setPathCallback(
                    [weak](const FileSystem::Path & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->directoryModel->setPath(value);
                    }
                });

                _p->pathObserver = ValueObserver<FileSystem::Path>::create(
                    _p->directoryModel->observePath(),
                    [pathWidget](const FileSystem::Path & value)
                {
                    pathWidget->setPath(value);
                });

                _p->fileInfoObserver = ListObserver<FileSystem::FileInfo>::create(
                    _p->directoryModel->observeFileInfoList(),
                    [weak](const std::vector<FileSystem::FileInfo> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fileInfoList = value;
                        widget->_updateItems();
                    }
                });

                _p->hasUpObserver = ValueObserver<bool>::create(
                    _p->directoryModel->observeHasUp(),
                    [upButton](bool value)
                {
                    upButton->setEnabled(value);
                });

                _p->historyObserver = ListObserver<FileSystem::Path>::create(
                    _p->directoryModel->observeHistory(),
                    [pathWidget](const std::vector<FileSystem::Path> & value)
                {
                    pathWidget->setHistory(value);
                });

                _p->historyIndexObserver = ValueObserver<size_t>::create(
                    _p->directoryModel->observeHistoryIndex(),
                    [pathWidget](size_t value)
                {
                    pathWidget->setHistoryIndex(value);
                });

                _p->hasBackObserver = ValueObserver<bool>::create(
                    _p->directoryModel->observeHasBack(),
                    [backButton](bool value)
                {
                    backButton->setEnabled(value);
                });

                _p->hasForwardObserver = ValueObserver<bool>::create(
                    _p->directoryModel->observeHasForward(),
                    [forwardButton](bool value)
                {
                    forwardButton->setEnabled(value);
                });
            }

            Widget::Widget() :
                _p(new Private)
            {}

            Widget::~Widget()
            {}

            std::shared_ptr<Widget> Widget::create(Context * context)
            {
                auto out = std::shared_ptr<Widget>(new Widget);
                out->_init(context);
                return out;
            }

            void Widget::setPath(const FileSystem::Path & value)
            {
                _p->directoryModel->setPath(value);
            }

            void Widget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void Widget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void Widget::_updateItems()
            {
                auto context = getContext();
                _p->itemLayout->clearWidgets();
                for (const auto & fileInfo : _p->fileInfoList)
                {
                    auto button = Button::List::create(context);
                    button->setText(fileInfo.getFileName(Frame::Invalid, false));
                    button->setTextHAlign(TextHAlign::Left);
                    _p->itemLayout->addWidget(button);
                }
                _resize();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
