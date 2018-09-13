//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvViewLib/ImagePrefsWidget.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ImagePrefs.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/InputDialog.h>
#include <djvUI/QuestionDialog.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/ToolButton.h>

#include <djvGraphics/Image.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QListWidget>
#include <QFormLayout>
#include <QVBoxLayout>

namespace
{

class SmallListWidget : public QListWidget
{
public:
    QSize sizeHint() const
    {
        const QSize size = QListWidget::sizeHint();
        return QSize(size.width(), size.height() / 2);
    }
};

} // namespace

namespace djv
{
    namespace ViewLib
    {
        struct ImagePrefsWidget::Private
        {
            Private() :
                frameStoreFileReloadWidget(0),
                mirrorHWidget(0),
                mirrorVWidget(0),
                scaleWidget(0),
                rotateWidget(0),
                colorProfileWidget(0),
                displayProfileWidget(0),
                displayProfileListWidget(0),
                channelWidget(0)
            {}

            QCheckBox *    frameStoreFileReloadWidget;
            QCheckBox *    mirrorHWidget;
            QCheckBox *    mirrorVWidget;
            QComboBox *    scaleWidget;
            QComboBox *    rotateWidget;
            QCheckBox *    colorProfileWidget;
            QComboBox *    displayProfileWidget;
            QListWidget *  displayProfileListWidget;
            QComboBox *    channelWidget;
        };

        ImagePrefsWidget::ImagePrefsWidget(Context * context) :
            AbstractPrefsWidget(qApp->translate("djv::ViewLib::ImagePrefsWidget", "Images"), context),
            _p(new Private)
        {
            // Create the widgets.
            _p->frameStoreFileReloadWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Automatically store the current frame when files are reloaded"));

            _p->mirrorHWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Mirror horizontally"));

            _p->mirrorVWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Mirror vertically"));

            _p->scaleWidget = new QComboBox;
            _p->scaleWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->scaleWidget->addItems(Util::imageScaleLabels());

            _p->rotateWidget = new QComboBox;
            _p->rotateWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->rotateWidget->addItems(Util::imageRotateLabels());

            _p->colorProfileWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Enable color profile"));

            _p->displayProfileWidget = new QComboBox;
            _p->displayProfileWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            _p->displayProfileListWidget = new SmallListWidget;

            UI::ToolButton * addDisplayProfileButton = new UI::ToolButton(
                context->iconLibrary()->icon("djvAddIcon.png"));
            addDisplayProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Add a new display profile"));

            UI::ToolButton * removeDisplayProfileButton = new UI::ToolButton(
                context->iconLibrary()->icon("djvRemoveIcon.png"));
            removeDisplayProfileButton->setAutoRepeat(true);
            removeDisplayProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Remove the selected display profile"));

            UI::ToolButton * moveDisplayProfileUpButton = new UI::ToolButton(
                context->iconLibrary()->icon("djvUpIcon.png"));
            moveDisplayProfileUpButton->setAutoRepeat(true);
            moveDisplayProfileUpButton->setToolTip(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Move the selected display profile up"));

            UI::ToolButton * moveDisplayProfileDownButton = new UI::ToolButton(
                context->iconLibrary()->icon("djvDownIcon.png"));
            moveDisplayProfileDownButton->setAutoRepeat(true);
            moveDisplayProfileDownButton->setToolTip(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Move the selected display profile down"));

            _p->channelWidget = new QComboBox;
            _p->channelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->channelWidget->addItems(Graphics::OpenGLImageOptions::channelLabels());

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);

            UI::PrefsGroupBox * prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Images"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->frameStoreFileReloadWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Transforms"),
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Set how the image is transformed."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->mirrorHWidget);
            formLayout->addRow(_p->mirrorVWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Scale:"),
                _p->scaleWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Rotation:"),
                _p->rotateWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Color Profile"),
                qApp->translate("djv::ViewLib::ImagePrefsWidget",
                    "Set whether the image's color profile is enabled."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->colorProfileWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Display Profile"),
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Set a custom display profile."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Current profile:"),
                _p->displayProfileWidget);
            formLayout->addRow(new QLabel(qApp->translate("djv::ViewLib::ImagePrefsWidget", "Available profiles:")));
            formLayout->addRow(_p->displayProfileListWidget);
            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->addStretch();
            hLayout->addWidget(addDisplayProfileButton);
            hLayout->addWidget(removeDisplayProfileButton);
            hLayout->addWidget(moveDisplayProfileUpButton);
            hLayout->addWidget(moveDisplayProfileDownButton);
            formLayout->addRow(hLayout);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Channels"),
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Set which image channels are displayed."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Channels:"),
                _p->channelWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->frameStoreFileReloadWidget,
                SIGNAL(toggled(bool)),
                SLOT(frameStoreFileReloadCallback(bool)));
            connect(
                _p->mirrorHWidget,
                SIGNAL(toggled(bool)),
                SLOT(mirrorHCallback(bool)));
            connect(
                _p->mirrorVWidget,
                SIGNAL(toggled(bool)),
                SLOT(mirrorVCallback(bool)));
            connect(
                _p->scaleWidget,
                SIGNAL(activated(int)),
                SLOT(scaleCallback(int)));
            connect(
                _p->rotateWidget,
                SIGNAL(activated(int)),
                SLOT(rotateCallback(int)));
            connect(
                _p->colorProfileWidget,
                SIGNAL(toggled(bool)),
                SLOT(colorProfileCallback(bool)));
            connect(
                _p->displayProfileWidget,
                SIGNAL(activated(int)),
                SLOT(displayProfileCallback(int)));
            connect(
                _p->displayProfileListWidget,
                SIGNAL(itemChanged(QListWidgetItem *)),
                SLOT(displayProfileCallback(QListWidgetItem *)));
            connect(
                addDisplayProfileButton,
                SIGNAL(clicked()),
                SLOT(addDisplayProfileCallback()));
            connect(
                removeDisplayProfileButton,
                SIGNAL(clicked()),
                SLOT(removeDisplayProfileCallback()));
            connect(
                moveDisplayProfileUpButton,
                SIGNAL(clicked()),
                SLOT(moveDisplayProfileUpCallback()));
            connect(
                moveDisplayProfileDownButton,
                SIGNAL(clicked()),
                SLOT(moveDisplayProfileDownCallback()));
            connect(
                _p->channelWidget,
                SIGNAL(activated(int)),
                SLOT(channelCallback(int)));
            connect(
                context->imagePrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
        }

        ImagePrefsWidget::~ImagePrefsWidget()
        {}

        void ImagePrefsWidget::resetPreferences()
        {
            context()->imagePrefs()->setFrameStoreFileReload(ImagePrefs::frameStoreFileReloadDefault());
            context()->imagePrefs()->setMirror(ImagePrefs::mirrorDefault());
            context()->imagePrefs()->setScale(ImagePrefs::scaleDefault());
            context()->imagePrefs()->setRotate(ImagePrefs::rotateDefault());
            context()->imagePrefs()->setColorProfile(ImagePrefs::colorProfileDefault());
            context()->imagePrefs()->setDisplayProfileIndex(ImagePrefs::displayProfileIndexDefault());
            context()->imagePrefs()->setChannel(ImagePrefs::channelDefault());
            widgetUpdate();
        }

        void ImagePrefsWidget::frameStoreFileReloadCallback(bool in)
        {
            context()->imagePrefs()->setFrameStoreFileReload(in);
        }

        void ImagePrefsWidget::mirrorHCallback(bool in)
        {
            Graphics::PixelDataInfo::Mirror mirror = context()->imagePrefs()->mirror();
            mirror.x = in;
            context()->imagePrefs()->setMirror(mirror);
        }

        void ImagePrefsWidget::mirrorVCallback(bool in)
        {
            Graphics::PixelDataInfo::Mirror mirror = context()->imagePrefs()->mirror();
            mirror.y = in;
            context()->imagePrefs()->setMirror(mirror);
        }

        void ImagePrefsWidget::scaleCallback(int in)
        {
            context()->imagePrefs()->setScale(
                static_cast<Util::IMAGE_SCALE>(in));
        }

        void ImagePrefsWidget::rotateCallback(int in)
        {
            context()->imagePrefs()->setRotate(
                static_cast<Util::IMAGE_ROTATE>(in));
        }

        void ImagePrefsWidget::colorProfileCallback(bool in)
        {
            context()->imagePrefs()->setColorProfile(in);
        }

        void ImagePrefsWidget::displayProfileCallback(int in)
        {
            context()->imagePrefs()->setDisplayProfileIndex(in - 1);
        }

        void ImagePrefsWidget::displayProfileCallback(QListWidgetItem * item)
        {
            QVector<DisplayProfile> displayProfiles =
                context()->imagePrefs()->displayProfiles();
            const int index = _p->displayProfileListWidget->row(item);
            const QString name = item->data(Qt::EditRole).toString();
            displayProfiles[index].name = name;
            context()->imagePrefs()->setDisplayProfiles(displayProfiles);
        }

        void ImagePrefsWidget::addDisplayProfileCallback()
        {
            UI::InputDialog dialog(
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "Add a new display profile:"));
            if (QDialog::Accepted == dialog.exec())
            {
                QVector<DisplayProfile> displayProfiles =
                    context()->imagePrefs()->displayProfiles();
                DisplayProfile displayProfile;
                displayProfile.name = dialog.text();
                displayProfiles += displayProfile;
                context()->imagePrefs()->setDisplayProfiles(displayProfiles);
            }
        }

        void ImagePrefsWidget::removeDisplayProfileCallback()
        {
            int index = _p->displayProfileListWidget->currentRow();
            QVector<DisplayProfile> displayProfiles =
                context()->imagePrefs()->displayProfiles();
            if (-1 == index)
                index = displayProfiles.count() - 1;
            if (index != -1)
            {
                UI::QuestionDialog dialog(
                    qApp->translate("djv::ViewLib::ImagePrefsWidget", \
                        "Are you sure you want to remove the display profile \"%1\"?").
                    arg(displayProfiles[index].name));
                if (QDialog::Accepted == dialog.exec())
                {
                    displayProfiles.remove(index);
                    context()->imagePrefs()->setDisplayProfiles(displayProfiles);
                    _p->displayProfileListWidget->setCurrentRow(
                        index >= displayProfiles.count() ? (index - 1) : index);
                }
            }
        }

        void ImagePrefsWidget::moveDisplayProfileUpCallback()
        {
            int index = _p->displayProfileListWidget->currentRow();
            if (index != -1)
            {
                QVector<DisplayProfile> displayProfiles =
                    context()->imagePrefs()->displayProfiles();
                DisplayProfile displayProfile = displayProfiles[index];
                displayProfiles.remove(index);
                if (index > 0)
                    --index;
                displayProfiles.insert(index, displayProfile);
                context()->imagePrefs()->setDisplayProfiles(displayProfiles);
                _p->displayProfileListWidget->setCurrentRow(index);
            }
        }

        void ImagePrefsWidget::moveDisplayProfileDownCallback()
        {
            int index = _p->displayProfileListWidget->currentRow();
            if (index != -1)
            {
                QVector<DisplayProfile> displayProfiles =
                    context()->imagePrefs()->displayProfiles();
                DisplayProfile displayProfile = displayProfiles[index];
                displayProfiles.remove(index);
                if (index < displayProfiles.count())
                    ++index;
                displayProfiles.insert(index, displayProfile);
                context()->imagePrefs()->setDisplayProfiles(displayProfiles);
                _p->displayProfileListWidget->setCurrentRow(index);
            }
        }

        void ImagePrefsWidget::channelCallback(int in)
        {
            context()->imagePrefs()->setChannel(static_cast<Graphics::OpenGLImageOptions::CHANNEL>(in));
        }

        void ImagePrefsWidget::widgetUpdate()
        {
            //DJV_DEBUG("ImagePrefsWidget::widgetUpdate");
            djvSignalBlocker signalBlocker(QObjectList() <<
                _p->frameStoreFileReloadWidget <<
                _p->mirrorHWidget <<
                _p->mirrorVWidget <<
                _p->scaleWidget <<
                _p->rotateWidget <<
                _p->colorProfileWidget <<
                _p->displayProfileWidget <<
                _p->displayProfileListWidget <<
                _p->channelWidget);
            _p->frameStoreFileReloadWidget->setChecked(context()->imagePrefs()->hasFrameStoreFileReload());
            _p->mirrorHWidget->setChecked(context()->imagePrefs()->mirror().x);
            _p->mirrorVWidget->setChecked(context()->imagePrefs()->mirror().y);
            _p->scaleWidget->setCurrentIndex(context()->imagePrefs()->scale());
            _p->rotateWidget->setCurrentIndex(context()->imagePrefs()->rotate());
            _p->colorProfileWidget->setChecked(context()->imagePrefs()->hasColorProfile());
            _p->displayProfileWidget->setCurrentIndex(context()->imagePrefs()->displayProfileIndex() + 1);
            _p->displayProfileWidget->clear();
            _p->displayProfileWidget->addItems(QStringList() <<
                qApp->translate("djv::ViewLib::ImagePrefsWidget", "None") <<
                context()->imagePrefs()->displayProfileNames());
            _p->displayProfileWidget->setCurrentIndex(context()->imagePrefs()->displayProfileIndex() + 1);
            _p->displayProfileListWidget->clear();
            const QVector<DisplayProfile> & displayProfiles =
                context()->imagePrefs()->displayProfiles();
            for (int i = 0; i < displayProfiles.count(); ++i)
            {
                QListWidgetItem * item = new QListWidgetItem(_p->displayProfileListWidget);
                item->setFlags(
                    Qt::ItemIsSelectable |
                    Qt::ItemIsEditable |
                    Qt::ItemIsEnabled);
                item->setData(Qt::DisplayRole, displayProfiles[i].name);
                item->setData(Qt::EditRole, displayProfiles[i].name);
            }
            _p->channelWidget->setCurrentIndex(context()->imagePrefs()->channel());
        }

    } // namespace ViewLib
} // namespace djv
