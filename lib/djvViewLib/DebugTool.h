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

#include <djvViewLib/ToolObject.h>

#include <djvAV/Time.h>

#include <QAbstractTableModel>

namespace djv
{
    namespace ViewLib
    {
        class Media;

        class DebugModel : public QAbstractTableModel
        {
            Q_OBJECT

        public:
            DebugModel(QObject * parent = nullptr);
            ~DebugModel() override;

            void setMedia(const std::shared_ptr<Media>&);

            int columnCount(const QModelIndex & parent = QModelIndex()) const override;
            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

        private Q_SLOTS:
            void _updateModel();

        private:
            DJV_PRIVATE();
        };

        class DebugTool : public IToolObject
        {
            Q_OBJECT

        public:
            DebugTool(const std::shared_ptr<Context> &, QObject * parent = nullptr);
            ~DebugTool() override;

            QPointer<QDockWidget> createDockWidget() override;
            std::string getDockWidgetSortKey() const override;
            Qt::DockWidgetArea getDockWidgetArea() const override;

        public Q_SLOTS:
            void setCurrentMedia(const std::shared_ptr<Media> &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv

