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

#include <djvCore/TextSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileIO.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/Timer.h>

#include <condition_variable>
#include <future>
#include <locale>
#include <set>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        struct TextSystem::Private
        {
            std::shared_ptr<LogSystem> logSystem;

            std::vector<std::string> locales;
            std::string currentLocale;
            bool currentLocaleChanged = false;
            std::condition_variable currentLocaleChangedCV;
            std::shared_ptr<ValueSubject<std::string> > currentLocaleSubject;

            std::map<std::string, std::map<std::string, std::string> > text;

            std::thread thread;
            std::mutex mutex;
            std::shared_ptr<Time::Timer> timer;
        };

        namespace
        {
            std::string parseLocale(const std::string & value)
            {
                std::string out;
#if defined(DJV_PLATFORM_WINDOWS)
                //! \todo Windows locale.
#elif defined(DJV_PLATFORM_OSX)
                //! \todo OSX locale.
#else
                auto pieces = String::split(value, '_');
                if (!pieces.size())
                {
                    pieces = String::split(value, '.');
                }
                if (pieces.size())
                {
                    out = pieces[0];
                }
#endif // DJV_PLATFORM_WINDOWS
                return out;
            }
        } // namespace

        void TextSystem::_init(const FileSystem::Path & path, Context * context)
        {
            ISystemBase::_init("djv::Core::TextSystem", context);

            auto logSystem = context->getSystemT<LogSystem>();
            addDependency(logSystem);
            addDependency(context->getSystemT<Time::TimerSystem>());

            DJV_PRIVATE_PTR();
            p.logSystem = logSystem;
            p.currentLocaleSubject = ValueSubject<std::string>::create();

            p.thread = std::thread(
                [this, path]
            {
                DJV_PRIVATE_PTR();
                std::unique_lock<std::mutex> lock(p.mutex);

                p.currentLocale = "en";
                p.currentLocaleChanged = true;
                std::string djvLocale = OS::getEnv("DJV_LANG");
                std::stringstream s;
                if (djvLocale.size())
                {
                    p.currentLocale = djvLocale;
                }
                else
                {
                    try
                    {
                        std::locale locale("");
                        s << "Current std::locale: " << locale.name();
                        p.logSystem->log(getSystemName(), s.str());
                        std::string cppLocale = parseLocale(locale.name());
                        if (cppLocale.size())
                        {
                            p.currentLocale = cppLocale;
                        }
                    }
                    catch (const std::exception & e)
                    {
                        p.logSystem->log(getSystemName(), e.what(), LogLevel::Error);
                    }
                }
                s.str(std::string());
                s << "Current locale: " << p.currentLocale;
                p.logSystem->log(getSystemName(), s.str());

                // Find the .text files.
                FileSystem::DirectoryListOptions options;
                options.filter = "\\.text$";
                const auto fileInfos = FileSystem::FileInfo::directoryList(path, options);

                // Extract the locale names.
                std::set<std::string> localeSet;
                for (const auto & fileInfo : fileInfos)
                {
                    std::string temp = FileSystem::Path(fileInfo.getPath().getBaseName()).getExtension();
                    if (temp.size() && '.' == temp[0])
                    {
                        temp.erase(temp.begin());
                    }
                    if (temp != "all")
                    {
                        localeSet.insert(temp);
                    }
                }
                for (const auto & locale : localeSet)
                {
                    p.locales.push_back(locale);
                }
                s.str(std::string());
                s << "Found locales: " << String::join(p.locales, ", ");
                p.logSystem->log(getSystemName(), s.str());

                // Read the .text files.
                _readText(path);
            });

            p.timer = Time::Timer::create(context);
            p.timer->setRepeating(true);
            p.timer->start(
                Time::getMilliseconds(Time::TimerValue::Medium),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                std::string currentLocale;
                {
                    std::unique_lock<std::mutex> lock(p.mutex);
                    if (p.currentLocaleChangedCV.wait_for(
                        lock,
                        std::chrono::milliseconds(0),
                        [this]
                    {
                        return _p->currentLocaleChanged;
                    }))
                    {
                        currentLocale = p.currentLocale;
                        p.currentLocaleChanged = false;
                    }
                }
                if (!currentLocale.empty())
                {
                    p.currentLocaleSubject->setAlways(p.currentLocale);
                }
            });
        }

        TextSystem::TextSystem() :
            _p(new Private)
        {}

        TextSystem::~TextSystem()
        {
            DJV_PRIVATE_PTR();
            if (p.thread.joinable())
            {
                p.thread.join();
            }
        }

        std::shared_ptr<TextSystem> TextSystem::create(const FileSystem::Path & path, Context * context)
        {
            auto out = std::shared_ptr<TextSystem>(new TextSystem);
            out->_init(path, context);
            return out;
        }

        const std::vector<std::string> & TextSystem::getLocales() const
        {
            DJV_PRIVATE_PTR();
            std::unique_lock<std::mutex> lock(p.mutex);
            return p.locales;
        }

        const std::string & TextSystem::getCurrentLocale() const
        {
            DJV_PRIVATE_PTR();
            std::unique_lock<std::mutex> lock(p.mutex);
            return p.currentLocale;
        }

        std::shared_ptr<IValueSubject<std::string> > TextSystem::observeCurrentLocale() const
        {
            return _p->currentLocaleSubject;
        }

        void TextSystem::setCurrentLocale(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            std::unique_lock<std::mutex> lock(p.mutex);
            if (value == p.currentLocale)
                return;
            p.currentLocale = value;
            p.currentLocaleChanged = true;
        }
        
        const std::string & TextSystem::getText(const std::string & id) const
        {
            DJV_PRIVATE_PTR();
            std::unique_lock<std::mutex> lock(p.mutex);
            const auto i = p.text.find(p.currentLocale);
            if (i != p.text.end())
            {
                const auto j = i->second.find(id);
                if (j != i->second.end())
                {
                    return j->second;
                }
            }
            return id;
        }

        void TextSystem::_readText(const FileSystem::Path & path)
        {
            DJV_PRIVATE_PTR();
            p.text.clear();
            p.logSystem->log(getSystemName(), "Reading text files:");
            FileSystem::DirectoryListOptions options;
            options.filter = "\\.text$";
            for (const auto & i : FileSystem::FileInfo::directoryList(path, options))
            {
                p.logSystem->log(getSystemName(), String::indent(1) + i.getPath().get());
                try
                {
                    const auto & path = i.getPath();
                    const auto & baseName = path.getBaseName();
                    std::string locale;
                    for (auto i = baseName.rbegin(); i != baseName.rend() && *i != '.'; ++i)
                    {
                        locale.insert(locale.begin(), *i);
                    }

                    FileSystem::FileIO fileIO;
                    fileIO.open(path, FileSystem::FileIO::Mode::Read);
                    std::vector<char> buf;
                    const size_t fileSize = fileIO.getSize();
                    buf.resize(fileSize);
                    fileIO.read(buf.data(), fileSize);
                    const char* bufP = buf.data();
                    const char* bufEnd = bufP + fileSize;
                    //const char * bufP = reinterpret_cast<const char *>(fileIO.mmapP());
                    //const char * bufEnd = reinterpret_cast<const char *>(fileIO.mmapEnd());

                    // Parse the JSON.
                    picojson::value v;
                    std::string error;
                    picojson::parse(v, bufP, bufEnd, &error);
                    if (!error.empty())
                    {
                        std::stringstream s;
                        s << DJV_TEXT("Error reading the text file") << " '" << path << "'. " << error;
                        throw std::runtime_error(s.str());
                    }

                    if (v.is<picojson::array>())
                    {
                        for (const auto & item : v.get<picojson::array>())
                        {
                            if (item.is<picojson::object>())
                            {
                                std::string id;
                                std::string text;
                                std::string description;
                                const auto & obj = item.get<picojson::object>();
                                for (auto i = obj.begin(); i != obj.end(); ++i)
                                {
                                    if ("id" == i->first)
                                    {
                                        id = i->second.to_str();
                                    }
                                    else if ("text" == i->first)
                                    {
                                        text = i->second.to_str();
                                    }
                                    else if ("description" == i->first)
                                    {
                                        description = i->second.to_str();
                                    }
                                }
                                if (!id.empty())
                                {
                                    p.text[locale][id] = text;
                                }
                            }
                        }
                    }
                }
                catch (const std::exception & e)
                {
                    p.logSystem->log(getSystemName(), e.what(), LogLevel::Error);
                }
            }
        }

    } // namespace Core
} // namespace djv

