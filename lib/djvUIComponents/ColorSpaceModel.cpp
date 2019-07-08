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

#include <djvUIComponents/ColorSpaceModel.h>

#include <djvUI/ColorSpaceSettings.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ColorSpaceModel::Private
        {
            std::vector<AV::OCIO::Display> ocioDisplays;

            std::shared_ptr<ListSubject<std::string> > colorSpaces;
            std::shared_ptr<ListSubject<std::string> > displays;
            std::shared_ptr<ListSubject<std::string> > views;
            std::shared_ptr<ValueSubject<std::string> > colorSpace;
            std::shared_ptr<ValueSubject<std::string> > display;
            std::shared_ptr<ValueSubject<std::string> > view;
            std::shared_ptr<ValueSubject<std::string> > outputColorSpace;

            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<ListObserver<AV::OCIO::Display> > colorDisplaysObserver;
        };

        void ColorSpaceModel::_init(Context* context)
        {
            DJV_PRIVATE_PTR();
            p.colorSpaces = ListSubject<std::string>::create();
            p.displays = ListSubject<std::string>::create();
            p.views = ListSubject<std::string>::create();
            p.colorSpace = ValueSubject<std::string>::create();
            p.display = ValueSubject<std::string>::create();
            p.view = ValueSubject<std::string>::create();
            p.outputColorSpace = ValueSubject<std::string>::create();

            auto settingsSystem = context->getSystemT<Settings::System>();
            auto colorSpaceSettings = settingsSystem->getSettingsT<Settings::ColorSpace>();
            p.colorSpace->setIfChanged(colorSpaceSettings->observeColorSpace()->get());
            p.display->setIfChanged(colorSpaceSettings->observeDisplay()->get());
            p.view->setIfChanged(colorSpaceSettings->observeView()->get());
            p.outputColorSpace->setIfChanged(colorSpaceSettings->observeOutputColorSpace()->get());

            auto weak = std::weak_ptr<ColorSpaceModel>(std::dynamic_pointer_cast<ColorSpaceModel>(shared_from_this()));
            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.colorSpacesObserver = ListObserver<std::string>::create(
                ocioSystem->observeColorSpaces(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto model = weak.lock())
                    {
                        std::vector<std::string> colorSpaces;
                        colorSpaces.push_back("");
                        std::string current;
                        for (const auto& j : value)
                        {
                            colorSpaces.push_back(j);
                            if (model->_p->colorSpace->get() == j)
                            {
                                current = j;
                            }
                        }
                        model->_p->colorSpaces->setIfChanged(colorSpaces);
                        model->_p->colorSpace->setIfChanged(current);
                    }
                });

            p.colorDisplaysObserver = ListObserver<AV::OCIO::Display>::create(
                ocioSystem->observeDisplays(),
                [weak](const std::vector<AV::OCIO::Display>& value)
                {
                    if (auto model = weak.lock())
                    {
                        model->_p->ocioDisplays.clear();
                        model->_p->ocioDisplays.push_back(AV::OCIO::Display());
                        for (const auto& i : value)
                        {
                            model->_p->ocioDisplays.push_back(i);
                        }
                        model->_modelUpdate();
                    }
                });
        }

        ColorSpaceModel::ColorSpaceModel() :
            _p(new Private)
        {}

        ColorSpaceModel::~ColorSpaceModel()
        {}

        std::shared_ptr<ColorSpaceModel> ColorSpaceModel::create(Context* context)
        {
            auto out = std::shared_ptr<ColorSpaceModel>(new ColorSpaceModel);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IListSubject<std::string> > ColorSpaceModel::observeColorSpaces() const
        {
            return _p->colorSpaces;
        }

        std::shared_ptr<Core::IListSubject<std::string> > ColorSpaceModel::observeDisplays() const
        {
            return _p->displays;
        }

        std::shared_ptr<Core::IListSubject<std::string> > ColorSpaceModel::observeViews() const
        {
            return _p->views;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ColorSpaceModel::observeColorSpace() const
        {
            return _p->colorSpace;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ColorSpaceModel::observeDisplay() const
        {
            return _p->display;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ColorSpaceModel::observeView() const
        {
            return _p->view;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ColorSpaceModel::observeOutputColorSpace() const
        {
            return _p->outputColorSpace;
        }

        void ColorSpaceModel::setColorSpace(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            _p->colorSpace->setIfChanged(value);
        }

        void ColorSpaceModel::setDisplay(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (_p->display->setIfChanged(value))
            {
                _modelUpdate();
            }
        }

        void ColorSpaceModel::setView(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            if (_p->view->setIfChanged(value))
            {
                _modelUpdate();
            }
        }

        int ColorSpaceModel::colorSpaceToIndex(const std::string& value) const
        {
            DJV_PRIVATE_PTR();
            const size_t i = p.colorSpaces->indexOf(value);
            return i != invalidListIndex ? static_cast<int>(i) : -1;
        }

        int ColorSpaceModel::displayToIndex(const std::string& value) const
        {
            DJV_PRIVATE_PTR();
            const size_t i = p.displays->indexOf(value);
            return i != invalidListIndex ? static_cast<int>(i) : -1;
        }

        int ColorSpaceModel::viewToIndex(const std::string& value) const
        {
            DJV_PRIVATE_PTR();
            const size_t i = p.views->indexOf(value);
            return i != invalidListIndex ? static_cast<int>(i) : -1;
        }

        std::string ColorSpaceModel::indexToColorSpace(int value) const
        {
            DJV_PRIVATE_PTR();
            return value >= 0 && value < p.colorSpaces->getSize() ? p.colorSpaces->getItem(value) : std::string();
        }

        std::string ColorSpaceModel::indexToDisplay(int value) const
        {
            DJV_PRIVATE_PTR();
            return value >= 0 && value < p.displays->getSize() ? p.displays->getItem(value) : std::string();
        }

        std::string ColorSpaceModel::indexToView(int value) const
        {
            DJV_PRIVATE_PTR();
            return value >= 0 && value < p.views->getSize() ? p.views->getItem(value) : std::string();
        }

        void ColorSpaceModel::_modelUpdate()
        {
            DJV_PRIVATE_PTR();

            std::vector<std::string> displays;
            std::vector<std::string> views;
            std::string display;
            std::string view;
            std::string outputColorSpace;
            for (size_t i = 0; i < p.ocioDisplays.size(); ++i)
            {
                const auto& d = p.ocioDisplays[i];
                displays.push_back(d.name);
                if (p.display->get() == d.name)
                {
                    display = d.name;
                    for (size_t j  = 0; j < d.views.size(); ++j)
                    {
                        const auto& v = d.views[j];
                        views.push_back(v.name);
                        if (p.view->get() == v.name)
                        {
                            view = v.name;
                            outputColorSpace = v.colorSpace;
                        }
                    }
                }
            }

            p.displays->setIfChanged(displays);
            p.display->setIfChanged(display);
            p.views->setIfChanged(views);
            p.view->setIfChanged(view);
            p.outputColorSpace->setIfChanged(outputColorSpace);
        }

    } // namespace UI
} // namespace djv

