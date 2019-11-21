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

#include <djvViewApp/PlaybackSpeedWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/LineEdit.h>

#include <djvCore/Speed.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSpeedWidget::Private
        {
            std::vector<Time::Speed> speeds;
            Time::Speed speed;
            Time::Speed defaultSpeed;
            Time::Speed customSpeed = Time::Speed(1);
            bool playEveryFrame;

            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::ButtonGroup> speedButtonGroup;
            std::shared_ptr<UI::VerticalLayout> speedButtonLayout;
            std::shared_ptr<UI::ListButton> defaultSpeedButton;
            std::shared_ptr<UI::CheckBox> playEveryFrameCheckBox;
            std::shared_ptr<UI::ListButton> useCustomSpeedButton;
            std::shared_ptr<UI::LineEdit> customSpeedLineEdit;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
            std::function<void(const Core::Time::Speed&)> speedCallback;
            std::function<void(bool)> playEveryFrameCallback;
            std::function<void(bool)> useCustomSpeedCallback;
            std::function<void(const Core::Time::Speed&)> setCustomSpeedCallback;

        };

        void PlaybackSpeedWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::PlaybackSpeedWidget");

            p.speeds =
            {
                Time::Speed(240),
                Time::Speed(120),
                Time::Speed(60),
                Time::Speed(60000, 1001),
                Time::Speed(50),
                Time::Speed(48),
                Time::Speed(30),
                Time::Speed(30000, 1001),
                Time::Speed(25),
                Time::Speed(24),
                Time::Speed(24000, 1001),
                Time::Speed(16),
                Time::Speed(12),
                Time::Speed(8),
                Time::Speed(6)
            };

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.titleLabel->setBackgroundRole(UI::ColorRole::Trough);

            p.speedButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            p.speedButtonLayout = UI::VerticalLayout::create(context);
            p.speedButtonLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));

            p.defaultSpeedButton = UI::ListButton::create(context);

            p.playEveryFrameCheckBox = UI::CheckBox::create(context);

            p.useCustomSpeedButton = UI::ListButton::create(context);
            p.customSpeedLineEdit = UI::LineEdit::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            layout->addChild(p.titleLabel);
            layout->addSeparator();

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            vLayout->addChild(p.defaultSpeedButton);
            vLayout->addChild(p.playEveryFrameCheckBox);
            vLayout->addChild(p.speedButtonLayout);
            layout->addSeparator();
            vLayout->addChild(p.useCustomSpeedButton);
            vLayout->addChild(p.customSpeedLineEdit);

            layout->addChild(vLayout);
            _widgetUpdate();

            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.scrollWidget->setMinimumSizeRole(UI::MetricsRole::Menu);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->addChild(layout);
            addChild(p.scrollWidget);

            auto contextWeak = std::weak_ptr<Context>(context);

            auto weak = std::weak_ptr<PlaybackSpeedWidget>(
                std::dynamic_pointer_cast<PlaybackSpeedWidget>(shared_from_this()));

            p.speedButtonGroup->setPushCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_doSpeedCallback(widget->_p->speeds[value]);
                    }
                });

            p.defaultSpeedButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_doSpeedCallback(widget->_p->defaultSpeed);
                    }
                });

            p.playEveryFrameCheckBox->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->playEveryFrameCallback)
                        {
                            widget->_p->playEveryFrameCallback(value);
                        }
                    }
                });

            p.useCustomSpeedButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_doSpeedCallback(widget->_p->customSpeed);
                    }
                });

            p.customSpeedLineEdit->setTextEditCallback(
                [weak](const std::string& value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        {
                            try
                            {
                                if (std::stof(value) >= 1) 
                                {
                                    //widget->_doSpeedCallback(Time::Speed(std::stof(value)));
                                    widget->_p->customSpeed = Time::Speed(std::stof(value));
                                }
                                else if (std::stof(value) > 0.F && std::stof(value) < 1.F)
                                {
                                    //widget->_doSpeedCallback(Time::Speed(std::floor(std::stof(value) * 1000), 1000));
                                    widget->_p->customSpeed = Time::Speed(std::floor(std::stof(value) * 1000), 1000);
                                }
                            }
                            catch (const std::exception&)
                            {
                                std::stringstream ss;
                                ss << "Cannot parse the value.";
                                widget->_log(ss.str(), LogLevel::Error);
                            }
                        }
                        widget->_doSpeedCallback(widget->_p->customSpeed);
                        widget->_widgetUpdate();
                    }
                });
        }

        PlaybackSpeedWidget::PlaybackSpeedWidget() :
            _p(new Private)
        {}

        PlaybackSpeedWidget::~PlaybackSpeedWidget()
        {}

        std::shared_ptr<PlaybackSpeedWidget> PlaybackSpeedWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSpeedWidget>(new PlaybackSpeedWidget);
            out->_init(context);
            return out;
        }

        void PlaybackSpeedWidget::setSpeed(const Time::Speed& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.speed)
                return;
            p.speed = value;
            _widgetUpdate();
        }

        void PlaybackSpeedWidget::setDefaultSpeed(const Time::Speed& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.defaultSpeed)
                return;
            p.defaultSpeed = value;
            _widgetUpdate();
        }

        void PlaybackSpeedWidget::setPlayEveryFrame(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.playEveryFrame)
                return;
            p.playEveryFrame = value;
            _widgetUpdate();
        }

        void PlaybackSpeedWidget::setSpeedCallback(const std::function<void(const Time::Speed&)>& value)
        {
            _p->speedCallback = value;
        }

        void PlaybackSpeedWidget::setPlayEveryFrameCallback(const std::function<void(bool)>& value)
        {
            _p->playEveryFrameCallback = value;
        }

        void PlaybackSpeedWidget::setUseCustomSpeedCallback(const std::function<void(bool)>& value)
        {
            _p->useCustomSpeedCallback = value;
        }

        void PlaybackSpeedWidget::setCustomSpeedCallback(const std::function<void(const Time::Speed&)>& value)
        {
            _p->speedCallback = value;
        }

        void PlaybackSpeedWidget::_preLayoutEvent(Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->scrollWidget->getMinimumSize() + getMargin().getSize(style));
        }
        
        void PlaybackSpeedWidget::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->scrollWidget->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void PlaybackSpeedWidget::_initEvent(Event::Init & event)
        {
            Widget::_initEvent(event);
            _widgetUpdate();
        }

        void PlaybackSpeedWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.titleLabel->setText(_getText(DJV_TEXT("Playback Speed")));

                p.speedButtonGroup->clearButtons();
                p.speedButtonLayout->clearChildren();
                for (const auto& i : p.speeds)
                {
                    auto button = UI::ListButton::create(context);
                    std::stringstream ss;
                    ss.precision(2);
                    ss << std::fixed << i.toFloat();
                    button->setText(ss.str());
                    p.speedButtonGroup->addButton(button);
                    p.speedButtonLayout->addChild(button);
                }

                p.defaultSpeedButton->setEnabled(p.speed != p.defaultSpeed);
                std::stringstream ss;
                ss << DJV_TEXT("Reset speed") << " (";
                ss.precision(2);
                ss << std::fixed << p.defaultSpeed.toFloat() << ")";
                p.defaultSpeedButton->setText(ss.str());
                p.defaultSpeedButton->setTooltip(_getText(DJV_TEXT("Reset speed tooltip")));

                p.playEveryFrameCheckBox->setChecked(p.playEveryFrame);
                p.playEveryFrameCheckBox->setText(DJV_TEXT("Play every frame"));
                p.playEveryFrameCheckBox->setTooltip(_getText(DJV_TEXT("Play every frame tooltip")));

                p.useCustomSpeedButton->setText(DJV_TEXT("Custom playback speed"));
                p.useCustomSpeedButton->setTooltip(_getText(DJV_TEXT("Custom playback speed tooltip")));
                std::stringstream cs;
                cs.precision(2);
                cs << std::fixed << p.customSpeed.toFloat();
                p.customSpeedLineEdit->setText(cs.str());
            }
        }

        void PlaybackSpeedWidget::_doSpeedCallback(const Time::Speed& value)
        {
            DJV_PRIVATE_PTR();
            if (p.speedCallback)
            {
                p.speedCallback(value);
            }
        }

    } // namespace ViewApp
} // namespace djv

