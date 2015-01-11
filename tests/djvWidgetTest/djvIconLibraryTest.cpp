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

//! \file djvIconLibraryTest.cpp

#include <djvIconLibraryTest.h>

#include <djvIconLibrary.h>
#include <djvSearchBox.h>

#include <djvMath.h>

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QPainter>
#include <QTreeView>
#include <QVBoxLayout>

struct IconLibraryItem
{
    IconLibraryItem(
        const QPixmap & pixmap        = QPixmap(),
        const QPixmap & displayPixmap = QPixmap(),
        const QString & name          = QString()) :
        pixmap       (pixmap),
        displayPixmap(displayPixmap),
        name         (name)
    {}

    QPixmap pixmap;
    QPixmap displayPixmap;
    QString name;
};

class IconLibraryModel : public QAbstractListModel
{
public:

    explicit IconLibraryModel(QObject * parent = 0);

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    virtual QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:

    QVector<IconLibraryItem> _items;
};

IconLibraryModel::IconLibraryModel(QObject * parent) :
    QAbstractListModel(parent)
{
    const QStringList names = djvIconLibrary::global()->names();

    for (int i = 0; i < names.count(); ++i)
    {
        const QPixmap & pixmap = djvIconLibrary::global()->pixmap(names[i]);

        const QSize & defaultSize = djvIconLibrary::global()->defaultSize();

        QSize size(
            djvMath::max(pixmap.width(), defaultSize.width()),
            djvMath::max(pixmap.height(), defaultSize.height()));

        QPixmap displayPixmap(size);
        displayPixmap.fill(Qt::transparent);

        QPainter painter(&displayPixmap);
        painter.drawPixmap(
            size.width () / 2 - pixmap.width () / 2,
            size.height() / 2 - pixmap.height() / 2,
            pixmap);

        _items += IconLibraryItem(
            pixmap,
            displayPixmap,
            names[i]);
    }
}

int IconLibraryModel::rowCount(const QModelIndex & parent) const
{
    return _items.count();
}

namespace
{

enum HEADER
{
    HEADER_NAME,
    HEADER_WIDTH,
    HEADER_HEIGHT,

    HEADER_COUNT
};

const QStringList headers = QStringList() <<
    "Name" <<
    "Width" <<
    "Height";

} // namespace

int IconLibraryModel::columnCount(const QModelIndex & parent) const
{
    return HEADER_COUNT;
}

QVariant IconLibraryModel::data(const QModelIndex & index, int role) const
{
    if (index.isValid())
    {
        const int row = index.row();

        if (row >= 0 && row < _items.count())
        {
            switch (role)
            {
                case Qt::DisplayRole:

                switch (index.column())
                {
                    case HEADER_NAME:   return _items[row].name;
                    case HEADER_WIDTH:  return _items[row].pixmap.size().width();
                    case HEADER_HEIGHT: return _items[row].pixmap.size().height();
                }

                break;

            case Qt::DecorationRole:

                switch (index.column())
                {
                    case HEADER_NAME: return _items[row].displayPixmap;
                }

                break;
            }
        }
    }

    return QVariant();
}

QVariant IconLibraryModel::headerData(int section, Qt::Orientation, int role) const
{
    if (section >= 0 && section < HEADER_COUNT)
    {
        if (Qt::DisplayRole == role)
        {
            return headers[section];
        }
    }

    return QVariant();
}

void IconLibraryModel::sort(int column, Qt::SortOrder order)
{

}

QString djvIconLibraryTest::name()
{
    return "djvIconLibraryTest";
}

void djvIconLibraryTest::run(const QStringList & args)
{
    QWidget * window = new QWidget;

    IconLibraryModel * model = new IconLibraryModel(window);

    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(window);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->sort(0, Qt::AscendingOrder);

    QTreeView * view = new QTreeView;
    view->setRootIsDecorated(false);
    view->setIconSize(djvIconLibrary::global()->defaultSize());
    view->setModel(proxyModel);
    view->setSortingEnabled(true);

    view->header()->resizeSections(QHeaderView::ResizeToContents);
    view->header()->setSortIndicator(0, Qt::AscendingOrder);

    djvSearchBox * searchBox = new djvSearchBox;

    QVBoxLayout * layout = new QVBoxLayout(window);
    layout->addWidget(searchBox);
    layout->addWidget(view);

    connect(
        searchBox,
        SIGNAL(textChanged(const QString &)),
        proxyModel,
        SLOT(setFilterFixedString(const QString &)));

    window->resize(400, 600);
    window->show();
}
