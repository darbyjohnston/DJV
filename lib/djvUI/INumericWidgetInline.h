// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

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
        inline void INumericWidget<T>::setValueCallback(const std::function<void(T, TextEditReason)>& callback)
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
            _isMinObserver = Core::ValueObserver<bool>::create(
                _model->observeIsMin(),
                [this](bool value)
                {
                    _setIsMin(value);
                });
            _isMaxObserver = Core::ValueObserver<bool>::create(
                _model->observeIsMax(),
                [this](bool value)
                {
                    _setIsMax(value);
                });
        }

        template<typename T>
        inline void INumericWidget<T>::_doCallback(TextEditReason reason)
        {
            if (_callback)
            {
                _callback(_model->observeValue()->get(), reason);
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
                    _doCallback(TextEditReason::Accepted);
                }
                break;
            case NumericWidgetKey::End:
                if (_model)
                {
                    out = true;
                    _model->setMax();
                    _doCallback(TextEditReason::Accepted);
                }
                break;
            case NumericWidgetKey::Up:
            case NumericWidgetKey::Right:
                if (_model)
                {
                    out = true;
                    _model->incrementSmall();
                    _doCallback(TextEditReason::Accepted);
                }
                break;
            case NumericWidgetKey::Down:
            case NumericWidgetKey::Left:
                if (_model)
                {
                    out = true;
                    _model->decrementSmall();
                    _doCallback(TextEditReason::Accepted);
                }
                break;
            case NumericWidgetKey::PageUp:
                if (_model)
                {
                    out = true;
                    _model->incrementLarge();
                    _doCallback(TextEditReason::Accepted);
                }
                break;
            case NumericWidgetKey::PageDown:
                if (_model)
                {
                    out = true;
                    _model->decrementLarge();
                    _doCallback(TextEditReason::Accepted);
                }
                break;
            default: break;
            }
            return out;
        }

        template<typename T>
        inline INumericEdit<T>::~INumericEdit()
        {}
        
        template<typename T>
        inline bool INumericEdit<T>::_keyPress(NumericWidgetKey key)
        {
            bool out = false;
            switch (key)
            {
            case NumericWidgetKey::Home:
            case NumericWidgetKey::End:
            case NumericWidgetKey::Up:
            case NumericWidgetKey::Down:
            case NumericWidgetKey::PageUp:
            case NumericWidgetKey::PageDown:
                out = INumericWidget<T>::_keyPress(key);
                break;
            default: break;
            }
            return out;
        }

        template<typename T>
        inline INumericSlider<T>::~INumericSlider()
        {}

        template<typename T>
        inline void INumericSlider<T>::_pointerMove(float pos, const Core::Time::Duration& delay)
        {
            _value = _posToValue(pos);
            if (INumericWidget<T>::_model)
            {
                if (Core::Time::Duration::zero() == delay)
                {
                    INumericWidget<T>::_model->setValue(_value);
                    INumericWidget<T>::_doCallback(TextEditReason::Accepted);
                }
            }
        }

        template<typename T>
        inline void INumericSlider<T>::_buttonPress(float pos, const Core::Time::Duration& delay)
        {
            _value = _posToValue(pos);
            if (INumericWidget<T>::_model)
            {
                if (Core::Time::Duration::zero() == delay)
                {
                    INumericWidget<T>::_model->setValue(_value);
                    INumericWidget<T>::_doCallback(TextEditReason::Accepted);
                }
            }
        }

        template<typename T>
        inline void INumericSlider<T>::_buttonRelease(const Core::Time::Duration& delay)
        {
            if (INumericWidget<T>::_model)
            {
                if (delay > Core::Time::Duration::zero())
                {
                    INumericWidget<T>::_model->setValue(_value);
                    INumericWidget<T>::_doCallback(TextEditReason::Accepted);
                }
            }
        }

        template<typename T>
        inline void INumericSlider<T>::_valueUpdate()
        {
            if (INumericWidget<T>::_model)
            {
                INumericWidget<T>::_model->setValue(_value);
                INumericWidget<T>::_doCallback(TextEditReason::Accepted);
            }
        }

    } // namespace UI
} // namespace djv
