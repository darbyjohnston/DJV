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

#include <djvPrefsDialog.h>

#include <djvFileBrowserPrefsWidget.h>
#include <djvGuiContext.h>
#include <djvHelpPrefsWidget.h>
#include <djvImageIoWidget.h>
#include <djvImagePrefsWidget.h>
#include <djvQuestionDialog.h>
#include <djvSequencePrefsWidget.h>
#include <djvStylePrefsWidget.h>
#include <djvTimePrefsWidget.h>

#include <djvImageIo.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTreeWidget>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
// TreeWidgetItem
//------------------------------------------------------------------------------

namespace
{

class TreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit TreeWidgetItem(TreeWidgetItem * parent = nullptr) :
        QTreeWidgetItem(parent)
    {
        init();
    }
    explicit TreeWidgetItem(djvAbstractPrefsWidget * widget, TreeWidgetItem * parent = nullptr) :
        QTreeWidgetItem(parent),
        _widget(widget)
    {
        init();

        setText(0, widget->name());
    }

    djvAbstractPrefsWidget * widget() const { return _widget; }

private:
    void init()
    {
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    djvAbstractPrefsWidget * _widget = nullptr;
};

} // namespace

//------------------------------------------------------------------------------
// djvPrefsDialog::Private
//------------------------------------------------------------------------------

struct djvPrefsDialog::Private
{
    QVector<djvAbstractPrefsWidget *> widgets;
    QTreeWidget *                     browser         = nullptr;
    QMap<QString, TreeWidgetItem *>   browserGroups;
    QScrollArea *                     scrollArea      = nullptr;
    QDialogButtonBox *                buttonBox       = nullptr;
    QPushButton *                     resetPageButton = nullptr;
    QPushButton *                     resetAllButton  = nullptr;
};

//------------------------------------------------------------------------------
// djvPrefsDialog
//------------------------------------------------------------------------------

djvPrefsDialog::djvPrefsDialog(djvGuiContext * context, QWidget * parent) :
    QDialog(parent),
    _p(new Private)
{
    //DJV_DEBUG("djvPrefsDialog::djvPrefsDialog");
    
    // Create the widgets.
    _p->browser = new QTreeWidget;
    _p->browser->setColumnCount(1);
    _p->browser->header()->hide();

    _p->scrollArea = new QScrollArea;
    _p->scrollArea->setWidgetResizable(true);
    
    _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    
    _p->resetPageButton = _p->buttonBox->addButton(
        qApp->translate("djvPrefsDialog", "Reset Page"),
        QDialogButtonBox::ResetRole);
    _p->resetPageButton->setAutoDefault(false);

    _p->resetAllButton = _p->buttonBox->addButton(
        qApp->translate("djvPrefsDialog", "Reset All"),
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
    setWindowTitle(qApp->translate("djvPrefsDialog", "Preferences Dialog"));

    addWidget(
        new djvFileBrowserPrefsWidget(context),
        qApp->translate("djvPrefsDialog", "General"));
    addWidget(
        new djvHelpPrefsWidget(context),
        qApp->translate("djvPrefsDialog", "General"));
    addWidget(
        new djvImagePrefsWidget(context),
        qApp->translate("djvPrefsDialog", "General"));
    addWidget(
        new djvSequencePrefsWidget(context),
        qApp->translate("djvPrefsDialog", "General"));
    addWidget(
        new djvStylePrefsWidget(context),
        qApp->translate("djvPrefsDialog", "General"));
    addWidget(
        new djvTimePrefsWidget(context),
        qApp->translate("djvPrefsDialog", "General"));

    const QList<djvPlugin *> & imageIoPlugins = context->imageIoFactory()->plugins();

    for (int i = 0; i < imageIoPlugins.count(); ++i)
    {
        if (djvImageIo * imageIoPlugin = dynamic_cast<djvImageIo *>(imageIoPlugins[i]))
        {
            if (djvAbstractPrefsWidget * widget =
                context->imageIoWidgetFactory()->createWidget(imageIoPlugin))
            {
                addWidget(
                    widget,
                    qApp->translate("djvPrefsDialog", "Image I/O"));
            }
        }
    }
    
    resize(600, 500);

    splitter->setSizes(QList<int>() << 160 << 440);

    // Setup the callbacks.
    connect(
        _p->browser,
        SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        SLOT(browserCallback(QTreeWidgetItem *, QTreeWidgetItem *)));
    connect(
        _p->buttonBox,
        SIGNAL(clicked(QAbstractButton *)),
        SLOT(buttonCallback(QAbstractButton *)));
}

djvPrefsDialog::~djvPrefsDialog()
{
    //DJV_DEBUG("djvPrefsDialog::~djvPrefsDialog");
}

void djvPrefsDialog::addWidget(djvAbstractPrefsWidget * widget, const QString & group)
{
    //DJV_DEBUG("djvPrefsDialog::addWidget");
    //DJV_DEBUG_PRINT("group = " << group);
    //DJV_DEBUG_PRINT("name = " << widget->name());

    _p->widgets += widget;
    widget->setParent(this);
    widget->hide();

    TreeWidgetItem * groupItem = 0;
    if (_p->browserGroups.contains(group))
    {
        groupItem = _p->browserGroups[group];
    }
    else
    {
        groupItem = new TreeWidgetItem;
        groupItem->setText(0, group);
        _p->browser->addTopLevelItem(groupItem);
        _p->browser->expandItem(groupItem);
        _p->browserGroups[group] = groupItem;
    }
    new TreeWidgetItem(widget, groupItem);
}

void djvPrefsDialog::browserCallback(QTreeWidgetItem * current, QTreeWidgetItem *)
{
    if (TreeWidgetItem * item = dynamic_cast<TreeWidgetItem *>(current))
    {
        if (QWidget * oldWidget = _p->scrollArea->takeWidget())
        {
            oldWidget->setParent(this);
            oldWidget->hide();
        }
        if (djvAbstractPrefsWidget * widget = item->widget())
        {
            _p->scrollArea->setWidget(widget);
            widget->show();
        }
    }
}

void djvPrefsDialog::buttonCallback(QAbstractButton * button)
{
    if (button == _p->resetAllButton)
    {
        djvQuestionDialog dialog(
            qApp->translate("djvPrefsDialog", "Are you sure you want to reset all of the preferences?"));
        if (QDialog::Accepted == dialog.exec())
        {
            Q_FOREACH(djvAbstractPrefsWidget * widget, _p->widgets)
            {
                widget->resetPreferences();
            }
        }
    }
    if (button == _p->resetPageButton)
    {
        djvQuestionDialog dialog(
            qApp->translate("djvPrefsDialog", "Are you sure you want to reset the preferences for the current page?"));

        if (QDialog::Accepted == dialog.exec())
        {
            if (TreeWidgetItem * item =
                dynamic_cast<TreeWidgetItem *>(_p->browser->currentItem()))
            {
                if (djvAbstractPrefsWidget * widget = item->widget())
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
