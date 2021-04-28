// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! Annotation system.
        class AnnotateSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(AnnotateSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            AnnotateSystem();

        public:
            ~AnnotateSystem() override;

            static std::shared_ptr<AnnotateSystem> create(const std::shared_ptr<System::Context>&);

            int getSortKey() const override;
            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<std::shared_ptr<UI::Menu> > getMenus() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetActions() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetToolBarActions() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;
            void deleteToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            glm::vec2 _xformDrag(const glm::vec2&) const;
            void _dragStart(const glm::vec2&);
            void _dragMove(const glm::vec2&);
            void _dragEnd(const glm::vec2&);
            
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

