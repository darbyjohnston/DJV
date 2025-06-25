// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Models/OCIOModel.h>

#include <feather-tk/core/Context.h>
#include <feather-tk/core/OS.h>

#if defined(TLRENDER_OCIO)
#include <OpenColorIO/OpenColorIO.h>
#endif // TLRENDER_OCIO

#if defined(TLRENDER_OCIO)
namespace OCIO = OCIO_NAMESPACE;
#endif // TLRENDER_OCIO

namespace djv
{
    namespace app
    {
        bool OCIOModelData::operator == (const OCIOModelData& other) const
        {
            return
                enabled == other.enabled &&
                fileName == other.fileName &&
                inputs == other.inputs &&
                inputIndex == other.inputIndex &&
                displays == other.displays &&
                displayIndex == other.displayIndex &&
                views == other.views &&
                viewIndex == other.viewIndex &&
                looks == other.looks &&
                lookIndex == other.lookIndex;
        }

        bool OCIOModelData::operator != (const OCIOModelData& other) const
        {
            return !(*this == other);
        }

        struct OCIOModel::Private
        {
            std::weak_ptr<feather_tk::Context> context;
#if defined(TLRENDER_OCIO)
            OCIO_NAMESPACE::ConstConfigRcPtr ocioConfig;
#endif // TLRENDER_OCIO
            std::shared_ptr<feather_tk::ObservableValue<tl::timeline::OCIOOptions> > options;
            std::shared_ptr<feather_tk::ObservableValue<OCIOModelData> > data;
        };

        void OCIOModel::_init(const std::shared_ptr<feather_tk::Context>& context)
        {
            FEATHER_TK_P();

            p.context = context;

            p.options = feather_tk::ObservableValue<tl::timeline::OCIOOptions>::create();
            p.data = feather_tk::ObservableValue<OCIOModelData>::create();

#if defined(TLRENDER_OCIO)
            std::string env;
            if (feather_tk::getEnv("OCIO", env) && !env.empty())
            {
                try
                {
                    p.ocioConfig.reset();
                    p.ocioConfig = OCIO::Config::CreateFromEnv();
                    if (p.ocioConfig)
                    {
                        tl::timeline::OCIOOptions options;
                        options.fileName = env;
                        const char* display = p.ocioConfig->getDefaultDisplay();
                        options.display = display;
                        options.view = p.ocioConfig->getDefaultView(display);
                        p.options->setIfChanged(options);
                        p.data->setIfChanged(_getData(options));
                    }
                }
                catch (const std::exception& e)
                {
                    if (const auto context = p.context.lock())
                    {
                        context->log(std::string(), e.what(), feather_tk::LogType::Error);
                    }
                }
            }
#endif // TLRENDER_OCIO
        }

        OCIOModel::OCIOModel() :
            _p(new Private)
        {}

        OCIOModel::~OCIOModel()
        {}

        std::shared_ptr<OCIOModel> OCIOModel::create(const std::shared_ptr<feather_tk::Context>& context)
        {
            auto out = std::shared_ptr<OCIOModel>(new OCIOModel);
            out->_init(context);
            return out;
        }

        std::shared_ptr<feather_tk::IObservableValue<tl::timeline::OCIOOptions> > OCIOModel::observeOptions() const
        {
            return _p->options;
        }

        void OCIOModel::setOptions(const tl::timeline::OCIOOptions& value)
        {
            FEATHER_TK_P();
            const bool changed = value.fileName != p.options->get().fileName;
#if defined(TLRENDER_OCIO)
            if (changed)
            {
                try
                {
                    p.ocioConfig.reset();
                    p.ocioConfig = OCIO::Config::CreateFromFile(value.fileName.c_str());
                }
                catch (const std::exception& e)
                {
                }
            }
#endif // TLRENDER_OCIO
            auto options = value;
#if defined(TLRENDER_OCIO)
            if (changed && p.ocioConfig)
            {
                const char* display = p.ocioConfig->getDefaultDisplay();
                options.display = display;
                options.view = p.ocioConfig->getDefaultView(display);
            }
#endif // TLRENDER_OCIO
            p.options->setIfChanged(options);
            p.data->setIfChanged(_getData(options));
        }

        void OCIOModel::setEnabled(bool value)
        {
            FEATHER_TK_P();
            auto options = p.options->get();
            options.enabled = value;
            p.options->setIfChanged(options);
            p.data->setIfChanged(_getData(options));
        }

        void OCIOModel::setConfig(const std::string& fileName)
        {
            FEATHER_TK_P();
            const bool changed = fileName != p.options->get().fileName;
#if defined(TLRENDER_OCIO)
            if (changed)
            {
                try
                {
                    p.ocioConfig.reset();
                    p.ocioConfig = OCIO::Config::CreateFromFile(fileName.c_str());
                }
                catch (const std::exception&)
                {
                }
            }
#endif // TLRENDER_OCIO
            tl::timeline::OCIOOptions options;
            options.enabled = true;
            options.fileName = fileName;
#if defined(TLRENDER_OCIO)
            if (changed && p.ocioConfig)
            {
                const char* display = p.ocioConfig->getDefaultDisplay();
                options.display = display;
                options.view = p.ocioConfig->getDefaultView(display);
            }
#endif // TLRENDER_OCIO
            p.options->setIfChanged(options);
            p.data->setIfChanged(_getData(options));
        }

        std::shared_ptr<feather_tk::IObservableValue<OCIOModelData> > OCIOModel::observeData() const
        {
            return _p->data;
        }

        void OCIOModel::setInputIndex(size_t value)
        {
            FEATHER_TK_P();
            auto data = p.data->get();
            if (value >= 0 && value < data.inputs.size())
            {
                auto options = p.options->get();
                options.enabled = true;
                options.input = value > 0 ? data.inputs[value] : std::string();
                data = _getData(options);
                if (data.displays[data.displayIndex] != options.display)
                {
                    options.display = std::string();
                }
                if (data.views[data.viewIndex] != options.view)
                {
                    options.view = std::string();
                }
                if (data.looks[data.lookIndex] != options.look)
                {
                    options.look = std::string();
                }
                p.options->setIfChanged(options);
            }
        }

        void OCIOModel::setDisplayIndex(size_t value)
        {
            FEATHER_TK_P();
            auto data = p.data->get();
            if (value >= 0 && value < data.displays.size())
            {
                auto options = p.options->get();
                options.enabled = true;
                options.display = value > 0 ? data.displays[value] : std::string();
                data = _getData(options);
                if (data.views[data.viewIndex] != options.view)
                {
                    options.view = std::string();
                }
                if (data.looks[data.lookIndex] != options.look)
                {
                    options.look = std::string();
                }
                p.options->setIfChanged(options);
            }
        }

        void OCIOModel::setViewIndex(size_t value)
        {
            FEATHER_TK_P();
            auto data = p.data->get();
            if (value >= 0 && value < data.views.size())
            {
                auto options = p.options->get();
                options.enabled = true;
                options.view = value > 0 ? data.views[value] : std::string();
                data = _getData(options);
                if (data.looks[data.lookIndex] != options.look)
                {
                    options.look = std::string();
                }
                p.options->setIfChanged(options);
            }
        }

        void OCIOModel::setLookIndex(size_t value)
        {
            FEATHER_TK_P();
            const auto& looks = p.data->get().looks;
            if (value >= 0 && value < looks.size())
            {
                auto options = p.options->get();
                options.enabled = true;
                options.look = value > 0 ? looks[value] : std::string();
                p.options->setIfChanged(options);
            }
        }

        OCIOModelData OCIOModel::_getData(const tl::timeline::OCIOOptions& options) const
        {
            FEATHER_TK_P();
            OCIOModelData out;
            out.enabled = options.enabled;
            out.fileName = options.fileName;
#if defined(TLRENDER_OCIO)
            if (p.ocioConfig)
            {
                out.inputs.push_back("None");
                for (int i = 0; i < p.ocioConfig->getNumColorSpaces(); ++i)
                {
                    out.inputs.push_back(p.ocioConfig->getColorSpaceNameByIndex(i));
                }
                auto j = std::find(out.inputs.begin(), out.inputs.end(), options.input);
                if (j != out.inputs.end())
                {
                    out.inputIndex = j - out.inputs.begin();
                }

                out.displays.push_back("None");
                for (int i = 0; i < p.ocioConfig->getNumDisplays(); ++i)
                {
                    out.displays.push_back(p.ocioConfig->getDisplay(i));
                }
                j = std::find(out.displays.begin(), out.displays.end(), options.display);
                if (j != out.displays.end())
                {
                    out.displayIndex = j - out.displays.begin();
                }

                out.views.push_back("None");
                const std::string display = options.display;
                for (int i = 0; i < p.ocioConfig->getNumViews(display.c_str()); ++i)
                {
                    out.views.push_back(p.ocioConfig->getView(display.c_str(), i));
                }
                j = std::find(out.views.begin(), out.views.end(), options.view);
                if (j != out.views.end())
                {
                    out.viewIndex = j - out.views.begin();
                }

                out.looks.push_back("None");
                for (int i = 0; i < p.ocioConfig->getNumLooks(); ++i)
                {
                    out.looks.push_back(p.ocioConfig->getLookNameByIndex(i));
                }
                j = std::find(out.looks.begin(), out.looks.end(), options.look);
                if (j != out.looks.end())
                {
                    out.lookIndex = j - out.looks.begin();
                }
            }
#endif // TLRENDER_OCIO
            return out;
        }
    }
}
