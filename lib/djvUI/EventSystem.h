// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/IEventSystem.h>

namespace djv
{
    namespace UI
    {
        class Widget;
        class Window;

        //! This class provides a window system interface.
        class EventSystem : public System::Event::IEventSystem
        {
            DJV_NON_COPYABLE(EventSystem);
            
        protected:
            void _init(const std::string&, const std::shared_ptr<System::Context>&);

            EventSystem();

        public:
            virtual ~EventSystem() = 0;

            void resizeRequest();
            void redrawRequest();

            bool areTooltipsEnabled() const;

            void tick() override;

        protected:
            const std::vector<std::weak_ptr<Window> >& _getWindows() const;
            void _addWindow(const std::shared_ptr<Window>&);

            bool _resizeRequestReset();
            bool _redrawRequestReset();

            virtual void _pushClipRect(const Math::BBox2f&);
            virtual void _popClipRect();

            void _initLayoutRecursive(const std::shared_ptr<Widget>&, System::Event::InitLayout&);
            void _preLayoutRecursive(const std::shared_ptr<Widget>&, System::Event::PreLayout&);
            void _layoutRecursive(const std::shared_ptr<Widget>&, System::Event::Layout&);
            void _clipRecursive(const std::shared_ptr<Widget>&, System::Event::Clip&);
            void _paintRecursive(
                const std::shared_ptr<Widget>&,
                System::Event::Paint&,
                System::Event::PaintOverlay&);

            void _init(System::Event::Init&) override;
            void _update(System::Event::Update&) override;

        private:
            DJV_PRIVATE();

            friend class Window;
        };

    } // namespace UI
} // namespace djv
