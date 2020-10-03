// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/NUXSystem.h>

#include <djvViewApp/NUXSettings.h>
#include <djvViewApp/NUXWidget.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct NUXSystem::Private
        {
            std::shared_ptr<NUXSettings> settings;
        };

        void NUXSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            IViewSystem::_init("djv::ViewApp::NUXSystem", context);

            DJV_PRIVATE_PTR();
            p.settings = NUXSettings::create(context);
        }

        NUXSystem::NUXSystem() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSystem> NUXSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<NUXSystem>();
            if (!out)
            {
                out = std::shared_ptr<NUXSystem>(new NUXSystem);
                out->_init(context);
            }
            return out;
        }

        std::shared_ptr<NUXWidget> NUXSystem::createNUXWidget()
        {
            DJV_PRIVATE_PTR();
            std::shared_ptr<NUXWidget> out;
            if (auto context = getContext().lock())
            {
                if (p.settings->observeNUX()->get())
                {
                    p.settings->setNUX(false);
                    out = NUXWidget::create(context);
                }
            }
            return out;
        }

    } // namespace ViewApp
} // namespace djv

