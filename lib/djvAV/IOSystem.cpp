// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/IOSystem.h>

#include <djvAV/Cineon.h>
#include <djvAV/DPX.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IFF.h>
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

#include <djvCore/Context.h>
#include <djvCore/FileSystem.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            struct System::Private
            {
                std::shared_ptr<TextSystem> textSystem;
                std::shared_ptr<ValueSubject<bool> > optionsChanged;
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
                std::set<std::string> sequenceExtensions;
                std::set<std::string> nonSequenceExtensions;
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::AV::IO::System", context);

                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<GLFW::System>());

                p.textSystem = context->getSystemT<TextSystem>();

                p.optionsChanged = ValueSubject<bool>::create();

                p.plugins[Cineon::pluginName] = Cineon::Plugin::create(context);
                p.plugins[DPX::pluginName] = DPX::Plugin::create(context);
                p.plugins[IFF::pluginName] = IFF::Plugin::create(context);
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
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            std::set<std::string> System::getPluginNames() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto& i : p.plugins)
                {
                    out.insert(i.second->getPluginName());
                }
                return out;
            }

            std::set<std::string> System::getFileExtensions() const
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

            rapidjson::Value System::getOptions(const std::string& pluginName, rapidjson::Document::AllocatorType& allocator) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    return i->second->getOptions(allocator);
                }
                return rapidjson::Value();
            }

            void System::setOptions(const std::string& pluginName, const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    i->second->setOptions(value);
                    p.optionsChanged->setAlways(true);
                }
            }

            std::shared_ptr<IValueSubject<bool> > System::observeOptionsChanged() const
            {
                return _p->optionsChanged;
            }

            const std::set<std::string>& System::getSequenceExtensions() const
            {
                return _p->sequenceExtensions;
            }

            const std::set<std::string>& System::getNonSequenceExtensions() const
            {
                return _p->nonSequenceExtensions;
            }

            bool System::canSequence(const FileSystem::FileInfo& fileInfo) const
            {
                DJV_PRIVATE_PTR();
                return std::find(
                    p.sequenceExtensions.begin(),
                    p.sequenceExtensions.end(),
                    fileInfo.getPath().getExtension()) != p.sequenceExtensions.end();
            }

            bool System::canRead(const FileSystem::FileInfo& fileInfo) const
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

            bool System::canWrite(const FileSystem::FileInfo& fileInfo, const Info& info) const
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

            std::shared_ptr<IRead> System::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options)
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
                    throw FileSystem::Error(String::Format("{0}: {1}").
                        arg(fileInfo.getFileName()).
                        arg(p.textSystem->getText(DJV_TEXT("error_file_read"))));
                }
                return out;
            }

            std::shared_ptr<IWrite> System::write(const FileSystem::FileInfo& fileInfo, const Info& info, const WriteOptions& options)
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
                    throw FileSystem::Error(String::Format("{0}: {1}").
                        arg(fileInfo.getFileName()).
                        arg(p.textSystem->getText(DJV_TEXT("error_file_write"))));
                }
                return out;
            }

        } // namespace IO
    } // namespace AV
} // namespace djv
