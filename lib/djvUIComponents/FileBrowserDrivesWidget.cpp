// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>

#include <djvCore/DrivesModel.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct DrivesWidget::Private
            {
                std::vector<FileSystem::Path> drives;

                std::shared_ptr<VerticalLayout> layout;

                std::function<void(const FileSystem::Path &)> callback;

                std::shared_ptr<ListObserver<FileSystem::Path> > drivesObserver;
            };

            void DrivesWidget::_init(const std::shared_ptr<FileSystem::DrivesModel>& model, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::DrivesWidget");

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                addChild(p.layout);

                auto weak = std::weak_ptr<DrivesWidget>(std::dynamic_pointer_cast<DrivesWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                p.drivesObserver = ListObserver<FileSystem::Path>::create(
                    model->observeDrives(),
                    [weak, contextWeak](const std::vector<FileSystem::Path> & value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->layout->clearChildren();
                                for (const auto& i : value)
                                {
                                    auto button = ListButton::create(context);
                                    std::string s = i.getFileName();
                                    if (s.empty())
                                    {
                                        s = std::string(i);
                                    }
                                    button->setText(s);

                                    widget->_p->layout->addChild(button);

                                    const auto path = i;
                                    button->setClickedCallback(
                                        [weak, path]
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                if (widget->_p->callback)
                                                {
                                                    widget->_p->callback(path);
                                                }
                                            }
                                        });
                                }
                            }
                        }
                    });
            }

            DrivesWidget::DrivesWidget() :
                _p(new Private)
            {}

            DrivesWidget::~DrivesWidget()
            {}

            std::shared_ptr<DrivesWidget> DrivesWidget::create(const std::shared_ptr<FileSystem::DrivesModel>& model, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DrivesWidget>(new DrivesWidget);
                out->_init(model, context);
                return out;
            }

            void DrivesWidget::setCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->callback = value;
            }

            void DrivesWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void DrivesWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
