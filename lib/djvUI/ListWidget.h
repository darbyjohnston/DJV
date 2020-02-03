//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a list widget.
        //!
        //! \todo Add support for icons. Should we use actions instead of items?
        //! \todo Keep the current item visible in the scroll widget.
        class ListWidget : public Widget
        {
            DJV_NON_COPYABLE(ListWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ListWidget();

        public:
            virtual ~ListWidget();

            static std::shared_ptr<ListWidget> create(const std::shared_ptr<Core::Context>&);

            const std::vector<std::string> & getItems() const;
            void setItems(const std::vector<std::string> &);
            void addItem(const std::string &);
            void clearItems(Callback = Callback::Suppress);

            int getCurrentItem() const;
            void setCurrentItem(int, Callback = Callback::Suppress);
            void firstItem(Callback = Callback::Suppress);
            void lastItem(Callback = Callback::Suppress);
            void prevItem(Callback = Callback::Suppress);
            void nextItem(Callback = Callback::Suppress);
            void setCallback(const std::function<void(int)> &);

            void setBorder(bool);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;

        private:
            void _updateItems();
            void _updateCurrentItem(Callback);

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

