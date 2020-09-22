// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Color.h>

#include <djvCore/ICommand.h>
#include <djvCore/ValueObserver.h>

#include <glm/vec2.hpp>

#include <memory>

namespace djv
{
    namespace System
    {
        class Context;
        
    } // namespace System
    
    namespace Render2D
    {
        class Render;
        
    } // namespace Render
    
    namespace ViewApp
    {
        class Media;

        enum class AnnotateTool
        {
            Polyline,
            Line,
            Rectangle,
            Ellipse,

            Count,
            First = Polyline
        };

        enum class AnnotateLineSize
        {
            Small,
            Medium,
            Large,

            Count,
            First = Small
        };

        struct AnnotateOptions
        {
            AnnotateTool tool = AnnotateTool::First;
            Image::Color color = Image::Color(1.F, 1.F, 1.F);
            float lineSize = 0.F;
        };
        
        class AnnotatePrimitive : public std::enable_shared_from_this<AnnotatePrimitive>
        {
            DJV_NON_COPYABLE(AnnotatePrimitive);
            
        protected:
            void _init(const AnnotateOptions&, const std::shared_ptr<System::Context>&);
            AnnotatePrimitive();
            
        public:
            virtual ~AnnotatePrimitive() = 0;
            
            const AnnotateOptions& getOptions() const;
            
            virtual void draw(const std::shared_ptr<Render2D::Render>&) = 0;
            
            virtual void addPoint(const glm::vec2&) = 0;
            
        protected:
            const std::weak_ptr<System::Context>& getContext() const;
                        
        private:
            DJV_PRIVATE();
        };

        class AnnotatePolyline : public AnnotatePrimitive
        {
            DJV_NON_COPYABLE(AnnotatePolyline);

        protected:
            void _init(const AnnotateOptions&, const std::shared_ptr<System::Context>&);
            AnnotatePolyline();

        public:
            ~AnnotatePolyline() override;

            static std::shared_ptr<AnnotatePolyline> create(const AnnotateOptions&, const std::shared_ptr<System::Context>&);

            void draw(const std::shared_ptr<Render2D::Render>&) override;
            void addPoint(const glm::vec2&) override;

        private:
            DJV_PRIVATE();
        };

        class AnnotateLine : public AnnotatePrimitive
        {
            DJV_NON_COPYABLE(AnnotateLine);

        protected:
            void _init(const AnnotateOptions&, const std::shared_ptr<System::Context>&);
            AnnotateLine();

        public:
            ~AnnotateLine() override;

            static std::shared_ptr<AnnotateLine> create(const AnnotateOptions&, const std::shared_ptr<System::Context>&);

            void draw(const std::shared_ptr<Render2D::Render>&) override;
            void addPoint(const glm::vec2&) override;

        private:
            DJV_PRIVATE();
        };

        class AnnotateRectangle : public AnnotatePrimitive
        {
            DJV_NON_COPYABLE(AnnotateRectangle);

        protected:
            void _init(const AnnotateOptions&, const std::shared_ptr<System::Context>&);
            AnnotateRectangle();

        public:
            ~AnnotateRectangle() override;

            static std::shared_ptr<AnnotateRectangle> create(const AnnotateOptions&, const std::shared_ptr<System::Context>&);

            void draw(const std::shared_ptr<Render2D::Render>&) override;
            void addPoint(const glm::vec2&) override;

        private:
            DJV_PRIVATE();
        };

        class AnnotateEllipse : public AnnotatePrimitive
        {
            DJV_NON_COPYABLE(AnnotateEllipse);

        protected:
            void _init(const AnnotateOptions&, const std::shared_ptr<System::Context>&);
            AnnotateEllipse();

        public:
            ~AnnotateEllipse() override;

            static std::shared_ptr<AnnotateEllipse> create(const AnnotateOptions&, const std::shared_ptr<System::Context>&);

            void draw(const std::shared_ptr<Render2D::Render>&) override;
            void addPoint(const glm::vec2&) override;

        private:
            DJV_PRIVATE();
        };

        class AnnotateCommand : public Core::ICommand
        {
            DJV_NON_COPYABLE(AnnotateCommand);

        protected:
            void _init(
                const std::shared_ptr<AnnotatePrimitive>&,
                const std::shared_ptr<Media>&);
            AnnotateCommand();

        public:
            static std::shared_ptr<AnnotateCommand> create(
                const std::shared_ptr<AnnotatePrimitive>&,
                const std::shared_ptr<Media>&);

            std::shared_ptr<Core::IValueSubject<bool> > observeUndo() const;
            const std::shared_ptr<AnnotatePrimitive>& getPrimitive() const;
            const std::weak_ptr<Media>& getMedia() const;

            void exec() override;
            void undo() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

