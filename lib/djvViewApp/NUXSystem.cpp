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

#include <djvUIComponents/LanguageSettingsWidget.h>
#include <djvUIComponents/DisplaySettingsWidget.h>

#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
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

                struct Primitive
                {
                    std::string text;
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

                _timer = Time::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<BackgroundWidget>(std::dynamic_pointer_cast<BackgroundWidget>(shared_from_this()));
                _timer->start(
                    Time::getMilliseconds(Time::TimerValue::Fast),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
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
                    AV::Font::Info fontInfo;
                    fontInfo.family = 1;
                    fontInfo.face = 1;
                    fontInfo.size = ceilf(i.size);
                    render->setCurrentFont(fontInfo);
                    render->drawText(i.text, glm::vec2(floorf(i.pos.x), floorf(i.pos.y)));
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
                while (_primitives.size() < primitiveCount)
                {
                    Primitive p;
                    p.text = "DJV";
                    p.size = Math::getRandom(512.f, 4096.f);
                    p.pos = glm::vec2(Math::getRandom(-1000.f, 1000.f), Math::getRandom(-1000.f, 1000.f));
                    p.vel = glm::vec2(Math::getRandom(-100.f, 100.f), Math::getRandom(-100.f, 100.f));
                    p.lifespan = Math::getRandom(5.f, 20.f);
                    _primitives.push_back(p);
                }
                _redraw();
            }

        } // namespace

        struct NUXWidget::Private
        {
            size_t index = 0;
            std::shared_ptr<UI::Label> titleLabel;
            std::map<std::string, std::shared_ptr<UI::Label> > labels;
            std::map < std::string, std::shared_ptr<UI::PushButton> > buttons;
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

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::FontTitle);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);

            p.labels["Language"] = UI::Label::create(context);
            p.labels["DisplaySize"] = UI::Label::create(context);
            p.labels["DisplayPalette"] = UI::Label::create(context);
            for (const auto& i : p.labels)
            {
                i.second->setFontSizeRole(UI::MetricsRole::FontLarge);
                i.second->setTextHAlign(UI::TextHAlign::Left);
            }

            p.buttons["Next"] = UI::PushButton::create(context);
            p.buttons["Prev"] = UI::PushButton::create(context);
            p.buttons["Finish"] = UI::PushButton::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setHAlign(UI::HAlign::Center);
            layout->setVAlign(UI::VAlign::Center);
            layout->addChild(p.titleLabel);
            layout->addSeparator();
            auto soloLayout = UI::SoloLayout::create(context);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.labels["Language"]);
            vLayout->addChild(languageWidget);
            soloLayout->addChild(vLayout);
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.labels["DisplaySize"]);
            vLayout->addChild(displaySizeWidget);
            soloLayout->addChild(vLayout);
            vLayout = UI::VerticalLayout::create(context);
            vLayout->addChild(p.labels["DisplayPalette"]);
            vLayout->addChild(displayPaletteWidget);
            soloLayout->addChild(vLayout);
            layout->addChild(soloLayout);
            layout->setStretch(soloLayout, UI::RowStretch::Expand);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            auto hLayout2 = UI::HorizontalLayout::create(context);
            hLayout2->addChild(p.buttons["Prev"]);
            hLayout2->addChild(p.buttons["Next"]);
            hLayout->addChild(hLayout2);
            hLayout->addExpander();
            hLayout->addChild(p.buttons["Finish"]);
            layout->addSeparator();
            layout->addChild(hLayout);

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

            std::stringstream ss;
            ss << _getText(DJV_TEXT("DJV "));
            ss << " " << DJV_VERSION;
            p.titleLabel->setText(ss.str());

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

