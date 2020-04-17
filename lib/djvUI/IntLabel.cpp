// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/IntLabel.h>

#include <djvUI/Label.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct IntLabel::Private
        {
            std::shared_ptr<IntValueModel> model;
            std::shared_ptr<Label> label;
            std::shared_ptr<ValueObserver<IntRange> > rangeObserver;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void IntLabel::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::IntLabel");

            p.label = Label::create(context);
            p.label->setTextHAlign(TextHAlign::Right);
            addChild(p.label);

            _textUpdate();
        }

        IntLabel::IntLabel() :
            _p(new Private)
        {}

        IntLabel::~IntLabel()
        {}

        std::shared_ptr<IntLabel> IntLabel::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<IntLabel>(new IntLabel);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<IntValueModel> & IntLabel::getModel() const
        {
            return _p->model;
        }

        void IntLabel::setModel(const std::shared_ptr<IntValueModel> & model)
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                p.valueObserver.reset();
            }
            p.model = model;
            if (p.model)
            {
                auto weak = std::weak_ptr<IntLabel>(std::dynamic_pointer_cast<IntLabel>(shared_from_this()));
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
        
        std::string IntLabel::getSizeString(const IntRange& range)
        {
            std::string out;
            const size_t digits = std::max(Math::getNumDigits(range.min), Math::getNumDigits(range.max));
            out += std::string(digits, '0');
            if (range.min < 0 || range.max < 0)
            {
                out = '-' + out;
            }
            return out;
        }

        void IntLabel::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.label->getMinimumSize() + getMargin().getSize(style));
        }

        void IntLabel::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.label->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void IntLabel::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                {
                    std::stringstream ss;
                    ss << p.model->observeValue()->get();
                    p.label->setText(ss.str());
                }
                p.label->setSizeString(getSizeString(p.model->observeRange()->get()));
            }
        }

    } // namespace UI
} // namespace djv
