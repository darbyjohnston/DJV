// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/GLFWSettingsWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/UISettings.h>

#include <djvGL/GLFWSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct GLFWWidget::Private
            {
                GL::SwapInterval swapInterval = GL::SwapInterval::Default;
                std::shared_ptr<UI::ComboBox> swapIntervalComboBox;
                std::shared_ptr<UI::FormLayout> layout;
                std::shared_ptr<Observer::Value<GL::SwapInterval> > swapIntervalObserver;
            };

            void GLFWWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IWidget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::GLFWWidget");

                p.swapIntervalComboBox = UI::ComboBox::create(context);

                p.layout = UI::FormLayout::create(context);
                p.layout->addChild(p.swapIntervalComboBox);
                addChild(p.layout);

                auto weak = std::weak_ptr<GLFWWidget>(std::dynamic_pointer_cast<GLFWWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.swapIntervalComboBox->setCallback(
                    [weak, contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto glfwSystem = context->getSystemT<GL::GLFW::GLFWSystem>();
                                glfwSystem->setSwapInterval(static_cast<GL::SwapInterval>(value));
                            }
                        }
                    });

                auto glfwSystem = context->getSystemT<GL::GLFW::GLFWSystem>();
                p.swapIntervalObserver = Observer::Value<GL::SwapInterval>::create(
                    glfwSystem->observeSwapInterval(),
                    [weak](GL::SwapInterval value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->swapInterval = value;
                            widget->_widgetUpdate();
                        }
                    });
            }

            GLFWWidget::GLFWWidget() :
                _p(new Private)
            {}

            std::shared_ptr<GLFWWidget> GLFWWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<GLFWWidget>(new GLFWWidget);
                out->_init(context);
                return out;
            }

            std::string GLFWWidget::getSettingsGroup() const
            {
                return DJV_TEXT("settings_title_glfw");
            }

            std::string GLFWWidget::getSettingsSortKey() const
            {
                return "c";
            }

            void GLFWWidget::_initEvent(System::Event::Init& event)
            {
                IWidget::_initEvent(event);
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            void GLFWWidget::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();

                std::vector<std::string> items;
                for (auto i : GL::getSwapIntervalEnums())
                {
                    std::stringstream ss;
                    ss << i;
                    items.push_back(_getText(ss.str()));
                }
                p.swapIntervalComboBox->setItems(items);
                p.swapIntervalComboBox->setCurrentItem(static_cast<int>(p.swapInterval));

                p.layout->setText(p.swapIntervalComboBox, _getText(DJV_TEXT("settings_glfw_swap_interval")) + ":");
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

