//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#pragma once

#include <djvCore/Core.h>

#include <QAbstractListModel>
#include <QFrame>
#include <QListView>
#include <QWidget>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace UIQt
    {
        class FileBrowserHistory : public QObject
        {
            Q_OBJECT
               
        public:
            FileBrowserHistory(QObject * parent = nullptr);
            ~FileBrowserHistory() override;

            const std::vector<std::string> & getHistory() const;
            size_t getHistorySize() const;
            int getHistoryIndex() const;
            bool hasBack() const;
            bool hasForward() const;

        public Q_SLOTS:
            void setPath(const std::string &);
            void back();
            void forward();

        Q_SIGNALS:
            void pathChanged(const std::string &);
            void backEnabled(bool);
            void forwardEnabled(bool);

        private:
            DJV_PRIVATE();
        };

        class FileBrowserHistoryModel : public QAbstractListModel
        {
            Q_OBJECT

        public:
            FileBrowserHistoryModel(const QPointer<FileBrowserHistory> &);
            ~FileBrowserHistoryModel() override;

            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

        private Q_SLOTS:
            void _updateModel();

        private:
            DJV_PRIVATE();
        };

        //! \todo Use a separate thread for listing directories
        //! \todo Add directory watching functionality
        class FileBrowserModel : public QAbstractListModel
        {
            Q_OBJECT

        public:
            FileBrowserModel(const std::shared_ptr<Core::Context> &, QObject * parent = nullptr);
            ~FileBrowserModel() override;

            std::string getPath() const;
            bool hasUp() const;
            bool hasBack() const;
            bool hasForward() const;
            QPointer<FileBrowserHistory> getHistory() const;

            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
            Qt::ItemFlags flags(const QModelIndex &) const;
            QStringList mimeTypes() const override;
            QMimeData * mimeData(const QModelIndexList &) const override;

        public Q_SLOTS:
            void setPath(const std::string &);
            void up();
            void back();
            void forward();

        Q_SIGNALS:
            void pathChanged(const std::string &);
            void upEnabled(bool);
            void backEnabled(bool);
            void forwardEnabled(bool);

        protected:
            void timerEvent(QTimerEvent *) override;

        private:
            DJV_PRIVATE();
        };

        class PathWidget : public QFrame
        {
            Q_OBJECT

        public:
            PathWidget(const std::shared_ptr<Core::Context> &, QWidget * parent = nullptr);
            ~PathWidget() override;

            const std::string & getPath() const;

        public Q_SLOTS:
            void setPath(const std::string &);

        Q_SIGNALS:
            void pathChanged(const std::string &);

        private Q_SLOTS:
            void _widgetUpdate();

        private:
            DJV_PRIVATE();
        };

        class FileBrowserHeader : public QWidget
        {
            Q_OBJECT

        public:
            FileBrowserHeader(const std::shared_ptr<Core::Context> &, QWidget * parent = nullptr);
            ~FileBrowserHeader() override;

            const std::string & getPath() const;

        public Q_SLOTS:
            void setPath(const std::string &);
            void setUpEnabled(bool);
            void setBackEnabled(bool);
            void setForwardEnabled(bool);

        Q_SIGNALS:
            void pathChanged(const std::string &);
            void up();
            void back();
            void forward();
            void filterChanged(const std::string &);

        private Q_SLOTS:
            void _widgetUpdate();

        private:
            DJV_PRIVATE();
        };

        class FileBrowserFooter : public QWidget
        {
            Q_OBJECT

        public:
            FileBrowserFooter(const std::shared_ptr<Core::Context> &, QWidget * parent = nullptr);
            ~FileBrowserFooter() override;

        public Q_SLOTS:
            void setViewMode(QListView::ViewMode);

        Q_SIGNALS:
            void filterChanged(const std::string &);
            void viewModeChanged(QListView::ViewMode);

        private Q_SLOTS:
            void _viewModeCallback(int);

        private:
            DJV_PRIVATE();
        };

    } // namespace UIQt
} // namespace djv
