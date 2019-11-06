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
        namespace Render
        {
            class Render2D;
            
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
            
            virtual void draw(const std::shared_ptr<AV::Render::Render2D>&) = 0;
            
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
            
            void draw(const std::shared_ptr<AV::Render::Render2D>&) override;
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

