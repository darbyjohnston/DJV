// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaWidgetPrivate.h>

#include <djvViewApp/Media.h>
#include <djvViewApp/PlaybackSettings.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/CheckBox.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/SpeedFunc.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PlaybackSpeedWidget::Private
        {
            Private(PlaybackSpeedWidget& p) :
                p(p)
            {}

            PlaybackSpeedWidget& p;

            std::shared_ptr<Media> media;
            PlaybackSpeed playbackSpeed = PlaybackSpeed::First;
            Math::Rational defaultSpeed;
            Math::Rational customSpeed = Math::Rational(1);
            Math::Rational speed;
            bool playEveryFrame = false;

            std::shared_ptr<UI::Label> titleLabel;
            std::map<PlaybackSpeed, std::shared_ptr<UI::CheckBox> > speedCheckBoxes;
            std::map<AV::FPS, std::shared_ptr<UI::ListButton> > presetSpeedButtons;
            std::shared_ptr<UI::FloatEdit> customSpeedFloatEdit;
            std::shared_ptr<UI::Label> defaultSpeedLabel;
            std::shared_ptr<UI::ButtonGroup> speedButtonGroup;
            std::shared_ptr<UI::ButtonGroup> presetSpeedButtonGroup;
            std::shared_ptr<UI::VerticalLayout> speedButtonLayout;
            std::shared_ptr<UI::CheckBox> playEveryFrameCheckBox;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;

            std::shared_ptr<ValueObserver<Math::Rational> > speedObserver;
            std::shared_ptr<ValueObserver<PlaybackSpeed> > playbackSpeedObserver;
            std::shared_ptr<ValueObserver<Math::Rational> > defaultSpeedObserver;
            std::shared_ptr<ValueObserver<Math::Rational> > customSpeedObserver;
            std::shared_ptr<ValueObserver<bool> > playEveryFrameObserver;

            void setPlaybackSpeed(PlaybackSpeed);
            void setCustomSpeed(const Math::Rational&);
        };

        void PlaybackSpeedWidget::_init(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::PlaybackSpeedWidget");

            p.media = media;

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::MetricsRole::MarginSmall);
            p.titleLabel->setBackgroundRole(UI::ColorRole::Trough);

            p.customSpeedFloatEdit = UI::FloatEdit::create(context);
            p.customSpeedFloatEdit->setRange(Math::FloatRange(.1F, 1000.F));
            p.customSpeedFloatEdit->getModel()->setSmallIncrement(1.F);
            p.customSpeedFloatEdit->getModel()->setLargeIncrement(10.F);

            p.defaultSpeedLabel = UI::Label::create(context);
            p.defaultSpeedLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.speedButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            std::vector<std::shared_ptr<UI::Button::IButton> > buttons;
            std::vector<std::shared_ptr<UI::Widget> > widgets;
            for (auto i : getPlaybackSpeedEnums())
            {
                auto checkBox = UI::CheckBox::create(context);
                buttons.push_back(checkBox);
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
                p.speedCheckBoxes[i] = checkBox;
            }
            p.speedButtonGroup->setButtons(buttons);
            p.presetSpeedButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            buttons.clear();
            for (auto i : AV::getFPSEnums())
            {
                auto button = UI::ListButton::create(context);
                buttons.push_back(button);
                widgets.push_back(button);
                p.presetSpeedButtons[i] = button;
            }
            p.presetSpeedButtonGroup->setButtons(buttons);
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

            auto contextWeak = std::weak_ptr<System::Context>(context);
            auto weak = std::weak_ptr<PlaybackSpeedWidget>(
                std::dynamic_pointer_cast<PlaybackSpeedWidget>(shared_from_this()));
            p.speedButtonGroup->setRadioCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->setPlaybackSpeed(static_cast<PlaybackSpeed>(value));
                    }
                });

            p.presetSpeedButtonGroup->setPushCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->setPlaybackSpeed(PlaybackSpeed::Custom);
                        widget->_p->setCustomSpeed(AV::fromSpeed(static_cast<AV::FPS>(value)));
                    }
                });

            p.playEveryFrameCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                if (auto media = widget->_p->media)
                                {
                                    media->setPlayEveryFrame(value);
                                }
                                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                                {
                                    playbackSettings->setPlayEveryFrame(value);
                                }
                            }
                        }
                    }
                });

            p.customSpeedFloatEdit->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        Math::Rational customSpeed;
                        if (value >= 1.F)
                        {
                            customSpeed = AV::fromSpeed(value);
                        }
                        else if (value > 0.F && value < 1.F)
                        {
                            customSpeed = Math::Rational(static_cast<int>(std::floor(value * 1000.F)), 1000);
                        }
                        widget->_p->setCustomSpeed(customSpeed);
                    }
                });

            p.speedObserver = ValueObserver<Math::Rational>::create(
                media->observeSpeed(),
                [weak](const Math::Rational& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value;
                        widget->_widgetUpdate();
                    }
                });

            p.playbackSpeedObserver = ValueObserver<PlaybackSpeed>::create(
                media->observePlaybackSpeed(),
                [weak](PlaybackSpeed value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->playbackSpeed = value;
                        widget->_widgetUpdate();
                    }
                });

            p.customSpeedObserver = ValueObserver<Math::Rational>::create(
                media->observeCustomSpeed(),
                [weak](const Math::Rational& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->customSpeed = value;
                        widget->_widgetUpdate();
                    }
                });

            p.defaultSpeedObserver = ValueObserver<Math::Rational>::create(
                media->observeDefaultSpeed(),
                [weak](const Math::Rational& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->defaultSpeed = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        PlaybackSpeedWidget::PlaybackSpeedWidget() :
            _p(new Private(*this))
        {}

        PlaybackSpeedWidget::~PlaybackSpeedWidget()
        {}

        std::shared_ptr<PlaybackSpeedWidget> PlaybackSpeedWidget::create(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<PlaybackSpeedWidget>(new PlaybackSpeedWidget);
            out->_init(media, context);
            return out;
        }

        void PlaybackSpeedWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->scrollWidget->getMinimumSize() + getMargin().getSize(style));
        }
        
        void PlaybackSpeedWidget::_layoutEvent(System::Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->scrollWidget->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void PlaybackSpeedWidget::_initEvent(System::Event::Init & event)
        {
            if (event.getData().text)
            {
                _textUpdate();
            }
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
                p.speedCheckBoxes[i]->setText(ss.str());
            }

            for (auto i : AV::getFPSEnums())
            {
                std::stringstream ss;
                ss << i;
                p.presetSpeedButtons[i]->setText(_getText(ss.str()));
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

        void PlaybackSpeedWidget::Private::setPlaybackSpeed(PlaybackSpeed value)
        {
            if (media)
            {
                media->setPlaybackSpeed(value);
            }
            if (auto context = p.getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                {
                    playbackSettings->setPlaybackSpeed(value);
                }
            }
        }

        void PlaybackSpeedWidget::Private::setCustomSpeed(const Math::Rational& value)
        {
            if (media)
            {
                media->setCustomSpeed(value);
            }
            if (auto context = p.getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
                {
                    playbackSettings->setCustomSpeed(value);
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

