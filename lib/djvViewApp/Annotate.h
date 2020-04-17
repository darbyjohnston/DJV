// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ViewApp.h>

#include <djvAV/Color.h>

#include <glm/vec2.hpp>

#include <memory>

namespace djv
{
    namespace Core
    {
        class Context;
        
    } // namespace Core
    
    namespace AV
    {
        namespace Render2D
        {
            class Render;
            
        } // namespace Render
    } // namespace AV
    
    namespace ViewApp
    {
        enum class AnnotateType
        {
            Polyline,
            Line,
            Rectangle,
            Ellipse,
            
            Count,
            First = Polyline
        };
        DJV_ENUM_HELPERS(AnnotateType);
        
        struct AnnotateOptions
        {
            AnnotateType type = AnnotateType::First;
            AV::Image::Color color = AV::Image::Color(1.F, 1.F, 1.F);
            float lineWidth = 1.F;
        };
        
        class AnnotatePrimitive : public std::enable_shared_from_this<AnnotatePrimitive>
        {
            DJV_NON_COPYABLE(AnnotatePrimitive);
            
        protected:
            void _init(const AnnotateOptions&, const std::shared_ptr<Core::Context>&);
            AnnotatePrimitive();
            
        public:
            virtual ~AnnotatePrimitive() = 0;
            
            const AnnotateOptions& getOptions() const;
            
            virtual void draw(const std::shared_ptr<AV::Render2D::Render>&) = 0;
            
            virtual void addPoint(const glm::vec2&) = 0;
            
        protected:
            const std::weak_ptr<Core::Context>& getContext() const;
                        
        private:
            DJV_PRIVATE();
        };
        
        class AnnotateLine : public AnnotatePrimitive
        {
            DJV_NON_COPYABLE(AnnotateLine);
            
        protected:
            void _init(const AnnotateOptions&, const std::shared_ptr<Core::Context>&);
            AnnotateLine();
            
        public:
            ~AnnotateLine() override;
            
            static std::shared_ptr<AnnotateLine> create(const AnnotateOptions&, const std::shared_ptr<Core::Context>&);
            
            void draw(const std::shared_ptr<AV::Render2D::Render>&) override;
            void addPoint(const glm::vec2&) override;
            
        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
    
    picojson::value toJSON(ViewApp::AnnotateType);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::AnnotateType&);
    
    DJV_ENUM_SERIALIZE_HELPERS(ViewApp::AnnotateType);
    
} // namespace djv

