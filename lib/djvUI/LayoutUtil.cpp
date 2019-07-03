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

#include <djvUI/LayoutUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            namespace
            {
                enum PopupCandidate
                {
                    BelowRight,
                    BelowLeft,
                    AboveRight,
                    AboveLeft
                };

                BBox2f getPopupCandidate(PopupCandidate value, const glm::vec2& pos, const glm::vec2& minimumSize)
                {
                    BBox2f out;
                    switch (value)
                    {
                    case BelowRight:
                        out = BBox2f(
                            glm::vec2(pos.x, pos.y),
                            glm::vec2(pos.x + minimumSize.x, pos.y + minimumSize.y));
                        break;
                    case BelowLeft:
                        out = BBox2f(
                            glm::vec2(pos.x - minimumSize.x, pos.y),
                            glm::vec2(pos.x, pos.y + minimumSize.y));
                        break;
                    case AboveRight:
                        out = BBox2f(
                            glm::vec2(pos.x, pos.y - minimumSize.y),
                            glm::vec2(pos.x + minimumSize.x, pos.y));
                        break;
                    case AboveLeft:
                        out = BBox2f(
                            glm::vec2(pos.x - minimumSize.x, pos.y - minimumSize.y),
                            glm::vec2(pos.x, pos.y));
                        break;
                    }
                    return out;
                }

                BBox2f getPopupCandidate(PopupCandidate value, const BBox2f& button, const glm::vec2& minimumSize)
                {
                    BBox2f out;
                    switch (value)
                    {
                    case BelowLeft:
                        out = BBox2f(
                            glm::vec2(
                                std::min(button.max.x - minimumSize.x, button.min.x),
                                button.max.y),
                            glm::vec2(
                                button.max.x,
                                button.max.y + minimumSize.y));
                        break;
                    case BelowRight:
                        out = BBox2f(
                            glm::vec2(
                                button.min.x,
                                button.max.y),
                            glm::vec2(
                                std::max(button.min.x + minimumSize.x, button.max.x),
                                button.max.y + minimumSize.y));
                        break;
                    case AboveRight:
                        out = BBox2f(
                            glm::vec2(
                                button.min.x,
                                button.min.y - minimumSize.y),
                            glm::vec2(
                                std::max(button.min.x + minimumSize.x, button.max.x),
                                button.min.y));
                        break;
                    case AboveLeft:
                        out = BBox2f(
                            glm::vec2(
                                std::min(button.max.x - minimumSize.x, button.min.x),
                                button.min.y - minimumSize.y),
                            glm::vec2(
                                button.max.x,
                                button.min.y));
                        break;
                    }
                    return out;
                }

            } // namespace

            BBox2f getPopupGeometry(const BBox2f& area, const glm::vec2& pos, const glm::vec2& minimumSize)
            {
                BBox2f out(0.f, 0.f, 0.f, 0.f);
                std::map<float, BBox2f> popupCandidates;
                size_t j = 0;
                for (; j < 4; ++j)
                {
                    const BBox2f popupCandidate = getPopupCandidate(static_cast<PopupCandidate>(j), pos, minimumSize);
                    const BBox2f clipped = popupCandidate.intersect(area);
                    if (popupCandidate == clipped)
                    {
                        out = clipped;
                        break;
                    }
                    popupCandidates[clipped.getArea()] = clipped;
                }
                if (4 == j)
                {
                    out = popupCandidates.rbegin()->second;
                }
                return out;
            }

            BBox2f getPopupGeometry(const BBox2f& area, const BBox2f& button, const glm::vec2& minimumSize)
            {
                BBox2f out(0.f, 0.f, 0.f, 0.f);
                std::map<float, BBox2f> popupCandidates;
                size_t j = 0;
                for (; j < 4; ++j)
                {
                    const BBox2f popupCandidate = getPopupCandidate(static_cast<PopupCandidate>(j), button, minimumSize);
                    const BBox2f clipped = popupCandidate.intersect(area);
                    if (popupCandidate == clipped)
                    {
                        out = clipped;
                        break;
                    }
                    popupCandidates[clipped.getArea()] = clipped;
                }
                if (4 == j)
                {
                    out = popupCandidates.rbegin()->second;
                }
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

