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

#include <djvViewLib/SettingsDialog.h>

#include <djvViewLib/IViewSystem.h>

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/GroupBox.h>
#include <djvUI/FlatButton.h>
#include <djvUI/FlowLayout.h>
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
                    std::map<std::string, std::shared_ptr<UI::FlatButton> > buttons;
                };
                std::map<std::string, Group> _groups;
                std::shared_ptr<UI::VerticalLayout> _layout;
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
                            auto button = UI::FlatButton::create(context);
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
                _layout->setMargin(UI::MetricsRole::MarginLarge);
                _layout->setSpacing(UI::MetricsRole::SpacingLarge);
                for (const auto & i : _groups)
                {
                    _layout->addWidget(i.second.groupBox);
                    auto flowLayout = UI::FlowLayout::create(context);
                    for (const auto & j : i.second.buttons)
                    {
                        flowLayout->addWidget(j.second);
                    }
                    i.second.groupBox->addWidget(flowLayout);
                }
                _layout->setParent(shared_from_this());
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
                return _layout->getHeightForWidth(value);
            }

            void MainWidget::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void MainWidget::_layoutEvent(Event::Layout &)
            {
                _layout->setGeometry(getGeometry());
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

		struct SettingsDialog::Private
		{
            std::shared_ptr<MainWidget> mainWidget;
            std::shared_ptr<UI::SoloLayout> soloLayout;
            std::map<std::shared_ptr<UI::Widget>, std::string> titles;
		};

		void SettingsDialog::_init(Context * context)
		{
			IDialog::_init(context);

			DJV_PRIVATE_PTR();

            //auto backButton = UI::FlatButton::create(context);
            //backButton->setIcon("djvIconArrowLeft");
            //backButton->hide();
            //addBackButton(backButton);

            p.mainWidget = MainWidget::create(context);
            p.titles[p.mainWidget] = DJV_TEXT("Settings");

			p.soloLayout = UI::SoloLayout::create(context);
            p.soloLayout->addWidget(p.mainWidget);
            auto weak = std::weak_ptr<SettingsDialog>(std::dynamic_pointer_cast<SettingsDialog>(shared_from_this()));
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

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addWidget(p.soloLayout);
            addWidget(scrollWidget, UI::RowStretch::Expand);

            /*p.mainWidget->setWidgetCallback(
                [weak, backButton](const std::shared_ptr<UI::ISettingsWidget> & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->setCurrentWidget(value);
                    widget->_textUpdate();
                    backButton->show();
                }
            });

            backButton->setClickedCallback(
                [weak, backButton]
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->setCurrentWidget(widget->_p->mainWidget);
                    widget->_textUpdate();
                    backButton->hide();
                }
            });*/
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

        void SettingsDialog::_localeEvent(Event::Locale &)
        {
            _textUpdate();
        }

        void SettingsDialog::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto i = p.titles.find(p.soloLayout->getCurrentWidget());
            if (i != p.titles.end())
            {
                setTitle(_getText(i->second));
            }
        }

    } // namespace ViewLib
} // namespace djv

