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

#include <djvViewLib/AbstractGroup.h>
#include <djvViewLib/Enum.h>

#include <memory>

class QAction;
class QDockWidget;

namespace djv
{
    namespace Core
    {
        class FileInfo;

    } // namespace Core

    namespace ViewLib
    {
        namespace Annotate
        {
            class Data;
        
        } // namespace Annotate

        //! This class provides the annotate group.
        class AnnotateGroup : public AbstractGroup
        {
            Q_OBJECT

        public:
            AnnotateGroup(
                const QPointer<AnnotateGroup> & copy,
                const QPointer<Session> &,
                const QPointer<ViewContext> &);
            ~AnnotateGroup() override;

            //! Get the list of annotations.
            const QList<Annotate::Data *> & annotations() const;

            //! Get the list of annotations for the current frame.
            const QList<Annotate::Data *> & frameAnnotations() const;

            //! Get the current annotation.
            Annotate::Data * currentAnnotation() const;

            //! Get whether the annotations are visible.
            bool areAnnotationsVisible() const;

            //! Get the annotate tool dock widget.
            QPointer<QDockWidget> annotateToolDockWidget() const;

            QPointer<QMenu> createMenu() const override;
            QPointer<QToolBar> createToolBar() const override;

        public Q_SLOTS:
            //! Create a new annotation.
            void newAnnotation(const QString & text = QString());

            //! Set the current annotation.
            void setCurrentAnnotation(djv::ViewLib::Annotate::Data *);

            //! Set the summary.
            void setSummary(const QString &);

            //! Export the annotations.
            void exportAnnotations(const djv::Core::FileInfo&, const djv::Core::FileInfo&, const QString&);

        Q_SIGNALS:
            //! This signal is emitted when the list of annotations is changed.
            void annotationsChanged(const QList<djv::ViewLib::Annotate::Data *> &);

            //! This signal is emitted when the list of annotations for the current frame is changed.
            void frameAnnotationsChanged(const QList<djv::ViewLib::Annotate::Data *> &);

            //! This signal is emitted when the current annotation is changed.
            void currentAnnotationChanged(djv::ViewLib::Annotate::Data *);

            //! This signal is emitted when an annotation is added.
            void annotationAdded(djv::ViewLib::Annotate::Data *);

            //! This signal is emitted when the annotations visibility is changed.
            void annotationsVisibleChanged(bool);

            //! This signal is emitted when the summary is changed.
            void summaryChanged(const QString&);

            //! This signal is emitted when the export file info is changed.
            void exportChanged(const djv::Core::FileInfo&);

        private Q_SLOTS:
            void deleteAnnotation();
            void deleteAllAnnotations();
            void nextAnnotation();
            void prevAnnotation();
            void undoDrawing();
            void redoDrawing();
            void clearDrawing();
            void loadAnnotations();
            void saveAnnotations();

            void pickPressedCallback(const glm::ivec2 &);
            void pickReleasedCallback(const glm::ivec2 &);
            void pickMovedCallback(const glm::ivec2 &);

            void update();

        private:
            bool doSave() const;
            glm::ivec2 transformMousePos(const glm::ivec2 &) const;

            void colorUpdate();

            DJV_PRIVATE_COPY(AnnotateGroup);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
