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
            std::shared_ptr<FloatValueModel> model;
            int precision = 2;
            std::shared_ptr<LineEdit> lineEdit;
            std::function<void(float)> callback;
            std::shared_ptr<ValueObserver<FloatRange> > rangeObserver;
            std::shared_ptr<ValueObserver<float> > valueObserver;
        };

        void FloatEdit::_init(Context * context)
        {
            Widget::_init(context);

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
                    if (widget->_p->model)
                    {
                        try
                        {
                            widget->_p->model->setValue(std::stof(value));
                        }
                        catch (const std::exception &)
                        {}
                        widget->_textUpdate();
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(widget->_p->model->observeValue()->get());
                        }
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

        FloatRange FloatEdit::getRange() const
        {
            return _p->model->observeRange()->get();
        }

        void FloatEdit::setRange(const FloatRange& value)
        {
            _p->model->setRange(value);
        }

        float FloatEdit::getValue() const
        {
            return _p->model->observeValue()->get();
        }

        void FloatEdit::setValue(float value)
        {
            _p->model->setValue(value);
        }

        void FloatEdit::setValueCallback(const std::function<void(float)>& callback)
        {
            _p->callback = callback;
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

        const std::shared_ptr<FloatValueModel> & FloatEdit::getModel() const
        {
            return _p->model;
        }

        void FloatEdit::setModel(const std::shared_ptr<FloatValueModel> & model)
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                p.rangeObserver.reset();
                p.valueObserver.reset();
            }
            p.model = model;
            if (p.model)
            {
                auto weak = std::weak_ptr<FloatEdit>(std::dynamic_pointer_cast<FloatEdit>(shared_from_this()));
                p.rangeObserver = ValueObserver<FloatRange>::create(
                    p.model->observeRange(),
                    [weak](const FloatRange & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = ValueObserver<float>::create(
                    p.model->observeValue(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
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
            if (p.model)
            {
                {
                    std::stringstream ss;
                    ss.precision(p.precision);
                    ss << std::fixed << p.model->observeValue()->get();
                    p.lineEdit->setText(ss.str());
                    p.lineEdit->setSizeString(FloatLabel::getSizeString(p.model->observeRange()->get(), p.precision));
                }
            }
            _resize();
        }

        void FloatEdit::_keyPressEvent(Event::KeyPress& event)
        {
            DJV_PRIVATE_PTR();
            switch (event.getKey())
            {
            case GLFW_KEY_UP:
                if (p.model)
                {
                    event.accept();
                    p.model->incrementSmall();
                    if (p.callback)
                    {
                        p.callback(p.model->observeValue()->get());
                    }
                }
                break;
            case GLFW_KEY_PAGE_UP:
                if (p.model)
                {
                    event.accept();
                    p.model->incrementLarge();
                    if (p.callback)
                    {
                        p.callback(p.model->observeValue()->get());
                    }
                }
                break;
            case GLFW_KEY_DOWN:
                if (p.model)
                {
                    event.accept();
                    p.model->decrementSmall();
                    if (p.callback)
                    {
                        p.callback(p.model->observeValue()->get());
                    }
                }
                break;
            case GLFW_KEY_PAGE_DOWN:
                if (p.model)
                {
                    event.accept();
                    p.model->decrementLarge();
                    if (p.callback)
                    {
                        p.callback(p.model->observeValue()->get());
                    }
                }
                break;
            }
        }

    } // namespace UI
} // namespace djv
