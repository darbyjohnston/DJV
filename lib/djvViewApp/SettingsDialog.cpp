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

#include <djvViewApp/SettingsDialog.h>

#include <djvViewApp/IViewSystem.h>

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlatButton.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/IButton.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/Splitter.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class ListWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ListWidget);

            protected:
                void _init(Context *);
                ListWidget();

            public:
                virtual ~ListWidget();

                static std::shared_ptr<ListWidget> create(Context *);

                void setCallback(const std::function<void(const std::shared_ptr<UI::ISettingsWidget> &)> &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                std::multimap<std::string, std::shared_ptr<UI::FlatButton> > _buttons;
                std::shared_ptr<UI::ButtonGroup> _buttonGroup;
                std::map<std::shared_ptr<UI::FlatButton>, std::shared_ptr<UI::ISettingsWidget> > _buttonToWidget;
                std::map<size_t, std::shared_ptr<UI::FlatButton> > _indexToButton;
                std::shared_ptr<UI::ScrollWidget> _scrollWidget;
                std::function<void(const std::shared_ptr<UI::ISettingsWidget> &)> _callback;
            };

            void ListWidget::_init(Context * context)
            {
                Widget::_init(context);

                _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);

                for (auto system : context->getSystemsT<IViewSystem>())
                {
                    for (auto widget : system->getSettingsWidgets())
                    {
                        auto button = UI::FlatButton::create(context);
                        _buttons.insert(std::make_pair(widget->getSortKey(), button));
                        _buttonToWidget[button] = widget;
                    }
                }
                for (const auto & button : _buttons)
                {
                    _indexToButton[_buttonGroup->getButtonCount()] = button.second;
                    _buttonGroup->addButton(button.second);
                }

                auto layout = UI::VerticalLayout::create(context);
                layout->setSpacing(UI::MetricsRole::None);
                for (const auto & button : _buttons)
                {
                    layout->addChild(button.second);
                }
                _scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                _scrollWidget->setBorder(false);
                _scrollWidget->addChild(layout);
                addChild(_scrollWidget);

                auto weak = std::weak_ptr<ListWidget>(std::dynamic_pointer_cast<ListWidget>(shared_from_this()));
                _buttonGroup->setRadioCallback(
                    [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = widget->_indexToButton.find(value);
                        if (i != widget->_indexToButton.end())
                        {
                            const auto j = widget->_buttonToWidget.find(i->second);
                            if (j != widget->_buttonToWidget.end())
                            {
                                if (widget->_callback)
                                {
                                    widget->_callback(j->second);
                                }
                            }
                        }
                    }
                });
            }

            ListWidget::ListWidget()
            {}

            ListWidget::~ListWidget()
            {}

            std::shared_ptr<ListWidget> ListWidget::create(Context * context)
            {
                auto out = std::shared_ptr<ListWidget>(new ListWidget);
                out->_init(context);
                return out;
            }

            void ListWidget::setCallback(const std::function<void(const std::shared_ptr<UI::ISettingsWidget> &)> & value)
            {
                _callback = value;
            }

            float ListWidget::getHeightForWidth(float value) const
            {
                return _scrollWidget->getHeightForWidth(value);
            }

            void ListWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_scrollWidget->getMinimumSize());
            }

            void ListWidget::_layoutEvent(Event::Layout &)
            {
                _scrollWidget->setGeometry(getGeometry());
            }

            void ListWidget::_localeEvent(Event::Locale & event)
            {
                for (const auto & i : _buttonToWidget)
                {
                    i.first->setText(_getText(i.second->getName()));
                }
            }

        } // namespace

        struct SettingsDialog::Private
        {
            std::shared_ptr<ListWidget> listWidget;
            std::shared_ptr<UI::SoloLayout> soloLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
        };

        void SettingsDialog::_init(Context * context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            p.listWidget = ListWidget::create(context);

            p.soloLayout = UI::SoloLayout::create(context);
            p.soloLayout->addChild(p.listWidget);
            auto weak = std::weak_ptr<SettingsDialog>(std::dynamic_pointer_cast<SettingsDialog>(shared_from_this()));
            for (auto system : context->getSystemsT<IViewSystem>())
            {
                for (auto widget : system->getSettingsWidgets())
                {
                    p.soloLayout->addChild(widget);
                }
            }

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            auto splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, context);
            splitter->setHandleStyle(UI::Layout::SplitterHandleStyle::Edges);
            splitter->addChild(p.listWidget);
            splitter->addChild(p.soloLayout);
            splitter->setSplit({ .2f, 1.f });
            p.layout->addChild(splitter);
            p.layout->setStretch(splitter, UI::RowStretch::Expand);
            addChild(p.layout);
            setStretch(p.layout, UI::RowStretch::Expand);

            p.listWidget->setCallback(
                [weak](const std::shared_ptr<UI::ISettingsWidget> & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->setCurrentWidget(value);
                }
            });
        }

        SettingsDialog::SettingsDialog() :
            _p(new Private)
        {}

        SettingsDialog::~SettingsDialog()
        {}

        std::shared_ptr<SettingsDialog> SettingsDialog::create(Context * context)
        {
            auto out = std::shared_ptr<SettingsDialog>(new SettingsDialog);
            out->_init(context);
            return out;
        }

        void SettingsDialog::_localeEvent(Event::Locale & event)
        {
            IDialog::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("SETTINGS")));
        }

    } // namespace ViewApp
} // namespace djv

