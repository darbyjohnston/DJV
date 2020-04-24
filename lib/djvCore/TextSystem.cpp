// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/TextSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/PicoJSON.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/String.h>
#include <djvCore/StringFormat.h>
#include <djvCore/Timer.h>

#include <future>
#include <locale>
#include <mutex>
#include <set>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        struct TextSystem::Private
        {
            std::shared_ptr<ResourceSystem> resourceSystem;
            std::shared_ptr<LogSystem> logSystem;

            std::vector<FileSystem::FileInfo> textFiles;

            std::vector<std::string> locales;
            std::string systemLocale;
            std::shared_ptr<ValueSubject<std::string> > currentLocale;

            TextMap text;
            std::shared_ptr<ValueSubject<bool> > textChanged;

            mutable std::mutex mutex;
            std::vector<std::future<TextMap> > readFutures;
            std::future<std::vector<FileSystem::FileInfo> > statFuture;
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
                        std::string value2;
                        if (piecesSize > 0)
                        {
                            value2 = String::join(pieces, '=');
                        }
                        keyValues[category] = value2;
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
            p.currentLocale = ValueSubject<std::string>::create("en");
            p.textChanged = ValueSubject<bool>::create();

            // Find the .text files.
            p.textFiles = _getTextFiles();

            // Extract the locale names.
            std::set<std::string> localeSet;
            for (const auto& textFile : p.textFiles)
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
            for (const auto& locale : localeSet)
            {
                p.locales.push_back(locale);
            }
            std::stringstream ss;
            ss.str(std::string());
            ss << "Found text files: " << String::join(p.locales, ", ");
            p.logSystem->log(getSystemName(), ss.str());

            // Get the system locale.
            std::string djvLang = OS::getEnv("DJV_LANG");
            if (djvLang.size())
            {
                ss << "DJV_LANG: " << djvLang;
                p.systemLocale = djvLang;
            }
            else
            {
                try
                {
                    std::locale locale("");
                    std::string localeName = locale.name();
                    ss << "std::locale: " << localeName;
                    p.logSystem->log(getSystemName(), ss.str());
                    std::string cppLocale = parseLocale(localeName);
                    if (cppLocale.size())
                    {
                        p.systemLocale = cppLocale;
                    }
                }
                catch (const std::exception& e)
                {
                    p.logSystem->log(getSystemName(), e.what(), LogLevel::Error);
                }
            }

            // Check that the system locale is valid.
            const auto i = localeSet.find(p.systemLocale);
            if (i == localeSet.end())
            {
                // Fall back to using English.
                const auto j = localeSet.find("en");
                if (j != localeSet.end())
                {
                    p.systemLocale = *j;
                }
                else if (localeSet.size())
                {
                    // Fall back to using the first one in the list.
                    p.systemLocale = *localeSet.begin();
                }
            }
            ss.str(std::string());
            ss << "System locale: " << p.systemLocale;
            p.logSystem->log(getSystemName(), ss.str());

            // Load the text.
            for (const auto& j : p.textFiles)
            {
                _reload(j);
            }

            p.timer = Time::Timer::create(context);
            p.timer->setRepeating(true);
            auto weak = std::weak_ptr<TextSystem>(std::dynamic_pointer_cast<TextSystem>(shared_from_this()));
            p.timer->start(
                Time::getTime(Time::TimerValue::Slow),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    if (auto system = weak.lock())
                    {
                        bool textChanged = false;
                        auto j = system->_p->readFutures.begin();
                        while (j != system->_p->readFutures.end())
                        {
                            if (j->valid() &&
                                j->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                            {
                                for (const auto& k : j->get())
                                {
                                    std::unique_lock<std::mutex> lock(system->_p->mutex);
                                    for (const auto& l : k.second)
                                    {
                                        system->_p->text[k.first][l.first] = l.second;
                                        textChanged = true;
                                    }
                                }
                                j = system->_p->readFutures.erase(j);
                            }
                            else
                            {
                                ++j;
                            }
                        }
                        if (textChanged)
                        {
                            system->_p->textChanged->setAlways(true);
                        }

                        bool stat = false;
                        if (system->_p->statFuture.valid())
                        {
                            if (system->_p->statFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                            {
                                auto textFiles = system->_p->statFuture.get();
                                for (auto k = system->_p->textFiles.begin(), l = textFiles.begin();
                                    k != system->_p->textFiles.end() && l != textFiles.end();
                                    ++k, ++l)
                                {
                                    if (l->getTime() > k->getTime())
                                    {
                                        system->_reload(*l);
                                        *k = *l;
                                    }
                                }
                                stat = true;
                            }
                        }
                        else
                        {
                            stat = true;
                        }
                        if (stat)
                        {
                            auto textFiles = system->_p->textFiles;
                            system->_p->statFuture = std::async(
                                std::launch::async,
                                [textFiles]
                                {
                                    std::vector<FileSystem::FileInfo> out;
                                    for (const auto& textFile : textFiles)
                                    {
                                        out.push_back(FileSystem::FileInfo(textFile.getPath()));
                                    }
                                    return out;
                                });
                        }
                    }
                });
        }

        TextSystem::TextSystem() :
            _p(new Private)
        {}

        TextSystem::~TextSystem()
        {}

        std::shared_ptr<TextSystem> TextSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TextSystem>(new TextSystem);
            out->_init(context);
            return out;
        }

        const std::vector<std::string> & TextSystem::getLocales() const
        {
            return _p->locales;
        }

        const std::string& TextSystem::getSystemLocale() const
        {
            return _p->systemLocale;
        }

        std::shared_ptr<IValueSubject<std::string> > TextSystem::observeCurrentLocale() const
        {
            return _p->currentLocale;
        }

        void TextSystem::setCurrentLocale(const std::string & value)
        {
            DJV_PRIVATE_PTR();
            if (p.currentLocale->setIfChanged(value))
            {
                p.textChanged->setAlways(true);
            }
        }

        const std::string& TextSystem::getText(const std::string& id)
        {
            DJV_PRIVATE_PTR();
            _readAllFutures();
            {
                std::unique_lock<std::mutex> lock(p.mutex);
                const auto i = p.text.find(p.currentLocale->get());
                if (i != p.text.end())
                {
                    const auto j = i->second.find(id);
                    if (j != i->second.end())
                    {
                        return j->second;
                    }
                }
            }
            return id;
        }

        const std::string& TextSystem::getID(const std::string& text)
        {
            DJV_PRIVATE_PTR();
            _readAllFutures();
            {
                std::unique_lock<std::mutex> lock(p.mutex);
                const auto i = p.text.find(p.currentLocale->get());
                if (i != p.text.end())
                {
                    for (const auto& j : i->second)
                    {
                        if (text == j.second)
                        {
                            return j.first;
                        }
                    }
                }
            }
            return text;
        }

        std::shared_ptr<IValueSubject<bool> > TextSystem::observeTextChanged() const
        {
            return _p->textChanged;
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
            auto list = FileSystem::FileInfo::directoryList(documentsPath, options);
            out.insert(out.end(), list.begin(), list.end());

            try
            {
                const auto envPaths = OS::getStringListEnv("DJV_TEXT_PATH");
                for (const auto& path : envPaths)
                {
                    list = FileSystem::FileInfo::directoryList(FileSystem::Path(path), options);
                    out.insert(out.end(), list.begin(), list.end());
                }
            }
            catch (const std::exception& e)
            {
                p.logSystem->log(
                    getSystemName(),
                    String::Format("{0}: {1}").arg("DJV_TEXT_PATH").arg(e.what()),
                    LogLevel::Error);
            }

            return out;
        }
        
        void TextSystem::_reload(const FileSystem::FileInfo& value)
        {
            DJV_PRIVATE_PTR();
            auto weak = std::weak_ptr<TextSystem>(std::dynamic_pointer_cast<TextSystem>(shared_from_this()));
            auto fileInfo = value;
            p.readFutures.push_back(std::async(
                std::launch::async,
                [weak, fileInfo]
                {
                    TextMap out;
                    if (auto system = weak.lock())
                    {
                        out = system->_readText(fileInfo);
                    }
                    return out;
                }));
        }

        TextSystem::TextMap TextSystem::_readText(const FileSystem::FileInfo& textFile)
        {
            DJV_PRIVATE_PTR();
            TextMap out;
            {
                std::stringstream ss;
                ss << "Reading text file: " << textFile.getPath().get();
                p.logSystem->log(getSystemName(), ss.str());
            }
            try
            {
                const auto& path = textFile.getPath();
                const auto& baseName = path.getBaseName();
                std::string locale;
                for (auto j = baseName.rbegin(); j != baseName.rend() && *j != '.'; ++j)
                {
                    locale.insert(locale.begin(), *j);
                }
                
                auto fileIO = FileSystem::FileIO::create();
                fileIO->open(std::string(path), FileSystem::FileIO::Mode::Read);
#if defined(DJV_MMAP)
                const char* bufP = reinterpret_cast<const char*>(fileIO->mmapP());
                const char* bufEnd = reinterpret_cast<const char*>(fileIO->mmapEnd());
#else // DJV_MMAP
                std::vector<char> buf;
                const size_t fileSize = fileIO->getSize();
                buf.resize(fileSize);
                fileIO->read(buf.data(), fileSize);
                const char* bufP = buf.data();
                const char* bufEnd = bufP + fileSize;
#endif // DJV_MMAP

                // Parse the JSON.
                picojson::value v;
                std::string error;
                picojson::parse(v, bufP, bufEnd, &error);
                if (!error.empty())
                {
                    throw FileSystem::Error(String::Format("{0}: {1}").arg(path.get()).arg(error));
                }
                    
                if (v.is<picojson::object>())
                {
                    std::string id;
                    std::string text;
                    const auto& obj = v.get<picojson::object>();
                    for (auto i = obj.begin(); i != obj.end(); ++i)
                    {
                        id = i->first;
                        text = i->second.to_str();
                        if (!id.empty())
                        {
                            out[locale][id] = text;
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                p.logSystem->log(getSystemName(), e.what(), LogLevel::Error);
            }
            return out;
        }
        
        void TextSystem::_readAllFutures()
        {
            DJV_PRIVATE_PTR();
            bool textChanged = false;
            auto j = p.readFutures.begin();
            while (j != p.readFutures.end())
            {
                if (j->valid() &&
                    j->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    for (const auto& k : j->get())
                    {
                        std::unique_lock<std::mutex> lock(p.mutex);
                        for (const auto& l : k.second)
                        {
                            p.text[k.first][l.first] = l.second;
                            textChanged = true;
                        }
                    }
                    j = p.readFutures.erase(j);
                }
                else
                {
                    ++j;
                }
            }
            if (textChanged)
            {
                p.textChanged->setAlways(true);
            }
        }

    } // namespace Core
} // namespace djv

