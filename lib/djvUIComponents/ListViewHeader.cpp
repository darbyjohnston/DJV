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

#include <djvUIComponents/ListViewHeader.h>

#include <djvUI/FlatButton.h>
#include <djvUI/Splitter.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ListViewHeader::Private
        {
            std::shared_ptr<Layout::Splitter> splitter;
        };

        void ListViewHeader::_init(Context * context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ListViewHeader");
            p.splitter = Layout::Splitter::create(Orientation::Horizontal, context);
            addChild(p.splitter);
        }

        ListViewHeader::ListViewHeader() :
            _p(new Private)
        {}

        ListViewHeader::~ListViewHeader()
        {}

        std::shared_ptr<ListViewHeader> ListViewHeader::create(Context * context)
        {
            auto out = std::shared_ptr<ListViewHeader>(new ListViewHeader);
            out->_init(context);
            return out;
        }

        void ListViewHeader::setText(const std::vector<std::string> & value)
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.splitter->clearChildren();
            for (const auto & i : value)
            {
                auto button = FlatButton::create(i, context);
                p.splitter->addChild(button);
            }
        }

        const std::vector<float> & ListViewHeader::getSplit() const
        {
            return _p->splitter->getSplit();
        }

        void ListViewHeader::setSplit(const std::vector<float> & value)
        {
            _p->splitter->setSplit(value);
        }

        void ListViewHeader::setSplitCallback(const std::function<void(const std::vector<float> &)> & callback)
        {
            _p->splitter->setSplitCallback(callback);
        }

        void ListViewHeader::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->splitter->getMinimumSize());
        }

        void ListViewHeader::_layoutEvent(Event::Layout & event)
        {
            _p->splitter->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv
