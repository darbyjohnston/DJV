// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/UI.h>

#include <glm/vec2.hpp>

#include <memory>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace UI
    {
        class ITooltipWidget;
        class Window;

        //! This class provides tooltips.
        class Tooltip : public std::enable_shared_from_this<Tooltip>
        {
            DJV_NON_COPYABLE(Tooltip);

        protected:
            void _init(
                const std::shared_ptr<Window>&,
                const glm::vec2& pos,
                const std::shared_ptr<ITooltipWidget>&,
                const std::shared_ptr<Core::Context>&);
            Tooltip();

        public:
            ~Tooltip();

            static std::shared_ptr<Tooltip> create(
                const std::shared_ptr<Window>&,
                const glm::vec2& pos,
                const std::shared_ptr<ITooltipWidget>&,
                const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<ITooltipWidget>& getWidget() const;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

