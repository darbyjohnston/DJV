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

#include <djvViewLib/SystemLogDialog.h>

#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TextBlock.h>

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>
#include <djvCore/Path.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct SystemLogDialog::Private
        {
            bool shown = false;
            std::shared_ptr<UI::TextBlock> textBlock;
            std::shared_ptr< UI::PushButton> copyButton;
            std::shared_ptr< UI::PushButton> reloadButton;
            std::shared_ptr< UI::PushButton> clearButton;
        };

        void SystemLogDialog::_init(Context * context)
        {
            IDialog::_init(context);

            setClassName("djv::ViewLib::SystemLogDialog");

            DJV_PRIVATE_PTR();
            p.textBlock = UI::TextBlock::create(context);
            p.textBlock->setFontFamily(AV::Font::Info::familyMono);
            p.textBlock->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.textBlock->setMargin(UI::MetricsRole::MarginSmall);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.textBlock);

            //! \todo Implement me!
            p.copyButton = UI::PushButton::create(context);
            p.copyButton->setEnabled(false);
            p.reloadButton = UI::PushButton::create(context);
            p.clearButton = UI::PushButton::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(scrollWidget);
            layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::MetricsRole::MarginSmall);
            hLayout->addExpander();
            hLayout->addChild(p.copyButton);
            hLayout->addChild(p.reloadButton);
            hLayout->addChild(p.clearButton);
            layout->addChild(hLayout);
            addChild(layout);
            setStretch(layout, UI::RowStretch::Expand);

            auto weak = std::weak_ptr<SystemLogDialog>(std::dynamic_pointer_cast<SystemLogDialog>(shared_from_this()));
            p.reloadButton->setClickedCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->reloadLog();
                }
            });
            p.clearButton->setClickedCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->clearLog();
                }
            });
        }

        SystemLogDialog::SystemLogDialog() :
            _p(new Private)
        {}

        SystemLogDialog::~SystemLogDialog()
        {}

        std::shared_ptr<SystemLogDialog> SystemLogDialog::create(Context * context)
        {
            auto out = std::shared_ptr<SystemLogDialog>(new SystemLogDialog);
            out->_init(context);
            return out;
        }

        void SystemLogDialog::reloadLog()
        {
            try
            {
                auto context = getContext();
                const auto log = FileSystem::FileIO::readLines(context->getPath(FileSystem::ResourcePath::LogFile));
                _p->textBlock->setText(String::join(log, '\n'));
            }
            catch (const std::exception & e)
            {
                _log(e.what(), LogLevel::Error);
            }
        }

        void SystemLogDialog::clearLog()
        {
            _p->textBlock->setText(std::string());
        }

        void SystemLogDialog::_localeEvent(Event::Locale & event)
        {
            IDialog::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("System Log")));
            p.copyButton->setText(_getText(DJV_TEXT("Copy")));
            p.reloadButton->setText(_getText(DJV_TEXT("Reload")));
            p.clearButton->setText(_getText(DJV_TEXT("Clear")));
        }

    } // namespace ViewLib
} // namespace djv

