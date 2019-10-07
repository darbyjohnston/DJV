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

#include <djvUI/IntEdit.h>

#include <djvUI/IntLabel.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct IntEdit::Private
        {
            std::shared_ptr<ValueObserver<IntRange> > rangeObserver;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void IntEdit::_init(const std::shared_ptr<Context>& context)
        {
            NumericEdit::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::UI::IntEdit");
            setModel(IntValueModel::create());
        }

        IntEdit::IntEdit() :
            _p(new Private)
        {}

        IntEdit::~IntEdit()
        {}

        std::shared_ptr<IntEdit> IntEdit::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<IntEdit>(new IntEdit);
            out->_init(context);
            return out;
        }

        void IntEdit::setModel(const std::shared_ptr<INumericValueModel<int> > & model)
        {
            INumericEdit<int>::setModel(model);
            DJV_PRIVATE_PTR();
            if (model)
            {
                auto weak = std::weak_ptr<IntEdit>(std::dynamic_pointer_cast<IntEdit>(shared_from_this()));
                p.rangeObserver = ValueObserver<IntRange>::create(
                    model->observeRange(),
                    [weak](const IntRange&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = ValueObserver<int>::create(
                    model->observeValue(),
                    [weak](int)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
            }
            else
            {
                p.rangeObserver.reset();
                p.valueObserver.reset();
            }
        }

        void IntEdit::_finishedEditing(const std::string& value, TextFinished finished)
        {
            if (auto model = getModel())
            {
                try
                {
                    model->setValue(std::stoi(value));
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
                _textUpdate();
                _doCallback(finished);
            }
        }

        bool IntEdit::_keyPress(int value)
        {
            return INumericEdit<int>::_keyPress(fromGLFWKey(value));
        }

        void IntEdit::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto model = getModel())
            {
                std::stringstream ss;
                ss << model->observeValue()->get();
                NumericEdit::_textUpdate(ss.str(), IntLabel::getSizeString(model->observeRange()->get()));
            }
            _resize();
        }

    } // namespace UI
} // namespace djv
