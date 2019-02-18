//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUIComponents/ActionWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ListButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ActionWidget::Private
        {
            std::shared_ptr<ListButton> button;
            std::shared_ptr<ValueObserver<ButtonType> > buttonTypeObserver;
            std::shared_ptr<ValueObserver<bool> > checkedObserver;
            std::shared_ptr<ValueObserver<std::string> > iconObserver;
            std::shared_ptr<ValueObserver<std::string> > textObserver;
            std::shared_ptr<ValueObserver<bool> > enabledObserver;
        };
        
        void ActionWidget::_init(const std::shared_ptr<Action> & action, Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            addAction(action);

            p.button = ListButton::create(context);
            p.button->setParent(shared_from_this());

            auto weak = std::weak_ptr<ActionWidget>(std::dynamic_pointer_cast<ActionWidget>(shared_from_this()));
            p.button->setClickedCallback(
                [weak, action]
            {
                action->doClicked();
            });
            p.button->setCheckedCallback(
                [weak, action](bool value)
            {
                action->setChecked(value);
                action->doChecked();
            });

            p.buttonTypeObserver = ValueObserver<ButtonType>::create(
                action->observeButtonType(),
                [weak](ButtonType value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->button->setButtonType(value);
                }
            });
            p.checkedObserver = ValueObserver<bool>::create(
                action->observeChecked(),
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->button->setChecked(value);
                }
            });
            p.iconObserver = ValueObserver<std::string>::create(
                action->observeIcon(),
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->button->setIcon(value);
                }
            });
            p.textObserver = ValueObserver<std::string>::create(
                action->observeText(),
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->button->setText(value);
                }
            });
            p.enabledObserver = ValueObserver<bool>::create(
                action->observeEnabled(),
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->button->setEnabled(value);
                }
            });
        }

        ActionWidget::ActionWidget() :
            _p(new Private)
        {}

        ActionWidget::~ActionWidget()
        {}

        std::shared_ptr<ActionWidget> ActionWidget::create(const std::shared_ptr<Action> & action, Context * context)
        {
            auto out = std::shared_ptr<ActionWidget>(new ActionWidget);
            out->_init(action, context);
            return out;
        }

        void ActionWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->button->getMinimumSize());
        }

        void ActionWidget::_layoutEvent(Event::Layout&)
        {
            _p->button->setGeometry(getGeometry());
        }
        
    } // namespace UI
} // namespace djv

