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

            //! Get the annotate tool dock widget.
            QPointer<QDockWidget> annotateToolDockWidget() const;

            QPointer<QMenu> createMenu() const override;
            QPointer<QToolBar> createToolBar() const override;

        public Q_SLOTS:
            //! Create a new annotation.
            void newAnnotation(const QString & text = QString());

            //! Delete the current annotation.
            void deleteAnnotation();

            //! Delete all of the annotations.
            void clearAnnotations();

            //! Set the current annotation.
            void setCurrentAnnotation(djv::ViewLib::Annotate::Data *);

            //! Go to the next annotation.
            void nextAnnotation();

            //! Go to the previous annotation.
            void prevAnnotation();

            //! Undo the last annotation drawing.
            void undoDrawing();

            //! Redo the last annotation drawing.
            void redoDrawing();

            //! Clear the current annotation's drawing.
            void clearDrawing();

            //! Export the annotations.
            void exportAnnotations();

        Q_SIGNALS:
            //! This signal is emitted when the list of annotations is changed.
            void annotationsChanged(const QList<djv::ViewLib::Annotate::Data *> &);

            //! This signal is emitted when the list of annotations for the current frame is changed.
            void frameAnnotationsChanged(const QList<djv::ViewLib::Annotate::Data *> &);

            //! This signal is emitted when the current annotation is changed.
            void currentAnnotationChanged(djv::ViewLib::Annotate::Data *);

            //! This signal is emitted when an annotation is added.
            void annotationAdded(djv::ViewLib::Annotate::Data *);

        private Q_SLOTS:
            void pickPressedCallback(const glm::ivec2 &);
            void pickReleasedCallback(const glm::ivec2 &);
            void pickMovedCallback(const glm::ivec2 &);

            void update();

        private:
            glm::ivec2 transformMousePos(const glm::ivec2 &) const;

            void colorUpdate();

            DJV_PRIVATE_COPY(AnnotateGroup);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
