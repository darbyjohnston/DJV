// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/NUXWidget.h>

#include <djvViewApp/MemorySettingsWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/LanguageSettingsWidget.h>
#include <djvUIComponents/StyleSettingsWidget.h>
#include <djvUIComponents/TimeSettingsWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ActionButton.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Window.h>

#include <djvRender2D/Render.h>

#include <djvAV/IOSystem.h>

#include <djvSystem/Animation.h>
#include <djvSystem/Context.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/TextSystem.h>
#include <djvSystem/TimerFunc.h>

#include <djvMath/Math.h>

#include <djvCore/RandomFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            const size_t primitiveCount = 10;
            const float  primitiveOpacity = .5F;

            class BackgroundWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(BackgroundWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                BackgroundWidget()
                {}

            public:
                static std::shared_ptr<BackgroundWidget> create(const std::shared_ptr<System::Context>&);

            protected:
                void _paintEvent(System::Event::Paint&) override;

            private:
                void _primitivesUpdate(const Time::Duration&);

                std::vector<std::shared_ptr<AV::IO::IRead> > _read;
                std::vector<std::shared_ptr<Image::Image> > _images;
                struct Primitive
                {
                    std::shared_ptr<Image::Image> image;
                    float size = 0.F;
                    glm::vec2 pos = glm::vec2(0.F, 0.F);
                    glm::vec2 vel = glm::vec2(0.F, 0.F);
                    float age = 0.F;
                    float lifespan = 0.F;
                };
                std::vector<Primitive> _primitives;
                std::shared_ptr<System::Timer> _timer;
            };

            void BackgroundWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                try
                {
                    auto io = context->getSystemT<AV::IO::IOSystem>();
                    auto resourceSystem = context->getSystemT<System::ResourceSystem>();
                    const auto& iconsPath = resourceSystem->getPath(System::File::ResourcePath::Icons);
                    _read.push_back(io->read(System::File::Path(iconsPath, "djvLogo512.png")));
                    _read.push_back(io->read(System::File::Path(iconsPath, "djvLogo1024.png")));
                    _read.push_back(io->read(System::File::Path(iconsPath, "djvLogo2048.png")));
                }
                catch (const std::exception& e)
                {
                    auto logSystem = context->getSystemT<System::LogSystem>();
                    logSystem->log("djv::ViewApp::BackgroundWidget", e.what());
                }

                _timer = System::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<BackgroundWidget>(std::dynamic_pointer_cast<BackgroundWidget>(shared_from_this()));
                _timer->start(
                    System::getTimerDuration(System::TimerValue::Fast),
                    [weak](const std::chrono::steady_clock::time_point&, const Time::Duration& dt)
                {
                    if (auto widget = weak.lock())
                    {
                        auto i = widget->_read.begin();
                        while (i != widget->_read.end())
                        {
                            bool erase = false;
                            {
                                std::unique_lock<std::mutex> lock((*i)->getMutex());
                                auto& queue = (*i)->getVideoQueue();
                                if (!queue.isEmpty())
                                {
                                    erase = true;
                                    widget->_images.push_back(queue.popFrame().image);
                                }
                                else if (queue.isFinished())
                                {
                                    erase = true;
                                }
                            }
                            if (erase)
                            {
                                i = widget->_read.erase(i);
                            }
                            else
                            {
                                ++i;
                            }
                        }
                        widget->_primitivesUpdate(dt);
                    }
                });
            }

            std::shared_ptr<BackgroundWidget> BackgroundWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr< BackgroundWidget>(new BackgroundWidget);
                out->_init(context);
                return out;
            }

            void BackgroundWidget::_paintEvent(System::Event::Paint&)
            {
                const auto& render = _getRender();
                const auto& style = _getStyle();
                const Math::BBox2f& g = getGeometry();
                render->setFillColor(style->getColor(UI::ColorRole::Background));
                render->drawRect(g);
                for (const auto& i : _primitives)
                {
                    Image::Color color = style->getColor(UI::ColorRole::Button);
                    const float v = 1.F - ((cos((i.age / i.lifespan) * Math::pi * 2.F) + 1.F) * .5F);
                    color.setF32(color.getF32(3) * primitiveOpacity * v, 3);
                    render->setFillColor(color);
                    render->drawFilledImage(i.image, i.pos);
                }
            }

            void BackgroundWidget::_primitivesUpdate(const Time::Duration& value)
            {
                std::vector<std::vector<Primitive>::iterator> dead;
                for (auto i = _primitives.begin(); i != _primitives.end(); ++i)
                {
                    const float s = std::chrono::duration<float>(value).count();
                    i->pos.x += i->vel.x * s;
                    i->pos.y += i->vel.y * s;
                    i->age += s;
                    if (i->age > i->lifespan)
                    {
                        dead.push_back(i);
                    }
                }
                for (auto i = dead.rbegin(); i != dead.rend(); ++i)
                {
                    _primitives.erase(*i);
                }
                while (_primitives.size() < primitiveCount && _images.size())
                {
                    Primitive p;
                    p.image = _images[Random::getRandom(static_cast<int>(_images.size()) - 1)];
                    p.size = Random::getRandom(1, 4) * 512.F;
                    p.pos = glm::vec2(Random::getRandom(-1000.F, 1000.F), Random::getRandom(-1000.F, 1000.F));
                    p.vel = glm::vec2(Random::getRandom(-50.F, 50.F), Random::getRandom(-50.F, 50.F));
                    p.lifespan = Random::getRandom(5.F, 20.F);
                    _primitives.push_back(p);
                }
                _redraw();
            }

        } // namespace

        struct NUXWidget::Private
        {
            std::shared_ptr<UI::Icon> logoIcon;
            std::vector<std::shared_ptr<Widget> > navWidgets;
            size_t navIndex = 0;
            std::map<std::string, std::shared_ptr<UI::Widget> > formWidgets;
            std::map<std::string, std::shared_ptr<UI::FormLayout> > formLayouts;
            std::map<std::string, std::shared_ptr<UI::PushButton> > navButtons;
            std::shared_ptr<UI::Action> fullScreenAction;
            std::shared_ptr<UI::PopupMenu> settingsPopupMenu;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::shared_ptr<System::Animation::Animation> fadeOutAnimation;
            std::function<void(void)> finishCallback;
            std::shared_ptr<Observer::Value<bool> > fullScreenObserver;
        };

        void NUXWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Window::_init(context);
            DJV_PRIVATE_PTR();

            setBackgroundRole(UI::ColorRole::None);

            p.logoIcon = UI::Icon::create(context);
            p.logoIcon->setIcon("djvLogoStartScreen");
            p.logoIcon->setHAlign(UI::HAlign::Left);

            p.formWidgets["Language"] = UIComponents::LanguageWidget::create(context);
            p.formLayouts["Language"] = UI::FormLayout::create(context);
            p.formLayouts["Language"]->addChild(p.formWidgets["Language"]);
            p.navWidgets.push_back(p.formLayouts["Language"]);

            p.formWidgets["StyleSize"] = UIComponents::SizeWidget::create(context);
            p.formLayouts["StyleSize"] = UI::FormLayout::create(context);
            p.formLayouts["StyleSize"]->addChild(p.formWidgets["StyleSize"]);
            p.navWidgets.push_back(p.formLayouts["StyleSize"]);

            p.formWidgets["StylePalette"] = UIComponents::PaletteWidget::create(context);
            p.formLayouts["StylePalette"] = UI::FormLayout::create(context);
            p.formLayouts["StylePalette"]->addChild(p.formWidgets["StylePalette"]);
            p.navWidgets.push_back(p.formLayouts["StylePalette"]);

            p.formWidgets["TimeUnits"] = UIComponents::TimeUnitsWidget::create(context);
            p.formLayouts["TimeUnits"] = UI::FormLayout::create(context);
            p.formLayouts["TimeUnits"]->addChild(p.formWidgets["TimeUnits"]);
            p.navWidgets.push_back(p.formLayouts["TimeUnits"]);

            p.formWidgets["MemoryCacheEnabled"] = MemoryCacheEnabledWidget::create(context);
            p.formWidgets["MemoryCacheSize"] = MemoryCacheSizeWidget::create(context);
            p.formLayouts["MemoryCache"] = UI::FormLayout::create(context);
            p.formLayouts["MemoryCache"]->addChild(p.formWidgets["MemoryCacheEnabled"]);
            p.formLayouts["MemoryCache"]->addChild(p.formWidgets["MemoryCacheSize"]);
            p.navWidgets.push_back(p.formLayouts["MemoryCache"]);

            p.navButtons["Next"] = UI::PushButton::create(context);
            p.navButtons["Prev"] = UI::PushButton::create(context);
            p.navButtons["Finish"] = UI::PushButton::create(context);

            p.fullScreenAction = UI::Action::create();
            p.fullScreenAction->setButtonType(UI::ButtonType::Toggle);
            p.fullScreenAction->setIcon("djvIconWindowFullScreen");
            auto settingsMenu = UI::Menu::create(context);
            settingsMenu->setIcon("djvIconSettings");
            settingsMenu->setMinimumSizeRole(UI::MetricsRole::None);
            settingsMenu->addAction(p.fullScreenAction);
            p.settingsPopupMenu = UI::PopupMenu::create(context);
            p.settingsPopupMenu->setMenu(settingsMenu);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::MetricsRole::MarginDialog);
            vLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            vLayout->setHAlign(UI::HAlign::Center);
            vLayout->addChild(p.logoIcon);
            auto soloLayout = UI::SoloLayout::create(context);
            soloLayout->addChild(p.formLayouts["Language"]);
            soloLayout->addChild(p.formLayouts["StyleSize"]);
            soloLayout->addChild(p.formLayouts["StylePalette"]);
            soloLayout->addChild(p.formLayouts["TimeUnits"]);
            soloLayout->addChild(p.formLayouts["MemoryCache"]);
            vLayout->addChild(soloLayout);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->addChild(p.navButtons["Prev"]);
            hLayout->addChild(p.navButtons["Next"]);
            hLayout->addExpander();
            hLayout->addChild(p.navButtons["Finish"]);
            hLayout->addChild(p.settingsPopupMenu);
            vLayout->addChild(hLayout);
            auto layout = UI::VerticalLayout::create(context);
            layout->setBackgroundRole(UI::ColorRole::Hovered);
            layout->setVAlign(UI::VAlign::Center);
            layout->addChild(vLayout);

            auto stackLayout = UI::StackLayout::create(context);
            auto backgroundWidget = BackgroundWidget::create(context);
            stackLayout->addChild(backgroundWidget);
            stackLayout->addChild(layout);

            p.overlay = UI::Layout::Overlay::create(context);
            p.overlay->setCaptureKeyboard(true);
            p.overlay->setCapturePointer(true);
            p.overlay->setFadeIn(false);
            p.overlay->setVisible(true);
            p.overlay->addChild(stackLayout);
            addChild(p.overlay);

            _widgetUpdate();

            auto weak = std::weak_ptr<NUXWidget>(std::dynamic_pointer_cast<NUXWidget>(shared_from_this()));
            p.navButtons["Next"]->setClickedCallback(
                [weak, soloLayout]
            {
                if (auto widget = weak.lock())
                {
                    const size_t size = widget->_p->navWidgets.size();
                    if (size > 0 && widget->_p->navIndex < size)
                    {
                        ++widget->_p->navIndex;
                        soloLayout->setCurrentWidget(widget->_p->navWidgets[widget->_p->navIndex], UI::Side::Left);
                    }
                    widget->_widgetUpdate();
                }
            });

            p.navButtons["Prev"]->setClickedCallback(
                [weak, soloLayout]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->navIndex > 0)
                    {
                        --widget->_p->navIndex;
                        soloLayout->setCurrentWidget(widget->_p->navWidgets[widget->_p->navIndex], UI::Side::Right);
                    }
                    widget->_widgetUpdate();
                }
            });

            p.navButtons["Finish"]->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->fadeOutAnimation->start(
                        1.F,
                        0.F,
                        std::chrono::milliseconds(1000),
                        [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->overlay->setOpacity(value);
                        }
                    },
                        [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->finishCallback)
                            {
                                widget->_p->finishCallback();
                            }
                        }
                    });
                }
            });

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.fullScreenAction->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto windowSystem = context->getSystemT<WindowSystem>();
                        windowSystem->setFullScreen(value);
                    }
                });

            auto windowSystem = context->getSystemT<WindowSystem>();
            p.fullScreenObserver = Observer::Value<bool>::create(
                windowSystem->observeFullScreen(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fullScreenAction->setChecked(value);
                    }
                });

            p.fadeOutAnimation = System::Animation::Animation::create(context);
        }

        NUXWidget::NUXWidget() :
            _p(new Private)
        {}

        std::shared_ptr<NUXWidget> NUXWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<NUXWidget>(new NUXWidget);
            out->_init(context);
            return out;
        }

        void NUXWidget::setFinishCallback(const std::function<void(void)>& callback)
        {
            _p->finishCallback = callback;
        }

        void NUXWidget::_initEvent(System::Event::Init& event)
        {
            Window::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.formLayouts["Language"]->setText(p.formWidgets["Language"], _getText(DJV_TEXT("settings_language")) + ":");

                p.formLayouts["StyleSize"]->setText(p.formWidgets["StyleSize"], _getText(DJV_TEXT("settings_style_size")) + ":");

                p.formLayouts["StylePalette"]->setText(p.formWidgets["StylePalette"], _getText(DJV_TEXT("settings_style_palette")) + ":");

                p.formLayouts["TimeUnits"]->setText(p.formWidgets["TimeUnits"], _getText(DJV_TEXT("settings_general_time_units")) + ":");

                p.formLayouts["MemoryCache"]->setText(p.formWidgets["MemoryCacheEnabled"], _getText(DJV_TEXT("settings_memory_cache_enabled")) + ":");
                p.formLayouts["MemoryCache"]->setText(p.formWidgets["MemoryCacheSize"], _getText(DJV_TEXT("settings_memory_cache_size")) + ":");

                p.navButtons["Next"]->setText(_getText(DJV_TEXT("startup_next")));
                p.navButtons["Prev"]->setText(_getText(DJV_TEXT("startup_previous")));
                p.navButtons["Finish"]->setText(_getText(DJV_TEXT("startup_finish")));

                p.fullScreenAction->setText(_getText(DJV_TEXT("menu_window_full_screen")));
                p.fullScreenAction->setTooltip(_getText(DJV_TEXT("menu_window_full_screen_tooltip")));
            }
        }

        void NUXWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            const size_t size = p.navWidgets.size();
            p.navButtons["Next"]->setEnabled(p.navIndex < (static_cast<int>(size) - 1));
            p.navButtons["Prev"]->setEnabled(p.navIndex > 0);
        }

    } // namespace ViewApp
} // namespace djv

