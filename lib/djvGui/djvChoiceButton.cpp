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

//! \file djvChoiceButton.cpp

#include <djvChoiceButton.h>

#include <djvDebug.h>
#include <djvMath.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionButton>

//------------------------------------------------------------------------------
// djvChoiceButton::P
//------------------------------------------------------------------------------

struct djvChoiceButton::P
{
    P() :
        actionGroup (0),
        currentIndex(0),
        mousePress  (0)
    {}
    
    QActionGroup * actionGroup;
    int            currentIndex;
    bool           mousePress;
};

//------------------------------------------------------------------------------
// djvChoiceButton
//------------------------------------------------------------------------------

djvChoiceButton::djvChoiceButton(QWidget * parent) :
    djvAbstractToolButton(parent),
    _p(new P)
{
    connect(
        this,
        SIGNAL(clicked()),
        SLOT(clickedCallback()));
}

djvChoiceButton::djvChoiceButton(QActionGroup * actionGroup, QWidget * parent) :
    djvAbstractToolButton(parent),
    _p(new P)
{
    setActionGroup(actionGroup);

    connect(
        this,
        SIGNAL(clicked()),
        SLOT(clickedCallback()));
}

djvChoiceButton::~djvChoiceButton()
{
    delete _p;
}

QActionGroup * djvChoiceButton::actionGroup() const
{
    return _p->actionGroup;
}

int djvChoiceButton::currentIndex() const
{
    return _p->currentIndex;
}

QSize djvChoiceButton::sizeHint() const
{
    QSize sizeHint(25, 25);

    const int margin = 2;

    if (_p->actionGroup)
    {
        QList<QAction *> actions = _p->actionGroup->actions();

        QStyleOptionToolButton opt;
        opt.iconSize = actions.count() > 0 ?
            actions[0]->icon().actualSize(sizeHint) :
            sizeHint;
        opt.iconSize += QSize(margin * 2, margin * 2);

        sizeHint = opt.iconSize.expandedTo(QApplication::globalStrut());
    }

    return sizeHint;
}

void djvChoiceButton::setActionGroup(QActionGroup * actionGroup)
{
    if (actionGroup == _p->actionGroup)
        return;

    // Disconnect the old action group.

    if (_p->actionGroup)
    {
        disconnect(
            _p->actionGroup,
            SIGNAL(triggered(QAction *)),
            this,
            SLOT(actionGroupCallback(QAction *)));
    }

    // Set the new action group.

    _p->actionGroup = actionGroup;

    if (_p->actionGroup)
    {
        connect(
            _p->actionGroup,
            SIGNAL(triggered(QAction *)),
            SLOT(actionGroupCallback(QAction *)));
    }

    // Set the current index.

    int currentIndex = _p->currentIndex;

    if (_p->actionGroup)
    {
        const QList<QAction *> actions = _p->actionGroup->actions();

        for (int i = 0; i < actions.count(); ++i)
        {
            if (actions[i]->isChecked())
            {
                currentIndex = i;

                break;
            }
        }
    }

    setCurrentIndex(currentIndex);

    // Update the widget.

    updateGeometry();
    update();
}

void djvChoiceButton::setCurrentIndex(int index)
{
    QList<QAction *> actions;
    
    if (_p->actionGroup)
        actions = _p->actionGroup->actions();

    const int tmp = djvMath::clamp(
        index,
        0,
        actions.count() - 1);

    if (tmp == _p->currentIndex)
        return;
    
    //DJV_DEBUG("djvChoiceButton::setCurrentIndex");
    //DJV_DEBUG_PRINT("index = tmp");

    _p->currentIndex = tmp;
    
    if (_p->currentIndex >= 0 && _p->currentIndex < actions.count())
    {
        actions[_p->currentIndex]->trigger();
    }

    update();
    
    Q_EMIT currentIndexChanged(_p->currentIndex);
}

void djvChoiceButton::mousePressEvent(QMouseEvent * event)
{
    QAbstractButton::mousePressEvent(event);

    _p->mousePress = true;

    update();
}

void djvChoiceButton::mouseReleaseEvent(QMouseEvent * event)
{
    QAbstractButton::mouseReleaseEvent(event);

    _p->mousePress = false;

    update();
}

void djvChoiceButton::paintEvent(QPaintEvent * event)
{
    djvAbstractToolButton::paintEvent(event);

    QPainter painter(this);
    
    // Draw the icon.

    if (_p->actionGroup)
    {
        const QList<QAction *> actions = _p->actionGroup->actions();

        if (_p->currentIndex >= 0 && _p->currentIndex < actions.count())
        {
            QIcon::Mode  mode  = QIcon::Normal;
            QIcon::State state = QIcon::Off;

            if (! isEnabled())
                mode = QIcon::Disabled;

            const QPixmap & pixmap =
                actions[_p->currentIndex]->icon().pixmap(width(), height(), mode, state);

            painter.drawPixmap(
                width () / 2 - pixmap.width () / 2,
                height() / 2 - pixmap.height() / 2,
                pixmap);
        }
    }
}

void djvChoiceButton::actionGroupCallback(QAction * action)
{
    if (_p->actionGroup)
    {
        setCurrentIndex(_p->actionGroup->actions().indexOf(action));
    }
}

void djvChoiceButton::clickedCallback()
{
    QList<QAction *> actions;
    
    if (_p->actionGroup)
    {
        actions = _p->actionGroup->actions();
    }

    const int index = djvMath::wrap<int>(
        _p->currentIndex + 1,
        0,
        actions.count() - 1);

    setCurrentIndex(index);
}
