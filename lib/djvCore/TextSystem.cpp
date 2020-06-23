// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/TextSystem.h>

#include <djvCore/Context.h>
#include <djvCore/DirectoryWatcher.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/OS.h>
#include <djvCore/RapidJSON.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/String.h>
#include <djvCore/StringFormat.h>
#include <djvCore/Timer.h>

#include <rapidjson/error/en.h>

#include <future>
#include <locale>
#include <map>
#include <mutex>
#include <set>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        struct TextSystem::Private
        {
            Private(TextSystem& p) :
                p(p)
            {}

            TextSystem& p;

            std::shared_ptr<ResourceSystem> resourceSystem;
            std::shared_ptr<LogSystem> logSystem;

            std::vector<FileSystem::FileInfo> textFiles;

            std::vector<std::string> locales;
            std::string systemLocale;
            std::shared_ptr<ValueSubject<std::string> > currentLocale;

            typedef std::map<std::string, std::map<std::string, std::string> > TextMap;
            TextMap text;
            std::shared_ptr<ValueSubject<bool> > textChanged;

            mutable std::mutex mutex;
            std::vector<std::future<TextMap> > readFutures;
            std::shared_ptr<Time::Timer> timer;
            std::shared_ptr<FileSystem::DirectoryWatcher> directoryWatcher;

            std::vector<FileSystem::FileInfo> getTextFiles() const;

            void reload(const FileSystem::FileInfo&);

            TextMap readText(const FileSystem::FileInfo&);
            void readAllFutures();

            void startTimer();
            void stopTimer();
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
#elif defined(DJV_PLATFORM_MACOS)
                //! \todo macOS locale.
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
            p.textFiles = p.getTextFiles();

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
            {
                std::stringstream ss;
                ss.str(std::string());
                ss << "Found text files: " << String::join(p.locales, ", ");
                p.logSystem->log(getSystemName(), ss.str());
            }

            // Get the system locale.
            std::string djvLang = OS::getEnv("DJV_LANG");
            if (djvLang.size())
            {
                {
                    std::stringstream ss;
                    ss << "DJV_LANG: " << djvLang;
                    p.logSystem->log(getSystemName(), ss.str());
                }
                p.systemLocale = djvLang;
            }
            else
            {
                try
                {
                    const std::locale locale("");
                    const std::string localeName = locale.name();
                    {
                        std::stringstream ss;
                        ss << "std::locale: " << localeName;
                        p.logSystem->log(getSystemName(), ss.str());
                    }
                    const std::string cppLocale = parseLocale(localeName);
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
            {
                std::stringstream ss;
                ss << "System locale: " << p.systemLocale;
                p.logSystem->log(getSystemName(), ss.str());
            }

            // Load the text.
            for (const auto& j : p.textFiles)
            {
                p.reload(j);
            }

            // Start a directory watcher to check for changes to the text files.
            p.directoryWatcher = FileSystem::DirectoryWatcher::create(context);
            p.directoryWatcher->setPath(p.resourceSystem->getPath(FileSystem::ResourcePath::Text));
            p.directoryWatcher->setCallback(
                [this]
                {
                    for (const auto& j : _p->textFiles)
                    {
                        _p->reload(j);
                    }
                    _p->startTimer();
                });
            
            // Create a timer to wait for text files to be read.
            p.timer = Time::Timer::create(context);
            p.timer->setRepeating(true);
        }

        TextSystem::TextSystem() :
            _p(new Private(*this))
        {}

        TextSystem::~TextSystem()
        {}

        std::shared_ptr<TextSystem> TextSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TextSystem>(new TextSystem);
            out->_init(context);
            return out;
        }

        const std::vector<std::string>& TextSystem::getLocales() const
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

        void TextSystem::setCurrentLocale(const std::string& value)
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
            p.readAllFutures();
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
            p.readAllFutures();
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

        std::vector<FileSystem::FileInfo> TextSystem::Private::getTextFiles() const
        {
            std::vector<FileSystem::FileInfo> out;
            FileSystem::DirectoryListOptions options;
            options.filter = "\\.text$";

            const auto textPath = resourceSystem->getPath(FileSystem::ResourcePath::Text);
            out = FileSystem::FileInfo::directoryList(textPath, options);
            
            const auto documentsPath = resourceSystem->getPath(FileSystem::ResourcePath::Documents);
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
                logSystem->log(
                    p.getSystemName(),
                    String::Format("{0}: {1}").arg("DJV_TEXT_PATH").arg(e.what()),
                    LogLevel::Error);
            }

            return out;
        }
        
        void TextSystem::Private::reload(const FileSystem::FileInfo& value)
        {
            auto fileInfo = value;
            readFutures.push_back(std::async(
                std::launch::async,
                [this, fileInfo]
                {
                    return readText(fileInfo);
                }));
        }

        TextSystem::Private::TextMap TextSystem::Private::readText(const FileSystem::FileInfo& textFile)
        {
            TextMap out;
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
                fileIO->open(path.get(), FileSystem::FileIO::Mode::Read);
                size_t bufSize = 0;
#if defined(DJV_MMAP)
                const char* bufP = reinterpret_cast<const char*>(fileIO->mmapP());
                const char* bufEnd = reinterpret_cast<const char*>(fileIO->mmapEnd());
                bufSize = bufEnd - bufP;
#else // DJV_MMAP
                std::vector<char> buf;
                bufSize = fileIO->getSize();
                buf.resize(bufSize);
                fileIO->read(buf.data(), bufSize);
                const char* bufP = buf.data();
#endif // DJV_MMAP

                // Parse the JSON.
                rapidjson::Document document;
                rapidjson::ParseResult result = document.Parse(bufP, bufSize);
                if (!result)
                {
                    size_t line = 0;
                    size_t character = 0;
                    RapidJSON::errorLineNumber(bufP, bufSize, result.Offset(), line, character);
                    throw std::runtime_error(String::Format("{0}: {1} {2} {3}, {4} {5}").
                        arg(path.get()).
                        arg(rapidjson::GetParseError_En(result.Code())).
                        arg("Line").
                        arg(line).
                        arg("Character").
                        arg(character));
                }
                for (const auto& i: document.GetObject())
                {
                    if (i.value.IsString())
                    {
                        out[locale][i.name.GetString()] = i.value.GetString();
                    }
                }
                {
                    std::stringstream ss;
                    ss << textFile.getPath().get() << "' strings: " << out[locale].size();
                    logSystem->log(p.getSystemName(), ss.str());
                }
            }
            catch (const std::exception& e)
            {
                logSystem->log(p.getSystemName(), e.what(), LogLevel::Error);
            }
            return out;
        }
        
        void TextSystem::Private::readAllFutures()
        {
            timer->stop();
            bool textChanged = false;
            for (auto& j : readFutures)
            {
                for (const auto& k : j.get())
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    for (const auto& l : k.second)
                    {
                        text[k.first][l.first] = l.second;
                        textChanged = true;
                    }
                }
            }
            readFutures.clear();
            if (textChanged)
            {
                this->textChanged->setAlways(true);
            }
        }

        void TextSystem::Private::startTimer()
        {
            timer->start(
                Time::getTime(Time::TimerValue::Slow),
                [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    bool textChanged = false;
                    auto j = readFutures.begin();
                    while (j != readFutures.end())
                    {
                        if (j->valid() &&
                            j->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            for (const auto& k : j->get())
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                for (const auto& l : k.second)
                                {
                                    text[k.first][l.first] = l.second;
                                    textChanged = true;
                                }
                            }
                            j = readFutures.erase(j);
                        }
                        else
                        {
                            ++j;
                        }
                    }
                    if (readFutures.empty())
                    {
                        stopTimer();
                    }
                    if (textChanged)
                    {
                        this->textChanged->setAlways(true);
                    }
                });
        }

        void TextSystem::Private::stopTimer()
        {
            timer->stop();
        }

    } // namespace Core
} // namespace djv

