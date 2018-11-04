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

#include <djvUI/PrefsDialog.h>

#include <djvUI/FileBrowserPrefsWidget.h>
#include <djvUI/HelpPrefsWidget.h>
#include <djvUI/ImageIOWidget.h>
#include <djvUI/OpenGLPrefsWidget.h>
#include <djvUI/QuestionDialog.h>
#include <djvUI/SequencePrefsWidget.h>
#include <djvUI/StylePrefsWidget.h>
#include <djvUI/TimePrefsWidget.h>
#include <djvUI/UIContext.h>

#include <djvGraphics/ImageIO.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QKeyEvent>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        namespace
        {
            class ListWidgetItem : public QListWidgetItem
            {
            public:
                explicit ListWidgetItem(QListWidget * parent = nullptr) :
                    QListWidgetItem(parent)
                {
                    init();
                }
                explicit ListWidgetItem(AbstractPrefsWidget * widget, QListWidget * parent = nullptr) :
                    QListWidgetItem(parent),
                    _widget(widget)
                {
                    init();
                    setText(widget->name());
                }

                AbstractPrefsWidget * widget() const { return _widget; }

            private:
                void init()
                {
                    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                }

                QPointer<AbstractPrefsWidget> _widget;
            };

        } // namespace

        struct PrefsDialog::Private
        {
            QVector<QPointer<AbstractPrefsWidget> > widgets;
            QPointer<QListWidget> browser;
            QPointer<QScrollArea> scrollArea;
            QPointer<QDialogButtonBox> buttonBox;
            QPointer<QPushButton> resetPageButton;
            QPointer<QPushButton> resetAllButton;
        };

        PrefsDialog::PrefsDialog(const QPointer<UIContext> & context, QWidget * parent) :
            QDialog(parent),
            _p(new Private)
        {
            //DJV_DEBUG("PrefsDialog::PrefsDialog");

            // Create the widgets.
            _p->browser = new QListWidget;
            _p->scrollArea = new QScrollArea;
            _p->scrollArea->setWidgetResizable(true);
            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
            _p->resetPageButton = _p->buttonBox->addButton(
                qApp->translate("djv::UI::PrefsDialog", "Reset Page"),
                QDialogButtonBox::ResetRole);
            _p->resetPageButton->setAutoDefault(false);
            _p->resetAllButton = _p->buttonBox->addButton(
                qApp->translate("djv::UI::PrefsDialog", "Reset All"),
                QDialogButtonBox::ResetRole);
            _p->resetAllButton->setAutoDefault(false);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            QSplitter * splitter = new QSplitter;
            splitter->addWidget(_p->browser);
            splitter->addWidget(_p->scrollArea);
            layout->addWidget(splitter, 1);
            layout->addWidget(_p->buttonBox);

            // Initialize.
            setWindowTitle(qApp->translate("djv::UI::PrefsDialog", "Preferences"));
            setWindowFlags(Qt::Dialog | Qt::Tool | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);

            addWidget(new HelpPrefsWidget(context));
            addWidget(new TimePrefsWidget(context));
            addWidget(new SequencePrefsWidget(context));
            addWidget(new OpenGLPrefsWidget(context));
            addWidget(new StylePrefsWidget(context));
            addWidget(new FileBrowserPrefsWidget(context));

            const QList<Core::Plugin *> & imageIOPlugins = context->imageIOFactory()->plugins();
            for (int i = 0; i < imageIOPlugins.count(); ++i)
            {
                if (Graphics::ImageIO * imageIOPlugin = dynamic_cast<Graphics::ImageIO *>(imageIOPlugins[i]))
                {
                    if (AbstractPrefsWidget * widget = context->imageIOWidgetFactory()->createWidget(imageIOPlugin))
                    {
                        addWidget(widget);
                    }
                }
            }

            resize(800, 600);
            splitter->setSizes(QList<int>() << 160 << 640);

            // Setup the callbacks.
            connect(
                _p->browser,
                SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
                SLOT(browserCallback(QListWidgetItem *, QListWidgetItem *)));
            connect(
                _p->buttonBox,
                SIGNAL(clicked(QAbstractButton *)),
                SLOT(buttonCallback(QAbstractButton *)));
        }

        PrefsDialog::~PrefsDialog()
        {}

        void PrefsDialog::addWidget(AbstractPrefsWidget * widget)
        {
            //DJV_DEBUG("PrefsDialog::addWidget");
            //DJV_DEBUG_PRINT("name = " << widget->name());

            _p->widgets.insert(0, widget);
            widget->setParent(this);
            widget->hide();

            _p->browser->insertItem(0, new ListWidgetItem(widget));
        }
        
        void PrefsDialog::keyPressEvent(QKeyEvent * event)
        {
            if (Qt::Key_Escape == event->key())
            {
                reject();
            }
        }

        void PrefsDialog::browserCallback(QListWidgetItem * current, QListWidgetItem *)
        {
            if (ListWidgetItem * item = dynamic_cast<ListWidgetItem *>(current))
            {
                if (QWidget * oldWidget = _p->scrollArea->takeWidget())
                {
                    oldWidget->setParent(this);
                    oldWidget->hide();
                }
                if (AbstractPrefsWidget * widget = item->widget())
                {
                    _p->scrollArea->setWidget(widget);
                    widget->show();
                }
            }
        }

        void PrefsDialog::buttonCallback(QAbstractButton * button)
        {
            if (button == _p->resetAllButton)
            {
                QuestionDialog dialog(
                    qApp->translate("djv::UI::PrefsDialog", "Are you sure you want to reset all of the preferences?"));
                if (QDialog::Accepted == dialog.exec())
                {
                    Q_FOREACH(AbstractPrefsWidget * widget, _p->widgets)
                    {
                        widget->resetPreferences();
                    }
                }
            }
            if (button == _p->resetPageButton)
            {
                QuestionDialog dialog(
                    qApp->translate("djv::UI::PrefsDialog", "Are you sure you want to reset the preferences for the current page?"));

                if (QDialog::Accepted == dialog.exec())
                {
                    if (ListWidgetItem * item = dynamic_cast<ListWidgetItem *>(_p->browser->currentItem()))
                    {
                        if (AbstractPrefsWidget * widget = item->widget())
                        {
                            widget->resetPreferences();
                        }
                    }
                }
            }
            else if (QDialogButtonBox::RejectRole == _p->buttonBox->buttonRole(button))
            {
                hide();
            }
        }

    } // namespace UI
} // namespace djv
