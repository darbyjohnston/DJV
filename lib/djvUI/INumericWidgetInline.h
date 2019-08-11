//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

namespace djv
{
    namespace UI
    {
        template<typename T>
        inline INumericWidget<T>::~INumericWidget()
        {}

        template<typename T>
        inline const Core::Range::Range<T>& INumericWidget<T>::getRange() const
        {
            return _model->observeRange()->get();
        }

        template<typename T>
        inline void INumericWidget<T>::setRange(const Core::Range::Range<T>& value)
        {
            _model->setRange(value);
        }

        template<typename T>
        inline T INumericWidget<T>::getValue() const
        {
            return _model->observeValue()->get();
        }

        template<typename T>
        inline void INumericWidget<T>::setValue(T value)
        {
            _model->setValue(value);
        }

        template<typename T>
        inline void INumericWidget<T>::setValueCallback(const std::function<void(T)>& callback)
        {
            _callback = callback;
        }

        template<typename T>
        inline const std::shared_ptr<Core::INumericValueModel<T> >& INumericWidget<T>::getModel() const
        {
            return _model;
        }

        template<typename T>
        inline void INumericWidget<T>::setModel(const std::shared_ptr<Core::INumericValueModel<T> >& value)
        {
            _model = value;
        }

        template<typename T>
        inline void INumericWidget<T>::_doCallback()
        {
            if (_callback)
            {
                _callback(_model->observeValue()->get());
            }
        }

        template<typename T>
        inline bool INumericWidget<T>::_keyPress(NumericWidgetKey key)
        {
            bool out = false;
            switch (key)
            {
            case NumericWidgetKey::Home:
                if (_model)
                {
                    out = true;
                    _model->setMin();
                }
                break;
            case NumericWidgetKey::End:
                if (_model)
                {
                    out = true;
                    _model->setMax();
                }
                break;
            case NumericWidgetKey::Up:
            case NumericWidgetKey::Right:
                if (_model)
                {
                    out = true;
                    _model->incrementSmall();
                }
                break;
            case NumericWidgetKey::Down:
            case NumericWidgetKey::Left:
                if (_model)
                {
                    out = true;
                    _model->decrementSmall();
                }
                break;
            case NumericWidgetKey::PageUp:
                if (_model)
                {
                    out = true;
                    _model->incrementLarge();
                }
                break;
            case NumericWidgetKey::PageDown:
                if (_model)
                {
                    out = true;
                    _model->decrementLarge();
                }
                break;
            }
            return out;
        }

        template<typename T>
        inline INumericEdit<T>::~INumericEdit()
        {}

        template<typename T>
        inline INumericSlider<T>::~INumericSlider()
        {}

        template<typename T>
        inline void INumericSlider<T>::_pointerMove(float pos, std::chrono::milliseconds delay)
        {
            _value = _posToValue(pos);
            if (INumericWidget<T>::_model)
            {
                if (std::chrono::milliseconds(0) == delay)
                {
                    INumericWidget<T>::_model->setValue(_value);
                    INumericWidget<T>::_doCallback();
                }
            }
        }

        template<typename T>
        inline void INumericSlider<T>::_buttonPress(float pos, std::chrono::milliseconds delay)
        {
            _value = _posToValue(pos);
            if (INumericWidget<T>::_model)
            {
                if (std::chrono::milliseconds(0) == delay)
                {
                    INumericWidget<T>::_model->setValue(_value);
                    INumericWidget<T>::_doCallback();
                }
            }
        }

        template<typename T>
        inline void INumericSlider<T>::_buttonRelease(std::chrono::milliseconds delay)
        {
            if (INumericWidget<T>::_model)
            {
                if (delay > std::chrono::milliseconds(0))
                {
                    INumericWidget<T>::_model->setValue(_value);
                    INumericWidget<T>::_doCallback();
                }
            }
        }

        template<typename T>
        inline void INumericSlider<T>::_valueUpdate()
        {
            if (INumericWidget<T>::_model)
            {
                INumericWidget<T>::_model->setValue(_value);
                INumericWidget<T>::_doCallback();
            }
        }

    } // namespace UI
} // namespace djv
