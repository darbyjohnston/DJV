// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace UI
    {
        namespace Numeric
        {
            template<typename T>
            inline IWidget<T>::~IWidget()
            {}

            template<typename T>
            inline const Math::Range<T>& IWidget<T>::getRange() const
            {
                return _model->observeRange()->get();
            }

            template<typename T>
            inline void IWidget<T>::setRange(const Math::Range<T>& value)
            {
                _model->setRange(value);
            }

            template<typename T>
            inline T IWidget<T>::getValue() const
            {
                return _model->observeValue()->get();
            }

            template<typename T>
            inline void IWidget<T>::setValue(T value)
            {
                _model->setValue(value);
            }

            template<typename T>
            inline void IWidget<T>::setValueCallback(const std::function<void(T, TextEditReason)>& callback)
            {
                _callback = callback;
            }

            template<typename T>
            inline T IWidget<T>::getSmallIncrement() const
            {
                return _model->observeSmallIncrement()->get();
            }

            template<typename T>
            inline T IWidget<T>::getLargeIncrement() const
            {
                return _model->observeLargeIncrement()->get();
            }

            template<typename T>
            inline void IWidget<T>::setSmallIncrement(T value)
            {
                _model->setSmallIncrement(value);
            }

            template<typename T>
            inline void IWidget<T>::setLargeIncrement(T value)
            {
                _model->setLargeIncrement(value);
            }

            template<typename T>
            inline const std::shared_ptr<Math::INumericValueModel<T> >& IWidget<T>::getModel() const
            {
                return _model;
            }

            template<typename T>
            inline void IWidget<T>::setModel(const std::shared_ptr<Math::INumericValueModel<T> >& value)
            {
                _model = value;
                _isMinObserver = Core::Observer::Value<bool>::create(
                    _model->observeIsMin(),
                    [this](bool value)
                    {
                        _setIsMin(value);
                    });
                _isMaxObserver = Core::Observer::Value<bool>::create(
                    _model->observeIsMax(),
                    [this](bool value)
                    {
                        _setIsMax(value);
                    });
            }

            template<typename T>
            inline void IWidget<T>::_doCallback(TextEditReason reason)
            {
                if (_callback)
                {
                    _callback(_model->observeValue()->get(), reason);
                }
            }

            template<typename T>
            inline bool IWidget<T>::_doKeyPress(Key key)
            {
                bool out = false;
                switch (key)
                {
                case Key::Home:
                    if (_model)
                    {
                        out = true;
                        _model->setMin();
                        _doCallback(TextEditReason::Accepted);
                    }
                    break;
                case Key::End:
                    if (_model)
                    {
                        out = true;
                        _model->setMax();
                        _doCallback(TextEditReason::Accepted);
                    }
                    break;
                case Key::Up:
                case Key::Right:
                    if (_model)
                    {
                        out = true;
                        _model->incrementSmall();
                        _doCallback(TextEditReason::Accepted);
                    }
                    break;
                case Key::Down:
                case Key::Left:
                    if (_model)
                    {
                        out = true;
                        _model->decrementSmall();
                        _doCallback(TextEditReason::Accepted);
                    }
                    break;
                case Key::PageUp:
                    if (_model)
                    {
                        out = true;
                        _model->incrementLarge();
                        _doCallback(TextEditReason::Accepted);
                    }
                    break;
                case Key::PageDown:
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
            inline IEdit<T>::~IEdit()
            {}

            template<typename T>
            inline bool IEdit<T>::_doKeyPress(Key key)
            {
                bool out = false;
                switch (key)
                {
                case Key::Home:
                case Key::End:
                case Key::Up:
                case Key::Down:
                case Key::PageUp:
                case Key::PageDown:
                    out = IWidget<T>::_doKeyPress(key);
                    break;
                default: break;
                }
                return out;
            }

            template<typename T>
            inline ISlider<T>::~ISlider()
            {}

            template<typename T>
            inline void ISlider<T>::_pointerMove(float pos, const Core::Time::Duration& delay)
            {
                _value = _posToValue(pos);
                if (IWidget<T>::_model)
                {
                    if (Core::Time::Duration::zero() == delay)
                    {
                        IWidget<T>::_model->setValue(_value);
                        IWidget<T>::_doCallback(TextEditReason::Accepted);
                    }
                }
            }

            template<typename T>
            inline void ISlider<T>::_buttonPress(float pos, const Core::Time::Duration& delay)
            {
                _value = _posToValue(pos);
                if (IWidget<T>::_model)
                {
                    if (Core::Time::Duration::zero() == delay)
                    {
                        IWidget<T>::_model->setValue(_value);
                        IWidget<T>::_doCallback(TextEditReason::Accepted);
                    }
                }
            }

            template<typename T>
            inline void ISlider<T>::_buttonRelease(const Core::Time::Duration& delay)
            {
                if (IWidget<T>::_model)
                {
                    if (delay > Core::Time::Duration::zero())
                    {
                        IWidget<T>::_model->setValue(_value);
                        IWidget<T>::_doCallback(TextEditReason::Accepted);
                    }
                }
            }

            template<typename T>
            inline void ISlider<T>::_valueUpdate()
            {
                if (IWidget<T>::_model)
                {
                    IWidget<T>::_model->setValue(_value);
                    IWidget<T>::_doCallback(TextEditReason::Accepted);
                }
            }

        } // namespace Numeric
    } // namespace UI
} // namespace djv
