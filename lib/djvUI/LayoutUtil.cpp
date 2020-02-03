//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUI/LayoutUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            BBox2f getPopupGeometry(
                Popup value,
                const glm::vec2& pos,
                const glm::vec2& minimumSize)
            {
                BBox2f out;
                switch (value)
                {
                case Popup::BelowRight:
                    out = BBox2f(
                        glm::vec2(pos.x, pos.y),
                        glm::vec2(pos.x + minimumSize.x, pos.y + minimumSize.y));
                    break;
                case Popup::BelowLeft:
                    out = BBox2f(
                        glm::vec2(pos.x - minimumSize.x, pos.y),
                        glm::vec2(pos.x, pos.y + minimumSize.y));
                    break;
                case Popup::AboveRight:
                    out = BBox2f(
                        glm::vec2(pos.x, pos.y - minimumSize.y),
                        glm::vec2(pos.x + minimumSize.x, pos.y));
                    break;
                case Popup::AboveLeft:
                    out = BBox2f(
                        glm::vec2(pos.x - minimumSize.x, pos.y - minimumSize.y),
                        glm::vec2(pos.x, pos.y));
                    break;
                default: break;
                }
                return out;
            }

            BBox2f getPopupGeometry(
                Popup value,
                const BBox2f& button,
                const glm::vec2& minimumSize)
            {
                BBox2f out;
                switch (value)
                {
                case Popup::BelowLeft:
                    out = BBox2f(
                        glm::vec2(
                            std::min(button.max.x - minimumSize.x, button.min.x),
                            button.max.y),
                        glm::vec2(
                            button.max.x,
                            button.max.y + minimumSize.y));
                    break;
                case Popup::BelowRight:
                    out = BBox2f(
                        glm::vec2(
                            button.min.x,
                            button.max.y),
                        glm::vec2(
                            std::max(button.min.x + minimumSize.x, button.max.x),
                            button.max.y + minimumSize.y));
                    break;
                case Popup::AboveRight:
                    out = BBox2f(
                        glm::vec2(
                            button.min.x,
                            button.min.y - minimumSize.y),
                        glm::vec2(
                            std::max(button.min.x + minimumSize.x, button.max.x),
                            button.min.y));
                    break;
                case Popup::AboveLeft:
                    out = BBox2f(
                        glm::vec2(
                            std::min(button.max.x - minimumSize.x, button.min.x),
                            button.min.y - minimumSize.y),
                        glm::vec2(
                            button.max.x,
                            button.min.y));
                    break;
                default: break;
                }
                return out;
            }

            Popup getPopup(
                Popup value,
                const BBox2f& area,
                const glm::vec2& pos,
                const glm::vec2& minimumSize)
            {
                Popup out = Popup::First;

                std::vector<Popup> popupList;
                popupList.push_back(value);
                auto popupEnums = getPopupEnums();
                const auto i = std::find(popupEnums.begin(), popupEnums.end(), value);
                if (i != popupEnums.end())
                {
                    popupEnums.erase(i);
                }
                for (auto j : popupEnums)
                {
                    popupList.push_back(j);
                }

                std::map<float, Popup> popupAreas;
                size_t j = 0;
                const size_t popupListSize = popupList.size();
                for (; j < popupListSize; ++j)
                {
                    Popup popup = popupList[j];
                    const BBox2f g = getPopupGeometry(popup, pos, minimumSize);
                    const BBox2f clipped = g.intersect(area);
                    if (g == clipped)
                    {
                        out = popup;
                        break;
                    }
                    popupAreas[clipped.getArea()] = popup;
                }
                if (popupListSize == j)
                {
                    out = popupAreas.rbegin()->second;
                }
                return out;
            }

            Popup getPopup(
                Popup value,
                const BBox2f& area,
                const BBox2f& button,
                const glm::vec2& minimumSize)
            {
                Popup out = Popup::First;

                std::vector<Popup> popupList;
                popupList.push_back(value);
                auto popupEnums = getPopupEnums();
                const auto i = std::find(popupEnums.begin(), popupEnums.end(), value);
                if (i != popupEnums.end())
                {
                    popupEnums.erase(i);
                }
                for (auto j : popupEnums)
                {
                    popupList.push_back(j);
                }

                std::map<float, Popup> popupAreas;
                size_t j = 0;
                const size_t popupListSize = popupList.size();
                for (; j < popupListSize; ++j)
                {
                    Popup popup = popupList[j];
                    const BBox2f g = getPopupGeometry(popup, button, minimumSize);
                    const BBox2f clipped = g.intersect(area);
                    if (g == clipped)
                    {
                        out = popup;
                        break;
                    }
                    popupAreas[clipped.getArea()] = popup;
                }
                if (popupListSize == j)
                {
                    out = popupAreas.rbegin()->second;
                }
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

