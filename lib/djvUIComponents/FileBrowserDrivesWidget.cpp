// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>

#include <djvSystem/DrivesModel.h>
#include <djvSystem/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct DrivesWidget::Private
            {
                std::vector<System::File::Path> drives;

                std::shared_ptr<VerticalLayout> layout;

                std::function<void(const System::File::Path &)> callback;

                std::shared_ptr<ListObserver<System::File::Path> > drivesObserver;
            };

            void DrivesWidget::_init(const std::shared_ptr<System::File::DrivesModel>& model, const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::DrivesWidget");

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                addChild(p.layout);

                auto weak = std::weak_ptr<DrivesWidget>(std::dynamic_pointer_cast<DrivesWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.drivesObserver = ListObserver<System::File::Path>::create(
                    model->observeDrives(),
                    [weak, contextWeak](const std::vector<System::File::Path> & value)
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

            std::shared_ptr<DrivesWidget> DrivesWidget::create(const std::shared_ptr<System::File::DrivesModel>& model, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<DrivesWidget>(new DrivesWidget);
                out->_init(model, context);
                return out;
            }

            void DrivesWidget::setCallback(const std::function<void(const System::File::Path &)> & value)
            {
                _p->callback = value;
            }

            void DrivesWidget::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void DrivesWidget::_layoutEvent(System::Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv
