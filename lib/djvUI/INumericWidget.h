// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>

#include <djvMath/INumericValueModel.h>
#include <djvMath/Range.h>

#include <djvCore/Time.h>
#include <djvCore/ValueObserver.h>

#include <chrono>

namespace djv
{
    namespace UI
    {
        //! Numeric widgets.
        namespace Numeric
        {
            //! Numeric widget keyboard shortcuts.
            enum class Key
            {
                None,
                Home,
                End,
                Up,
                Right,
                Down,
                Left,
                PageUp,
                PageDown
            };

            //! Convert a GLFW key to a numeric widget keyboard shortcut.
            Key glfwToKey(int);

            //! Base class for numeric widgets.
            template<typename T>
            class IWidget
            {
            public:
                virtual ~IWidget() = 0;

                //! \name Range
                ///@{

                const Math::Range<T>& getRange() const;

                void setRange(const Math::Range<T>&);

                ///@}

                //! \name Value
                ///@{

                T getValue() const;

                void setValue(T);
                void setValueCallback(const std::function<void(T, TextEditReason)>&);

                ///@}

                //! \name Increment
                ///@{

                T getSmallIncrement() const;
                T getLargeIncrement() const;

                void setSmallIncrement(T);
                void setLargeIncrement(T);

                ///@}

                //! \name Model
                ///@{

                const std::shared_ptr<Math::INumericValueModel<T> >& getModel() const;

                virtual void setModel(const std::shared_ptr<Math::INumericValueModel<T> >&);

                ///@}

            protected:
                void _doCallback(TextEditReason);
                virtual bool _doKeyPress(Key);
                virtual void _setIsMin(bool) {}
                virtual void _setIsMax(bool) {}

                std::shared_ptr<Math::INumericValueModel<T> > _model;

            private:
                std::shared_ptr<Core::Observer::Value<bool> > _isMinObserver;
                std::shared_ptr<Core::Observer::Value<bool> > _isMaxObserver;
                std::function<void(T, TextEditReason)> _callback;
            };

            //! Base class for numeric editor widgets.
            template<typename T>
            class IEdit : public IWidget<T>
            {
            public:
                virtual ~IEdit() = 0;

            protected:
                bool _doKeyPress(Key) override;
            };

            //! Base class for numeric slider widgets.
            template<typename T>
            class ISlider : public IWidget<T>
            {
            public:
                virtual ~ISlider() = 0;

            protected:
                void _pointerMove(float, const Core::Time::Duration&);
                void _buttonPress(float, const Core::Time::Duration&);
                void _buttonRelease(const Core::Time::Duration&);
                void _valueUpdate();

                virtual float _valueToPos(T) const = 0;
                virtual T _posToValue(float) const = 0;

                T _value = static_cast<T>(0);
            };

        } // namespace Numeric
    } // namespace UI
} // namespace djv

#include <djvUI/INumericWidgetInline.h>
