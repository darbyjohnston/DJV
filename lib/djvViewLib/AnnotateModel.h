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

#pragma once

#include <djvViewLib/ViewLib.h>

#include <djvCore/Sequence.h>
#include <djvCore/Util.h>

#include <QAbstractItemModel>

#include <memory>

namespace djv
{
    namespace ViewLib
    {
        class ViewContext;

        namespace Annotate
        {
            class Data;

        } // namespace Annotate

        //! This class provides a model for an annotation collection.
        class AnnotateModel : public QAbstractItemModel
        {
            Q_OBJECT

        public:
            explicit AnnotateModel(const QPointer<ViewContext> &, QObject * parent = nullptr);
            ~AnnotateModel() override;

            QModelIndex	index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;
            QModelIndex	parent(const QModelIndex & = QModelIndex()) const override;
            Qt::ItemFlags flags(const QModelIndex &) const override;
            QVariant data(const QModelIndex &, int role = Qt::DisplayRole) const override;
            QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const override;
            int rowCount(const QModelIndex & parent = QModelIndex()) const override;
            int columnCount(const QModelIndex & parent = QModelIndex()) const override;

        public Q_SLOTS:
            void setAnnotations(const QList<QPointer<djv::ViewLib::Annotate::Data> > &);
            void setSpeed(const djv::Core::Speed &);

        private:
            DJV_PRIVATE_COPY(AnnotateModel);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
