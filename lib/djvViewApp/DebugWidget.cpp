// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/DebugWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUIComponents/LineGraphWidget.h>
#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Bellows.h>
#include <djvUI/EventSystem.h>
#include <djvUI/IconSystem.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvAV/IO.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

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
                
                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init&) override;

                virtual void _widgetUpdate() = 0;

                std::map<std::string, std::shared_ptr<UI::Text::Block> > _textBlocks;
                std::map<std::string, std::shared_ptr<UIComponents::LineGraphWidget> > _lineGraphs;
                std::map<std::string, std::shared_ptr<UIComponents::ThermometerWidget> > _thermometerWidgets;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<System::Timer> _timer;
            };
            
            IDebugWidget::~IDebugWidget()
            {}

            float IDebugWidget::getHeightForWidth(float value) const
            {
                return _layout->getHeightForWidth(value);
            }

            void IDebugWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void IDebugWidget::_layoutEvent(System::Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void IDebugWidget::_initEvent(System::Event::Init& event)
            {
                if (event.getData().text)
                {
                    _widgetUpdate();
                }
            }

            class GeneralDebugWidget : public IDebugWidget
            {
                DJV_NON_COPYABLE(GeneralDebugWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                GeneralDebugWidget();

            public:
                static std::shared_ptr<GeneralDebugWidget> create(const std::shared_ptr<System::Context>&);

            protected:
                void _widgetUpdate() override;
            };

            void GeneralDebugWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IDebugWidget::_init(context);

                setClassName("djv::ViewApp::GeneralDebugWidget");

                _textBlocks["FPS"] = UI::Text::Block::create(context);
                _lineGraphs["FPS"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["FPS"]->setPrecision(2);

                _textBlocks["TotalSystemTime"] = UI::Text::Block::create(context);
                _lineGraphs["TotalSystemTime"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["TotalSystemTime"]->setPrecision(0);

                _textBlocks["TopSystemTime"] = UI::Text::Block::create(context);
                _lineGraphs["TopSystemTime"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["TopSystemTime"]->setPrecision(0);

                _textBlocks["ObjectCount"] = UI::Text::Block::create(context);
                _lineGraphs["ObjectCount"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["ObjectCount"]->setPrecision(0);

                _textBlocks["WidgetCount"] = UI::Text::Block::create(context);
                _lineGraphs["WidgetCount"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["WidgetCount"]->setPrecision(0);

                _textBlocks["Hover"] = UI::Text::Block::create(context);
                _textBlocks["Grab"] = UI::Text::Block::create(context);
                _textBlocks["KeyGrab"] = UI::Text::Block::create(context);
                _textBlocks["TextFocus"] = UI::Text::Block::create(context);

                _textBlocks["GlyphCache"] = UI::Text::Block::create(context);
                _thermometerWidgets["GlyphCache"] = UIComponents::ThermometerWidget::create(context);

                _textBlocks["ThumbnailInfoCache"] = UI::Text::Block::create(context);
                _thermometerWidgets["ThumbnailInfoCache"] = UIComponents::ThermometerWidget::create(context);

                _textBlocks["ThumbnailImageCache"] = UI::Text::Block::create(context);
                _thermometerWidgets["ThumbnailImageCache"] = UIComponents::ThermometerWidget::create(context);

                _textBlocks["IconCache"] = UI::Text::Block::create(context);
                _thermometerWidgets["IconCache"] = UIComponents::ThermometerWidget::create(context);

                for (auto& i : _textBlocks)
                {
                    i.second->setFontFamily(Render2D::Font::familyMono);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::Margin);
                _layout->addChild(_textBlocks["FPS"]);
                _layout->addChild(_lineGraphs["FPS"]);
                _layout->addChild(_textBlocks["TotalSystemTime"]);
                _layout->addChild(_lineGraphs["TotalSystemTime"]);
                _layout->addChild(_textBlocks["TopSystemTime"]);
                _layout->addChild(_lineGraphs["TopSystemTime"]);
                _layout->addChild(_textBlocks["ObjectCount"]);
                _layout->addChild(_lineGraphs["ObjectCount"]);
                _layout->addChild(_textBlocks["WidgetCount"]);
                _layout->addChild(_lineGraphs["WidgetCount"]);
                _layout->addChild(_textBlocks["Hover"]);
                _layout->addChild(_textBlocks["Grab"]);
                _layout->addChild(_textBlocks["KeyGrab"]);
                _layout->addChild(_textBlocks["TextFocus"]);
                _layout->addChild(_textBlocks["GlyphCache"]);
                _layout->addChild(_thermometerWidgets["GlyphCache"]);
                _layout->addChild(_textBlocks["ThumbnailInfoCache"]);
                _layout->addChild(_thermometerWidgets["ThumbnailInfoCache"]);
                _layout->addChild(_textBlocks["ThumbnailImageCache"]);
                _layout->addChild(_thermometerWidgets["ThumbnailImageCache"]);
                _layout->addChild(_textBlocks["IconCache"]);
                _layout->addChild(_thermometerWidgets["IconCache"]);
                addChild(_layout);

                _timer = System::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<GeneralDebugWidget>(std::dynamic_pointer_cast<GeneralDebugWidget>(shared_from_this()));
                _timer->start(
                    System::getTimerDuration(System::TimerValue::Medium),
                    [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });
            }

            GeneralDebugWidget::GeneralDebugWidget()
            {}

            std::shared_ptr<GeneralDebugWidget> GeneralDebugWidget::create(const std::shared_ptr<System::Context>& context)
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
                    const auto& systemTickTimes = context->getSystemTickTimes();
                    Time::Duration totalSystemTime = Time::Duration::zero();
                    for (const auto& i : systemTickTimes)
                    {
                        totalSystemTime += i.second;
                    }
                    std::string topSystemTime;
                    Time::Duration topSystemTimeValue = Time::Duration::zero();
                    if (systemTickTimes.size())
                    {
                        auto i = systemTickTimes.begin();
                        topSystemTime = i->first;
                        topSystemTimeValue = i->second;
                    }
                    const size_t objectCount = IObject::getGlobalObjectCount();
                    const size_t widgetCount = UI::Widget::getGlobalWidgetCount();
                    auto eventSystem = context->getSystemT<UI::EventSystem>();
                    auto fontSystem = context->getSystemT<Render2D::Font::FontSystem>();
                    const float glyphCachePercentage = fontSystem->getGlyphCachePercentage();
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    const float thumbnailInfoCachePercentage = thumbnailSystem->getInfoCachePercentage();
                    const float thumbnailImageCachePercentage = thumbnailSystem->getImageCachePercentage();
                    auto iconSystem = context->getSystemT<UI::IconSystem>();
                    const float iconCachePercentage = iconSystem->getCachePercentage();

                    _lineGraphs["FPS"]->addSample(fps);
                    _lineGraphs["TotalSystemTime"]->addSample(totalSystemTime.count());
                    _lineGraphs["TopSystemTime"]->addSample(topSystemTimeValue.count());
                    _lineGraphs["ObjectCount"]->addSample(objectCount);
                    _lineGraphs["WidgetCount"]->addSample(widgetCount);
                    _thermometerWidgets["ThumbnailInfoCache"]->setPercentage(thumbnailInfoCachePercentage);
                    _thermometerWidgets["ThumbnailImageCache"]->setPercentage(thumbnailImageCachePercentage);
                    _thermometerWidgets["IconCache"]->setPercentage(iconCachePercentage);
                    _thermometerWidgets["GlyphCache"]->setPercentage(glyphCachePercentage);

                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_fps")) << ": ";
                        ss.precision(2);
                        ss << std::fixed << fps;
                        _textBlocks["FPS"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_total_system_time")) << ": ";
                        ss << std::fixed << totalSystemTime.count();
                        _textBlocks["TotalSystemTime"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_top_system_time")) << ": ";
                        ss << std::fixed << topSystemTime << ", " << topSystemTimeValue.count();
                        _textBlocks["TopSystemTime"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_object_count")) << ": ";
                        ss << objectCount;
                        _textBlocks["ObjectCount"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_widget_count")) << ": ";
                        ss << widgetCount;
                        _textBlocks["WidgetCount"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeHover()->get();
                        ss << _getText(DJV_TEXT("debug_general_hover")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("debug_general_hover_none")));
                        _textBlocks["Hover"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeGrab()->get();
                        ss << _getText(DJV_TEXT("debug_general_grab")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("debug_general_grab_none")));
                        _textBlocks["Grab"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        auto object = eventSystem->observeKeyGrab()->get();
                        ss << _getText(DJV_TEXT("debug_general_key_grab")) << ": " << (object ? object->getClassName() : _getText(DJV_TEXT("debug_general_key_grab_none")));
                        _textBlocks["KeyGrab"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        std::string name;
                        if (auto object = eventSystem->getTextFocus().lock())
                        {
                            name = object->getClassName();
                        }
                        else
                        {
                            name = _getText(DJV_TEXT("debug_general_text_focus_none"));
                        }
                        ss << _getText(DJV_TEXT("debug_general_text_focus")) << ": " << name;
                        _textBlocks["TextFocus"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_font_system_glyph_cache")) << ": ";
                        ss.precision(2);
                        ss << std::fixed << glyphCachePercentage << "%";
                        _textBlocks["GlyphCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_thumbnail_system_information_cache")) << ": ";
                        ss.precision(2);
                        ss << std::fixed << thumbnailInfoCachePercentage << "%";
                        _textBlocks["ThumbnailInfoCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_thumbnail_system_image_cache")) << ": ";
                        ss.precision(2);
                        ss << std::fixed << thumbnailImageCachePercentage << "%";
                        _textBlocks["ThumbnailImageCache"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_general_icon_system_cache")) << ": ";
                        ss.precision(2);
                        ss << std::fixed << iconCachePercentage << "%";
                        _textBlocks["IconCache"]->setText(ss.str());
                    }
                }
            }

            class RenderDebugWidget : public IDebugWidget
            {
                DJV_NON_COPYABLE(RenderDebugWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                RenderDebugWidget();

            public:
                static std::shared_ptr<RenderDebugWidget> create(const std::shared_ptr<System::Context>&);

            protected:
                void _widgetUpdate() override;
            };

            void RenderDebugWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                IDebugWidget::_init(context);

                setClassName("djv::ViewApp::RenderDebugWidget");

                _textBlocks["Primitives"] = UI::Text::Block::create(context);
                _lineGraphs["Primitives"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["Primitives"]->setPrecision(0);

                _textBlocks["TextureAtlas"] = UI::Text::Block::create(context);
                _thermometerWidgets["TextureAtlas"] = UIComponents::ThermometerWidget::create(context);

                _textBlocks["DynamicTextureCount"] = UI::Text::Block::create(context);
                _lineGraphs["DynamicTextureCount"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["DynamicTextureCount"]->setPrecision(0);

                _textBlocks["VBOSize"] = UI::Text::Block::create(context);
                _lineGraphs["VBOSize"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["VBOSize"]->setPrecision(0);

                for (auto& i : _textBlocks)
                {
                    i.second->setFontFamily(Render2D::Font::familyMono);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::Margin);
                _layout->addChild(_textBlocks["Primitives"]);
                _layout->addChild(_lineGraphs["Primitives"]);
                _layout->addChild(_textBlocks["TextureAtlas"]);
                _layout->addChild(_thermometerWidgets["TextureAtlas"]);
                _layout->addChild(_textBlocks["DynamicTextureCount"]);
                _layout->addChild(_lineGraphs["DynamicTextureCount"]);
                _layout->addChild(_textBlocks["VBOSize"]);
                _layout->addChild(_lineGraphs["VBOSize"]);
                addChild(_layout);

                _timer = System::Timer::create(context);
                _timer->setRepeating(true);
                auto weak = std::weak_ptr<RenderDebugWidget>(std::dynamic_pointer_cast<RenderDebugWidget>(shared_from_this()));
                _timer->start(
                    System::getTimerDuration(System::TimerValue::Medium),
                    [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });
            }

            RenderDebugWidget::RenderDebugWidget()
            {}

            std::shared_ptr<RenderDebugWidget> RenderDebugWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<RenderDebugWidget>(new RenderDebugWidget);
                out->_init(context);
                return out;
            }

            void RenderDebugWidget::_widgetUpdate()
            {
                const auto& render = _getRender();
                const size_t primitives = render->getPrimitivesCount();
                const float textureAtlasPercentage = render->getTextureAtlasPercentage();
                const size_t dynamicTextureCount = render->getDynamicTextureCount();
                const size_t vboSize = render->getVBOSize();

                _lineGraphs["Primitives"]->addSample(primitives);
                _thermometerWidgets["TextureAtlas"]->setPercentage(textureAtlasPercentage);
                _lineGraphs["DynamicTextureCount"]->addSample(dynamicTextureCount);
                _lineGraphs["VBOSize"]->addSample(vboSize);

                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("debug_render_primitives")) << ": ";
                    ss << primitives;
                    _textBlocks["Primitives"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("debug_render_texture_atlas")) << ": ";
                    ss.precision(2);
                    ss << std::fixed << textureAtlasPercentage << "%";
                    _textBlocks["TextureAtlas"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("debug_render_dynamic_texture_count")) << ": ";
                    ss << dynamicTextureCount;
                    _textBlocks["DynamicTextureCount"]->setText(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("debug_render_vbo_size")) << ": ";
                    ss << vboSize;
                    _textBlocks["VBOSize"]->setText(ss.str());
                }
            }

            class MediaDebugWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(MediaDebugWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                MediaDebugWidget();

            public:
                static std::shared_ptr<MediaDebugWidget> create(const std::shared_ptr<System::Context>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init&) override;

            private:
                void _widgetUpdate();

                Math::Frame::Sequence _sequence;
                Math::Frame::Index _currentFrame = 0;
                size_t _videoQueueMax = 0;
                size_t _videoQueueCount = 0;
                size_t _audioQueueMax = 0;
                size_t _audioQueueCount = 0;
                std::map<std::string, std::shared_ptr<UI::Text::Block> > _textBlocks;
                std::map<std::string, std::shared_ptr<UIComponents::LineGraphWidget> > _lineGraphs;
                std::shared_ptr<UI::VerticalLayout> _layout;
                std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > _currentMediaObserver;
                std::shared_ptr<Observer::Value<Math::Frame::Sequence> > _sequenceObserver;
                std::shared_ptr<Observer::Value<Math::Frame::Index> > _currentFrameObserver;
                std::shared_ptr<Observer::Value<size_t> > _videoQueueMaxObserver;
                std::shared_ptr<Observer::Value<size_t> > _videoQueueCountObserver;
                std::shared_ptr<Observer::Value<size_t> > _audioQueueMaxObserver;
                std::shared_ptr<Observer::Value<size_t> > _audioQueueCountObserver;
            };

            void MediaDebugWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::MediaDebugWidget");

                _textBlocks["CurrentFrame"] = UI::Text::Block::create(context);
                
                _textBlocks["VideoQueue"] = UI::Text::Block::create(context);
                _lineGraphs["VideoQueue"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["VideoQueue"]->setPrecision(0);

                _textBlocks["AudioQueue"] = UI::Text::Block::create(context);
                _lineGraphs["AudioQueue"] = UIComponents::LineGraphWidget::create(context);
                _lineGraphs["AudioQueue"]->setPrecision(0);

                for (auto& i : _textBlocks)
                {
                    i.second->setFontFamily(Render2D::Font::familyMono);
                }

                _layout = UI::VerticalLayout::create(context);
                _layout->setMargin(UI::MetricsRole::Margin);
                _layout->addChild(_textBlocks["CurrentFrame"]);
                _layout->addChild(_textBlocks["VideoQueue"]);
                _layout->addChild(_lineGraphs["VideoQueue"]);
                _layout->addChild(_textBlocks["AudioQueue"]);
                _layout->addChild(_lineGraphs["AudioQueue"]);
                addChild(_layout);

                auto weak = std::weak_ptr<MediaDebugWidget>(std::dynamic_pointer_cast<MediaDebugWidget>(shared_from_this()));
                if (auto fileSystem = context->getSystemT<FileSystem>())
                {
                    _currentMediaObserver = Observer::Value<std::shared_ptr<Media>>::create(
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
                                widget->_sequenceObserver = Observer::Value<Math::Frame::Sequence>::create(
                                    value->observeSequence(),
                                    [weak](const Math::Frame::Sequence& value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_sequence = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_currentFrameObserver = Observer::Value<Math::Frame::Index>::create(
                                    value->observeCurrentFrame(),
                                    [weak](Math::Frame::Index value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_currentFrame = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_videoQueueMaxObserver = Observer::Value<size_t>::create(
                                    value->observeVideoQueueMax(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_videoQueueMax = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_videoQueueCountObserver = Observer::Value<size_t>::create(
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
                                widget->_audioQueueMaxObserver = Observer::Value<size_t>::create(
                                    value->observeAudioQueueMax(),
                                    [weak](size_t value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_audioQueueMax = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_audioQueueCountObserver = Observer::Value<size_t>::create(
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
                                widget->_sequence = Math::Frame::Sequence();
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

            std::shared_ptr<MediaDebugWidget> MediaDebugWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<MediaDebugWidget>(new MediaDebugWidget);
                out->_init(context);
                return out;
            }

            void MediaDebugWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(_layout->getMinimumSize());
            }

            void MediaDebugWidget::_layoutEvent(System::Event::Layout&)
            {
                _layout->setGeometry(getGeometry());
            }

            void MediaDebugWidget::_initEvent(System::Event::Init& event)
            {
                if (event.getData().text)
                {
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_media_video_queue")) << ":";
                        _textBlocks["VideoQueue"]->setText(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << _getText(DJV_TEXT("debug_media_audio_queue")) << ":";
                        _textBlocks["AudioQueue"]->setText(ss.str());
                    }
                    _widgetUpdate();
                }
            }

            void MediaDebugWidget::_widgetUpdate()
            {
                {
                    std::stringstream ss;
                    ss << _getText(DJV_TEXT("debug_media_current_time")) << ": ";
                    ss << _currentFrame << " / " << _sequence.getFrameCount();
                    _textBlocks["CurrentFrame"]->setText(ss.str());
                }
            }

        } // namespace

        struct DebugWidget::Private
        {
            std::map < std::string, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::VerticalLayout> layout;
        };

        void DebugWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::DebugWidget");

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);

            auto generalDebugWidget = GeneralDebugWidget::create(context);
            p.bellows["General"] = UI::Bellows::create(context);
            p.bellows["General"]->addChild(generalDebugWidget);
            p.bellows["General"]->setOpen(true, false);
            p.layout->addChild(p.bellows["General"]);

            auto renderDebugWidget = RenderDebugWidget::create(context);
            p.bellows["Render"] = UI::Bellows::create(context);
            p.bellows["Render"]->addChild(renderDebugWidget);
            p.layout->addChild(p.bellows["Render"]);

            auto mediaDebugWidget = MediaDebugWidget::create(context);
            p.bellows["Media"] = UI::Bellows::create(context);
            p.bellows["Media"]->addChild(mediaDebugWidget);
            p.layout->addChild(p.bellows["Media"]);

            addChild(p.layout);
        }

        DebugWidget::DebugWidget() :
            _p(new Private)
        {}

        DebugWidget::~DebugWidget()
        {}

        std::shared_ptr<DebugWidget> DebugWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<DebugWidget>(new DebugWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> DebugWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void DebugWidget::setBellowsState(const std::map<std::string, bool>& value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : value)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second, false);
                }
            }
        }

        float DebugWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void DebugWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void DebugWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void DebugWidget::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.bellows["General"]->setText(_getText(DJV_TEXT("debug_section_general")));
                p.bellows["Render"]->setText(_getText(DJV_TEXT("debug_section_render")));
                p.bellows["Media"]->setText(_getText(DJV_TEXT("debug_section_media")));
            }
        }

    } // namespace ViewApp
} // namespace djv

