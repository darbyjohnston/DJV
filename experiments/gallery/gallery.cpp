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

#include <djvDesktopApp/Application.h>

#include <djvUI/ImageWidget.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>

#include <curl/curl.h>

using namespace djv;

size_t curlCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    const size_t totalSize = size * nmemb;
    *reinterpret_cast<std::string*>(userp) += std::string(reinterpret_cast<char*>(contents), totalSize);
    return totalSize;
}

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        std::string s;

        curl_global_init(CURL_GLOBAL_ALL);
        auto curlHandle = curl_easy_init();
        curl_easy_setopt(curlHandle, CURLOPT_URL, "https://collectionapi.metmuseum.org/public/collection/v1/departments");
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void*)&s);
        curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        if (auto res = curl_easy_perform(curlHandle))
        {
            throw std::runtime_error(curl_easy_strerror(res));
        }
        std::cout << s << std::endl;

        /*auto app = Desktop::Application::create(argc, argv);

        auto window = UI::Window::create(app.get());
        window->show();

        return app->run();*/
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}
