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

#include <djvUI/MultiChoiceDialog.h>

#include <QApplication>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct MultiChoiceDialog::Private
        {
            Private(
                const QString &       label,
                const QStringList &   choices,
                const QVector<bool> & values) :
                choices(choices),
                values(values),
                label(label)
            {}

            QStringList choices;
            QVector<bool> values;
            QVector<QPointer<QCheckBox> > buttons;
            QPointer<QVBoxLayout> buttonLayout;
            QString label;
            QPointer<QLabel> labelWidget;
        };

        MultiChoiceDialog::MultiChoiceDialog(
            const QString &       label,
            const QStringList &   choices,
            const QVector<bool> & values,
            QWidget *             parent) :
            QDialog(parent),
            _p(new Private(label, choices, values))
        {
            _p->labelWidget = new QLabel(label);

            QDialogButtonBox * buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Ok |
                QDialogButtonBox::Cancel);

            QVBoxLayout * layout = new QVBoxLayout(this);
            QVBoxLayout * vLayout = new QVBoxLayout;
            vLayout->setMargin(20);
            vLayout->addWidget(_p->labelWidget);
            _p->buttonLayout = new QVBoxLayout;
            vLayout->addLayout(_p->buttonLayout);
            layout->addLayout(vLayout);
            layout->addWidget(buttonBox);

            setWindowTitle(qApp->translate("djv::UI::MultiChoiceDialog", "Choose one or more options"));

            widgetUpdate();

            connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
            connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        }

        MultiChoiceDialog::~MultiChoiceDialog()
        {}

        const QStringList & MultiChoiceDialog::choices() const
        {
            return _p->choices;
        }

        void MultiChoiceDialog::setChoices(const QStringList & choices)
        {
            _p->choices = choices;
            _p->values = QVector<bool>(choices.count());
            widgetUpdate();
        }

        const QVector<bool> & MultiChoiceDialog::values() const
        {
            return _p->values;
        }

        QVector<int> MultiChoiceDialog::indices() const
        {
            QVector<int> out;
            for (int i = 0; i < _p->values.count(); ++i)
            {
                if (_p->values[i])
                {
                    out += i;
                }
            }
            return out;
        }

        void MultiChoiceDialog::setValues(const QVector<bool> & values)
        {
            if (values == _p->values)
                return;
            _p->values = values;
            valuesUpdate();
        }

        const QString & MultiChoiceDialog::label() const
        {
            return _p->label;
        }

        void MultiChoiceDialog::setLabel(const QString & label)
        {
            _p->label = label;
            widgetUpdate();
        }

        void MultiChoiceDialog::buttonCallback()
        {
            QVector<bool> values;
            for (int i = 0; i < _p->buttons.count(); ++i)
            {
                values += _p->buttons[i]->isChecked();
            }
            setValues(values);
        }

        void MultiChoiceDialog::valuesUpdate()
        {
            for (int i = 0; i < _p->buttons.count() && i < _p->values.count(); ++i)
            {
                _p->buttons[i]->setChecked(_p->values[i]);
            }
        }

        void MultiChoiceDialog::widgetUpdate()
        {
            _p->labelWidget->setText(_p->label);
            for (int i = 0; i < _p->buttons.count(); ++i)
            {
                delete _p->buttons[i];
            }
            _p->buttons.clear();
            for (int i = 0; i < _p->choices.count(); ++i)
            {
                QCheckBox * button = new QCheckBox(_p->choices[i]);
                button->setChecked(i < _p->values.count() ? _p->values[i] : false);
                connect(button, SIGNAL(toggled(bool)), SLOT(buttonCallback()));
                _p->buttons += button;
                _p->buttonLayout->addWidget(button);
            }
        }

    } // namespace UI
} // namespace djv
