// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FloatEdit.h>

#include <djvUI/FloatLabel.h>
#include <djvUI/LineEdit.h>

#include <djvMath/NumericValueModels.h>

#include <djvCore/ValueObserver.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct FloatEdit::Private
        {
            int precision = 2;
            std::shared_ptr<Observer::ValueObserver<Math::FloatRange> > rangeObserver;
            std::shared_ptr<Observer::ValueObserver<float> > valueObserver;
        };

        void FloatEdit::_init(const std::shared_ptr<System::Context>& context)
        {
            NumericEdit::_init(context);
            setClassName("djv::UI::FloatEdit");
            setModel(Math::FloatValueModel::create());
        }

        FloatEdit::FloatEdit() :
            _p(new Private)
        {}

        FloatEdit::~FloatEdit()
        {}
        
        std::shared_ptr<FloatEdit> FloatEdit::create(const std::shared_ptr<System::Context>& context)
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

        void FloatEdit::setModel(const std::shared_ptr<Math::INumericValueModel<float> >& model)
        {
            INumericEdit<float>::setModel(model);
            DJV_PRIVATE_PTR();
            if (model)
            {
                auto weak = std::weak_ptr<FloatEdit>(std::dynamic_pointer_cast<FloatEdit>(shared_from_this()));
                p.rangeObserver = Observer::ValueObserver<Math::FloatRange>::create(
                    model->observeRange(),
                    [weak](const Math::FloatRange& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = Observer::ValueObserver<float>::create(
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

        void FloatEdit::_setIsMin(bool value)
        {
            NumericEdit::_setIsMin(value);
        }

        void FloatEdit::_setIsMax(bool value)
        {
            NumericEdit::_setIsMax(value);
        }

        void FloatEdit::_textEdit(const std::string& value, TextEditReason reason)
        {
            if (auto model = getModel())
            {
                try
                {
                    model->setValue(std::stof(value));
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
                _textUpdate();
                _doCallback(reason);
            }
        }

        bool FloatEdit::_keyPress(int value)
        {
            return INumericEdit<float>::_doKeyPress(fromGLFWKey(value));
        }

        void FloatEdit::_scroll(float value)
        {
            if (auto model = getModel())
            {
                model->setValue(model->observeValue()->get() + model->observeSmallIncrement()->get() * value);
            }
        }

        void FloatEdit::_incrementValue()
        {
            if (auto model = getModel())
            {
                model->incrementSmall();
                _doCallback(TextEditReason::Accepted);
            }
        }

        void FloatEdit::_decrementValue()
        {
            if (auto model = getModel())
            {
                model->decrementSmall();
                _doCallback(TextEditReason::Accepted);
            }
        }

        void FloatEdit::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto model = getModel())
            {
                std::stringstream ss;
                ss.precision(p.precision);
                const float value = model->observeValue()->get();
                ss << std::fixed << value;
                NumericEdit::_textUpdate(ss.str(), FloatLabel::getSizeString(model->observeRange()->get(), p.precision));
            }
            _resize();
        }

    } // namespace UI
} // namespace djv
