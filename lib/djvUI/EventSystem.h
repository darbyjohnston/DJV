// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/IEventSystem.h>

namespace djv
{
    namespace UI
    {
        class Widget;
        class Window;

        //! This class provides a window system interface.
        class EventSystem : public Core::Event::IEventSystem
        {
            DJV_NON_COPYABLE(EventSystem);
            
        protected:
            void _init(const std::string &, const std::shared_ptr<Core::Context>&);

            EventSystem();

        public:
            virtual ~EventSystem() = 0;

            void tick() override;

        protected:
            virtual void _pushClipRect(const Core::BBox2f &);
            virtual void _popClipRect();

            bool _resizeRequest(const std::shared_ptr<Widget> &) const;
            bool _redrawRequest(const std::shared_ptr<Widget> &) const;

            void _initLayoutRecursive(const std::shared_ptr<Widget>&, Core::Event::InitLayout&);
            void _preLayoutRecursive(const std::shared_ptr<Widget>&, Core::Event::PreLayout&);
            void _layoutRecursive(const std::shared_ptr<Widget> &, Core::Event::Layout &);
            void _clipRecursive(const std::shared_ptr<Widget> &, Core::Event::Clip &);
            void _paintRecursive(
                const std::shared_ptr<Widget> &,
                Core::Event::Paint &,
                Core::Event::PaintOverlay&);

            void _initObject(const std::shared_ptr<Core::IObject> &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
