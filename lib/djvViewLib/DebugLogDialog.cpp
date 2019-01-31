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

#include <djvViewLib/DebugLogDialog.h>

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
        struct DebugLogDialog::Private
        {
            std::shared_ptr<UI::TextBlock> textBlock;
            std::shared_ptr< UI::Button::Push> copyButton;
            std::shared_ptr< UI::Button::Push> reloadButton;
            std::shared_ptr< UI::Button::Push> clearButton;
        };

        void DebugLogDialog::_init(Context * context)
        {
            IDialog::_init(context);

            setClassName("djv::ViewLib::DebugLogDialog");

            DJV_PRIVATE_PTR();
            p.textBlock = UI::TextBlock::create(context);
            p.textBlock->setFontFamily(AV::Font::Info::familyMono);
            p.textBlock->setFontSizeRole(UI::Style::MetricsRole::FontSmall);
            p.textBlock->setMargin(UI::Style::MetricsRole::Margin);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addWidget(p.textBlock);

            //! \todo Implement me!
            p.copyButton = UI::Button::Push::create(context);
            p.copyButton->setEnabled(false);
            p.reloadButton = UI::Button::Push::create(context);
            p.clearButton = UI::Button::Push::create(context);

            auto layout = UI::Layout::Vertical::create(context);
            layout->setSpacing(UI::Style::MetricsRole::None);
            layout->addWidget(scrollWidget, UI::Layout::RowStretch::Expand);
            layout->addSeparator();
            auto hLayout = UI::Layout::Horizontal::create(context);
            hLayout->setMargin(UI::Style::MetricsRole::MarginSmall);
            hLayout->addExpander();
            hLayout->addWidget(p.copyButton);
            hLayout->addWidget(p.reloadButton);
            hLayout->addWidget(p.clearButton);
            layout->addWidget(hLayout);
            addWidget(layout, UI::Layout::RowStretch::Expand);

            auto weak = std::weak_ptr<DebugLogDialog>(std::dynamic_pointer_cast<DebugLogDialog>(shared_from_this()));
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

        DebugLogDialog::DebugLogDialog() :
            _p(new Private)
        {}

        DebugLogDialog::~DebugLogDialog()
        {}

        std::shared_ptr<DebugLogDialog> DebugLogDialog::create(Context * context)
        {
            auto out = std::shared_ptr<DebugLogDialog>(new DebugLogDialog);
            out->_init(context);
            return out;
        }

        void DebugLogDialog::reloadLog()
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

        void DebugLogDialog::clearLog()
        {
            _p->textBlock->setText(std::string());
        }

        void DebugLogDialog::_localeEvent(Event::Locale &)
        {
            setTitle(_getText(DJV_TEXT("djv::ViewLib::DebugLogDialog", "Debugging Log")));

            DJV_PRIVATE_PTR();
            p.copyButton->setText(_getText(DJV_TEXT("djv::ViewLib::DebugLogDialog", "Copy")));
            p.reloadButton->setText(_getText(DJV_TEXT("djv::ViewLib::DebugLogDialog", "Reload")));
            p.clearButton->setText(_getText(DJV_TEXT("djv::ViewLib::DebugLogDialog", "Clear")));
        }

    } // namespace ViewLib
} // namespace djv

