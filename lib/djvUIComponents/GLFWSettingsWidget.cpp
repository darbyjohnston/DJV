// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/GLFWSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
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
            AV::SwapInterval swapInterval = AV::SwapInterval::Default;
            std::shared_ptr<ComboBox> swapIntervalComboBox;
            std::shared_ptr<FormLayout> layout;
            std::shared_ptr<ValueObserver<AV::SwapInterval> > swapIntervalObserver;
        };

        void GLFWSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::GLFWSettingsWidget");

            p.swapIntervalComboBox = ComboBox::create(context);

            p.layout = FormLayout::create(context);
            p.layout->addChild(p.swapIntervalComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<GLFWSettingsWidget>(std::dynamic_pointer_cast<GLFWSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.swapIntervalComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto glfwSystem = context->getSystemT<AV::GLFW::System>();
                            glfwSystem->setSwapInterval(static_cast<AV::SwapInterval>(value));
                        }
                    }
                });

            auto glfwSystem = context->getSystemT<AV::GLFW::System>();
            p.swapIntervalObserver = ValueObserver<AV::SwapInterval>::create(
                glfwSystem->observeSwapInterval(),
                [weak](AV::SwapInterval value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->swapInterval = value;
                    widget->_widgetUpdate();
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

        void GLFWSettingsWidget::setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>& value)
        {
            _p->layout->setLabelSizeGroup(value);
        }

        void GLFWSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void GLFWSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            std::vector<std::string> items;
            for (auto i : AV::getSwapIntervalEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.swapIntervalComboBox->setItems(items);
            p.swapIntervalComboBox->setCurrentItem(static_cast<int>(p.swapInterval));

            p.layout->setText(p.swapIntervalComboBox, _getText(DJV_TEXT("settings_glfw_swap_interval_setting")) + ":");
        }

    } // namespace UI
} // namespace djv

