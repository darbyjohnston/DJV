//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvViewApp/NUXSystem.h>

#include <djvViewApp/NUXSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/LanguageSettingsWidget.h>
#include <djvUIComponents/DisplaySettingsWidget.h>

#include <djvUI/ActionButton.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/TextSystem.h>
#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            const size_t primitiveCount = 10;
            const float  primitiveOpacity = .05f;

            class BackgroundWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(BackgroundWidget);

            protected:
                void _init(Context*);
                BackgroundWidget()
                {}

            public:
                static std::shared_ptr<BackgroundWidget> create(Context*);

            protected:
                void _paintEvent(Event::Paint&) override;

            private:
                void _primitivesUpdate(float);

                std::vector<std::shared_ptr<AV::IO::IRead> > _read;
                std::vector<std::shared_ptr<AV::Image::Image> > _images;
                struct Primitive
                {
                    std::shared_ptr<AV::Image::Image> image;
                    float size = 0.f;
                    glm::vec2 pos;
                    glm::vec2 vel;
                    float age = 0.f;
                    float lifespan = 0.f;
                };
                std::vector<Primitive> _primitives;
                std::shared_ptr<Time::Timer> _timer;
            };

            void BackgroundWidget::_init(Context* context)
            {
                Widget::_init(context);

                try
                {
                    auto io = context->getSystemT<AV::IO::System>();
                    auto resourceSystem = context->getSystemT<Core::ResourceSystem>();
                    const auto& iconsPath = resourceSystem->getPath(FileSystem::ResourcePath::IconsDirectory);
                    _read.push_back(io->read(FileSystem::Path(iconsPath, "djvLogo512.png")));
                    _read.push_back(io->read(FileSystem::Path(iconsPath, "djvLogo1024.png")));
                    _read.push_back(io->read(FileSystem::Path(iconsPath, "djvLogo2048.png")));
                }
                catch (const std::exception& e)
                {
                    auto logSystem = context->getSystemT<LogSystem>();
                    logSystem->log("djv::ViewApp::BackgroundWidget", e.what());
                }

                _timer = Time::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<BackgroundWidget>(std::dynamic_pointer_cast<BackgroundWidget>(shared_from_this()));
                _timer->start(
                    Time::getMilliseconds(Time::TimerValue::Fast),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto i = widget->_read.begin();
                        while (i != widget->_read.end())
                        {
                            bool image = false;
                            {
                                std::unique_lock<std::mutex> lock((*i)->getMutex());
                                auto& queue = (*i)->getVideoQueue();
                                if (queue.hasFrames())
                                {
                                    image = true;
                                    widget->_images.push_back(queue.popFrame().second);
                                }
                            }
                            if (image)
                            {
                                i = widget->_read.erase(i);
                            }
                            else
                            {
                                ++i;
                            }
                        }
                        widget->_primitivesUpdate(value);
                    }
                });
            }

            std::shared_ptr<BackgroundWidget> BackgroundWidget::create(Context* context)
            {
                auto out = std::shared_ptr< BackgroundWidget>(new BackgroundWidget);
                out->_init(context);
                return out;
            }

            void BackgroundWidget::_paintEvent(Event::Paint&)
            {
                auto render = _getRender();
                auto style = _getStyle();
                const BBox2f& g = getGeometry();
                render->setFillColor(_getColorWithOpacity(style->getColor(UI::ColorRole::Background)));
                render->drawRect(g);
                for (const auto& i : _primitives)
                {
                    AV::Image::Color color = _getColorWithOpacity(style->getColor(UI::ColorRole::Foreground));
                    color.setF32(color.getF32(3) * primitiveOpacity * sin(i.age / i.lifespan * Math::pi), 3);
                    render->setFillColor(color);
                    render->drawFilledImage(i.image, BBox2f(floorf(i.pos.x), floorf(i.pos.y), i.image->getWidth(), i.image->getHeight()));
                }
            }

            void BackgroundWidget::_primitivesUpdate(float value)
            {
                std::vector<std::vector<Primitive>::iterator> dead;
                for (auto i = _primitives.begin(); i != _primitives.end(); ++i)
                {
                    i->pos.x += i->vel.x * value;
                    i->pos.y += i->vel.y * value;
                    i->age += value;
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
                    p.image = _images[Math::getRandom(static_cast<int>(_images.size()) - 1)];
                    p.size = Math::getRandom(1, 4) * 512.f;
                    p.pos = glm::vec2(Math::getRandom(-1000.f, 1000.f), Math::getRandom(-1000.f, 1000.f));
                    p.vel = glm::vec2(Math::getRandom(-50.f, 50.f), Math::getRandom(-50.f, 50.f));
                    p.lifespan = Math::getRandom(5.f, 20.f);
                    _primitives.push_back(p);
                }
                _redraw();
            }

        } // namespace

        struct NUXWidget::Private
        {
            size_t index = 0;
            std::shared_ptr<UI::Icon> logoIcon;
            std::map<std::string, std::shared_ptr<UI::Label> > labels;
            std::map < std::string, std::shared_ptr<UI::PushButton> > buttons;
            std::shared_ptr<UI::PopupWidget> settingsPopupWidget;
            std::shared_ptr<UI::ActionButton> fullscreenButton;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::shared_ptr<Animation::Animation> fadeOutAnimation;
            std::function<void(void)> finishCallback;
        };

        void NUXWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            auto languageWidget = UI::LanguageWidget::create(context);
            languageWidget->setFontSizeRole(UI::MetricsRole::FontLarge);
            auto displaySizeWidget = UI::DisplaySizeWidget::create(context);
            displaySizeWidget->setFontSizeRole(UI::MetricsRole::FontLarge);
            auto displayPaletteWidget = UI::DisplayPaletteWidget::create(context);
            displayPaletteWidget->setFontSizeRole(UI::MetricsRole::FontLarge);

            p.logoIcon = UI::Icon::create(context);
            p.logoIcon->setIcon("djvLogoStartScreen");
            p.logoIcon->setHAlign(UI::HAlign::Left);

            p.labels["Language"] = UI::Label::create(context);
            p.labels["DisplaySize"] = UI::Label::create(context);
            p.labels["DisplayPalette"] = UI::Label::create(context);
            for (const auto& i : p.labels)
            {
                i.second->setTextHAlign(UI::TextHAlign::Left);
                i.second->setFontSizeRole(UI::MetricsRole::FontLarge);
            }

            p.buttons["Next"] = UI::PushButton::create(context);
            p.buttons["Prev"] = UI::PushButton::create(context);
            p.buttons["Finish"] = UI::PushButton::create(context);
            for (const auto& i : p.buttons)
            {
                i.second->setFontSizeRole(UI::MetricsRole::FontLarge);
            }

            p.fullscreenButton = UI::ActionButton::create(context);
            p.fullscreenButton->setShowShortcuts(false);
            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.fullscreenButton->addAction(windowSystem->getActions()["FullScreen"]);
            }
            p.settingsPopupWidget = UI::PopupWidget::create(context);
            p.settingsPopupWidget->setIcon("djvIconSettings");
            p.settingsPopupWidget->addChild(p.fullscreenButton);

            auto layout = UI::VerticalLayout::create(context);
            layout->setBackgroundRole(UI::ColorRole::Hovered);
            layout->setVAlign(UI::VAlign::Center);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::MetricsRole::MarginDialog);
            vLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            vLayout->setHAlign(UI::HAlign::Center);
            vLayout->addChild(p.logoIcon);
            vLayout->addSeparator();
            auto soloLayout = UI::SoloLayout::create(context);
            auto vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::MetricsRole::SpacingLarge);
            vLayout2->addChild(p.labels["Language"]);
            vLayout2->addChild(languageWidget);
            soloLayout->addChild(vLayout2);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::MetricsRole::SpacingLarge);
            vLayout2->addChild(p.labels["DisplaySize"]);
            vLayout2->addChild(displaySizeWidget);
            soloLayout->addChild(vLayout2);
            vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::MetricsRole::SpacingLarge);
            vLayout2->addChild(p.labels["DisplayPalette"]);
            vLayout2->addChild(displayPaletteWidget);
            soloLayout->addChild(vLayout2);
            vLayout->addChild(soloLayout);
            vLayout->setStretch(soloLayout, UI::RowStretch::Expand);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            hLayout->addChild(p.buttons["Prev"]);
            hLayout->addExpander();
            hLayout->addChild(p.buttons["Next"]);
            hLayout->addChild(p.buttons["Finish"]);
            vLayout->addChild(hLayout);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            hLayout->addExpander();
            hLayout->addChild(p.settingsPopupWidget);
            vLayout->addChild(hLayout);
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
            p.buttons["Next"]->setClickedCallback(
                [weak, soloLayout]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->index < 3)
                    {
                        ++widget->_p->index;
                        soloLayout->setCurrentIndex(widget->_p->index);
                    }
                    widget->_widgetUpdate();
                }
            });

            p.buttons["Prev"]->setClickedCallback(
                [weak, soloLayout]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->index > 0)
                    {
                        --widget->_p->index;
                        soloLayout->setCurrentIndex(widget->_p->index);
                    }
                    widget->_widgetUpdate();
                }
            });

            p.buttons["Finish"]->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->fadeOutAnimation->start(
                        1.f,
                        0.f,
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

            p.fadeOutAnimation = Animation::Animation::create(context);
        }

        NUXWidget::NUXWidget() :
            _p(new Private)
        {}

        std::shared_ptr<NUXWidget> NUXWidget::create(Context* context)
        {
            auto out = std::shared_ptr<NUXWidget>(new NUXWidget);
            out->_init(context);
            return out;
        }

        void NUXWidget::setFinishCallback(const std::function<void(void)>& callback)
        {
            _p->finishCallback = callback;
        }

        void NUXWidget::_preLayoutEvent(Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            _setMinimumSize(p.overlay->getMinimumSize());
        }

        void NUXWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            p.overlay->setGeometry(getGeometry());
        }

        void NUXWidget::_localeEvent(Event::Locale&)
        {
            DJV_PRIVATE_PTR();

            p.labels["Language"]->setText(_getText(DJV_TEXT("Choose your language")) + ": ");
            p.labels["DisplaySize"]->setText(_getText(DJV_TEXT("Choose a display size")) + ": ");
            p.labels["DisplayPalette"]->setText(_getText(DJV_TEXT("Choose a palette")) + ": ");
            p.buttons["Next"]->setText(_getText(DJV_TEXT("Next")));
            p.buttons["Prev"]->setText(_getText(DJV_TEXT("Previous")));
            p.buttons["Finish"]->setText(_getText(DJV_TEXT("Finish")));
        }

        void NUXWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.buttons["Next"]->setEnabled(p.index < 2);
            p.buttons["Prev"]->setEnabled(p.index > 0);
        }

        struct NUXSystem::Private
        {
            std::shared_ptr<NUXSettings> settings;
        };

        void NUXSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::NUXSystem", context);

            DJV_PRIVATE_PTR();
            p.settings = NUXSettings::create(context);
        }

        NUXSystem::NUXSystem() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSystem> NUXSystem::create(Context * context)
        {
            auto out = std::shared_ptr<NUXSystem>(new NUXSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<NUXWidget> NUXSystem::createNUXWidget()
        {
            DJV_PRIVATE_PTR();
            std::shared_ptr<NUXWidget> out;
            if (p.settings->observeNUX()->get())
            {
                p.settings->setNUX(false);
                out = NUXWidget::create(getContext());
            }
            return out;
        }

    } // namespace ViewApp
} // namespace djv

