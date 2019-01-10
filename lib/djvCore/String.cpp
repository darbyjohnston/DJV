//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/String.h>

#include <djvCore/Math.h>

#include <cctype>
#include <codecvt>
#include <iomanip>
#include <locale>

namespace djv
{
    namespace Core
    {
        namespace String
        {
            std::string toUpper(const std::string& value)
            {
                std::string out;
                for (auto i : value)
                {
                    out.push_back(std::toupper(i));
                }
                return out;
            }

            std::string toLower(const std::string& value)
            {
                std::string out;
                for (auto i : value)
                {
                    out.push_back(std::tolower(i));
                }
                return out;
            }

            std::string indent(size_t indent, size_t count)
            {
                return std::string(indent * count, ' ');
            }

            void removeTrailingNewline(std::string& value)
            {
                size_t size = value.size();
                if (size && '\n' == value[size - 1])
                {
                    value.pop_back();
                }
                if (size && '\r' == value[size - 1])
                {
                    value.pop_back();
                }
            }

            void fromString(const char * s, size_t size, float & out)
            {
                out = 0.f;

                // Find the sign.
                int isize = int(size);
                bool negativeSign = false;
                if ('-' == s[0])
                {
                    negativeSign = true;
                    ++s;
                    --isize;
                }
                else if ('+' == s[0])
                {
                    ++s;
                    --isize;
                }

                // Find the engineering notation.
                int e = 0;
                for (int j = isize - 1; j >= 0; --j)
                {
                    if ('e' == s[j] || 'E' == s[j])
                    {
                        if (j < isize - 1)
                        {
                            fromString(s + j + 1, isize - j - 1, e);
                        }
                        isize = j;
                        break;
                    }
                }

                // Find the decimal point.
                int decimalPoint = -1;
                for (int j = isize - 1; j >= 0; --j)
                {
                    if ('.' == s[j])
                    {
                        decimalPoint = j;
                        break;
                    }
                }

                // Add up the digits.
                float tens = 1.f;
                for (int j = (decimalPoint != -1 ? decimalPoint : isize) - 1; j >= 0; --j, tens *= 10.f)
                {
                    out += (s[j] - 48) * tens;
                }

                // Add up the decimal digits.
                if (decimalPoint != -1)
                {
                    tens = .1f;
                    for (int j = decimalPoint + 1; j < isize; ++j, tens /= 10.f)
                    {
                        out += (s[j] - 48) * tens;
                    }
                }

                // Apply the engineering notation.
                if (e != 0)
                {
                    tens = e < 0 ? .1f : 10.f;
                    if (e < 0) e = -e;
                    for (int j = 0; j < e; ++j)
                    {
                        out *= tens;
                    }
                }

                // Apply the sign.
                if (negativeSign)
                {
                    out = -out;
                }
            }

            std::wstring toWide(const std::string & value)
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.from_bytes(value);
            }

            std::string fromWide(const std::wstring & value)
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.to_bytes(value);
            }

            const std::string & getAlphabetLower()
            {
                static const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
                return alphabet;
            }

            const std::string & getAlphabetUpper()
            {
                static const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                return alphabet;
            }

            const std::vector<std::string>& getTestNames()
            {
                // Reference:
                // - http://calfish.ucdavis.edu/species/
                static const std::vector<std::string> fishes =
                {
                    DJV_TEXT("djv::Core::String", "Amargosa Canyon Speckled Dace"),
                    DJV_TEXT("djv::Core::String", "Amargosa River Pupfish"),
                    DJV_TEXT("djv::Core::String", "American Shad"),
                    DJV_TEXT("djv::Core::String", "Arroyo Chub"),
                    DJV_TEXT("djv::Core::String", "Bigeye Marbled Sculpin"),
                    DJV_TEXT("djv::Core::String", "Bigscale Logperch"),
                    DJV_TEXT("djv::Core::String", "Black Bullhead"),
                    DJV_TEXT("djv::Core::String", "Black Crappie"),
                    DJV_TEXT("djv::Core::String", "Blue Catfish"),
                    DJV_TEXT("djv::Core::String", "Blue Chub"),
                    DJV_TEXT("djv::Core::String", "Bluefin Killifish"),
                    DJV_TEXT("djv::Core::String", "Bluegill"),
                    DJV_TEXT("djv::Core::String", "Lepomis macrochirus"),
                    DJV_TEXT("djv::Core::String", "Bonytail"),
                    DJV_TEXT("djv::Core::String", "Brook Stickleback"),
                    DJV_TEXT("djv::Core::String", "Brook Trout"),
                    DJV_TEXT("djv::Core::String", "Brown Bullhead"),
                    DJV_TEXT("djv::Core::String", "Brown Trout"),
                    DJV_TEXT("djv::Core::String", "Bull Trout"),
                    DJV_TEXT("djv::Core::String", "California Coast Fall Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "California Golden Trout"),
                    DJV_TEXT("djv::Core::String", "California Killifish"),
                    DJV_TEXT("djv::Core::String", "California Tilapia"),
                    DJV_TEXT("djv::Core::String", "Central California Coast Winter Steelhead"),
                    DJV_TEXT("djv::Core::String", "Central California Roach"),
                    DJV_TEXT("djv::Core::String", "Central Coast Coho Salmon"),
                    DJV_TEXT("djv::Core::String", "Central Valley Fall Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "Central Valley late Fall Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "Oncorhynchus tshawytscha"),
                    DJV_TEXT("djv::Core::String", "Central Valley spring Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "Central Valley Steelhead"),
                    DJV_TEXT("djv::Core::String", "Central Valley Winter Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "Channel Catfish"),
                    DJV_TEXT("djv::Core::String", "Chum Salmon"),
                    DJV_TEXT("djv::Core::String", "Clear Lake Hitch"),
                    DJV_TEXT("djv::Core::String", "Clear Lake Prickly Sculpin"),
                    DJV_TEXT("djv::Core::String", "Clear Lake Roach"),
                    DJV_TEXT("djv::Core::String", "Clear Lake Splittail"),
                    DJV_TEXT("djv::Core::String", "Clear Lake Tule Perch"),
                    DJV_TEXT("djv::Core::String", "Coastal Cutthroat Trout"),
                    DJV_TEXT("djv::Core::String", "Coastal Rainbow Trout"),
                    DJV_TEXT("djv::Core::String", "Coastal Threespine Stickleback"),
                    DJV_TEXT("djv::Core::String", "Coastrange Sculpin"),
                    DJV_TEXT("djv::Core::String", "Colorado Pikeminnow"),
                    DJV_TEXT("djv::Core::String", "Colorado River Cutthroat Trout"),
                    DJV_TEXT("djv::Core::String", "Common Carp"),
                    DJV_TEXT("djv::Core::String", "Cottonball Marsh Pupfish"),
                    DJV_TEXT("djv::Core::String", "Cow Head Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Delta Smelt"),
                    DJV_TEXT("djv::Core::String", "Desert Pupfish"),
                    DJV_TEXT("djv::Core::String", "Eagle Lake Rainbow Trout"),
                    DJV_TEXT("djv::Core::String", "Eagle Lake Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Eulachon"),
                    DJV_TEXT("djv::Core::String", "Fathead Minnow"),
                    DJV_TEXT("djv::Core::String", "Flannelmouth Sucker"),
                    DJV_TEXT("djv::Core::String", "Flathead Catfish"),
                    DJV_TEXT("djv::Core::String", "Golden Shiner"),
                    DJV_TEXT("djv::Core::String", "Goldfish"),
                    DJV_TEXT("djv::Core::String", "Goose Lake Lamprey"),
                    DJV_TEXT("djv::Core::String", "Goose Lake Redband Trout"),
                    DJV_TEXT("djv::Core::String", "Goose Lake Sucker"),
                    DJV_TEXT("djv::Core::String", "Goose Lake Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Grass Carp"),
                    DJV_TEXT("djv::Core::String", "Green Sunfish"),
                    DJV_TEXT("djv::Core::String", "Gualala Roach"),
                    DJV_TEXT("djv::Core::String", "Hardhead"),
                    DJV_TEXT("djv::Core::String", "High Rock Springs Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Humboldt Sucker"),
                    DJV_TEXT("djv::Core::String", "Inland Threespine Stickleback"),
                    DJV_TEXT("djv::Core::String", "Kern Brook Lamprey"),
                    DJV_TEXT("djv::Core::String", "Kern River Rainbow Trout"),
                    DJV_TEXT("djv::Core::String", "Klamath Largescale Sucker"),
                    DJV_TEXT("djv::Core::String", "Klamath Mountains Province Summer Steelhead"),
                    DJV_TEXT("djv::Core::String", "Klamath Mountains Province Winter Steelhead"),
                    DJV_TEXT("djv::Core::String", "Klamath River Lamprey"),
                    DJV_TEXT("djv::Core::String", "Klamath Smallscale Sucker"),
                    DJV_TEXT("djv::Core::String", "Klamath Speckled Dace"),
                    DJV_TEXT("djv::Core::String", "Klamath Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Lahontan Cutthroat Trout"),
                    DJV_TEXT("djv::Core::String", "Lahontan Lake Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Lahontan Mountain Sucker"),
                    DJV_TEXT("djv::Core::String", "Lahontan Redside"),
                    DJV_TEXT("djv::Core::String", "Lahontan Speckled Dace"),
                    DJV_TEXT("djv::Core::String", "Lahontan Stream Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Lake Trout"),
                    DJV_TEXT("djv::Core::String", "Largemouth Bass"),
                    DJV_TEXT("djv::Core::String", "Little Kern Golden Trout"),
                    DJV_TEXT("djv::Core::String", "Long Valley Speckled Dace"),
                    DJV_TEXT("djv::Core::String", "Longfin Smelt"),
                    DJV_TEXT("djv::Core::String", "Longjaw Mudsucker"),
                    DJV_TEXT("djv::Core::String", "Lost River Sucker"),
                    DJV_TEXT("djv::Core::String", "Lower Klamath Marbled Sculpin"),
                    DJV_TEXT("djv::Core::String", "McCloud River Redband Trout"),
                    DJV_TEXT("djv::Core::String", "Mississippi Silversides"),
                    DJV_TEXT("djv::Core::String", "Modoc Sucker"),
                    DJV_TEXT("djv::Core::String", "Mojave Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Monterey Hitch"),
                    DJV_TEXT("djv::Core::String", "Monterey Roach"),
                    DJV_TEXT("djv::Core::String", "Monterey Sucker"),
                    DJV_TEXT("djv::Core::String", "Mountain Whitefish"),
                    DJV_TEXT("djv::Core::String", "Navarro Roach"),
                    DJV_TEXT("djv::Core::String", "Northern (Pit) Roach"),
                    DJV_TEXT("djv::Core::String", "Northern California Brook Lamprey"),
                    DJV_TEXT("djv::Core::String", "Northern California Coast Summer Steelhead"),
                    DJV_TEXT("djv::Core::String", "Northern California Coast Winter Steelhead"),
                    DJV_TEXT("djv::Core::String", "Northern Green Sturgeon"),
                    DJV_TEXT("djv::Core::String", "Owens Pupfish"),
                    DJV_TEXT("djv::Core::String", "Owens Speckled Dace"),
                    DJV_TEXT("djv::Core::String", "Owens Sucker"),
                    DJV_TEXT("djv::Core::String", "Owens Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Pacific Lamprey"),
                    DJV_TEXT("djv::Core::String", "Paiute Cutthroat Trout"),
                    DJV_TEXT("djv::Core::String", "Paiute Sculpin"),
                    DJV_TEXT("djv::Core::String", "Pink Salmon"),
                    DJV_TEXT("djv::Core::String", "Pit River Tui Chub"),
                    DJV_TEXT("djv::Core::String", "Pit Sculpin"),
                    DJV_TEXT("djv::Core::String", "Pit-Klamath Brook Lamprey"),
                    DJV_TEXT("djv::Core::String", "Porthole Livebearer"),
                    DJV_TEXT("djv::Core::String", "Prickly Sculpin"),
                    DJV_TEXT("djv::Core::String", "Pumpkinseed"),
                    DJV_TEXT("djv::Core::String", "Rainwater Killifish"),
                    DJV_TEXT("djv::Core::String", "Razorback Sucker"),
                    DJV_TEXT("djv::Core::String", "Red Hills Roach"),
                    DJV_TEXT("djv::Core::String", "Red Shiner"),
                    DJV_TEXT("djv::Core::String", "Redear Sunfish"),
                    DJV_TEXT("djv::Core::String", "Redeye Bass"),
                    DJV_TEXT("djv::Core::String", "Reticulate Sculpin"),
                    DJV_TEXT("djv::Core::String", "Riffle Sculpin"),
                    DJV_TEXT("djv::Core::String", "River Lamprey"),
                    DJV_TEXT("djv::Core::String", "Rough Sculpin"),
                    DJV_TEXT("djv::Core::String", "Russian River Roach"),
                    DJV_TEXT("djv::Core::String", "Russian River Tule Perch"),
                    DJV_TEXT("djv::Core::String", "Sacramento Blackfish"),
                    DJV_TEXT("djv::Core::String", "Sacramento Hitch"),
                    DJV_TEXT("djv::Core::String", "Sacramento Perch"),
                    DJV_TEXT("djv::Core::String", "Sacramento Pikeminnow"),
                    DJV_TEXT("djv::Core::String", "Sacramento Speckled Dace"),
                    DJV_TEXT("djv::Core::String", "Sacramento Splittail"),
                    DJV_TEXT("djv::Core::String", "Sacramento Sucker"),
                    DJV_TEXT("djv::Core::String", "Sacramento Tule Perch"),
                    DJV_TEXT("djv::Core::String", "Sailfin Molly"),
                    DJV_TEXT("djv::Core::String", "Salt Creek Pupfish"),
                    DJV_TEXT("djv::Core::String", "Santa Ana Speckled Dace"),
                    DJV_TEXT("djv::Core::String", "Santa Ana Sucker"),
                    DJV_TEXT("djv::Core::String", "Saratoga Springs Pupfish"),
                    DJV_TEXT("djv::Core::String", "Shay Creek Stickleback"),
                    DJV_TEXT("djv::Core::String", "Shimofuri Goby"),
                    DJV_TEXT("djv::Core::String", "Shiner Perch"),
                    DJV_TEXT("djv::Core::String", "Shortfin Molly"),
                    DJV_TEXT("djv::Core::String", "Shortnose Sucker"),
                    DJV_TEXT("djv::Core::String", "Shoshone Pupfish"),
                    DJV_TEXT("djv::Core::String", "Smallmouth Bass"),
                    DJV_TEXT("djv::Core::String", "Sockeye (Kokanee) Salmon"),
                    DJV_TEXT("djv::Core::String", "South Central California Coast Steelhead"),
                    DJV_TEXT("djv::Core::String", "Southern California Steelhead"),
                    DJV_TEXT("djv::Core::String", "Southern Green Sturgeon"),
                    DJV_TEXT("djv::Core::String", "Southern Oregon Northern California Coast Coho Salmon"),
                    DJV_TEXT("djv::Core::String", "Southern Oregon Northern California Coast Fall Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "Spotted Bass"),
                    DJV_TEXT("djv::Core::String", "Staghorn Sculpin"),
                    DJV_TEXT("djv::Core::String", "Starry Flounder"),
                    DJV_TEXT("djv::Core::String", "Striped Bass"),
                    DJV_TEXT("djv::Core::String", "Striped Mullet"),
                    DJV_TEXT("djv::Core::String", "Surf Smelt"),
                    DJV_TEXT("djv::Core::String", "Tahoe Sucker"),
                    DJV_TEXT("djv::Core::String", "Tecopa Pupfish"),
                    DJV_TEXT("djv::Core::String", "Tench"),
                    DJV_TEXT("djv::Core::String", "Thicktail Chub"),
                    DJV_TEXT("djv::Core::String", "Threadfin Shad"),
                    DJV_TEXT("djv::Core::String", "Tidewater Goby"),
                    DJV_TEXT("djv::Core::String", "Tomales Roach"),
                    DJV_TEXT("djv::Core::String", "Topsmelt"),
                    DJV_TEXT("djv::Core::String", "Unarmored Threespine Stickleback"),
                    DJV_TEXT("djv::Core::String", "Upper Klamath Marbled Sculpin"),
                    DJV_TEXT("djv::Core::String", "Upper Klamath-Trinity Fall Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "Upper Klamath-Trinity Spring Chinook Salmon"),
                    DJV_TEXT("djv::Core::String", "Wakasagi"),
                    DJV_TEXT("djv::Core::String", "Warmouth"),
                    DJV_TEXT("djv::Core::String", "Western Brook Lamprey"),
                    DJV_TEXT("djv::Core::String", "Western Mosquitofish"),
                    DJV_TEXT("djv::Core::String", "White Bass"),
                    DJV_TEXT("djv::Core::String", "White Catfish"),
                    DJV_TEXT("djv::Core::String", "White Crappie"),
                    DJV_TEXT("djv::Core::String", "White Sturgeon"),
                    DJV_TEXT("djv::Core::String", "Yellow Bullhead"),
                    DJV_TEXT("djv::Core::String", "Yellow Perch"),
                    DJV_TEXT("djv::Core::String", "Yellowfin Goby")
                };
                return fishes;
            }

            std::vector<std::string> getRandomNames(size_t value)
            {
                const std::vector<std::string>& data = getTestNames();
                std::vector<std::string> out;
                for (size_t i = 0; i < value; ++i)
                {
                    out.push_back(Math::getRandom(data));
                }
                return out;
            }

            const std::string& getRandomName()
            {
                const std::vector<std::string>& data = getTestNames();
                return Math::getRandom(data);
            }

            const std::vector<std::string>& getTestSentences()
            {
                // Reference:
                // - http://calfish.ucdavis.edu/species/?uid=1&ds=698
                static const std::vector<std::string> fishes =
                {
                    DJV_TEXT("djv::Core::String", "Speckled Dace are capable of living in an array of habitats from small springs or streams to large rivers and deep lakes."),
                    DJV_TEXT("djv::Core::String", "Speckled Dace prefer habitat that includes clear, well oxygenated water, with movement due to a current or waves."),
                    DJV_TEXT("djv::Core::String", "In addition the fish thrive in areas with deep cover or overhead protection from vegetation or woody debris."),
                    DJV_TEXT("djv::Core::String", "Speckled Dace predominantly occupy small streams of the second to third order where they feed and forage for aquatic insects."),
                    DJV_TEXT("djv::Core::String", "The species is very adaptable and is found in cold alpine lakes such as Lake Tahoe, but has also been able to survive adversity in temperatures at or above 31\u2103 with a dissolved oxygen concentration of 1 mg / L."),
                    DJV_TEXT("djv::Core::String", "In streams speckled dace swim along the bottom looking for small invertebrates while in lakes they are opportunistic feeders that may feed on zooplankton, algae, nymphs, or the resulting flying insects."),
                    DJV_TEXT("djv::Core::String", "Speckled Dace typically have a life span of three years, but may live 6 or more years, during which time females typically grow more rapidly than males."),
                    DJV_TEXT("djv::Core::String", "The fish become sexually mature in their second year and during the summer months the dace spawn."),
                    DJV_TEXT("djv::Core::String", "Stream dwellers spawn in riffles or gravely areas, while the lake inhabitants spawn in tributaries or in shallow shoreline regions."),
                    DJV_TEXT("djv::Core::String", "The embryos hatch in 6 days, and the larvae remain in the safety of the gravel for 7 - 8 days."),
                    DJV_TEXT("djv::Core::String", "The young fry then spend the early part of their lives in the shallow warm areas of the stream or lake where they hatched.")
                };
                return fishes;
            }

            const std::string& getRandomSentence()
            {
                const std::vector<std::string>& data = getTestSentences();
                const size_t size = data.size();
                return data[Math::getRandom(static_cast<int>(size) - 1)];
            }

            std::vector<std::string> getRandomSentences(size_t value)
            {
                const std::vector<std::string>& data = getTestSentences();
                std::vector<std::string> out;
                for (size_t i = 0; i < value; ++i)
                {
                    out.push_back(Math::getRandom(data));
                }
                return out;
            }

            std::string getRandomText(size_t sentenceCount)
            {
                const std::vector<std::string>& data = getTestSentences();
                std::vector<std::string> out;
                size_t i = 0;
                while (i < sentenceCount)
                {
                    std::vector<std::string> paragraph;
                    size_t paragraphLineCount = Math::getRandom(2, 5);
                    for (size_t j = 0; j < paragraphLineCount && i < sentenceCount; ++j, ++i)
                    {
                        const std::string& sentence = Math::getRandom(data);
                        paragraph.push_back(sentence);
                    }
                    out.push_back(String::join(paragraph, " "));
                }
                return String::join(out, "\n\n");
            }

        } // namespace String
    } // namespace Core

    namespace
    {
        const std::vector<std::string> boolLabels =
        {
            DJV_TEXT("djv::Core::String", "False"),
            DJV_TEXT("djv::Core::String", "True")
        };

    } // namespace
} // namespace djv
