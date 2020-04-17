// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IToolSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the annotation system.
        class AnnotateSystem : public IToolSystem
        {
            DJV_NON_COPYABLE(AnnotateSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            AnnotateSystem();

        public:
            ~AnnotateSystem() override;

            static std::shared_ptr<AnnotateSystem> create(const std::shared_ptr<Core::Context>&);

            ToolActionData getToolAction() const override;
            void setCurrentTool(bool) override;

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _closeWidget(const std::string&) override;
            void _textUpdate() override;

        private:
            glm::vec2 _xformDrag(const glm::vec2&) const;
            void _dragStart(const glm::vec2&);
            void _dragMove(const glm::vec2&);
            void _dragEnd(const glm::vec2&);
            
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

