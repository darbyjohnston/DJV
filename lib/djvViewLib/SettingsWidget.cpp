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

#include <djvViewLib/SettingsWidget.h>

#include <djvViewLib/IViewSystem.h>

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/FlatButton.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/IButton.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            class Button : public UI::Button::IButton
            {
                DJV_NON_COPYABLE(Button);

            protected:
                void _init(Context *);
                Button();

            public:
                virtual ~Button();

                static std::shared_ptr<Button> create(Context *);

                void setText(const std::string &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

            private:
                std::shared_ptr<UI::Label> _label;
                std::shared_ptr<UI::Icon> _icon;
                std::shared_ptr<UI::HorizontalLayout> _layout;
            };

            void Button::_init(Context * context)
            {
                Widget::_init(context);

                _label = UI::Label::create(context);
                _label->setTextHAlign(UI::TextHAlign::Left);
                
                _icon = UI::Icon::create(context);
                _icon->setIcon("djvIconArrowSmallRight");
                _icon->setIconSizeRole(UI::MetricsRole::IconSmall);
                _icon->setVAlign(UI::VAlign::Center);

                _layout = UI::HorizontalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::MarginSmall);
                _layout->setSpacing(UI::MetricsRole::SpacingSmall);
                _layout->addWidget(_label, UI::RowStretch::Expand);
                _layout->addWidget(_icon);
                _layout->setParent(shared_from_this());
            }

            Button::Button()
            {}

            Button::~Button()
            {}

            std::shared_ptr<Button> Button::create(Context * context)
            {
                auto out = std::shared_ptr<Button>(new Button);
                out->_init(context);
                return out;
            }

            void Button::setText(const std::string & value)
            {
                _label->setText(value);
            }

            float Button::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void Button::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void Button::_layoutEvent(Event::Layout &)
            {
                _layout->setGeometry(getGeometry());
            }

            class MainWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(MainWidget);

            protected:
                void _init(Context *);
                MainWidget();

            public:
                virtual ~MainWidget();

                static std::shared_ptr<MainWidget> create(Context *);

                void setWidgetCallback(const std::function<void(const std::shared_ptr<UI::ISettingsWidget> &)> &);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(Event::PreLayout &) override;
                void _layoutEvent(Event::Layout &) override;

                void _localeEvent(Event::Locale &) override;

            private:
                struct Group
                {
                    std::string name;
                    std::shared_ptr<UI::GroupBox> groupBox;
                    std::map<std::string, std::shared_ptr<Button> > buttons;
                };
                std::map<std::string, Group> _groups;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<UI::ScrollWidget> _scrollWidget;
                std::function<void(const std::shared_ptr<UI::ISettingsWidget> &)> _widgetCallback;
            };

            void MainWidget::_init(Context * context)
            {
                Widget::_init(context);

                auto weak = std::weak_ptr<MainWidget>(std::dynamic_pointer_cast<MainWidget>(shared_from_this()));
                for (auto i : context->getSystemsT<IViewSystem>())
                {
                    if (auto system = i.lock())
                    {
                        for (auto widget : system->getSettingsWidgets())
                        {
                            auto button = Button::create(context);
                            button->setClickedCallback(
                                [weak, widget]
                            {
                                if (auto mainWidget = weak.lock())
                                {
                                    if (mainWidget->_widgetCallback)
                                    {
                                        mainWidget->_widgetCallback(widget);
                                    }
                                }
                            });

                            const auto & sortKey = widget->getGroupSortKey();
                            const auto j = _groups.find(sortKey);
                            if (j == _groups.end())
                            {
                                Group group;
                                group.name = widget->getGroup();
                                group.groupBox = UI::GroupBox::create(context);
                                group.buttons[widget->getName()] = button;
                                _groups[sortKey] = group;
                            }
                            else
                            {
                                j->second.buttons[widget->getName()] = button;
                            }
                        }
                    }
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setSpacing(UI::MetricsRole::None);
                for (auto i = _groups.begin(); i != _groups.end(); ++i)
                {
                    _layout->addWidget(i->second.groupBox);
                    auto vLayout = UI::VerticalLayout::create(context);
                    vLayout->setSpacing(UI::MetricsRole::None);
                    for (const auto & j : i->second.buttons)
                    {
                        vLayout->addWidget(j.second);
                    }
                    i->second.groupBox->addWidget(vLayout);
                    _layout->addSeparator();
                }

                _scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                _scrollWidget->setBorder(false);
                _scrollWidget->addWidget(_layout);
                _scrollWidget->setParent(shared_from_this());
            }

            MainWidget::MainWidget()
            {}

            MainWidget::~MainWidget()
            {}

            std::shared_ptr<MainWidget> MainWidget::create(Context * context)
            {
                auto out = std::shared_ptr<MainWidget>(new MainWidget);
                out->_init(context);
                return out;
            }

            void MainWidget::setWidgetCallback(const std::function<void(const std::shared_ptr<UI::ISettingsWidget> &)> & value)
            {
                _widgetCallback = value;
            }

            float MainWidget::getHeightForWidth(float value) const
            {
                return _scrollWidget->getHeightForWidth(value);
            }

            void MainWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_scrollWidget->getMinimumSize());
            }

            void MainWidget::_layoutEvent(Event::Layout &)
            {
                _scrollWidget->setGeometry(getGeometry());
            }

            void MainWidget::_localeEvent(Event::Locale & event)
            {
                for (auto & i : _groups)
                {
                    i.second.groupBox->setText(_getText(i.second.name));
                    for (auto & j : i.second.buttons)
                    {
                        j.second->setText(_getText(j.first));
                    }
                }
            }

        } // namespace

		struct SettingsWidget::Private
		{
            std::shared_ptr<UI::Label> titleLabel;
            std::map<std::shared_ptr<UI::Widget>, std::string> titles;
            std::shared_ptr<UI::FlatButton> backButton;
            std::shared_ptr<MainWidget> mainWidget;
            std::shared_ptr<UI::SoloLayout> soloLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
		};

		void SettingsWidget::_init(Context * context)
		{
            Widget::_init(context);

			DJV_PRIVATE_PTR();

            setBackgroundRole(UI::ColorRole::Background);
            setPointerEnabled(true);

            p.backButton = UI::FlatButton::create(context);
            p.backButton->setIcon("djvIconArrowLeft");
            p.backButton->hide();

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::FontHeader);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);

            p.mainWidget = MainWidget::create(context);
            p.titles[p.mainWidget] = DJV_TEXT("Settings");

			p.soloLayout = UI::SoloLayout::create(context);
            p.soloLayout->addWidget(p.mainWidget);
            auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
            for (auto i : context->getSystemsT<IViewSystem>())
            {
                if (auto system = i.lock())
                {
                    for (auto widget : system->getSettingsWidgets())
                    {
                        p.soloLayout->addWidget(widget);
                        p.titles[widget] = widget->getName();
                    }
                }
            }

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addWidget(p.backButton);
            hLayout->addWidget(p.titleLabel);
            p.layout->addWidget(hLayout);
            p.layout->addSeparator();
            p.layout->addWidget(p.soloLayout, UI::RowStretch::Expand);
            p.layout->setParent(shared_from_this());

            p.mainWidget->setWidgetCallback(
                [weak](const std::shared_ptr<UI::ISettingsWidget> & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->setCurrentWidget(value);
                    widget->_textUpdate();
                    widget->_p->backButton->show();
                }
            });

            p.backButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->setCurrentWidget(widget->_p->mainWidget);
                    widget->_textUpdate();
                    widget->_p->backButton->hide();
                }
            });
        }

		SettingsWidget::SettingsWidget() :
			_p(new Private)
		{}

		SettingsWidget::~SettingsWidget()
		{}

		std::shared_ptr<SettingsWidget> SettingsWidget::create(Context * context)
		{
			auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
			out->_init(context);
			return out;
		}
        void SettingsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void SettingsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void SettingsWidget::_localeEvent(Event::Locale &)
        {
            _textUpdate();
        }

        void SettingsWidget::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto i = p.titles.find(p.soloLayout->getCurrentWidget());
            if (i != p.titles.end())
            {
                p.titleLabel->setText(_getText(i->second));
            }
        }

    } // namespace ViewLib
} // namespace djv

