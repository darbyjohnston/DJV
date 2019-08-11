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

#include <djvUI/FloatEdit.h>

#include <djvUI/FloatLabel.h>
#include <djvUI/LineEdit.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct FloatEdit::Private
        {
            int precision = 2;
            std::shared_ptr<LineEdit> lineEdit;
            std::shared_ptr<ValueObserver<FloatRange> > rangeObserver;
            std::shared_ptr<ValueObserver<float> > valueObserver;
        };

        void FloatEdit::_init(Context * context)
        {
            INumericEdit<float>::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::FloatEdit");

            p.lineEdit = LineEdit::create(context);
            addChild(p.lineEdit);

            setModel(FloatValueModel::create());

            auto weak = std::weak_ptr<FloatEdit>(std::dynamic_pointer_cast<FloatEdit>(shared_from_this()));
            p.lineEdit->setTextFinishedCallback(
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto model = widget->getModel())
                    {
                        try
                        {
                            model->setValue(std::stof(value));
                        }
                        catch (const std::exception &)
                        {}
                        widget->_textUpdate();
                        widget->_doCallback();
                    }
                }
            });
        }

        FloatEdit::FloatEdit() :
            _p(new Private)
        {}

        FloatEdit::~FloatEdit()
        {}
        
        std::shared_ptr<FloatEdit> FloatEdit::create(Context * context)
        {
            auto out = std::shared_ptr<FloatEdit>(new FloatEdit);
            out->_init(context);
            return out;
        }

        int FloatEdit::getPrecision()
        {
            return _p->precision;
        }

        void FloatEdit::setPrecision(int value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.precision)
                return;
            p.precision = value;
            _textUpdate();
        }

        void FloatEdit::setModel(const std::shared_ptr<INumericValueModel<float> > & model)
        {
            INumericEdit<float>::setModel(model);
            DJV_PRIVATE_PTR();
            if (model)
            {
                auto weak = std::weak_ptr<FloatEdit>(std::dynamic_pointer_cast<FloatEdit>(shared_from_this()));
                p.rangeObserver = ValueObserver<FloatRange>::create(
                    model->observeRange(),
                    [weak](const FloatRange & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = ValueObserver<float>::create(
                    model->observeValue(),
                    [weak](float value)
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

        void FloatEdit::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.lineEdit->getMinimumSize() + getMargin().getSize(style));
        }

        void FloatEdit::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.lineEdit->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void FloatEdit::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto model = getModel())
            {
                std::stringstream ss;
                ss.precision(p.precision);
                ss << std::fixed << model->observeValue()->get();
                p.lineEdit->setText(ss.str());
                p.lineEdit->setSizeString(FloatLabel::getSizeString(model->observeRange()->get(), p.precision));
            }
            _resize();
        }

        void FloatEdit::_keyPressEvent(Event::KeyPress& event)
        {
            DJV_PRIVATE_PTR();
            switch (event.getKey())
            {
            case GLFW_KEY_UP:
                if (auto model = getModel())
                {
                    event.accept();
                    model->incrementSmall();
                    _doCallback();
                }
                break;
            case GLFW_KEY_PAGE_UP:
                if (auto model = getModel())
                {
                    event.accept();
                    model->incrementLarge();
                    _doCallback();
                }
                break;
            case GLFW_KEY_DOWN:
                if (auto model = getModel())
                {
                    event.accept();
                    model->decrementSmall();
                    _doCallback();
                }
                break;
            case GLFW_KEY_PAGE_DOWN:
                if (auto model = getModel())
                {
                    event.accept();
                    model->decrementLarge();
                    _doCallback();
                }
                break;
            }
        }

    } // namespace UI
} // namespace djv
