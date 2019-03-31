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

class QAction;
class QDockWidget;

namespace djv
{
    namespace AV
    {
        class Color;
    
    } // namespace AV

    namespace ViewLib
    {
        namespace Annotations
        {
            struct Data;
        
        } // namespace Annotations

        //! This class provides the annotations group.
        class AnnotationsGroup : public AbstractGroup
        {
            Q_OBJECT

        public:
            AnnotationsGroup(
                const QPointer<AnnotationsGroup> & copy,
                const QPointer<Session> &,
                const QPointer<ViewContext> &);
            ~AnnotationsGroup() override;

            //! Get the annotations dock widget.
            QPointer<QDockWidget> dockWidget() const;

            //! Add an annotation.
            void addAnnotation(const Annotations::Data &);

            //! Get the current primitive type.
            Enum::PRIMITIVE primitive() const;

            //! Get the current color.
            const AV::Color & color() const;

            //! Get the current line width.
            size_t lineWidth() const;

            QPointer<QMenu> createMenu() const override;
            QPointer<QToolBar> createToolBar() const override;

        public Q_SLOTS:
            //! Set the current primitive type.
            void setPrimitive(Enum::PRIMITIVE);

            //! Set the current color.
            void setColor(const AV::Color &);

            //! Set the current line width.
            void setLineWidth(size_t);

        Q_SIGNALS:
            //! This signal is emitted when the primitive type is changed.
            void primitiveChanged(Enum::PRIMITIVE);

            //! This signal is emitted when the color is changed.
            void colorChanged(const AV::Color &);

            //! This signal is emitted when the line width is changed.
            void lineWidthChanged(size_t);

        private Q_SLOTS:
            void update();

        private:
            DJV_PRIVATE_COPY(AnnotationsGroup);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
