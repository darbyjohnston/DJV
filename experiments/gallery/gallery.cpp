//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include "jpeg.h"

#include <djvDesktopApp/Application.h>

#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvAV/Image.h>

#include <djvCore/Error.h>
#include <djvCore/LogSystem.h>
#include <djvCore/Timer.h>

#include <curl/curl.h>

using namespace djv;

size_t jsonCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    const size_t totalSize = size * nmemb;
    *reinterpret_cast<std::string*>(userp) += std::string(reinterpret_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t jpegCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    const size_t totalSize = size * nmemb;
    std::vector<uint8_t> tmp;
    tmp.resize(totalSize);
    memcpy(tmp.data(), contents, totalSize);
    auto data = reinterpret_cast<std::vector<uint8_t>*>(userp);
    data->insert(data->end(), tmp.begin(), tmp.end());
    return totalSize;
}

picojson::value readJson(const std::string& url)
{
    auto curlHandle = curl_easy_init();
    curl_easy_setopt(curlHandle, CURLOPT_URL, url);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, jsonCallback);
    std::string s;
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void*)& s);
    curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    if (auto res = curl_easy_perform(curlHandle))
    {
        throw std::runtime_error(curl_easy_strerror(res));
    }
    curl_easy_cleanup(curlHandle);

    picojson::value out;
    std::string error;
    picojson::parse(out, s.begin(), s.end(), &error);
    if (!error.empty())
    {
        throw std::invalid_argument(error);
    }
    return out;
}

std::vector<uint64_t> getIDs()
{
    std::vector<uint64_t> out;
    auto json = readJson("https://collectionapi.metmuseum.org/public/collection/v1/objects");
    if (json.is<picojson::object>())
    {
        const auto& object = json.get<picojson::object>();
        for (const auto& i : object)
        {
            if ("total" == i.first && i.second.is<double>())
            {
                for (const auto& j : object)
                {
                    if ("objectIDs" == j.first && j.second.is<picojson::array>())
                    {
                        for (const auto& k : j.second.get<picojson::array>())
                        {
                            if (k.is<double>())
                            {
                                out.push_back(static_cast<uint64_t>(k.get<double>()));
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    return out;
}

bool isPublicDomain(uint64_t id)
{
    bool out = false;

    std::stringstream ss;
    ss << "https://collectionapi.metmuseum.org/public/collection/v1/objects/";
    ss << id;
    auto json = readJson(ss.str());

    if (json.is<picojson::object>())
    {
        const auto& object = json.get<picojson::object>();
        for (const auto& i : object)
        {
            if ("isPublicDomain" == i.first && i.second.is<bool>())
            {
                out = i.second.get<bool>();
                break;
            }
        }
    }

    return out;
}

struct Image
{
    uint64_t id = 0;
    std::shared_ptr<AV::Image::Image> image;
    std::string title;
    std::string objectName;
    std::string department;
};

Image getImage(uint64_t id)
{
    Image out;

    std::stringstream ss;
    ss << "https://collectionapi.metmuseum.org/public/collection/v1/objects/";
    ss << id;
    auto json = readJson(ss.str());
    if (json.is<picojson::object>())
    {
        const auto& object = json.get<picojson::object>();
        for (const auto& i : object)
        {
            if ("isPublicDomain" == i.first && i.second.is<bool>() && i.second.get<bool>())
            {
                for (const auto& j : object)
                {
                    if ("primaryImage" == j.first && j.second.is<std::string>())
                    {
                        const std::string& s = j.second.get<std::string>();
                        if (!s.empty())
                        {
                            std::vector<uint8_t> data;
                            auto curlHandle = curl_easy_init();
                            curl_easy_setopt(curlHandle, CURLOPT_URL, s.c_str());
                            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, jpegCallback);
                            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void*)&data);
                            curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
                            if (auto res = curl_easy_perform(curlHandle))
                            {
                                throw std::runtime_error(curl_easy_strerror(res));
                            }
                            curl_easy_cleanup(curlHandle);
                            out.id = id;
                            out.image = jpegRead(data);
                        }
                    }
                    else if ("title" == j.first && j.second.is<std::string>())
                    {
                        out.title = j.second.get<std::string>();
                    }
                    else if ("objectName" == j.first && j.second.is<std::string>())
                    {
                        out.objectName = j.second.get<std::string>();
                    }
                    else if ("department" == j.first && j.second.is<std::string>())
                    {
                        out.department = j.second.get<std::string>();
                    }
                }
            }
        }
    }

    return out;
}

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        curl_global_init(CURL_GLOBAL_ALL);

        Core::Math::setRandomSeed();

        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
        auto app = Desktop::Application::create(args);

        auto ids = getIDs();
        std::vector<Image> images;
        std::vector<std::future<Image> > imageFutures;
        auto timer = Core::Time::Timer::create(app);
        timer->setRepeating(true);
        timer->start(
            std::chrono::milliseconds(1000),
            [&app, &ids, &images, &imageFutures](float)
            {
                if (ids.size())
                {
                    {
                        std::stringstream ss;
                        ss << "Images = " << images.size();
                        app->getSystemT<Core::LogSystem>()->log("gallery", ss.str());
                    }
                    {
                        std::stringstream ss;
                        ss << "Image futures = " << imageFutures.size();
                        app->getSystemT<Core::LogSystem>()->log("gallery", ss.str());
                    }
                    if (images.size() < 100 && imageFutures.size() < 3)
                    {
                        const uint64_t id = Core::Math::getRandom(static_cast<int>(ids.size() - 1));
                        imageFutures.push_back(std::async(
                            std::launch::async,
                            [&app, &ids, id]
                            {
                                Image out;
                                try
                                {
                                    if (isPublicDomain(id))
                                    {
                                        out = getImage(id);
                                    }
                                }
                                catch (const std::exception& e)
                                {
                                    app->getSystemT<Core::LogSystem>()->log("gallery", e.what(), Core::LogLevel::Error);
                                }
                                return out;
                            }));
                    }
                    auto i = imageFutures.begin();
                    while (i != imageFutures.end())
                    {
                        if (i->valid() &&
                            i->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            const auto result = i->get();
                            if (result.id)
                            {
                                const auto j = std::find_if(
                                    images.begin(), images.end(),
                                    [result](const Image& value)
                                    {
                                        return value.id == result.id;
                                    });
                                if (j == images.end())
                                {
                                    images.push_back(result);
                                }
                            }
                            i = imageFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
            });

        auto imageWidget = UI::ImageWidget::create(app);
        imageWidget->setHAlign(UI::HAlign::Center);
        imageWidget->setVAlign(UI::VAlign::Center);

        auto titleLabel = UI::Label::create(app);
        titleLabel->setFontSizeRole(UI::MetricsRole::FontLarge);
        titleLabel->setTextHAlign(UI::TextHAlign::Left);
        auto objectNameLabel = UI::Label::create(app);
        objectNameLabel->setTextHAlign(UI::TextHAlign::Left);
        auto departmentLabel = UI::Label::create(app);
        departmentLabel->setTextHAlign(UI::TextHAlign::Left);

        auto rowLayout = UI::VerticalLayout::create(app);
        rowLayout->setMargin(UI::MetricsRole::Margin);
        rowLayout->setSpacing(UI::MetricsRole::None);
        rowLayout->setVAlign(UI::VAlign::Bottom);
        rowLayout->setBackgroundRole(UI::ColorRole::Overlay);
        rowLayout->addChild(titleLabel);
        rowLayout->addChild(objectNameLabel);
        rowLayout->addChild(departmentLabel);
        
        auto window = UI::Window::create(app);
        window->addChild(imageWidget);
        window->addChild(rowLayout);
        window->show();

        size_t index = 0;
        auto timer2 = Core::Time::Timer::create(app);
        timer2->setRepeating(true);
        timer2->start(
            std::chrono::milliseconds(5000),
            [&app, &images, &index, imageWidget, titleLabel, objectNameLabel, departmentLabel](float)
            {
                if (images.size())
                {
                    const auto& image = images[index];
                    imageWidget->setImage(image.image);
                    titleLabel->setText(image.title);
                    objectNameLabel->setText(image.objectName);
                    departmentLabel->setText(image.department);
                    ++index;
                    if (index >= images.size())
                    {
                        index = 0;
                    }
                }
            });

        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    curl_global_cleanup();
    return r;
}
