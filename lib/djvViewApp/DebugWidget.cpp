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

#include <djvViewApp/DebugWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUIComponents/LineGraphWidget.h>
#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Bellows.h>
#include <djvUI/IconSystem.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvAV/IO.h>
#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/IEventSystem.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class IDebugWidget : public UI::Widget
            {
            public:
                virtual ~IDebugWidget() = 0;
                
            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _localeEvent(Event::Locale&) override;

                virtual void _widgetUpdate() = 0;

                std::map<std::string, std::shared_ptr<UI::Label> > _labels;
                std::map<std::string, std::shared_ptr<UI::LineGraphWidget> > _lineGraphs;
                std::map<std::string, std::shared_ptr<UI::ThermometerWidget> > _thermometerWidgets;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<Time::Timer> _timer;
            };
            
            IDebugWidget::~IDebugWidget()
            {}

            void IDebugWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void IDebugWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void IDebugWidget::_localeEvent(Event::Locale& event)
            {
                _widgetUpdate();
            }

            class GeneralDebugWidget : public IDebugWidget
            {
                DJV_NON_COPYABLE(GeneralDebugWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                GeneralDebugWidget();

            public:
                static std::shared_ptr<GeneralDebugWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _widgetUpdate() override;
            };

            void GeneralDebugWidget::_init(const std::shared_ptr<Context>& context)
            {
                IDebugWidget::_init(context);

                setClassName("djv::ViewApp::GeneralDebugWidget");

                _labels["FPS"] = UI::Label::create(context);

                _labels["ObjectCount"] = UI::Label::create(context);
                _lineGraphs["ObjectCount"] = UI::LineGraphWidget::create(context);
                _lineGraphs["ObjectCount"]->setPrecision(0);

                _labels["WidgetCount"] = UI::Label::create(context);
                _lineGraphs["WidgetCount"] = UI::LineGraphWidget::create(context);
                _lineGraphs["WidgetCount"]->setPrecision(0);

                _labels["Hover"] = UI::Label::create(context);
                _labels["Grab"] = UI::Label::create(context);
                _labels["KeyGrab"] = UI::Label::create(context);

                _labels["GlyphCache"] = UI::Label::create(context);
                _thermometerWidgets["GlyphCache"] = UI::ThermometerWidget::create(context);

                _labels["ThumbnailInfoCache"] = UI::Label::create(context);
                _thermometerWidgets["ThumbnailInfoCache"] = UI::ThermometerWidget::create(context);

                _labels["ThumbnailImageCache"] = UI::Label::create(context);
                _thermometerWidgets["ThumbnailImageCache"] = UI::ThermometerWidget::create(context);

                _labels["IconCache"] = UI::Label::create(context);
                _thermometerWidgets["IconCache"] = UI::ThermometerWidget::create(context);

                for (auto& i : _labels)
                {
                    i.second->setTextHAlign(UI::TextHAlign::Left);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::Margin);
                _layout->addChild(_labels["FPS"]);
                _layout->addChild(_labels["ObjectCount"]);
                _layout->addChild(_lineGraphs["ObjectCount"]);
                _layout->addChild(_labels["WidgetCount"]);
                _layout->addChild(_lineGraphs["WidgetCount"]);
                _layout->addChild(_labels["Hover"]);
                _layout->addChild(_labels["Grab"]);
                _layout->addChild(_labels["KeyGrab"]);
                _layout->addChild(_labels["GlyphCache"]);
                _layout->addChild(_thermometerWidgets["GlyphCache"]);
                _layout->addChild(_labels["ThumbnailInfoCache"]);
                _layout->addChild(_thermometerWidgets["ThumbnailInfoCache"]);
                _layout->addChild(_labels["ThumbnailImageCache"]);
                _layout->addChild(_thermometerWidgets["ThumbnailImageCache"]);
                _layout->addChild(_labels["IconCache"]);
                _layout->addChild(_thermometerWidgets["IconCache"]);
                addChild(_layout);

                _timer = Time::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<GeneralDebugWidget>(std::dynamic_pointer_cast<GeneralDebugWidget>(shared_from_this()));
                _timer->start(
                    Core::Time::getMilliseconds(Core::Time::TimerValue::Medium),
                    [weak](float)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });
            }

            GeneralDebugWidget::GeneralDebugWidget()
            {}

            std::shared_ptr<GeneralDebugWidget> GeneralDebugWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<GeneralDebugWidget>(new GeneralDebugWidget);
                out->_init(context);
                return out;
            }

            void GeneralDebugWidget::_widgetUpdate()
            {
                if (auto context = getContext().lock())
                {
                    const float fps = context->getFPSAverage();
                    const size_t objectCount = IObject::getGlobalObjectCount();
                    const size_t widgetCount = UI::Widget::getGlobalWidgetCount();
                    auto eventSystem = context->getSystemT<Event::IEventSystem>();
                    auto fontSystem = context->getSystemT<AV::Font::System>();
                    const float glyphCachePercentage = fontSystem->getGlyphCachePercentage();
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    const float thumbnailInfoCachePercentage = thumbnailSystem->getInfoCachePercentage();
                    const float thumbnailImageCachePercentage = thumbnailSystem->getImageCachePercentage();
                    auto iconSystem = context->getSystemT<UI::IconSystem>();
                    const float iconCachePercentage = iconSystem->getCachePercentage();

                    _lineGraphs["ObjectCount"]->addSample(objectCount);
                    _lineGraphs["WidgetCount"]->addSample(widgetCount);
                    _thermometerWidgets["ThumbnailInfoCache"]->setPercentage(thumbnailInfoCachePercentage);
                    _thermometerWidgets["ThumbnailImageCache"]->setPercentage(thumbnailImageCachePercentage);
                    _thermometerWidgets["IconCache"]->setPercentage(iconCachePercentage);
                    _thermometerWidgets["GlyphCache"]->setPercentage(glyphCachePercentage);

                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << _getText(DJV_TEXT("FPS")) << ": " << std::fixed << fps;
                        _labels["FPS"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Object count")) << ": " << objectCount;
                        _labels["ObjectCount"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("Widget count")) << ": " << widgetCount;
                        _labels["WidgetCount"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeHover()->get();
                        ss << _getText(DJV_TEXT("Hover")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("None")));
                        _labels["Hover"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeGrab()->get();
                        ss << _getText(DJV_TEXT("Grab")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("None")));
                        _labels["Grab"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeKeyGrab()->get();
                        ss << _getText(DJV_TEXT("Key grab")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("None")));
                        _labels["KeyGrab"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << _getText(DJV_TEXT("Font system glyph cache")) << ": " << std::fixed << glyphCachePercentage << "%";
                        _labels["GlyphCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << _getText(DJV_TEXT("Thumbnail system information cache")) << ": " << std::fixed << thumbnailInfoCachePercentage << "%";
                        _labels["ThumbnailInfoCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << _getText(DJV_TEXT("Thumbnail system image cache")) << ": " << std::fixed << thumbnailImageCachePercentage << "%";
                        _labels["ThumbnailImageCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss.precision(2);
                        ss << _getText(DJV_TEXT("Icon system cache")) << ": " << std::fixed << iconCachePercentage << "%";
                        _labels["IconCache"]->setText(ss.str());
                    }
                }
            }

            class RenderDebugWidget : public IDebugWidget
            {
                DJV_NON_COPYABLE(RenderDebugWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                RenderDebugWidget();

            public:
                static std::shared_ptr<RenderDebugWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _widgetUpdate() override;
            };

            void RenderDebugWidget::_init(const std::shared_ptr<Context>& context)
            {
                IDebugWidget::_init(context);

                setClassName("djv::ViewApp::RenderDebugWidget");

                _labels["TextureAtlas"] = UI::Label::create(context);
                _thermometerWidgets["TextureAtlas"] = UI::ThermometerWidget::create(context);

                _labels["DynamicTextureCount"] = UI::Label::create(context);
                _lineGraphs["DynamicTextureCount"] = UI::LineGraphWidget::create(context);
                _lineGraphs["DynamicTextureCount"]->setPrecision(0);

                _labels["VBOSize"] = UI::Label::create(context);
                _lineGraphs["VBOSize"] = UI::LineGraphWidget::create(context);
                _lineGraphs["VBOSize"]->setPrecision(0);

                for (auto& i : _labels)
                {
                    i.second->setTextHAlign(UI::TextHAlign::Left);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::Margin);
                _layout->addChild(_labels["TextureAtlas"]);
                _layout->addChild(_thermometerWidgets["TextureAtlas"]);
                _layout->addChild(_labels["DynamicTextureCount"]);
                _layout->addChild(_lineGraphs["DynamicTextureCount"]);
                _layout->addChild(_labels["VBOSize"]);
                _layout->addChild(_lineGraphs["VBOSize"]);
                addChild(_layout);

                _timer = Time::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<RenderDebugWidget>(std::dynamic_pointer_cast<RenderDebugWidget>(shared_from_this()));
                _timer->start(
                    Core::Time::getMilliseconds(Core::Time::TimerValue::Medium),
                    [weak](float)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });
            }

            RenderDebugWidget::RenderDebugWidget()
            {}

            std::shared_ptr<RenderDebugWidget> RenderDebugWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<RenderDebugWidget>(new RenderDebugWidget);
                out->_init(context);
                return out;
            }

            void RenderDebugWidget::_widgetUpdate()
            {
                auto render = _getRender();
                const float textureAtlasPercentage = render->getTextureAtlasPercentage();
                const size_t dynamicTextureCount = render->getDynamicTextureCount();
                const size_t vboSize = render->getVBOSize();

                _thermometerWidgets["TextureAtlas"]->setPercentage(textureAtlasPercentage);
                _lineGraphs["DynamicTextureCount"]->addSample(dynamicTextureCount);
                _lineGraphs["VBOSize"]->addSample(vboSize);

                {
                    std::stringstream ss;
                    ss.precision(2);
                    ss << _getText(DJV_TEXT("Texture atlas")) << ": " << std::fixed << textureAtlasPercentage << "%";
                    _labels["TextureAtlas"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Dynamic texture count")) << ": " << dynamicTextureCount;
                    _labels["DynamicTextureCount"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("VBO size")) << ": " << vboSize;
                    _labels["VBOSize"]->setText(ss.str());
                }
            }

            class MediaDebugWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(MediaDebugWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                MediaDebugWidget();

            public:
                static std::shared_ptr<MediaDebugWidget> create(const std::shared_ptr<Context>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _localeEvent(Event::Locale&) override;

            private:
                void _widgetUpdate();

                Frame::Sequence _sequence;
                Frame::Number _currentFrame = 0;
                size_t _videoQueueMax = 0;
                size_t _videoQueueCount = 0;
                size_t _audioQueueMax = 0;
                size_t _audioQueueCount = 0;
                std::map<std::string, std::shared_ptr<UI::Label> > _labels;
                std::map<std::string, std::shared_ptr<UI::LineGraphWidget> > _lineGraphs;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > _currentMediaObserver;
                std::shared_ptr<ValueObserver<Frame::Sequence> > _sequenceObserver;
                std::shared_ptr<ValueObserver<Frame::Number> > _currentFrameObserver;
                std::shared_ptr<ValueObserver<size_t> > _videoQueueMaxObserver;
                std::shared_ptr<ValueObserver<size_t> > _videoQueueCountObserver;
                std::shared_ptr<ValueObserver<size_t> > _audioQueueMaxObserver;
                std::shared_ptr<ValueObserver<size_t> > _audioQueueCountObserver;
            };

            void MediaDebugWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::MediaDebugWidget");

                _labels["CurrentFrame"] = UI::Label::create(context);
                
                _labels["VideoQueue"] = UI::Label::create(context);
                _lineGraphs["VideoQueue"] = UI::LineGraphWidget::create(context);
                _lineGraphs["VideoQueue"]->setPrecision(0);

                _labels["AudioQueue"] = UI::Label::create(context);
                _lineGraphs["AudioQueue"] = UI::LineGraphWidget::create(context);
                _lineGraphs["AudioQueue"]->setPrecision(0);

                for (auto& i : _labels)
                {
                    i.second->setTextHAlign(UI::TextHAlign::Left);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::Margin);
                _layout->addChild(_labels["CurrentFrame"]);
                _layout->addChild(_labels["VideoQueue"]);
                _layout->addChild(_lineGraphs["VideoQueue"]);
                _layout->addChild(_labels["AudioQueue"]);
                _layout->addChild(_lineGraphs["AudioQueue"]);
                addChild(_layout);

                auto weak = std::weak_ptr<MediaDebugWidget>(std::dynamic_pointer_cast<MediaDebugWidget>(shared_from_this()));
                if (auto fileSystem = context->getSystemT<FileSystem>())
                {
                    _currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                        fileSystem->observeCurrentMedia(),
                        [weak](const std::shared_ptr<Media> & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            for (const auto& i : widget->_lineGraphs)
                            {
                                i.second->resetSamples();
                            }

                            if (value)
                            {
                                widget->_sequenceObserver = ValueObserver<Frame::Sequence>::create(
                                    value->observeSequence(),
                                    [weak](const Frame::Sequence& value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_sequence = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_currentFrameObserver = ValueObserver<Frame::Number>::create(
                                    value->observeCurrentFrame(),
                                    [weak](Frame::Number value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_currentFrame = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_videoQueueMaxObserver = ValueObserver<size_t>::create(
                                    value->observeVideoQueueMax(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_videoQueueMax = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_videoQueueCountObserver = ValueObserver<size_t>::create(
                                    value->observeVideoQueueCount(),
                                    [weak](size_t value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_videoQueueCount = value;
                                            widget->_lineGraphs["VideoQueue"]->addSample(value);
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_audioQueueMaxObserver = ValueObserver<size_t>::create(
                                    value->observeAudioQueueMax(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_audioQueueMax = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_audioQueueCountObserver = ValueObserver<size_t>::create(
                                    value->observeAudioQueueCount(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_audioQueueCount = value;
                                        widget->_lineGraphs["AudioQueue"]->addSample(value);
                                        widget->_widgetUpdate();
                                    }
                                });
                            }
                            else
                            {
                                widget->_sequence = Frame::Sequence();
                                widget->_currentFrame = 0;
                                widget->_videoQueueMax = 0;
                                widget->_videoQueueCount = 0;
                                widget->_audioQueueMax = 0;
                                widget->_audioQueueCount = 0;
                                widget->_sequenceObserver.reset();
                                widget->_currentFrameObserver.reset();
                                widget->_videoQueueMaxObserver.reset();
                                widget->_videoQueueCountObserver.reset();
                                widget->_audioQueueMaxObserver.reset();
                                widget->_audioQueueCountObserver.reset();
                                widget->_widgetUpdate();
                            }
                        }
                    });
                }
            }

            MediaDebugWidget::MediaDebugWidget()
            {}

            std::shared_ptr<MediaDebugWidget> MediaDebugWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<MediaDebugWidget>(new MediaDebugWidget);
                out->_init(context);
                return out;
            }

            void MediaDebugWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void MediaDebugWidget::_layoutEvent(Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void MediaDebugWidget::_localeEvent(Event::Locale& event)
            {
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Video queue")) << ":";
                    _labels["VideoQueue"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Audio queue")) << ":";
                    _labels["AudioQueue"]->setText(ss.str());
                }
                _widgetUpdate();
            }

            void MediaDebugWidget::_widgetUpdate()
            {
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("Current time")) << ": " << _currentFrame << " / " << _sequence.getSize();
                    _labels["CurrentFrame"]->setText(ss.str());
                }
            }

        } // namespace

        struct DebugWidget::Private
        {
            std::map < std::string, std::shared_ptr<UI::Bellows> > bellows;
        };

        void DebugWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::DebugWidget");

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);

            auto generalDebugWidget = GeneralDebugWidget::create(context);
            p.bellows["General"] = UI::Bellows::create(context);
            p.bellows["General"]->addChild(generalDebugWidget);
            layout->addChild(p.bellows["General"]);

            auto renderDebugWidget = RenderDebugWidget::create(context);
            p.bellows["Render"] = UI::Bellows::create(context);
            p.bellows["Render"]->setOpen(false);
            p.bellows["Render"]->addChild(renderDebugWidget);
            layout->addChild(p.bellows["Render"]);

            auto mediaDebugWidget = MediaDebugWidget::create(context);
            p.bellows["Media"] = UI::Bellows::create(context);
            p.bellows["Media"]->setOpen(false);
            p.bellows["Media"]->addChild(mediaDebugWidget);
            layout->addChild(p.bellows["Media"]);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(layout);
            addChild(scrollWidget);
        }

        DebugWidget::DebugWidget() :
            _p(new Private)
        {}

        DebugWidget::~DebugWidget()
        {}

        std::shared_ptr<DebugWidget> DebugWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<DebugWidget>(new DebugWidget);
            out->_init(context);
            return out;
        }

        void DebugWidget::_localeEvent(Event::Locale & event)
        {
            MDIWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Debug")));
            p.bellows["General"]->setText(_getText(DJV_TEXT("General")));
            p.bellows["Render"]->setText(_getText(DJV_TEXT("Render")));
            p.bellows["Media"]->setText(_getText(DJV_TEXT("Media")));
        }

    } // namespace ViewApp
} // namespace djv

