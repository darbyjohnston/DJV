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

#include <djvUI/IntValueLabel.h>

#include <djvUI/Label.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct IntValueLabel::Private
        {
            std::shared_ptr<IntValueModel> model;
            std::shared_ptr<Label> label;
            std::shared_ptr<ValueObserver<IntRange> > rangeObserver;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void IntValueLabel::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::IntValueLabel");

            p.label = Label::create(context);
            p.label->setTextHAlign(TextHAlign::Right);
            addChild(p.label);

            _textUpdate();
        }

        IntValueLabel::IntValueLabel() :
            _p(new Private)
        {}

        IntValueLabel::~IntValueLabel()
        {}

        std::shared_ptr<IntValueLabel> IntValueLabel::create(Context * context)
        {
            auto out = std::shared_ptr<IntValueLabel>(new IntValueLabel);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<IntValueModel> & IntValueLabel::getModel() const
        {
            return _p->model;
        }

        void IntValueLabel::setModel(const std::shared_ptr<IntValueModel> & model)
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                p.valueObserver.reset();
            }
            p.model = model;
            if (p.model)
            {
                auto weak = std::weak_ptr<IntValueLabel>(std::dynamic_pointer_cast<IntValueLabel>(shared_from_this()));
                p.rangeObserver = ValueObserver<IntRange>::create(
                    p.model->observeRange(),
                    [weak](const IntRange & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = ValueObserver<int>::create(
                    p.model->observeValue(),
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
            }
        }

        void IntValueLabel::_preLayoutEvent(Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            if (auto style = _getStyle().lock())
            {
                _setMinimumSize(p.label->getMinimumSize() + getMargin().getSize(style));
            }
        }

        void IntValueLabel::_layoutEvent(Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            if (auto style = _getStyle().lock())
            {
                p.label->setGeometry(getMargin().bbox(getGeometry(), style));
            }
        }

        void IntValueLabel::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                {
                    std::stringstream ss;
                    ss << p.model->observeValue()->get();
                    p.label->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    const auto & range = p.model->observeRange()->get();
                    const size_t digits = std::max(Math::getNumDigits(range.min), Math::getNumDigits(range.max));
                    std::string sizeString;
                    if (range.min < 0 || range.max < 0)
                    {
                        sizeString += '-';
                    }
                    sizeString += std::string(digits, '0');
                    p.label->setSizeString(sizeString);
                }
            }
        }

    } // namespace UI
} // namespace djv
