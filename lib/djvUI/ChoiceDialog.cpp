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

#include <djvUI/ChoiceDialog.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPointer>
#include <QRadioButton>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct ChoiceDialog::Private
        {
            Private(
                const QString &     label,
                const QStringList & choices,
                int                 choice) :
                choices(choices),
                choice(choice),
                label(label)
            {}

            QStringList choices;
            int choice = 0;
            QVector<QPointer<QRadioButton> > buttons;
            QPointer<QVBoxLayout> buttonLayout;
            QString label;
            QPointer<QLabel> labelWidget;
            QPointer<QVBoxLayout> vLayout;
        };

        ChoiceDialog::ChoiceDialog(
            const QString &     label,
            const QStringList & choices,
            int                 choice,
            QWidget *           parent) :
            QDialog(parent),
            _p(new Private(label, choices, choice))
        {
            _p->labelWidget = new QLabel(label);

            auto buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Ok |
                QDialogButtonBox::Cancel);

            auto layout = new QVBoxLayout(this);
            auto vLayout = new QVBoxLayout;
            vLayout->setMargin(20);
            vLayout->addWidget(_p->labelWidget);
            _p->buttonLayout = new QVBoxLayout;
            vLayout->addLayout(_p->buttonLayout);
            layout->addLayout(vLayout);
            layout->addWidget(buttonBox);

            setWindowTitle(qApp->translate("djv::UI::ChoiceDialog", "Choice"));

            widgetUpdate();

            connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
            connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        }

        ChoiceDialog::~ChoiceDialog()
        {}

        const QStringList & ChoiceDialog::choices() const
        {
            return _p->choices;
        }

        void ChoiceDialog::setChoices(const QStringList & choices)
        {
            _p->choices = choices;
            widgetUpdate();
        }

        int ChoiceDialog::choice() const
        {
            return _p->choice;
        }

        void ChoiceDialog::setChoice(int choice)
        {
            if (choice == _p->choice)
                return;
            _p->choice = choice;
            choiceUpdate();
        }

        const QString & ChoiceDialog::label() const
        {
            return _p->label;
        }

        void ChoiceDialog::setLabel(const QString & label)
        {
            _p->label = label;
            widgetUpdate();
        }

        void ChoiceDialog::buttonCallback()
        {
            for (int i = 0; i < _p->buttons.count(); ++i)
            {
                if (_p->buttons[i]->isChecked())
                {
                    setChoice(i);
                    break;
                }
            }
        }

        void ChoiceDialog::choiceUpdate()
        {
            for (int i = 0; i < _p->buttons.count(); ++i)
            {
                _p->buttons[i]->setChecked(_p->choice == i);
            }
        }

        void ChoiceDialog::widgetUpdate()
        {
            _p->labelWidget->setText(_p->label);

            for (int i = 0; i < _p->buttons.count(); ++i)
            {
                delete _p->buttons[i];
            }
            _p->buttons.clear();

            for (int i = 0; i < _p->choices.count(); ++i)
            {
                auto button = new QRadioButton(_p->choices[i]);
                button->setChecked(_p->choice == i);
                connect(button, SIGNAL(toggled(bool)), SLOT(buttonCallback()));
                _p->buttons += button;
                _p->buttonLayout->addWidget(button);
            }
        }

    } // namespace UI
} // namespace 
