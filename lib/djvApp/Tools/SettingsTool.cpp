// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/SettingsToolPrivate.h>

#include <djvApp/Models/TimeUnitsModel.h>
#include <djvApp/App.h>

#if defined(TLRENDER_USD)
#include <tlIO/USD.h>
#endif // TLRENDER_USD

#include <feather-tk/ui/Bellows.h>
#include <feather-tk/ui/CheckBox.h>
#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/DialogSystem.h>
#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/DoubleEdit.h>
#include <feather-tk/ui/FloatEdit.h>
#include <feather-tk/ui/FloatEditSlider.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/IntEdit.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/LineEdit.h>
#include <feather-tk/ui/PushButton.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>
#include <feather-tk/core/Format.h>

namespace djv
{
    namespace app
    {
        ISettingsWidget::~ISettingsWidget()
        {}

        struct AdvancedSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            std::shared_ptr<feather_tk::CheckBox> compatCheckBox;
            std::shared_ptr<feather_tk::IntEdit> audioBufferFramesEdit;
            std::shared_ptr<feather_tk::IntEdit> videoRequestsEdit;
            std::shared_ptr<feather_tk::IntEdit> audioRequestsEdit;
            std::shared_ptr<feather_tk::VerticalLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<AdvancedSettings> > settingsObserver;
        };

        void AdvancedSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::AdvancedSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.compatCheckBox = feather_tk::CheckBox::create(context);
            p.compatCheckBox->setHStretch(feather_tk::Stretch::Expanding);
            p.compatCheckBox->setTooltip("Enable workarounds for timelines that may not conform exactly to specification.");

            p.audioBufferFramesEdit = feather_tk::IntEdit::create(context);
            p.audioBufferFramesEdit->setRange(1, 1000000);
            p.audioBufferFramesEdit->setStep(256);
            p.audioBufferFramesEdit->setLargeStep(1024);

            p.videoRequestsEdit = feather_tk::IntEdit::create(context);
            p.videoRequestsEdit->setRange(1, 64);

            p.audioRequestsEdit = feather_tk::IntEdit::create(context);
            p.audioRequestsEdit->setRange(1, 64);

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            auto label = feather_tk::Label::create(context, "Changes are applied to new files.", p.layout);
            auto formLayout = feather_tk::FormLayout::create(context, p.layout);
            formLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            formLayout->addRow("Compatibility:", p.compatCheckBox);
            formLayout->addRow("Audio buffer frames:", p.audioBufferFramesEdit);
            formLayout->addRow("Video requests:", p.videoRequestsEdit);
            formLayout->addRow("Audio requests:", p.audioRequestsEdit);

            p.settingsObserver = feather_tk::ValueObserver<AdvancedSettings>::create(
                p.model->observeAdvanced(),
                [this](const AdvancedSettings& value)
                {
                    FEATHER_TK_P();
                    p.compatCheckBox->setChecked(value.compat);
                    p.audioBufferFramesEdit->setValue(value.audioBufferFrameCount);
                    p.videoRequestsEdit->setValue(value.videoRequestMax);
                    p.audioRequestsEdit->setValue(value.audioRequestMax);
                });

            p.compatCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getAdvanced();
                    settings.compat = value;
                    p.model->setAdvanced(settings);
                });

            p.audioBufferFramesEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getAdvanced();
                    settings.audioBufferFrameCount = value;
                    p.model->setAdvanced(settings);
                });

            p.videoRequestsEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getAdvanced();
                    settings.videoRequestMax = value;
                    p.model->setAdvanced(settings);
                });

            p.audioRequestsEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getAdvanced();
                    settings.audioRequestMax = value;
                    p.model->setAdvanced(settings);
                });
        }

        AdvancedSettingsWidget::AdvancedSettingsWidget() :
            _p(new Private)
        {}

        AdvancedSettingsWidget::~AdvancedSettingsWidget()
        {}

        std::shared_ptr<AdvancedSettingsWidget> AdvancedSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AdvancedSettingsWidget>(new AdvancedSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void AdvancedSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void AdvancedSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct CacheSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            std::shared_ptr<feather_tk::FloatEdit> videoEdit;
            std::shared_ptr<feather_tk::FloatEdit> audioEdit;
            std::shared_ptr<feather_tk::FloatEdit> readBehindEdit;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::PlayerCacheOptions> > settingsObserver;
        };

        void CacheSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::CacheSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.videoEdit = feather_tk::FloatEdit::create(context);
            p.videoEdit->setRange(0.F, 1024.F);
            p.videoEdit->setStep(1.0);
            p.videoEdit->setLargeStep(10.0);

            p.audioEdit = feather_tk::FloatEdit::create(context);
            p.audioEdit->setRange(0.F, 1024.F);
            p.audioEdit->setStep(1.0);
            p.audioEdit->setLargeStep(10.0);

            p.readBehindEdit = feather_tk::FloatEdit::create(context);
            p.readBehindEdit->setRange(0.F, 10.F);
            p.readBehindEdit->setStep(0.1);
            p.readBehindEdit->setLargeStep(1.0);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Video cache (GB):", p.videoEdit);
            p.layout->addRow("Audio cache (GB):", p.audioEdit);
            p.layout->addRow("Read behind (seconds):", p.readBehindEdit);

            p.settingsObserver = feather_tk::ValueObserver<tl::timeline::PlayerCacheOptions>::create(
                p.model->observeCache(),
                [this](const tl::timeline::PlayerCacheOptions& value)
                {
                    FEATHER_TK_P();
                    p.videoEdit->setValue(value.videoGB);
                    p.audioEdit->setValue(value.audioGB);
                    p.readBehindEdit->setValue(value.readBehind);
                });

            p.videoEdit->setCallback(
                [this](float value)
                {
                    FEATHER_TK_P();
                    tl::timeline::PlayerCacheOptions settings = p.model->getCache();
                    settings.videoGB = value;
                    p.model->setCache(settings);
                });

            p.audioEdit->setCallback(
                [this](float value)
                {
                    FEATHER_TK_P();
                    tl::timeline::PlayerCacheOptions settings = p.model->getCache();
                    settings.audioGB = value;
                    p.model->setCache(settings);
                });

            p.readBehindEdit->setCallback(
                [this](float value)
                {
                    FEATHER_TK_P();
                    tl::timeline::PlayerCacheOptions settings = p.model->getCache();
                    settings.readBehind = value;
                    p.model->setCache(settings);
                });
        }

        CacheSettingsWidget::CacheSettingsWidget() :
            _p(new Private)
        {}

        CacheSettingsWidget::~CacheSettingsWidget()
        {}

        std::shared_ptr<CacheSettingsWidget> CacheSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<CacheSettingsWidget>(new CacheSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void CacheSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void CacheSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct FileBrowserSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            std::shared_ptr<feather_tk::CheckBox> nfdCheckBox;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<FileBrowserSettings> > settingsObserver;
        };

        void FileBrowserSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::FileBrowserSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.nfdCheckBox = feather_tk::CheckBox::create(context);
            p.nfdCheckBox->setHStretch(feather_tk::Stretch::Expanding);
#if !defined(FEATHER_TK_NFD)
            p.nfdCheckBox->setEnabled(false);
#endif // feather_tk_NFD

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Native file dialog:", p.nfdCheckBox);

            p.settingsObserver = feather_tk::ValueObserver<FileBrowserSettings>::create(
                p.model->observeFileBrowser(),
                [this](const FileBrowserSettings& value)
                {
                    FEATHER_TK_P();
                    p.nfdCheckBox->setChecked(value.nativeFileDialog);
                });

            p.nfdCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getFileBrowser();
                    settings.nativeFileDialog = value;
                    p.model->setFileBrowser(settings);
                });
        }

        FileBrowserSettingsWidget::FileBrowserSettingsWidget() :
            _p(new Private)
        {}

        FileBrowserSettingsWidget::~FileBrowserSettingsWidget()
        {}

        std::shared_ptr<FileBrowserSettingsWidget> FileBrowserSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FileBrowserSettingsWidget>(new FileBrowserSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void FileBrowserSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void FileBrowserSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct ImageSequenceSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            std::shared_ptr<feather_tk::ComboBox> audioComboBox;
            std::shared_ptr<feather_tk::LineEdit> audioExtensionsEdit;
            std::shared_ptr<feather_tk::LineEdit> audioFileNameEdit;
            std::shared_ptr<feather_tk::IntEdit> maxDigitsEdit;
            std::shared_ptr<feather_tk::DoubleEdit> defaultSpeedEdit;
            std::shared_ptr<feather_tk::IntEdit> threadsEdit;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<ImageSequenceSettings> > settingsObserver;
        };

        void ImageSequenceSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::ImageSequenceSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.audioComboBox = feather_tk::ComboBox::create(context, tl::timeline::getImageSequenceAudioLabels());
            p.audioComboBox->setHStretch(feather_tk::Stretch::Expanding);
            p.audioComboBox->setTooltip("How audio files are opened for image sequences.");

            p.audioExtensionsEdit = feather_tk::LineEdit::create(context);
            p.audioExtensionsEdit->setHStretch(feather_tk::Stretch::Expanding);
            p.audioExtensionsEdit->setTooltip(
                "List of audio file extensions to search for.\n"
                "\n"
                "Example: .wav .mp3");

            p.audioFileNameEdit = feather_tk::LineEdit::create(context);
            p.audioFileNameEdit->setHStretch(feather_tk::Stretch::Expanding);
            p.audioFileNameEdit->setTooltip("Audio file name to open.");

            p.maxDigitsEdit = feather_tk::IntEdit::create(context);
            p.maxDigitsEdit->setTooltip("Maximum number of digits allowed in a frame number.");

            p.defaultSpeedEdit = feather_tk::DoubleEdit::create(context);
            p.defaultSpeedEdit->setRange(1.0, 120.0);

            p.threadsEdit = feather_tk::IntEdit::create(context);
            p.threadsEdit->setRange(1, 64);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Audio:", p.audioComboBox);
            p.layout->addRow("Audio extensions:", p.audioExtensionsEdit);
            p.layout->addRow("Audio file name:", p.audioFileNameEdit);
            p.layout->addRow("Maximum digits:", p.maxDigitsEdit);
            p.layout->addRow("Default speed (FPS):", p.defaultSpeedEdit);
            p.layout->addRow("I/O threads:", p.threadsEdit);

            p.settingsObserver = feather_tk::ValueObserver<ImageSequenceSettings>::create(
                p.model->observeImageSequence(),
                [this](const ImageSequenceSettings& value)
                {
                    FEATHER_TK_P();
                    p.audioComboBox->setCurrentIndex(static_cast<int>(value.audio));
                    p.audioExtensionsEdit->setText(feather_tk::join(value.audioExtensions, ' '));
                    p.layout->setRowVisible(p.audioExtensionsEdit, tl::timeline::ImageSequenceAudio::Extension == value.audio);
                    p.audioFileNameEdit->setText(value.audioFileName);
                    p.layout->setRowVisible(p.audioFileNameEdit, tl::timeline::ImageSequenceAudio::FileName == value.audio);
                    p.maxDigitsEdit->setValue(value.maxDigits);
                    p.defaultSpeedEdit->setValue(value.io.defaultSpeed);
                    p.threadsEdit->setValue(value.io.threadCount);
                });

            p.audioComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    ImageSequenceSettings settings = p.model->getImageSequence();
                    settings.audio = static_cast<tl::timeline::ImageSequenceAudio>(value);
                    p.model->setImageSequence(settings);
                });

            p.audioExtensionsEdit->setTextCallback(
                [this](const std::string& value)
                {
                    FEATHER_TK_P();
                    ImageSequenceSettings settings = p.model->getImageSequence();
                    settings.audioExtensions = feather_tk::split(value, ' ');
                    p.model->setImageSequence(settings);
                });

            p.audioFileNameEdit->setTextCallback(
                [this](const std::string& value)
                {
                    FEATHER_TK_P();
                    ImageSequenceSettings settings = p.model->getImageSequence();
                    settings.audioFileName = value;
                    p.model->setImageSequence(settings);
                });

            p.maxDigitsEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    ImageSequenceSettings settings = p.model->getImageSequence();
                    settings.maxDigits = value;
                    p.model->setImageSequence(settings);
                });

            p.defaultSpeedEdit->setCallback(
                [this](double value)
                {
                    FEATHER_TK_P();
                    ImageSequenceSettings settings = p.model->getImageSequence();
                    settings.io.defaultSpeed = value;
                    p.model->setImageSequence(settings);
                });

            p.threadsEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    ImageSequenceSettings settings = p.model->getImageSequence();
                    settings.io.threadCount = value;
                    p.model->setImageSequence(settings);
                });
        }

        ImageSequenceSettingsWidget::ImageSequenceSettingsWidget() :
            _p(new Private)
        {}

        ImageSequenceSettingsWidget::~ImageSequenceSettingsWidget()
        {}

        std::shared_ptr<ImageSequenceSettingsWidget> ImageSequenceSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ImageSequenceSettingsWidget>(new ImageSequenceSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void ImageSequenceSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ImageSequenceSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct MiscSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            std::shared_ptr<feather_tk::CheckBox> tooltipsCheckBox;
            std::shared_ptr<feather_tk::CheckBox> showSetupCheckBox;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<MiscSettings> > settingsObserver;
        };

        void MiscSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::MiscSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.tooltipsCheckBox = feather_tk::CheckBox::create(context);
            p.tooltipsCheckBox->setHStretch(feather_tk::Stretch::Expanding);

            p.showSetupCheckBox = feather_tk::CheckBox::create(context);
            p.showSetupCheckBox->setHStretch(feather_tk::Stretch::Expanding);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Enable tooltips:", p.tooltipsCheckBox);
            p.layout->addRow("Show setup dialog:", p.showSetupCheckBox);

            p.settingsObserver = feather_tk::ValueObserver<MiscSettings>::create(
                p.model->observeMisc(),
                [this](const MiscSettings& value)
                {
                    FEATHER_TK_P();
                    p.tooltipsCheckBox->setChecked(value.tooltipsEnabled);
                    p.showSetupCheckBox->setChecked(value.showSetup);
                });

            p.tooltipsCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getMisc();
                    settings.tooltipsEnabled = value;
                    p.model->setMisc(settings);
                });

            p.showSetupCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getMisc();
                    settings.showSetup = value;
                    p.model->setMisc(settings);
                });
        }

        MiscSettingsWidget::MiscSettingsWidget() :
            _p(new Private)
        {}

        MiscSettingsWidget::~MiscSettingsWidget()
        {}

        std::shared_ptr<MiscSettingsWidget> MiscSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<MiscSettingsWidget>(new MiscSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void MiscSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void MiscSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct MouseSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;
            std::vector<std::string> buttonLabels;
            std::vector<feather_tk::KeyModifier> modifiers;
            std::vector<std::string> modifierLabels;
            std::shared_ptr<feather_tk::FloatEdit> wheelScaleEdit;

            std::map<MouseAction, std::shared_ptr<feather_tk::ComboBox> > buttonComboBoxes;
            std::map<MouseAction, std::shared_ptr<feather_tk::ComboBox> > modifierComboBoxes;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<MouseSettings> > settingsObserver;
        };

        void MouseSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::MouseSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            std::vector<std::string> mouseActionLabels;
            mouseActionLabels.push_back("Pan view");
            mouseActionLabels.push_back("Compare wipe");
            mouseActionLabels.push_back("Color picker");
            mouseActionLabels.push_back("Frame shuttle");

            p.buttonLabels.push_back("Left");
            p.buttonLabels.push_back("Right");
            p.buttonLabels.push_back("Middle");

            p.modifiers.push_back(feather_tk::KeyModifier::None);
            p.modifiers.push_back(feather_tk::KeyModifier::Shift);
            p.modifiers.push_back(feather_tk::KeyModifier::Control);
            p.modifiers.push_back(feather_tk::KeyModifier::Alt);
            p.modifiers.push_back(feather_tk::KeyModifier::Super);
            p.modifierLabels.push_back("None");
            p.modifierLabels.push_back(feather_tk::to_string(feather_tk::KeyModifier::Shift));
            p.modifierLabels.push_back(feather_tk::to_string(feather_tk::KeyModifier::Control));
            p.modifierLabels.push_back(feather_tk::to_string(feather_tk::KeyModifier::Alt));
            p.modifierLabels.push_back(feather_tk::to_string(feather_tk::KeyModifier::Super));

            for (const auto mouseAction : getMouseActionEnums())
            {
                p.buttonComboBoxes[mouseAction] = feather_tk::ComboBox::create(context, p.buttonLabels);
                p.buttonComboBoxes[mouseAction]->setHStretch(feather_tk::Stretch::Expanding);
                p.modifierComboBoxes[mouseAction] = feather_tk::ComboBox::create(context, p.modifierLabels);
                p.modifierComboBoxes[mouseAction]->setHStretch(feather_tk::Stretch::Expanding);
            }

            p.wheelScaleEdit = feather_tk::FloatEdit::create(context);
            p.wheelScaleEdit->setRange(1.01F, 4.F);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            for (const auto mouseAction : getMouseActionEnums())
            {
                auto hLayout = feather_tk::HorizontalLayout::create(context);
                hLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
                hLayout->setHStretch(feather_tk::Stretch::Expanding);
                p.buttonComboBoxes[mouseAction]->setParent(hLayout);
                p.modifierComboBoxes[mouseAction]->setParent(hLayout);
                p.layout->addRow(feather_tk::Format("{0}:").arg(mouseActionLabels[static_cast<size_t>(mouseAction)]), hLayout);
            }
            p.layout->addRow("Wheel scale:", p.wheelScaleEdit);

            p.settingsObserver = feather_tk::ValueObserver<MouseSettings>::create(
                p.model->observeMouse(),
                [this](const MouseSettings& value)
                {
                    FEATHER_TK_P();
                    for (const auto& i : value.bindings)
                    {
                        auto j = p.buttonComboBoxes.find(i.first);
                        if (j != p.buttonComboBoxes.end())
                        {
                            j->second->setCurrentIndex(i.second.button);
                        }
                        j = p.modifierComboBoxes.find(i.first);
                        if (j != p.modifierComboBoxes.end())
                        {
                            const auto k = std::find(p.modifiers.begin(), p.modifiers.end(), i.second.modifier);
                            if (k != p.modifiers.end())
                            {

                                j->second->setCurrentIndex(k - p.modifiers.begin());
                            }
                        }
                    }
                    p.wheelScaleEdit->setValue(value.wheelScale);
                });

            for (const auto mouseAction : getMouseActionEnums())
            {
                p.buttonComboBoxes[mouseAction]->setIndexCallback(
                    [this, mouseAction](int index)
                    {
                        FEATHER_TK_P();
                        auto settings = p.model->getMouse();
                        settings.bindings[mouseAction].button = index;
                        p.model->setMouse(settings);
                    });
                p.modifierComboBoxes[mouseAction]->setIndexCallback(
                    [this, mouseAction](int index)
                    {
                        FEATHER_TK_P();
                        if (index >= 0 && index < p.modifiers.size())
                        {
                            auto settings = p.model->getMouse();
                            settings.bindings[mouseAction].modifier = p.modifiers[index];
                            p.model->setMouse(settings);
                        }
                    });
            }

            p.wheelScaleEdit->setCallback(
                [this](float value)
                {
                    FEATHER_TK_P();
                    auto settings = p.model->getMouse();
                    settings.wheelScale = value;
                    p.model->setMouse(settings);
                });
        }

        MouseSettingsWidget::MouseSettingsWidget() :
            _p(new Private)
        {}

        MouseSettingsWidget::~MouseSettingsWidget()
        {}

        std::shared_ptr<MouseSettingsWidget> MouseSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<MouseSettingsWidget>(new MouseSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void MouseSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void MouseSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct TimeSettingsWidget::Private
        {
            std::shared_ptr<TimeUnitsModel> timeUnitsModel;

            std::shared_ptr<feather_tk::ComboBox> timeUnitsComboBox;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::TimeUnits> > timeUnitsObserver;
        };

        void TimeSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::TimeSettingsWidget", parent);
            FEATHER_TK_P();

            p.timeUnitsModel = app->getTimeUnitsModel();

            p.timeUnitsComboBox = feather_tk::ComboBox::create(context, tl::timeline::getTimeUnitsLabels());

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Time units:", p.timeUnitsComboBox);

            p.timeUnitsComboBox->setIndexCallback(
                [this](int value)
                {
                    _p->timeUnitsModel->setTimeUnits(static_cast<tl::timeline::TimeUnits>(value));
                });

            p.timeUnitsObserver = feather_tk::ValueObserver<tl::timeline::TimeUnits>::create(
                p.timeUnitsModel->observeTimeUnits(),
                [this](tl::timeline::TimeUnits value)
                {
                    _p->timeUnitsComboBox->setCurrentIndex(static_cast<int>(value));
                });

        }

        TimeSettingsWidget::TimeSettingsWidget() :
            _p(new Private)
        {}

        TimeSettingsWidget::~TimeSettingsWidget()
        {}

        std::shared_ptr<TimeSettingsWidget> TimeSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TimeSettingsWidget>(new TimeSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void TimeSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void TimeSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

#if defined(TLRENDER_FFMPEG)
        struct FFmpegSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            std::shared_ptr<feather_tk::CheckBox> yuvToRGBCheckBox;
            std::shared_ptr<feather_tk::IntEdit> threadsEdit;
            std::shared_ptr<feather_tk::VerticalLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<tl::ffmpeg::Options> > optionsObserver;
        };

        void FFmpegSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::FFmpegSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.yuvToRGBCheckBox = feather_tk::CheckBox::create(context);
            p.yuvToRGBCheckBox->setHStretch(feather_tk::Stretch::Expanding);

            p.threadsEdit = feather_tk::IntEdit::create(context);
            p.threadsEdit->setRange(0, 64);

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            auto label = feather_tk::Label::create(context, "Changes are applied to new files.", p.layout);
            auto formLayout = feather_tk::FormLayout::create(context, p.layout);
            formLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            formLayout->addRow("YUV to RGB conversion:", p.yuvToRGBCheckBox);
            formLayout->addRow("I/O threads:", p.threadsEdit);

            p.optionsObserver = feather_tk::ValueObserver<tl::ffmpeg::Options>::create(
                p.model->observeFFmpeg(),
                [this](const tl::ffmpeg::Options& value)
                {
                    FEATHER_TK_P();
                    p.yuvToRGBCheckBox->setChecked(value.yuvToRgb);
                    p.threadsEdit->setValue(value.threadCount);
                });

            p.yuvToRGBCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FEATHER_TK_P();
                    tl::ffmpeg::Options options = p.model->getFFmpeg();
                    options.yuvToRgb = value;
                    p.model->setFFmpeg(options);
                });

            p.threadsEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    tl::ffmpeg::Options options = p.model->getFFmpeg();
                    options.threadCount = value;
                    p.model->setFFmpeg(options);
                });
        }

        FFmpegSettingsWidget::FFmpegSettingsWidget() :
            _p(new Private)
        {}

        FFmpegSettingsWidget::~FFmpegSettingsWidget()
        {}

        std::shared_ptr<FFmpegSettingsWidget> FFmpegSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FFmpegSettingsWidget>(new FFmpegSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void FFmpegSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void FFmpegSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

#endif // TLRENDER_FFMPEG

#if defined(TLRENDER_USD)
        struct USDSettingsWidget::Private
        {
            std::shared_ptr<SettingsModel> model;

            std::shared_ptr<feather_tk::IntEdit> renderWidthEdit;
            std::shared_ptr<feather_tk::FloatEditSlider> complexitySlider;
            std::shared_ptr<feather_tk::ComboBox> drawModeComboBox;
            std::shared_ptr<feather_tk::CheckBox> lightingCheckBox;
            std::shared_ptr<feather_tk::CheckBox> sRGBCheckBox;
            std::shared_ptr<feather_tk::IntEdit> stageCacheEdit;
            std::shared_ptr<feather_tk::IntEdit> diskCacheEdit;
            std::shared_ptr<feather_tk::VerticalLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<tl::usd::Options> > optionsObserver;
        };

        void USDSettingsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            ISettingsWidget::_init(context, "djv::app::USDSettingsWidget", parent);
            FEATHER_TK_P();

            p.model = app->getSettingsModel();

            p.renderWidthEdit = feather_tk::IntEdit::create(context);
            p.renderWidthEdit->setRange(1, 8192);

            p.complexitySlider = feather_tk::FloatEditSlider::create(context);

            p.drawModeComboBox = feather_tk::ComboBox::create(context, tl::usd::getDrawModeLabels());
            p.drawModeComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.lightingCheckBox = feather_tk::CheckBox::create(context);
            p.lightingCheckBox->setHStretch(feather_tk::Stretch::Expanding);

            p.sRGBCheckBox = feather_tk::CheckBox::create(context);
            p.sRGBCheckBox->setHStretch(feather_tk::Stretch::Expanding);

            p.stageCacheEdit = feather_tk::IntEdit::create(context);
            p.stageCacheEdit->setRange(0, 10);

            p.diskCacheEdit = feather_tk::IntEdit::create(context);
            p.diskCacheEdit->setRange(0, 1024);

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            auto label = feather_tk::Label::create(context, "Changes are applied to new files.", p.layout);
            auto formLayout = feather_tk::FormLayout::create(context, p.layout);
            formLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            formLayout->addRow("Render width:", p.renderWidthEdit);
            formLayout->addRow("Render complexity:", p.complexitySlider);
            formLayout->addRow("Draw mode:", p.drawModeComboBox);
            formLayout->addRow("Enable lighting:", p.lightingCheckBox);
            formLayout->addRow("Enable sRGB color space:", p.sRGBCheckBox);
            formLayout->addRow("Stage cache size:", p.stageCacheEdit);
            formLayout->addRow("Disk cache size (GB):", p.diskCacheEdit);

            p.optionsObserver = feather_tk::ValueObserver<tl::usd::Options>::create(
                p.model->observeUSD(),
                [this](const tl::usd::Options& value)
                {
                    FEATHER_TK_P();
                    p.renderWidthEdit->setValue(value.renderWidth);
                    p.complexitySlider->setValue(value.complexity);
                    p.drawModeComboBox->setCurrentIndex(static_cast<int>(value.drawMode));
                    p.lightingCheckBox->setChecked(value.enableLighting);
                    p.sRGBCheckBox->setChecked(value.sRGB);
                    p.stageCacheEdit->setValue(value.stageCache);
                    p.diskCacheEdit->setValue(value.diskCache);
                });

            p.renderWidthEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    tl::usd::Options options = p.model->getUSD();
                    options.renderWidth = value;
                    p.model->setUSD(options);
                });

            p.complexitySlider->setCallback(
                [this](float value)
                {
                    FEATHER_TK_P();
                    tl::usd::Options options = p.model->getUSD();
                    options.complexity = value;
                    p.model->setUSD(options);
                });

            p.drawModeComboBox->setIndexCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    tl::usd::Options options = p.model->getUSD();
                    options.drawMode = static_cast<tl::usd::DrawMode>(value);
                    p.model->setUSD(options);
                });

            p.lightingCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FEATHER_TK_P();
                    tl::usd::Options options = p.model->getUSD();
                    options.enableLighting = value;
                    p.model->setUSD(options);
                });

            p.sRGBCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    FEATHER_TK_P();
                    tl::usd::Options options = p.model->getUSD();
                    options.sRGB = value;
                    p.model->setUSD(options);
                });

            p.stageCacheEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    tl::usd::Options options = p.model->getUSD();
                    options.stageCache = value;
                    p.model->setUSD(options);
                });

            p.diskCacheEdit->setCallback(
                [this](int value)
                {
                    FEATHER_TK_P();
                    tl::usd::Options options = p.model->getUSD();
                    options.diskCache = value;
                    p.model->setUSD(options);
                });
        }

        USDSettingsWidget::USDSettingsWidget() :
            _p(new Private)
        {}

        USDSettingsWidget::~USDSettingsWidget()
        {}

        std::shared_ptr<USDSettingsWidget> USDSettingsWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<USDSettingsWidget>(new USDSettingsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void USDSettingsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            ISettingsWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void USDSettingsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            ISettingsWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }
#endif // TLRENDER_USD

        struct SettingsTool::Private
        {
            std::shared_ptr<feather_tk::VerticalLayout> layout;
            std::shared_ptr<feather_tk::PushButton> saveButton;
            std::shared_ptr<feather_tk::PushButton> resetButton;
            std::map<std::string, std::shared_ptr<feather_tk::Bellows> > bellows;
        };

        void SettingsTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Settings,
                "djv::app::SettingsTool",
                parent);
            FEATHER_TK_P();

            auto advancedWidget = AdvancedSettingsWidget::create(context, app);
            auto cacheWidget = CacheSettingsWidget::create(context, app);
            auto fileBrowserWidget = FileBrowserSettingsWidget::create(context, app);
            auto imageSequenceWidget = ImageSequenceSettingsWidget::create(context, app);
            auto miscWidget = MiscSettingsWidget::create(context, app);
            auto mouseWidget = MouseSettingsWidget::create(context, app);
            auto shortcutsWidget = ShortcutsSettingsWidget::create(context, app);
            auto styleWidget = StyleSettingsWidget::create(context, app);
            auto timeWidget = TimeSettingsWidget::create(context, app);
#if defined(TLRENDER_FFMPEG)
            auto ffmpegWidget = FFmpegSettingsWidget::create(context, app);
#endif // TLRENDER_FFMPEG
#if defined(TLRENDER_USD)
            auto usdWidget = USDSettingsWidget::create(context, app);
#endif // TLRENDER_USD

            auto vLayout = feather_tk::VerticalLayout::create(context);
            vLayout->setSpacingRole(feather_tk::SizeRole::None);
            p.bellows["Cache"] = feather_tk::Bellows::create(context, "Cache", vLayout);
            p.bellows["Cache"]->setWidget(cacheWidget);
            p.bellows["FileBrowser"] = feather_tk::Bellows::create(context, "File Browser", vLayout);
            p.bellows["FileBrowser"]->setWidget(fileBrowserWidget);
            p.bellows["ImageSequences"] = feather_tk::Bellows::create(context, "Image Sequences", vLayout);
            p.bellows["ImageSequences"]->setWidget(imageSequenceWidget);
            p.bellows["Misc"] = feather_tk::Bellows::create(context, "Miscellaneous", vLayout);
            p.bellows["Misc"]->setWidget(miscWidget);
            p.bellows["Mouse"] = feather_tk::Bellows::create(context, "Mouse", vLayout);
            p.bellows["Mouse"]->setWidget(mouseWidget);
            p.bellows["Shortcuts"] = feather_tk::Bellows::create(context, "Keyboard Shortcuts", vLayout);
            p.bellows["Shortcuts"]->setWidget(shortcutsWidget);
            p.bellows["Style"] = feather_tk::Bellows::create(context, "Style", vLayout);
            p.bellows["Style"]->setWidget(styleWidget);
            p.bellows["Time"] = feather_tk::Bellows::create(context, "Time", vLayout);
            p.bellows["Time"]->setWidget(timeWidget);
#if defined(TLRENDER_FFMPEG)
            p.bellows["FFmpeg"] = feather_tk::Bellows::create(context, "FFmpeg", vLayout);
            p.bellows["FFmpeg"]->setWidget(ffmpegWidget);
#endif // TLRENDER_USD
#if defined(TLRENDER_USD)
            p.bellows["USD"] = feather_tk::Bellows::create(context, "USD", vLayout);
            p.bellows["USD"]->setWidget(usdWidget);
#endif // TLRENDER_USD
            p.bellows["Advanced"] = feather_tk::Bellows::create(context, "Advanced", vLayout);
            p.bellows["Advanced"]->setWidget(advancedWidget);

            p.saveButton = feather_tk::PushButton::create(context, "Save");
            p.saveButton->setTooltip("Save the settings. Settings are also saved on exit.");

            p.resetButton = feather_tk::PushButton::create(context, "Reset");
            p.resetButton->setTooltip("Restore settings to default values.");

            p.layout = feather_tk::VerticalLayout::create(context);
            p.layout->setSpacingRole(feather_tk::SizeRole::None);
            auto scrollWidget = feather_tk::ScrollWidget::create(context, feather_tk::ScrollType::Both, p.layout);
            scrollWidget->setWidget(vLayout);
            scrollWidget->setBorder(false);
            scrollWidget->setVStretch(feather_tk::Stretch::Expanding);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            auto hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.saveButton->setParent(hLayout);
            hLayout->addSpacer(feather_tk::Stretch::Expanding);
            p.resetButton->setParent(hLayout);
            _setWidget(p.layout);

            _loadSettings(p.bellows);

            std::weak_ptr<App> appWeak(app);
            p.saveButton->setClickedCallback(
                [this, appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getSettingsModel()->save();
                    }
                });

            p.resetButton->setClickedCallback(
                [this, appWeak]
                {
                    if (auto context = getContext())
                    {
                        if (auto dialogSystem = context->getSystem<feather_tk::DialogSystem>())
                        {
                            dialogSystem->confirm(
                                "Reset Settings",
                                "Reset settings to default values?",
                                getWindow(),
                                [appWeak](bool value)
                                {
                                    if (value)
                                    {
                                        if (auto app = appWeak.lock())
                                        {
                                            app->getSettingsModel()->reset();
                                        }
                                    }
                                });
                        }
                    }
                });
        }

        SettingsTool::SettingsTool() :
            _p(new Private)
        {}

        SettingsTool::~SettingsTool()
        {
            _saveSettings(_p->bellows);
        }

        std::shared_ptr<SettingsTool> SettingsTool::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<SettingsTool>(new SettingsTool);
            out->_init(context, app, parent);
            return out;
        }
    }
}
