// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PlaybackSpeedWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvCore/Speed.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSpeedWidget::Private
        {
            PlaybackSpeed playbackSpeed = PlaybackSpeed::First;
            Time::Speed defaultSpeed;
            Time::Speed customSpeed = Time::Speed(1);
            Time::Speed speed;
            bool playEveryFrame = false;

            std::shared_ptr<UI::Label> titleLabel;
            std::map<PlaybackSpeed, std::shared_ptr<UI::CheckBox> > checkBoxes;
            std::shared_ptr<UI::FloatEdit> customSpeedFloatEdit;
            std::shared_ptr<UI::Label> defaultSpeedLabel;
            std::shared_ptr<UI::ButtonGroup> speedButtonGroup;
            std::shared_ptr<UI::VerticalLayout> speedButtonLayout;
            std::shared_ptr<UI::CheckBox> playEveryFrameCheckBox;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
            std::function<void(PlaybackSpeed)> playbackSpeedCallback;
            std::function<void(const Time::Speed&)> customSpeedCallback;
            std::function<void(bool)> playEveryFrameCallback;
        };

        void PlaybackSpeedWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::PlaybackSpeedWidget");

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::MarginSmall);
            p.titleLabel->setBackgroundRole(UI::ColorRole::Trough);

            p.customSpeedFloatEdit = UI::FloatEdit::create(context);
            p.customSpeedFloatEdit->setRange(FloatRange(.1F, 1000.F));
            p.customSpeedFloatEdit->getModel()->setSmallIncrement(1.F);
            p.customSpeedFloatEdit->getModel()->setLargeIncrement(10.F);

            p.defaultSpeedLabel = UI::Label::create(context);
            p.defaultSpeedLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.speedButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            std::vector<std::shared_ptr<UI::Widget> > widgets;
            for (auto i : getPlaybackSpeedEnums())
            {
                auto checkBox = UI::CheckBox::create(context);
                p.speedButtonGroup->addButton(checkBox);
                switch (i)
                {
                case PlaybackSpeed::Default:
                {
                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(checkBox);
                    hLayout->setStretch(checkBox, UI::RowStretch::Expand);
                    hLayout->addChild(p.defaultSpeedLabel);
                    widgets.push_back(hLayout);
                    break;
                }
                case PlaybackSpeed::Custom:
                {
                    auto hLayout = UI::HorizontalLayout::create(context);
                    hLayout->setSpacing(UI::MetricsRole::None);
                    hLayout->addChild(checkBox);
                    hLayout->setStretch(checkBox, UI::RowStretch::Expand);
                    hLayout->addChild(p.customSpeedFloatEdit);
                    widgets.push_back(hLayout);
                    break;
                }
                default:
                    widgets.push_back(checkBox);
                    break;
                }
                p.checkBoxes[i] = checkBox;
            }
            p.speedButtonLayout = UI::VerticalLayout::create(context);
            p.speedButtonLayout->setSpacing(UI::MetricsRole::None);
            for (auto i = widgets.rbegin(); i != widgets.rend(); ++i)
            {
                p.speedButtonLayout->addChild(*i);
            }

            p.playEveryFrameCheckBox = UI::CheckBox::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(p.titleLabel);
            layout->addSeparator();

            layout->addChild(p.speedButtonLayout);
            layout->addSeparator();
            layout->addChild(p.playEveryFrameCheckBox);

            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.scrollWidget->setMinimumSizeRole(UI::MetricsRole::None);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->addChild(layout);
            addChild(p.scrollWidget);

            _textUpdate();
            _widgetUpdate();

            auto contextWeak = std::weak_ptr<Context>(context);
            auto weak = std::weak_ptr<PlaybackSpeedWidget>(
                std::dynamic_pointer_cast<PlaybackSpeedWidget>(shared_from_this()));
            p.speedButtonGroup->setPushCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->playbackSpeedCallback)
                        {
                            widget->_p->playbackSpeedCallback(static_cast<PlaybackSpeed>(value));
                        }
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

            p.customSpeedFloatEdit->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        Time::Speed customSpeed;
                        if (value >= 1.F) 
                        {
                            customSpeed = Time::Speed(value);
                        }
                        else if (value > 0.F && value < 1.F)
                        {
                            customSpeed = Time::Speed(static_cast<int>(std::floor(value * 1000.F)), 1000);
                        }
                        if (widget->_p->customSpeedCallback)
                        {
                            widget->_p->customSpeedCallback(customSpeed);
                        }
                    }
                });
        }

        PlaybackSpeedWidget::PlaybackSpeedWidget() :
            _p(new Private)
        {}

        PlaybackSpeedWidget::~PlaybackSpeedWidget()
        {}

        std::shared_ptr<PlaybackSpeedWidget> PlaybackSpeedWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSpeedWidget>(new PlaybackSpeedWidget);
            out->_init(context);
            return out;
        }

        void PlaybackSpeedWidget::setPlaybackSpeed(PlaybackSpeed value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.playbackSpeed)
                return;
            p.playbackSpeed = value;
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

        void PlaybackSpeedWidget::setCustomSpeed(const Time::Speed& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.customSpeed)
                return;
            p.customSpeed = value;
            _widgetUpdate();
        }

        void PlaybackSpeedWidget::setSpeed(const Time::Speed& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.speed)
                return;
            p.speed = value;
            _textUpdate();
        }

        void PlaybackSpeedWidget::setPlayEveryFrame(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.playEveryFrame)
                return;
            p.playEveryFrame = value;
            _widgetUpdate();
        }

        void PlaybackSpeedWidget::setPlaybackSpeedCallback(const std::function<void(PlaybackSpeed)>& value)
        {
            _p->playbackSpeedCallback = value;
        }

        void PlaybackSpeedWidget::setCustomSpeedCallback(const std::function<void(const Time::Speed&)>& value)
        {
            _p->customSpeedCallback = value;
        }

        void PlaybackSpeedWidget::setPlayEveryFrameCallback(const std::function<void(bool)>& value)
        {
            _p->playEveryFrameCallback = value;
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
            _textUpdate();
        }

        void PlaybackSpeedWidget::_textUpdate()
        {
            DJV_PRIVATE_PTR();

            p.titleLabel->setText(_getText(DJV_TEXT("playback_speed")));

            for (auto i : getPlaybackSpeedEnums())
            {
                std::stringstream ss;
                switch (i)
                {
                case PlaybackSpeed::Default:
                case PlaybackSpeed::Custom:
                {
                    std::stringstream ss2;
                    ss2 << i;
                    ss << _getText(ss2.str()) << ": ";
                    break;
                }
                default:
                {
                    std::stringstream ss2;
                    ss2 << i;
                    ss << _getText(ss2.str());
                    break;
                }
                }
                p.checkBoxes[i]->setText(ss.str());
            }

            {
                std::stringstream ss;
                ss.precision(2);
                ss << std::fixed << p.defaultSpeed.toFloat();
                p.defaultSpeedLabel->setText(ss.str());
            }

            p.playEveryFrameCheckBox->setText(_getText(DJV_TEXT("playback_play_every_frame")));
            p.playEveryFrameCheckBox->setTooltip(_getText(DJV_TEXT("playback_play_every_frame_tooltip")));
        }

        void PlaybackSpeedWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.customSpeedFloatEdit->setValue(p.customSpeed.toFloat());
            p.playEveryFrameCheckBox->setChecked(p.playEveryFrame);
            p.speedButtonGroup->setChecked(static_cast<int>(p.playbackSpeed));
        }

    } // namespace ViewApp
} // namespace djv

