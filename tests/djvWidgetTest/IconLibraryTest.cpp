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

#include <djvWidgetTest/IconLibraryTest.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/SearchBox.h>
#include <djvUI/UIContext.h>

#include <djvCore/Math.h>

#include <QApplication>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QPainter>
#include <QPointer>
#include <QStyle>
#include <QTreeView>
#include <QVBoxLayout>

namespace djv
{
    namespace WidgetTest
    {
        struct IconLibraryItem
        {
            IconLibraryItem(
                const QPixmap & pixmap = QPixmap(),
                const QPixmap & displayPixmap = QPixmap(),
                const QString & name = QString()) :
                pixmap(pixmap),
                displayPixmap(displayPixmap),
                name(name)
            {}

            QPixmap pixmap;
            QPixmap displayPixmap;
            QString name;
        };

        class IconLibraryModel : public QAbstractListModel
        {
        public:
            explicit IconLibraryModel(const QPointer<UI::UIContext> &, QObject * parent = nullptr);

            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            int columnCount(const QModelIndex & parent = QModelIndex()) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
            QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const override;
            void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

        private:
            QVector<IconLibraryItem> _items;
        };

        IconLibraryModel::IconLibraryModel(const QPointer<UI::UIContext> & context, QObject * parent) :
            QAbstractListModel(parent)
        {
            const QStringList names = context->iconLibrary()->names();
            for (int i = 0; i < names.count(); ++i)
            {
                const int iconSize = qApp->style()->pixelMetric(QStyle::PM_ToolBarIconSize);
                const QPixmap & pixmap = context->iconLibrary()->pixmap(names[i]);
                QSize size(
                    Core::Math::max(pixmap.width(), iconSize),
                    Core::Math::max(pixmap.height(), iconSize));
                QPixmap displayPixmap(size);
                displayPixmap.fill(Qt::transparent);
                QPainter painter(&displayPixmap);
                painter.drawPixmap(
                    size.width() / 2 - pixmap.width() / 2,
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
        {}

        IconLibraryTest::IconLibraryTest(const QPointer<UI::UIContext> & context) :
            AbstractWidgetTest(context)
        {}

        QString IconLibraryTest::name()
        {
            return "IconLibraryTest";
        }

        void IconLibraryTest::run(const QStringList & args)
        {
            QWidget * window = new QWidget;

            IconLibraryModel * model = new IconLibraryModel(context(), window);

            QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(window);
            proxyModel->setSourceModel(model);
            proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            proxyModel->sort(0, Qt::AscendingOrder);

            QTreeView * view = new QTreeView;
            view->setRootIsDecorated(false);
            view->setModel(proxyModel);
            view->setSortingEnabled(true);

            view->header()->resizeSections(QHeaderView::ResizeToContents);
            view->header()->setSortIndicator(0, Qt::AscendingOrder);

            UI::SearchBox * searchBox = new UI::SearchBox(context());

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

    } // namespace WidgetTest
} // namespace djv
