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

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>

#include <OpenColorIO/OpenColorIO.h>

using namespace djv::Core;
namespace OCIO = OCIO_NAMESPACE;

namespace djv
{
    namespace AV
    {
        bool OCIOView::operator == (const OCIOView& other) const
        {
            return
                name == other.name &&
                colorSpace == other.colorSpace &&
                looks == other.looks;
        }

        bool OCIODisplay::operator == (const OCIODisplay& other) const
        {
            return
                name == other.name &&
                defaultView == other.defaultView &&
                views == other.views;
        }

        struct OCIOSystem::Private
        {
            OCIO::ConstConfigRcPtr ocioConfig;
            std::shared_ptr<ListSubject<std::string> > colorSpaces;
            std::shared_ptr<ListSubject<OCIODisplay> > displays;
            std::shared_ptr<ValueSubject<std::string> > defaultDisplay;
        };

        void OCIOSystem::_init(Core::Context * context)
        {
            ISystem::_init("djv::AV::OCIOSystem", context);

            addDependency(context->getSystemT<CoreSystem>());

            DJV_PRIVATE_PTR();
            std::vector<std::string> colorSpaces;
            std::vector<OCIODisplay> displays;
            std::string defaultDisplay;
            try
            {
                p.ocioConfig = OCIO::GetCurrentConfig();
                {
                    std::stringstream ss;
                    ss << "Version: " << OCIO::GetVersion();
                    _log(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Config description: " << p.ocioConfig->getDescription();
                    _log(ss.str());
                }

                for (int i = 0; i < p.ocioConfig->getNumColorSpaces(); ++i)
                {
                    const char* colorSpace = p.ocioConfig->getColorSpaceNameByIndex(i);
                    colorSpaces.push_back(colorSpace);
                    std::stringstream ss;
                    ss << "Color space: " << colorSpace;
                    _log(ss.str());
                }

                defaultDisplay = p.ocioConfig->getDefaultDisplay();
                {
                    std::stringstream ss;
                    ss << "Default display: " << defaultDisplay;
                    _log(ss.str());
                }

                for (int i = 0; i < p.ocioConfig->getNumDisplays(); ++i)
                {
                    const char* displayName = p.ocioConfig->getDisplay(i);
                    OCIODisplay display;
                    display.name = displayName;
                    display.defaultView = p.ocioConfig->getDefaultView(displayName);
                    {
                        std::stringstream ss;
                        ss << "Display: " << display.name;
                        _log(ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << "    Default view: " << display.defaultView;
                        _log(ss.str());
                    }
                    for (int j = 0; j < p.ocioConfig->getNumViews(displayName); ++j)
                    {
                        const char* viewName = p.ocioConfig->getView(displayName, j);
                        OCIOView view;
                        view.name = viewName;
                        view.colorSpace = p.ocioConfig->getDisplayColorSpaceName(displayName, viewName);
                        view.looks = p.ocioConfig->getDisplayLooks(displayName, viewName);
                        {
                            std::stringstream ss;
                            ss << "    View: " << view.name;
                            _log(ss.str());
                        }
                        {
                            std::stringstream ss;
                            ss << "        Color space: " << view.colorSpace;
                            _log(ss.str());
                        }
                        {
                            std::stringstream ss;
                            ss << "        Looks: " << view.looks;
                            _log(ss.str());
                        }
                        display.views.push_back(view);
                    }
                    displays.push_back(display);
                }
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << e.what();
                _log(ss.str(), LogLevel::Error);
            }

            p.colorSpaces = ListSubject<std::string>::create(colorSpaces);
            p.displays = ListSubject<OCIODisplay>::create(displays);
            p.defaultDisplay = ValueSubject<std::string>::create(defaultDisplay);
        }

        OCIOSystem::OCIOSystem() :
            _p(new Private)
        {}

        OCIOSystem::~OCIOSystem()
        {}

        std::shared_ptr<OCIOSystem> OCIOSystem::create(Core::Context * context)
        {
            auto out = std::shared_ptr<OCIOSystem>(new OCIOSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IListSubject<std::string> > OCIOSystem::observeColorSpaces() const
        {
            return _p->colorSpaces;
        }

        std::shared_ptr<Core::IListSubject<OCIODisplay> > OCIOSystem::observeDisplays() const
        {
            return _p->displays;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > OCIOSystem::observeDefaultDisplay() const
        {
            return _p->defaultDisplay;
        }

    } // namespace AV
} // namespace djv

