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

#include <djvUI/Action.h>
#include <djvUI/LineEdit.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct IntEdit::Private
        {
            std::shared_ptr<IntValueModel> model;
            std::shared_ptr<LineEdit> lineEdit;
            std::function<void(int)> callback;
            std::shared_ptr<ValueObserver<IntRange> > rangeObserver;
            std::shared_ptr<ValueObserver<int> > valueObserver;
            std::shared_ptr<ValueObserver<bool> > incrementSmallObserver;
            std::shared_ptr<ValueObserver<bool> > incrementLargeObserver;
            std::shared_ptr<ValueObserver<bool> > decrementSmallObserver;
            std::shared_ptr<ValueObserver<bool> > decrementLargeObserver;
        };

        void IntEdit::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::IntEdit");

            auto incrementSmallAction = Action::create();
            incrementSmallAction->setShortcut(GLFW_KEY_UP);
            addAction(incrementSmallAction);
            auto incrementLargeAction = Action::create();
            incrementLargeAction->setShortcut(GLFW_KEY_PAGE_UP);
            addAction(incrementLargeAction);
            auto decrementSmallAction = Action::create();
            decrementSmallAction->setShortcut(GLFW_KEY_DOWN);
            addAction(decrementSmallAction);
            auto decrementLargeAction = Action::create();
            decrementLargeAction->setShortcut(GLFW_KEY_PAGE_DOWN);
            addAction(decrementLargeAction);

            p.lineEdit = LineEdit::create(context);
            p.lineEdit->setFont(AV::Font::familyMono);
            addChild(p.lineEdit);

            setModel(IntValueModel::create());

            auto weak = std::weak_ptr<IntEdit>(std::dynamic_pointer_cast<IntEdit>(shared_from_this()));
            p.lineEdit->setTextFinishedCallback(
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->model)
                    {
                        try
                        {
                            widget->_p->model->setValue(std::stoi(value));
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

            p.incrementSmallObserver = ValueObserver<bool>::create(
                incrementSmallAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->incrementSmall();
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(widget->_p->model->observeValue()->get());
                        }
                    }
                }
            });

            p.incrementLargeObserver = ValueObserver<bool>::create(
                incrementLargeAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->incrementLarge();
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(widget->_p->model->observeValue()->get());
                        }
                    }
                }
            });

            p.decrementSmallObserver = ValueObserver<bool>::create(
                decrementSmallAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->decrementSmall();
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(widget->_p->model->observeValue()->get());
                        }
                    }
                }
            });

            p.decrementLargeObserver = ValueObserver<bool>::create(
                decrementLargeAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->decrementLarge();
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(widget->_p->model->observeValue()->get());
                        }
                    }
                }
            });
        }

        IntEdit::IntEdit() :
            _p(new Private)
        {}

        IntEdit::~IntEdit()
        {}

        std::shared_ptr<IntEdit> IntEdit::create(Context * context)
        {
            auto out = std::shared_ptr<IntEdit>(new IntEdit);
            out->_init(context);
            return out;
        }

        IntRange IntEdit::getRange() const
        {
            return _p->model->observeRange()->get();
        }

        void IntEdit::setRange(const IntRange& value)
        {
            _p->model->setRange(value);
        }

        int IntEdit::getValue() const
        {
            return _p->model->observeValue()->get();
        }

        void IntEdit::setValue(int value)
        {
            _p->model->setValue(value);
        }

        void IntEdit::setValueCallback(const std::function<void(int)>& callback)
        {
            _p->callback = callback;
        }

        const std::shared_ptr<IntValueModel> & IntEdit::getModel() const
        {
            return _p->model;
        }

        void IntEdit::setModel(const std::shared_ptr<IntValueModel> & model)
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
                auto weak = std::weak_ptr<IntEdit>(std::dynamic_pointer_cast<IntEdit>(shared_from_this()));
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

        void IntEdit::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.lineEdit->getMinimumSize() + getMargin().getSize(style));
        }

        void IntEdit::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.lineEdit->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void IntEdit::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                {
                    std::stringstream ss;
                    ss << p.model->observeValue()->get();
                    p.lineEdit->setText(ss.str());

                    const auto & range = p.model->observeRange()->get();
                    const size_t digits = std::max(Math::getNumDigits(range.min), Math::getNumDigits(range.max));
                    std::string sizeString;
                    sizeString += std::string(digits, '0');
                    if (range.min < 0 || range.max < 0)
                    {
                        sizeString = '-' + sizeString;
                    }
                    p.lineEdit->setSizeString(sizeString);
                }
            }
        }

    } // namespace UI
} // namespace djv
