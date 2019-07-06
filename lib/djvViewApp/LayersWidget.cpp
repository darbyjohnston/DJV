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

#include <djvViewApp/LayersWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

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
            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<size_t> > layerObserver;
        };

        void LayersWidget::_init(Context * context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::LayersWidget");

            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);

            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.layout);
            addChild(scrollWidget);

            auto weak = std::weak_ptr<LayersWidget>(std::dynamic_pointer_cast<LayersWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
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

        std::shared_ptr<LayersWidget> LayersWidget::create(Context * context)
        {
            auto out = std::shared_ptr<LayersWidget>(new LayersWidget);
            out->_init(context);
            return out;
        }

        void LayersWidget::_localeEvent(Event::Locale & event)
        {
            MDIWidget::_localeEvent(event);
            setTitle(_getText(DJV_TEXT("Layers")));
            _widgetUpdate();
        }

        void LayersWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.buttonGroup->clearButtons();
            p.layout->clearChildren();
            for (const auto& i : p.info.video)
            {
                auto button = UI::ListButton::create(context);
                button->setText(_getText(i.info.name));
                p.buttonGroup->addButton(button);
                p.layout->addChild(button);
            }
            p.buttonGroup->setChecked(p.layer);
        }

    } // namespace ViewApp
} // namespace djv

