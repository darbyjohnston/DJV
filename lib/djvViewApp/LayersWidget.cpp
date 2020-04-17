// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/LayersWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/ListWidget.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct LayersWidget::Private
        {
            std::shared_ptr<Media> currentMedia;
            AV::IO::Info info;
            size_t layer = 0;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<size_t> > layerObserver;
        };

        void LayersWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::LayersWidget");

            p.listWidget = UI::ListWidget::create(context);
            p.listWidget->setBorder(false);
            p.listWidget->setShadowOverlay({ UI::Side::Top });
            p.listWidget->setBackgroundRole(UI::ColorRole::Background);
            addChild(p.listWidget);

            auto weak = std::weak_ptr<LayersWidget>(std::dynamic_pointer_cast<LayersWidget>(shared_from_this()));
            p.listWidget->setCallback(
                [weak](int value)
                {
                    if (auto system = weak.lock())
                    {
                        if (auto media = system->_p->currentMedia)
                        {
                            media->setLayer(static_cast<size_t>(std::max(value, 0)));
                        }
                    }
                });

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->currentMedia = value;
                            if (value)
                            {
                                system->_p->infoObserver = ValueObserver<AV::IO::Info>::create(
                                    value->observeInfo(),
                                    [weak](const AV::IO::Info& value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->info = value;
                                            system->_widgetUpdate();
                                        }
                                    });
                                system->_p->layerObserver = ValueObserver<size_t>::create(
                                    value->observeLayer(),
                                    [weak](size_t value)
                                    {
                                        if (auto system = weak.lock())
                                        {
                                            system->_p->layer = value;
                                            system->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                system->_p->info = AV::IO::Info();
                                system->_p->layer = 0;
                                system->_p->infoObserver.reset();
                                system->_p->layerObserver.reset();
                                system->_widgetUpdate();
                            }
                        }
                    });
            }
        }

        LayersWidget::LayersWidget() :
            _p(new Private)
        {}

        LayersWidget::~LayersWidget()
        {}

        std::shared_ptr<LayersWidget> LayersWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<LayersWidget>(new LayersWidget);
            out->_init(context);
            return out;
        }

        void LayersWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            setTitle(_getText(DJV_TEXT("layers_title")));
            _widgetUpdate();
        }

        void LayersWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> items;
            for (const auto& i : p.info.video)
            {
                items.push_back(_getText(i.info.name));
            }
            p.listWidget->setItems(items);
            p.listWidget->setCurrentItem(p.layer);
        }

    } // namespace ViewApp
} // namespace djv

