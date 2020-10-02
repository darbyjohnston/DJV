// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>

#include <djvSystem/FileInfo.h>
#include <djvSystem/RecentFilesModel.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace FileBrowser
        {
            struct RecentPathsWidget::Private
            {
                size_t textElide = 0;

                std::shared_ptr<UI::VerticalLayout> layout;

                std::function<void(const System::File::Path&)> callback;

                std::shared_ptr<Observer::List<System::File::Info> > recentPathsObserver;
            };

            void RecentPathsWidget::_init(
                const std::shared_ptr<System::File::RecentFilesModel> & model,
                size_t textElide,
                const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UIComponents::FileBrowser::RecentPathsWidget");

                p.textElide = textElide;

                p.layout = UI::VerticalLayout::create(context);
                p.layout->setSpacing(UI::MetricsRole::None);
                addChild(p.layout);

                auto weak = std::weak_ptr<RecentPathsWidget>(std::dynamic_pointer_cast<RecentPathsWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<System::Context>(context);
                p.recentPathsObserver = Observer::List<System::File::Info>::create(
                    model->observeFiles(),
                    [weak, contextWeak](const std::vector<System::File::Info> & value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->layout->clearChildren();
                                for (const auto& i : value)
                                {
                                    auto button = UI::ListButton::create(context);
                                    const auto path = i.getPath();
                                    button->setText(getPathLabel(path));
                                    button->setTextElide(widget->_p->textElide);
                                    button->setTooltip(path.get());

                                    widget->_p->layout->addChild(button);

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

            RecentPathsWidget::RecentPathsWidget() :
                _p(new Private)
            {}

            RecentPathsWidget::~RecentPathsWidget()
            {}

            std::shared_ptr<RecentPathsWidget> RecentPathsWidget::create(
                const std::shared_ptr<System::File::RecentFilesModel> & model,
                size_t textElide,
                const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<RecentPathsWidget>(new RecentPathsWidget);
                out->_init(model, textElide, context);
                return out;
            }

            void RecentPathsWidget::setCallback(const std::function<void(const System::File::Path &)> & value)
            {
                _p->callback = value;
            }

            void RecentPathsWidget::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void RecentPathsWidget::_layoutEvent(System::Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv
