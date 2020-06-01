// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/GLFWSettingsWidget.h>

#include <djvUI/CheckBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/UISettings.h>

#include <djvAV/GLFWSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct GLFWSettingsWidget::Private
        {
            std::shared_ptr<CheckBox> swapIntervalCheckBox;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > swapIntervalObserver;
        };

        void GLFWSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::GLFWSettingsWidget");

            p.swapIntervalCheckBox = CheckBox::create(context);

            p.layout = VerticalLayout::create(context);
            p.layout->addChild(p.swapIntervalCheckBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<GLFWSettingsWidget>(std::dynamic_pointer_cast<GLFWSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.swapIntervalCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto glfwSystem = context->getSystemT<AV::GLFW::System>();
                            glfwSystem->setSwapInterval(value);
                        }
                    }
                });

            auto glfwSystem = context->getSystemT<AV::GLFW::System>();
            p.swapIntervalObserver = ValueObserver<bool>::create(
                glfwSystem->observeSwapInterval(),
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->swapIntervalCheckBox->setChecked(value);
                }
            });
        }

        GLFWSettingsWidget::GLFWSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<GLFWSettingsWidget> GLFWSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<GLFWSettingsWidget>(new GLFWSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string GLFWSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_glfw_section_swap_interval");
        }

        std::string GLFWSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_glfw");
        }

        std::string GLFWSettingsWidget::getSettingsSortKey() const
        {
            return "Y";
        }

        void GLFWSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.swapIntervalCheckBox->setText(_getText(DJV_TEXT("settings_glfw_swap_interval_enable")));
        }

    } // namespace UI
} // namespace djv

