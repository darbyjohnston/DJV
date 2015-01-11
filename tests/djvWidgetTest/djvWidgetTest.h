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

//! \file djvWidgetTest.h

#ifndef DJV_WIDGET_TEST_H
#define DJV_WIDGET_TEST_H

#include <QAbstractListModel>
#include <QWidget>

class QListView;
class QSortFilterProxyModel;

class djvWidgetTest : public QObject
{
    Q_OBJECT
    
public:

    virtual QString name() = 0;

    virtual void run(const QStringList & args = QStringList()) = 0;
};

class djvWidgetTestModel : public QAbstractListModel
{
public:

    explicit djvWidgetTestModel(QObject * parent = 0);
    
    const QVector<djvWidgetTest *> & list() const;

    virtual QModelIndex	index(
        int                 row,
        int                 column,
        const QModelIndex & parent = QModelIndex()) const;
    
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole)
        const;

private:

    QVector<djvWidgetTest *> _list;
};

class djvWidgetTestWindow : public QWidget
{
    Q_OBJECT
    
public:

    djvWidgetTestWindow(djvWidgetTestModel *);

private Q_SLOTS:

    void runCallback(const QModelIndex &);
    void runCallback();
    
private:
    
    djvWidgetTestModel *    _model;
    QSortFilterProxyModel * _proxyModel;
    QListView *             _listView;
};

#endif // DJV_WIDGET_TEST_H
