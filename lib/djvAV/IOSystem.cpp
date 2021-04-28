// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/IOSystem.h>

#include <djvAV/Cineon.h>
#include <djvAV/DPX.h>
#include <djvAV/IFF.h>
#include <djvAV/PFM.h>
#include <djvAV/PPM.h>
#include <djvAV/RLA.h>
#include <djvAV/SGI.h>
#include <djvAV/Targa.h>

#if defined(FFmpeg_FOUND)
#include <djvAV/FFmpeg.h>
#endif // FFmpeg_FOUND
#if defined(JPEG_FOUND)
#include <djvAV/JPEG.h>
#endif // JPEG_FOUND
#if defined(OpenEXR_FOUND)
#include <djvAV/OpenEXR.h>
#endif // OpenEXR_FOUND
#if defined(PNG_FOUND)
#include <djvAV/PNG.h>
#endif // PNG_FOUND
#if defined(TIFF_FOUND)
#include <djvAV/TIFF.h>
#endif // TIFF_FOUND

#include <djvGL/GLFWSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/File.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            struct IOSystem::Private
            {
                std::shared_ptr<System::TextSystem> textSystem;
                std::shared_ptr<Observer::ValueSubject<bool> > optionsChanged;
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
                std::set<std::string> sequenceExtensions;
                std::set<std::string> nonSequenceExtensions;
            };

            void IOSystem::_init(const std::shared_ptr<System::Context>& context)
            {
                ISystem::_init("djv::AV::IO::IOSystem", context);

                DJV_PRIVATE_PTR();

                addDependency(GL::GLFW::GLFWSystem::create(context));

                p.textSystem = context->getSystemT<System::TextSystem>();

                p.optionsChanged = Observer::ValueSubject<bool>::create();

                p.plugins[Cineon::pluginName] = Cineon::Plugin::create(context);
                p.plugins[DPX::pluginName] = DPX::Plugin::create(context);
                p.plugins[IFF::pluginName] = IFF::Plugin::create(context);
                p.plugins[PFM::pluginName] = PFM::Plugin::create(context);
                p.plugins[PPM::pluginName] = PPM::Plugin::create(context);
                p.plugins[RLA::pluginName] = RLA::Plugin::create(context);
                p.plugins[SGI::pluginName] = SGI::Plugin::create(context);
                p.plugins[Targa::pluginName] = Targa::Plugin::create(context);
#if defined(FFmpeg_FOUND)
                p.plugins[FFmpeg::pluginName] = FFmpeg::Plugin::create(context);
#endif // FFmpeg_FOUND
#if defined(JPEG_FOUND)
                p.plugins[JPEG::pluginName] = JPEG::Plugin::create(context);
#endif // JPEG_FOUND
#if defined(PNG_FOUND)
                p.plugins[PNG::pluginName] = PNG::Plugin::create(context);
#endif // PNG_FOUND
#if defined(OpenEXR_FOUND)
                p.plugins[OpenEXR::pluginName] = OpenEXR::Plugin::create(context);
#endif // OpenEXR_FOUND
#if defined(TIFF_FOUND)
                p.plugins[TIFF::pluginName] = TIFF::Plugin::create(context);
#endif // TIFF_FOUND

                for (const auto& i : p.plugins)
                {
                    if (i.second->canSequence())
                    {
                        const auto& fileExtensions = i.second->getFileExtensions();
                        p.sequenceExtensions.insert(fileExtensions.begin(), fileExtensions.end());
                    }
                    else
                    {
                        const auto& fileExtensions = i.second->getFileExtensions();
                        p.nonSequenceExtensions.insert(fileExtensions.begin(), fileExtensions.end());
                    }
                
                    std::stringstream ss;
                    ss << "I/O plugin: " << i.second->getPluginName() << '\n';
                    ss << "    Information: " << i.second->getPluginInfo() << '\n';
                    ss << "    File extensions: " << String::joinSet(i.second->getFileExtensions(), ", ") << '\n';
                    _log(ss.str());
                }

                _logInitTime();
            }

            IOSystem::IOSystem() :
                _p(new Private)
            {}

            IOSystem::~IOSystem()
            {}

            std::shared_ptr<IOSystem> IOSystem::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = context->getSystemT<IOSystem>();
                if (!out)
                {
                    out = std::shared_ptr<IOSystem>(new IOSystem);
                    out->_init(context);
                }
                return out;
            }

            std::set<std::string> IOSystem::getPluginNames() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto& i : p.plugins)
                {
                    out.insert(i.second->getPluginName());
                }
                return out;
            }

            std::set<std::string> IOSystem::getFileExtensions() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto& i : p.plugins)
                {
                    const auto& fileExtensions = i.second->getFileExtensions();
                    out.insert(fileExtensions.begin(), fileExtensions.end());
                }
                return out;
            }

            rapidjson::Value IOSystem::getOptions(const std::string& pluginName, rapidjson::Document::AllocatorType& allocator) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    return i->second->getOptions(allocator);
                }
                return rapidjson::Value();
            }

            void IOSystem::setOptions(const std::string& pluginName, const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    i->second->setOptions(value);
                    p.optionsChanged->setAlways(true);
                }
            }

            std::shared_ptr<Observer::IValueSubject<bool> > IOSystem::observeOptionsChanged() const
            {
                return _p->optionsChanged;
            }

            const std::set<std::string>& IOSystem::getSequenceExtensions() const
            {
                return _p->sequenceExtensions;
            }

            const std::set<std::string>& IOSystem::getNonSequenceExtensions() const
            {
                return _p->nonSequenceExtensions;
            }

            bool IOSystem::canSequence(const System::File::Info& fileInfo) const
            {
                DJV_PRIVATE_PTR();
                return std::find(
                    p.sequenceExtensions.begin(),
                    p.sequenceExtensions.end(),
                    fileInfo.getPath().getExtension()) != p.sequenceExtensions.end();
            }

            bool IOSystem::canRead(const System::File::Info& fileInfo) const
            {
                DJV_PRIVATE_PTR();
                for (const auto& i : p.plugins)
                {
                    if (i.second->canRead(fileInfo))
                    {
                        return true;
                    }
                }
                return false;
            }

            std::shared_ptr<IRead> IOSystem::read(const System::File::Info& fileInfo, const ReadOptions& options)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<IRead> out;
                for (const auto& i : p.plugins)
                {
                    if (i.second->canRead(fileInfo))
                    {
                        out = i.second->read(fileInfo, options);
                        break;
                    }
                }
                if (!out)
                {
                    throw System::File::Error(String::Format("{0}: {1}").
                        arg(fileInfo.getFileName()).
                        arg(p.textSystem->getText(DJV_TEXT("error_file_read"))));
                }
                return out;
            }

            bool IOSystem::canWrite(const System::File::Info& fileInfo, const Info& info) const
            {
                DJV_PRIVATE_PTR();
                for (const auto& i : p.plugins)
                {
                    if (i.second->canWrite(fileInfo, info))
                    {
                        return true;
                    }
                }
                return false;
            }

            std::shared_ptr<IWrite> IOSystem::write(const System::File::Info& fileInfo, const Info& info, const WriteOptions& options)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<IWrite> out;
                for (const auto& i : p.plugins)
                {
                    if (i.second->canWrite(fileInfo, info))
                    {
                        out = i.second->write(fileInfo, info, options);
                        break;
                    }
                }
                if (!out)
                {
                    throw System::File::Error(String::Format("{0}: {1}").
                        arg(fileInfo.getFileName()).
                        arg(p.textSystem->getText(DJV_TEXT("error_file_write"))));
                }
                return out;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
