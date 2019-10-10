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
#include <djvCore/FileSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

#include <future>
#include <locale>
#include <set>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace
        {
            struct Text
            {
                std::string text;
                std::string id;
                std::string description;
            };
        }
        
        struct TextSystem::Private
        {
            std::shared_ptr<ResourceSystem> resourceSystem;
            std::shared_ptr<LogSystem> logSystem;

            std::vector<std::string> locales;
            std::string currentLocale;
            std::shared_ptr<ValueSubject<std::string> > currentLocaleSubject;

            std::map<std::string, std::map<std::string, std::string> > text;
            bool textChanged = true;
            std::shared_ptr<ValueSubject<bool> > textSubject;
            
            std::map<std::string, std::vector<Text> > fileText;
            std::set<std::string> saveFiles;

            std::thread thread;
            std::mutex mutex;
            std::shared_ptr<Time::Timer> timer;
        };

        namespace
        {
            std::string parseLocale(const std::string& value)
            {
                std::string locale = value;
                
                // Split the input into categories.
                const auto categories = String::split(value, ';');
                const size_t categoriesSize = categories.size();
                if (1 == categoriesSize)
                {
                    locale = categories[0];
                }
                else if (categoriesSize > 1)
                {
                    // Parse the categories.
                    std::map<std::string, std::string> keyValues;
                    for (const auto& i : categories)
                    {
                        auto pieces = String::split(i, '=');
                        const size_t piecesSize = pieces.size();
                        std::string category;
                        if (piecesSize > 0)
                        {
                            category = pieces[0];
                            pieces.erase(pieces.begin());
                        }
                        std::string value;
                        if (piecesSize > 0)
                        {
                            value = String::join(pieces, '=');
                        }
                        keyValues[category] = value;
                    }
                    
                    // Try using LC_MESSAGES.
                    const auto i = keyValues.find("LC_MESSAGES");
                    if (i != keyValues.end())
                    {
                        locale = i->second;
                    }
                    else if (keyValues.size())
                    {
                        // Fall back to using the first one in the list.
                        locale = keyValues.begin()->second;
                    }
                }
                
                std::string out;
#if defined(DJV_PLATFORM_WINDOWS)
                //! \todo Windows locale.
#elif defined(DJV_PLATFORM_OSX)
                //! \todo OSX locale.
#else
                auto pieces = String::split(locale, '_');
                if (!pieces.size())
                {
                    pieces = String::split(locale, '.');
                }
                if (pieces.size())
                {
                    out = pieces[0];
                }
#endif // DJV_PLATFORM_WINDOWS
                return out;
            }
        } // namespace

        void TextSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystemBase::_init("djv::Core::TextSystem", context);

            auto resourceSystem = context->getSystemT<ResourceSystem>();
            auto logSystem = context->getSystemT<LogSystem>();
            addDependency(resourceSystem);
            addDependency(logSystem);
            addDependency(context->getSystemT<Time::TimerSystem>());

            DJV_PRIVATE_PTR();
            p.resourceSystem = resourceSystem;
            p.logSystem = logSystem;
            p.currentLocaleSubject = ValueSubject<std::string>::create();
            p.textSubject = ValueSubject<bool>::create();

            p.thread = std::thread(
                [this]
            {
                DJV_PRIVATE_PTR();
                std::unique_lock<std::mutex> lock(p.mutex);

                p.currentLocale = "en";
                p.textChanged = true;
                std::string djvLang = OS::getEnv("DJV_LANG");
                std::stringstream ss;
                if (djvLang.size())
                {
                    p.currentLocale = djvLang;
                }
                else
                {
                    try
                    {
                        std::locale locale("");
                        std::string localeName = locale.name();
                        ss << "Current std::locale: " << localeName;
                        p.logSystem->log(getSystemName(), ss.str());
                        std::string cppLocale = parseLocale(localeName);
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
                ss.str(std::string());
                ss << "Found locale: " << p.currentLocale;
                p.logSystem->log(getSystemName(), ss.str());

                // Find the .text files.
                const auto textFiles = _getTextFiles();

                // Extract the locale names.
                std::set<std::string> localeSet;
                for (const auto& textFile : textFiles)
                {
                    std::string temp = FileSystem::Path(textFile.getPath().getBaseName()).getExtension();
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
                ss.str(std::string());
                ss << "Found text files: " << String::join(p.locales, ", ");
                p.logSystem->log(getSystemName(), ss.str());

                // Check that the current locale is valid.
                const auto i = localeSet.find(p.currentLocale);
                if (i == localeSet.end())
                {
                    // Fall back to using English.
                    const auto j = localeSet.find("en");
                    if (j != localeSet.end())
                    {
                        p.currentLocale = *j;
                    }
                    else if (localeSet.size())
                    {
                        // Fall back to using the first one in the list.
                        p.currentLocale = *localeSet.begin();
                    }
                }
                ss.str(std::string());
                ss << "Current locale: " << p.currentLocale;
                p.logSystem->log(getSystemName(), ss.str());

                // Read the .text files.
                _readText(textFiles);
            });
            
            p.timer = Time::Timer::create(context);
            p.timer->setRepeating(true);
            p.timer->start(
                Time::getMilliseconds(Time::TimerValue::Medium),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                std::string currentLocale;
                bool textChanged = false;
                {
                    std::unique_lock<std::mutex> lock(p.mutex);
                    currentLocale = p.currentLocale;
                    textChanged = p.textChanged;
                }
                p.currentLocaleSubject->setIfChanged(currentLocale);
                if (textChanged)
                {
                    p.textSubject->setAlways(true);
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
            if (p.saveFiles.size())
            {
                _writeText();
            }
        }

        std::shared_ptr<TextSystem> TextSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TextSystem>(new TextSystem);
            out->_init(context);
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
                
        void TextSystem::setText(const std::string& id, const std::string& value)
        {
            DJV_PRIVATE_PTR();
            std::unique_lock<std::mutex> lock(p.mutex);
            auto i = p.text.find(p.currentLocale);
            if (i != p.text.end())
            {
                auto j = i->second.find(id);
                if (j != i->second.end())
                {
                    j->second = value;
                    p.textChanged = true;
                    _setFileText(id, value);
                }
            }
        }
        
        std::vector<FileSystem::FileInfo> TextSystem::_getTextFiles() const
        {
            DJV_PRIVATE_PTR();
            std::vector<FileSystem::FileInfo> out;
            FileSystem::DirectoryListOptions options;
            options.filter = "\\.text$";
            const auto textPath = p.resourceSystem->getPath(FileSystem::ResourcePath::Text);
            out = FileSystem::FileInfo::directoryList(textPath, options);
            const auto documentsPath = p.resourceSystem->getPath(FileSystem::ResourcePath::Documents);
            auto overrides = FileSystem::FileInfo::directoryList(documentsPath, options);
            auto i = overrides.begin();
            while (i != overrides.end())
            {
                auto j = out.begin();
                for (; j != out.end(); ++j)
                {
                    if (i->getFileName(Frame::invalid, false) ==
                        j->getFileName(Frame::invalid, false))
                    {
                        break;
                    }
                }
                if (j != out.end())
                {
                    *j = *i;
                    i = overrides.erase(i);
                }
                else
                {
                    ++i;
                }
            }
            i = overrides.begin();
            for (; i != overrides.end(); ++i)
            {
                out.push_back(*i);
            }
            return out;
        }
        
        void TextSystem::_setFileText(const std::string& id, const std::string& text)
        {
            DJV_PRIVATE_PTR();
            for (auto& i : p.fileText)
            {
                for (auto& j : i.second)
                {
                    if (id == j.id)
                    {
                        j.text = text;
                        p.saveFiles.insert(i.first);
                    }
                }
            }
        }

        void TextSystem::_readText(const std::vector<FileSystem::FileInfo>& textFiles)
        {
            DJV_PRIVATE_PTR();
            p.text.clear();
            p.logSystem->log(getSystemName(), "Reading text files:");
            for (const auto& i : textFiles)
            {
                p.logSystem->log(getSystemName(), String::indent(1) + i.getPath().get());
                try
                {
                    const auto & path = i.getPath();
                    const auto & baseName = path.getBaseName();
                    std::string locale;
                    for (auto j = baseName.rbegin(); j != baseName.rend() && *j != '.'; ++j)
                    {
                        locale.insert(locale.begin(), *j);
                    }

                    FileSystem::FileIO fileIO;
                    fileIO.open(std::string(path), FileSystem::FileIO::Mode::Read);
#if defined(DJV_MMAP)
                    const char * bufP = reinterpret_cast<const char *>(fileIO.mmapP());
                    const char * bufEnd = reinterpret_cast<const char *>(fileIO.mmapEnd());
#else // DJV_MMAP
                    std::vector<char> buf;
                    const size_t fileSize = fileIO.getSize();
                    buf.resize(fileSize);
                    fileIO.read(buf.data(), fileSize);
                    const char* bufP = buf.data();
                    const char* bufEnd = bufP + fileSize;
#endif // DJV_MMAP

                    // Parse the JSON.
                    picojson::value v;
                    std::string error;
                    picojson::parse(v, bufP, bufEnd, &error);
                    if (!error.empty())
                    {
                        std::stringstream s;
                        s << DJV_TEXT("Error reading the text file") << " '" << path << "'. " << error;
                        throw FileSystem::Error(s.str());
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

        void TextSystem::_writeText()
        {
            DJV_PRIVATE_PTR();
            const FileSystem::Path documentsPath = p.resourceSystem->getPath(FileSystem::ResourcePath::Documents);
            for (const auto& i : p.saveFiles)
            {
                const auto j = p.fileText.find(i);
                if (j != p.fileText.end())
                {
                    const FileSystem::Path path(documentsPath, i);
                    FileSystem::FileIO fileIO;
                    fileIO.open(std::string(path), FileSystem::FileIO::Mode::Read);
                }
            }
        }

    } // namespace Core
} // namespace djv

