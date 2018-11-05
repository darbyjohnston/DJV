//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/WindowPrefsWidget.h>

#include <djvViewLib/ViewContext.h>
#include <djvViewLib/WindowPrefs.h>

#include <djvUI/PrefsGroupBox.h>
#include <djvUI/Vector2iEditWidget.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct WindowPrefsWidget::Private
        {
            QPointer<QCheckBox> autoFitWidget;
            QPointer<QComboBox> viewMaxWidget;
            QPointer<UI::Vector2iEditWidget> viewMaxUserWidget;
            QPointer<QComboBox> fullScreenUIWidget;
            QPointer<QButtonGroup> uiComponentVisibleButtonGroup;
            QPointer<QCheckBox> controlsWindowWidget;
        };

        WindowPrefsWidget::WindowPrefsWidget(const QPointer<ViewContext> & context) :
            AbstractPrefsWidget(
                qApp->translate("djv::ViewLib::WindowPrefsWidget", "Windows"), context),
            _p(new Private)
        {
            // Create the widgets.
            _p->autoFitWidget = new QCheckBox(
                qApp->translate(
                    "djv::ViewLib::WindowPrefsWidget",
                    "Automatically fit the window to the image"));

            _p->viewMaxWidget = new QComboBox;
            _p->viewMaxWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->viewMaxWidget->addItems(Enum::viewMaxLabels());

            _p->viewMaxUserWidget = new UI::Vector2iEditWidget;
            _p->viewMaxUserWidget->setRange(glm::ivec2(100, 100), glm::ivec2(8192, 8192));
            _p->viewMaxUserWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            _p->fullScreenUIWidget = new QComboBox;
            _p->fullScreenUIWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->fullScreenUIWidget->addItems(Enum::fullScreenUILabels());

            _p->uiComponentVisibleButtonGroup = new QButtonGroup(this);
            _p->uiComponentVisibleButtonGroup->setExclusive(false);
            for (int i = 0; i < Enum::uiComponentLabels().count(); ++i)
            {
                auto checkBox = new QCheckBox(Enum::uiComponentLabels()[i]);
                _p->uiComponentVisibleButtonGroup->addButton(checkBox, i);
            }

            _p->controlsWindowWidget = new QCheckBox(
                qApp->translate(
                    "djv::ViewLib::WindowPrefsWidget",
                    "Detach the controls into a separate window"));

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);

            auto prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::WindowPrefsWidget", "Window Size"), context.data());
            auto formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->autoFitWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::WindowPrefsWidget", "Maximum view size:"),
                _p->viewMaxWidget);
            formLayout->addRow("", _p->viewMaxUserWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::WindowPrefsWidget", "User Interface"),
                context.data());
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::WindowPrefsWidget", "What happens to the user interface in full screen mode:"),
                _p->fullScreenUIWidget);
            formLayout->addRow(new QLabel("Set which user interface components are visible:"));
            for (int i = 0; i < _p->uiComponentVisibleButtonGroup->buttons().count(); ++i)
                formLayout->addRow(_p->uiComponentVisibleButtonGroup->button(i));
            formLayout->addRow(_p->controlsWindowWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->autoFitWidget,
                SIGNAL(toggled(bool)),
                SLOT(autoFitCallback(bool)));
            connect(
                _p->viewMaxWidget,
                SIGNAL(activated(int)),
                SLOT(viewMaxCallback(int)));
            connect(
                _p->viewMaxUserWidget,
                SIGNAL(valueChanged(const glm::ivec2 &)),
                SLOT(viewMaxUserCallback(const glm::ivec2 &)));
            connect(
                _p->fullScreenUIWidget,
                SIGNAL(activated(int)),
                SLOT(fullScreenUICallback(int)));
            connect(
                _p->uiComponentVisibleButtonGroup,
                SIGNAL(buttonClicked(int)),
                SLOT(uiComponentVisibleCallback(int)));
            connect(
                context->windowPrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
            connect(
                _p->controlsWindowWidget,
                SIGNAL(toggled(bool)),
                SLOT(controlsWindowCallback(bool)));
        }

        WindowPrefsWidget::~WindowPrefsWidget()
        {}

        void WindowPrefsWidget::resetPreferences()
        {
            context()->windowPrefs()->setAutoFit(WindowPrefs::autoFitDefault());
            context()->windowPrefs()->setViewMax(WindowPrefs::viewMaxDefault());
            context()->windowPrefs()->setViewMaxUser(WindowPrefs::viewMaxUserDefault());
            context()->windowPrefs()->setFullScreenUI(WindowPrefs::fullScreenUIDefault());
            context()->windowPrefs()->setUIComponentVisible(WindowPrefs::uiComponentVisibleDefault());
            context()->windowPrefs()->setControlsWindow(WindowPrefs::controlsWindowDefault());
        }

        void WindowPrefsWidget::autoFitCallback(bool in)
        {
            context()->windowPrefs()->setAutoFit(in);
        }

        void WindowPrefsWidget::viewMaxCallback(int in)
        {
            context()->windowPrefs()->setViewMax(static_cast<Enum::VIEW_MAX>(in));

            widgetUpdate();
        }

        void WindowPrefsWidget::viewMaxUserCallback(const glm::ivec2 & in)
        {
            context()->windowPrefs()->setViewMaxUser(in);
        }

        void WindowPrefsWidget::fullScreenUICallback(int in)
        {
            context()->windowPrefs()->setFullScreenUI(static_cast<Enum::FULL_SCREEN_UI>(in));
        }

        void WindowPrefsWidget::uiComponentVisibleCallback(int id)
        {
            QVector<bool> visible = context()->windowPrefs()->uiComponentVisible();
            visible[id] = _p->uiComponentVisibleButtonGroup->button(id)->isChecked();
            context()->windowPrefs()->setUIComponentVisible(visible);
        }

        void WindowPrefsWidget::controlsWindowCallback(bool in)
        {
            context()->windowPrefs()->setControlsWindow(in);
        }

        void WindowPrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->autoFitWidget <<
                _p->viewMaxWidget <<
                _p->viewMaxUserWidget <<
                _p->fullScreenUIWidget <<
                _p->uiComponentVisibleButtonGroup <<
                _p->controlsWindowWidget);
            _p->autoFitWidget->setChecked(context()->windowPrefs()->hasAutoFit());
            _p->viewMaxWidget->setCurrentIndex(context()->windowPrefs()->viewMax());
            _p->viewMaxUserWidget->setValue(context()->windowPrefs()->viewMaxUser());
            _p->viewMaxUserWidget->setVisible(Enum::VIEW_MAX_USER == context()->windowPrefs()->viewMax());
            _p->fullScreenUIWidget->setCurrentIndex(context()->windowPrefs()->fullScreenUI());
            const QVector<bool> & visible = context()->windowPrefs()->uiComponentVisible();
            for (int i = 0; i < visible.count(); ++i)
            {
                _p->uiComponentVisibleButtonGroup->button(i)->setChecked(visible[i]);
            }
            _p->controlsWindowWidget->setChecked(context()->windowPrefs()->hasControlsWindow());
        }

    } // namespace ViewLib
} // namespace djv
