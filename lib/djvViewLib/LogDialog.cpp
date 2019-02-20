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

#include <djvViewLib/LogDialog.h>

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
        struct LogDialog::Private
        {
			bool shown = false;
            std::shared_ptr<UI::TextBlock> textBlock;
            std::shared_ptr< UI::PushButton> copyButton;
            std::shared_ptr< UI::PushButton> reloadButton;
            std::shared_ptr< UI::PushButton> clearButton;
        };

        void LogDialog::_init(Context * context)
        {
            IDialog::_init(context);

            setClassName("djv::ViewLib::LogDialog");

            DJV_PRIVATE_PTR();
            p.textBlock = UI::TextBlock::create(context);
            p.textBlock->setFontFamily(AV::Font::Info::familyMono);
            p.textBlock->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.textBlock->setMargin(UI::MetricsRole::Margin);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addWidget(p.textBlock);

            //! \todo Implement me!
            p.copyButton = UI::PushButton::create(context);
            p.copyButton->setEnabled(false);
            p.reloadButton = UI::PushButton::create(context);
            p.clearButton = UI::PushButton::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addWidget(scrollWidget, UI::RowStretch::Expand);
            layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::MetricsRole::MarginSmall);
            hLayout->addExpander();
            hLayout->addWidget(p.copyButton);
            hLayout->addWidget(p.reloadButton);
            hLayout->addWidget(p.clearButton);
            layout->addWidget(hLayout);
            addWidget(layout, UI::RowStretch::Expand);

            auto weak = std::weak_ptr<LogDialog>(std::dynamic_pointer_cast<LogDialog>(shared_from_this()));
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

		LogDialog::LogDialog() :
            _p(new Private)
        {}

		LogDialog::~LogDialog()
        {}

        std::shared_ptr<LogDialog> LogDialog::create(Context * context)
        {
            auto out = std::shared_ptr<LogDialog>(new LogDialog);
            out->_init(context);
            return out;
        }

        void LogDialog::reloadLog()
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

        void LogDialog::clearLog()
        {
            _p->textBlock->setText(std::string());
        }

        void LogDialog::_localeEvent(Event::Locale & event)
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

