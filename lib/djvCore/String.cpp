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

#include <djvCore/String.h>

#include <djvCore/Math.h>

#include <cctype>
#include <codecvt>
#include <iomanip>
#include <locale>
#include <regex>

namespace djv
{
    namespace Core
    {
        namespace String
        {
            std::string toUpper(const std::string & value)
            {
                std::string out;
                for (auto i : value)
                {
                    out.push_back(std::toupper(i));
                }
                return out;
            }

            std::string toLower(const std::string & value)
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

            void removeTrailingNewline(std::string & value)
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

            bool match(const std::string & value, const std::string & expression)
            {
                bool out = false;
                std::smatch m;
                try
                {
                    std::regex r(expression, std::regex_constants::icase);
                    std::regex_search(value, m, r);
                    out = m.size() > 0;
                }
                catch (const std::exception&)
                {
                    out = false;
                }
                return out;
            }

            void fromString(const char * s, size_t size, float & out)
            {
                out = 0.F;

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
                            fromString(s + j + 1, isize - static_cast<size_t>(j) - static_cast<size_t>(1), e);
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
                float tens = 1.F;
                for (int j = (decimalPoint != -1 ? decimalPoint : isize) - 1; j >= 0; --j, tens *= 10.F)
                {
                    out += (s[j] - 48) * tens;
                }

                // Add up the decimal digits.
                if (decimalPoint != -1)
                {
                    tens = .1F;
                    for (int j = decimalPoint + 1; j < isize; ++j, tens /= 10.F)
                    {
                        out += (s[j] - 48) * tens;
                    }
                }

                // Apply the engineering notation.
                if (e != 0)
                {
                    tens = e < 0 ? .1F : 10.F;
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

            std::string escape(const std::string & value)
            {
                std::string out;
                for (const auto i : value)
                {
                    if ('\\' == i)
                    {
                        out.push_back('\\');
                    }
                    out.push_back(i);
                }
                return out;
            }

            std::string unescape(const std::string & value)
            {
                std::string out;
                const size_t size = value.size();
                for (size_t i = 0; i < size; ++i)
                {
                    out.push_back(value[i]);
                    if (i < size - 1 && '\\' == value[i] && '\\' == value[i + 1])
                    {
                        ++i;
                    }
                }
                return out;
            }

            std::string getAlphabetLower()
            {
                const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
                return alphabet;
            }

            std::string getAlphabetUpper()
            {
                const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                return alphabet;
            }

            std::vector<std::string> getTestNames()
            {
                // Reference:
                // - http://calfish.ucdavis.edu/species/
                const std::vector<std::string> fishes =
                {
                    DJV_TEXT("Amargosa Canyon Speckled Dace"),
                    DJV_TEXT("Amargosa River Pupfish"),
                    DJV_TEXT("American Shad"),
                    DJV_TEXT("Arroyo Chub"),
                    DJV_TEXT("Bigeye Marbled Sculpin"),
                    DJV_TEXT("Bigscale Logperch"),
                    DJV_TEXT("Black Bullhead"),
                    DJV_TEXT("Black Crappie"),
                    DJV_TEXT("Blue Catfish"),
                    DJV_TEXT("Blue Chub"),
                    DJV_TEXT("Bluefin Killifish"),
                    DJV_TEXT("Bluegill"),
                    DJV_TEXT("Lepomis macrochirus"),
                    DJV_TEXT("Bonytail"),
                    DJV_TEXT("Brook Stickleback"),
                    DJV_TEXT("Brook Trout"),
                    DJV_TEXT("Brown Bullhead"),
                    DJV_TEXT("Brown Trout"),
                    DJV_TEXT("Bull Trout"),
                    DJV_TEXT("California Coast Fall Chinook Salmon"),
                    DJV_TEXT("California Golden Trout"),
                    DJV_TEXT("California Killifish"),
                    DJV_TEXT("California Tilapia"),
                    DJV_TEXT("Central California Coast Winter Steelhead"),
                    DJV_TEXT("Central California Roach"),
                    DJV_TEXT("Central Coast Coho Salmon"),
                    DJV_TEXT("Central Valley Fall Chinook Salmon"),
                    DJV_TEXT("Central Valley late Fall Chinook Salmon"),
                    DJV_TEXT("Oncorhynchus tshawytscha"),
                    DJV_TEXT("Central Valley spring Chinook Salmon"),
                    DJV_TEXT("Central Valley Steelhead"),
                    DJV_TEXT("Central Valley Winter Chinook Salmon"),
                    DJV_TEXT("Channel Catfish"),
                    DJV_TEXT("Chum Salmon"),
                    DJV_TEXT("Clear Lake Hitch"),
                    DJV_TEXT("Clear Lake Prickly Sculpin"),
                    DJV_TEXT("Clear Lake Roach"),
                    DJV_TEXT("Clear Lake Splittail"),
                    DJV_TEXT("Clear Lake Tule Perch"),
                    DJV_TEXT("Coastal Cutthroat Trout"),
                    DJV_TEXT("Coastal Rainbow Trout"),
                    DJV_TEXT("Coastal Threespine Stickleback"),
                    DJV_TEXT("Coastrange Sculpin"),
                    DJV_TEXT("Colorado Pikeminnow"),
                    DJV_TEXT("Colorado River Cutthroat Trout"),
                    DJV_TEXT("Common Carp"),
                    DJV_TEXT("Cottonball Marsh Pupfish"),
                    DJV_TEXT("Cow Head Tui Chub"),
                    DJV_TEXT("Delta Smelt"),
                    DJV_TEXT("Desert Pupfish"),
                    DJV_TEXT("Eagle Lake Rainbow Trout"),
                    DJV_TEXT("Eagle Lake Tui Chub"),
                    DJV_TEXT("Eulachon"),
                    DJV_TEXT("Fathead Minnow"),
                    DJV_TEXT("Flannelmouth Sucker"),
                    DJV_TEXT("Flathead Catfish"),
                    DJV_TEXT("Golden Shiner"),
                    DJV_TEXT("Goldfish"),
                    DJV_TEXT("Goose Lake Lamprey"),
                    DJV_TEXT("Goose Lake Redband Trout"),
                    DJV_TEXT("Goose Lake Sucker"),
                    DJV_TEXT("Goose Lake Tui Chub"),
                    DJV_TEXT("Grass Carp"),
                    DJV_TEXT("Green Sunfish"),
                    DJV_TEXT("Gualala Roach"),
                    DJV_TEXT("Hardhead"),
                    DJV_TEXT("High Rock Springs Tui Chub"),
                    DJV_TEXT("Humboldt Sucker"),
                    DJV_TEXT("Inland Threespine Stickleback"),
                    DJV_TEXT("Kern Brook Lamprey"),
                    DJV_TEXT("Kern River Rainbow Trout"),
                    DJV_TEXT("Klamath Largescale Sucker"),
                    DJV_TEXT("Klamath Mountains Province Summer Steelhead"),
                    DJV_TEXT("Klamath Mountains Province Winter Steelhead"),
                    DJV_TEXT("Klamath River Lamprey"),
                    DJV_TEXT("Klamath Smallscale Sucker"),
                    DJV_TEXT("Klamath Speckled Dace"),
                    DJV_TEXT("Klamath Tui Chub"),
                    DJV_TEXT("Lahontan Cutthroat Trout"),
                    DJV_TEXT("Lahontan Lake Tui Chub"),
                    DJV_TEXT("Lahontan Mountain Sucker"),
                    DJV_TEXT("Lahontan Redside"),
                    DJV_TEXT("Lahontan Speckled Dace"),
                    DJV_TEXT("Lahontan Stream Tui Chub"),
                    DJV_TEXT("Lake Trout"),
                    DJV_TEXT("Largemouth Bass"),
                    DJV_TEXT("Little Kern Golden Trout"),
                    DJV_TEXT("Long Valley Speckled Dace"),
                    DJV_TEXT("Longfin Smelt"),
                    DJV_TEXT("Longjaw Mudsucker"),
                    DJV_TEXT("Lost River Sucker"),
                    DJV_TEXT("Lower Klamath Marbled Sculpin"),
                    DJV_TEXT("McCloud River Redband Trout"),
                    DJV_TEXT("Mississippi Silversides"),
                    DJV_TEXT("Modoc Sucker"),
                    DJV_TEXT("Mojave Tui Chub"),
                    DJV_TEXT("Monterey Hitch"),
                    DJV_TEXT("Monterey Roach"),
                    DJV_TEXT("Monterey Sucker"),
                    DJV_TEXT("Mountain Whitefish"),
                    DJV_TEXT("Navarro Roach"),
                    DJV_TEXT("Northern (Pit) Roach"),
                    DJV_TEXT("Northern California Brook Lamprey"),
                    DJV_TEXT("Northern California Coast Summer Steelhead"),
                    DJV_TEXT("Northern California Coast Winter Steelhead"),
                    DJV_TEXT("Northern Green Sturgeon"),
                    DJV_TEXT("Owens Pupfish"),
                    DJV_TEXT("Owens Speckled Dace"),
                    DJV_TEXT("Owens Sucker"),
                    DJV_TEXT("Owens Tui Chub"),
                    DJV_TEXT("Pacific Lamprey"),
                    DJV_TEXT("Paiute Cutthroat Trout"),
                    DJV_TEXT("Paiute Sculpin"),
                    DJV_TEXT("Pink Salmon"),
                    DJV_TEXT("Pit River Tui Chub"),
                    DJV_TEXT("Pit Sculpin"),
                    DJV_TEXT("Pit-Klamath Brook Lamprey"),
                    DJV_TEXT("Porthole Livebearer"),
                    DJV_TEXT("Prickly Sculpin"),
                    DJV_TEXT("Pumpkinseed"),
                    DJV_TEXT("Rainwater Killifish"),
                    DJV_TEXT("Razorback Sucker"),
                    DJV_TEXT("Red Hills Roach"),
                    DJV_TEXT("Red Shiner"),
                    DJV_TEXT("Redear Sunfish"),
                    DJV_TEXT("Redeye Bass"),
                    DJV_TEXT("Reticulate Sculpin"),
                    DJV_TEXT("Riffle Sculpin"),
                    DJV_TEXT("River Lamprey"),
                    DJV_TEXT("Rough Sculpin"),
                    DJV_TEXT("Russian River Roach"),
                    DJV_TEXT("Russian River Tule Perch"),
                    DJV_TEXT("Sacramento Blackfish"),
                    DJV_TEXT("Sacramento Hitch"),
                    DJV_TEXT("Sacramento Perch"),
                    DJV_TEXT("Sacramento Pikeminnow"),
                    DJV_TEXT("Sacramento Speckled Dace"),
                    DJV_TEXT("Sacramento Splittail"),
                    DJV_TEXT("Sacramento Sucker"),
                    DJV_TEXT("Sacramento Tule Perch"),
                    DJV_TEXT("Sailfin Molly"),
                    DJV_TEXT("Salt Creek Pupfish"),
                    DJV_TEXT("Santa Ana Speckled Dace"),
                    DJV_TEXT("Santa Ana Sucker"),
                    DJV_TEXT("Saratoga Springs Pupfish"),
                    DJV_TEXT("Shay Creek Stickleback"),
                    DJV_TEXT("Shimofuri Goby"),
                    DJV_TEXT("Shiner Perch"),
                    DJV_TEXT("Shortfin Molly"),
                    DJV_TEXT("Shortnose Sucker"),
                    DJV_TEXT("Shoshone Pupfish"),
                    DJV_TEXT("Smallmouth Bass"),
                    DJV_TEXT("Sockeye (Kokanee) Salmon"),
                    DJV_TEXT("South Central California Coast Steelhead"),
                    DJV_TEXT("Southern California Steelhead"),
                    DJV_TEXT("Southern Green Sturgeon"),
                    DJV_TEXT("Southern Oregon Northern California Coast Coho Salmon"),
                    DJV_TEXT("Southern Oregon Northern California Coast Fall Chinook Salmon"),
                    DJV_TEXT("Spotted Bass"),
                    DJV_TEXT("Staghorn Sculpin"),
                    DJV_TEXT("Starry Flounder"),
                    DJV_TEXT("Striped Bass"),
                    DJV_TEXT("Striped Mullet"),
                    DJV_TEXT("Surf Smelt"),
                    DJV_TEXT("Tahoe Sucker"),
                    DJV_TEXT("Tecopa Pupfish"),
                    DJV_TEXT("Tench"),
                    DJV_TEXT("Thicktail Chub"),
                    DJV_TEXT("Threadfin Shad"),
                    DJV_TEXT("Tidewater Goby"),
                    DJV_TEXT("Tomales Roach"),
                    DJV_TEXT("Topsmelt"),
                    DJV_TEXT("Unarmored Threespine Stickleback"),
                    DJV_TEXT("Upper Klamath Marbled Sculpin"),
                    DJV_TEXT("Upper Klamath-Trinity Fall Chinook Salmon"),
                    DJV_TEXT("Upper Klamath-Trinity Spring Chinook Salmon"),
                    DJV_TEXT("Wakasagi"),
                    DJV_TEXT("Warmouth"),
                    DJV_TEXT("Western Brook Lamprey"),
                    DJV_TEXT("Western Mosquitofish"),
                    DJV_TEXT("White Bass"),
                    DJV_TEXT("White Catfish"),
                    DJV_TEXT("White Crappie"),
                    DJV_TEXT("White Sturgeon"),
                    DJV_TEXT("Yellow Bullhead"),
                    DJV_TEXT("Yellow Perch"),
                    DJV_TEXT("Yellowfin Goby")
                };
                return fishes;
            }

            std::vector<std::string> getRandomNames(size_t value)
            {
                const std::vector<std::string> & data = getTestNames();
                std::vector<std::string> out;
                for (size_t i = 0; i < value; ++i)
                {
                    out.push_back(Math::getRandom(data));
                }
                return out;
            }

            std::string getRandomName()
            {
                const std::vector<std::string> & data = getTestNames();
                return Math::getRandom(data);
            }

            std::vector<std::string> getTestSentences()
            {
                // The Raven, by Edgar Allan Poe
                const std::vector<std::string> text =
                {
                    "Once upon a midnight dreary, while I pondered, weak and weary,",
                    "Over many a quaint and curious volume of forgotten lore,",
                    "While I nodded, nearly napping, suddenly there came a tapping,",
                    "As of some one gently rapping, rapping at my chamber door.",
                    "\"'T is some visiter,\" I muttered, \"tapping at my chamber door--",
                    "                                          Only this, and nothing more.\"",
                    "",
                    "Ah, distinctly I remember it was in the bleak December,",
                    "And each separate dying ember wrought its ghost upon the floor.",
                    "Eagerly I wished the morrow:--vainly I had sought to borrow",
                    "From my books surcease of sorrow--sorrow for the lost Lenore--",
                    "For the rare and radiant maiden whom the angels name Lenore--",
                    "                                          Nameless here for evermore.",
                    "",
                    "And the silken sad uncertain rustling of each purple curtain",
                    "Thrilled me--filled me with fantastic terrors never felt before;",
                    "So that now, to still the beating of my heart, I stood repeating",
                    "\"'T is some visiter entreating entrance at my chamber door",
                    "Some late visiter entreating entrance at my chamber door;--",
                    "                                          This it is, and nothing more.\"",
                    "",
                    "Presently my soul grew stronger; hesitating then no longer,",
                    "\"Sir,\" said I, \"or Madam, truly your forgiveness I implore;",
                    "But the fact is I was napping, and so gently you came rapping,",
                    "And so faintly you came tapping, tapping at my chamber door,",
                    "That I scarce was sure I heard you\"--here I opened wide the door;--",
                    "                                          Darkness there, and nothing more.",
                    "",
                    "Deep into that darkness peering, long I stood there wondering, fearing,",
                    "Doubting, dreaming dreams no mortal ever dared to dream before;",
                    "But the silence was unbroken, and the darkness gave no token,",
                    "And the only word there spoken was the whispered word, \"Lenore!\"",
                    "This I whispered, and an echo murmured back the word, \"Lenore!\"",
                    "                                          Merely this and nothing more.",
                    "",
                    "Back into the chamber turning, all my soul within me burning,",
                    "Soon again I heard a tapping, somewhat louder than before.",
                    "\"Surely,\" said I, \"surely that is something at my window lattice;",
                    "Let me see, then, what thereat is, and this mystery explore--",
                    "Let my heart be still a moment and this mystery explore;--",
                    "                                          'T is the wind and nothing more!\"",
                    "",
                    "Open here I flung the shutter, when, with many a flirt and flutter,",
                    "In there stepped a stately Raven of the saintly days of yore.",
                    "Not the least obeisance made he; not a minute stopped or stayed he;",
                    "But, with mien of lord or lady, perched above my chamber door--",
                    "Perched upon a bust of Pallas just above my chamber door--",
                    "                                          Perched, and sat, and nothing more.",
                    "",
                    "Then this ebony bird beguiling my sad fancy into smiling,",
                    "By the grave and stern decorum of the countenance it wore,",
                    "\"Though thy crest be shorn and shaven, thou,\" I said, \"art sure no craven,",
                    "Ghastly grim and ancient Raven wandering from the Nightly shore,--",
                    "Tell me what thy lordly name is on the Night's Plutonian shore!\"",
                    "                                          Quoth the Raven, \"Nevermore.\"",
                    "",
                    "Much I marvelled this ungainly fowl to hear discourse so plainly,",
                    "Though its answer little meaning--little relevancy bore;",
                    "For we cannot help agreeing that no living human being",
                    "Ever yet was blessed with seeing bird above his chamber door--",
                    "Bird or beast upon the sculptured bust above his chamber door,",
                    "                                          With such name as \"Nevermore.\"",
                    "",
                    "But the Raven, sitting lonely on the placid bust, spoke only",
                    "That one word, as if his soul in that one word he did outpour.",
                    "Nothing further then he uttered--not a feather then he fluttered--",
                    "Till I scarcely more than muttered, \"Other friends have flown before--",
                    "On the morrow _he_ will leave me, as my hopes have flown before.\"",
                    "                                          Then the bird said, \"Nevermore.\"",
                    "",
                    "Startled at the stillness broken by reply so aptly spoken,",
                    "\"Doubtless,\" said I, \"what it utters is its only stock and store,",
                    "Caught from some unhappy master whom unmerciful Disaster",
                    "Followed fast and followed faster till his songs one burden bore--",
                    "Till the dirges of his Hope that melancholy burden bore",
                    "                                          Of 'Never--nevermore.'\"",
                    "",
                    "But the Raven still beguiling all my sad soul into smiling,",
                    "Straight I wheeled a cushioned seat in front of bird and bust and door;",
                    "Then, upon the velvet sinking, I betook myself to linking",
                    "Fancy unto fancy, thinking what this ominous bird of yore--",
                    "What this grim, ungainly, ghastly, gaunt and ominous bird of yore",
                    "                                          Meant in croaking \"Nevermore.\"",
                    "",
                    "This I sat engaged in guessing, but no syllable expressing",
                    "To the fowl whose fiery eyes now burned into my bosom's core;",
                    "This and more I sat divining, with my head at ease reclining",
                    "On the cushion's velvet lining that the lamplight gloated o'er,",
                    "But whose velvet violet lining with the lamplight gloating o'er",
                    "                                          _She_ shall press, ah, nevermore!",
                    "",
                    "Then, methought, the air grew denser, perfumed from an unseen censer",
                    "Swung by seraphim whose foot-falls tinkled on the tufted floor.",
                    "\"Wretch,\" I cried, \"thy God hath lent thee--by these angels he hath sent thee",
                    "Respite--respite and nepenthe from thy memories of Lenore!",
                    "Quaff, oh quaff this kind nepenthe, and forget this lost Lenore!\"",
                    "                                          Quoth the Raven, \"Nevermore.\"",
                    "",
                    "\"Prophet!\" said I, \"thing of evil!--prophet still, if bird or devil!--",
                    "Whether Tempter sent, or whether tempest tossed thee here ashore,",
                    "Desolate yet all undaunted, on this desert land enchanted--",
                    "On this home by Horror haunted--tell me truly, I implore--",
                    "Is there--_is_ there balm in Gilead?--tell me--tell me, I implore!\"",
                    "                                          Quoth the Raven, \"Nevermore.\"",
                    "",
                    "\"Prophet!\" said I, \"thing of evil--prophet still, if bird or devil!",
                    "By that Heaven that bends above, us--by that God we both adore--",
                    "Tell this soul with sorrow laden if, within the distant Aidenn,",
                    "It shall clasp a sainted maiden whom the angels name Lenore--",
                    "Clasp a rare and radiant maiden whom the angels name Lenore.\"",
                    "                                          Quoth the Raven, \"Nevermore.\"",
                    "",
                    "\"Be that word our sign of parting, bird or fiend!\" I shrieked, upstarting--",
                    "\"Get thee back into the tempest and the Night's Plutonian shore!",
                    "Leave no black plume as a token of that lie thy soul hath spoken!",
                    "Leave my loneliness unbroken!--quit the bust above my door!",
                    "Take thy beak from out my heart, and take thy form from off my door!\"",
                    "                                          Quoth the Raven, \"Nevermore.\"",
                    "",
                    "And the Raven, never flitting, still is sitting, still is sitting",
                    "On the pallid bust of Pallas just above my chamber door;",
                    "And his eyes have all the seeming of a demon's that is dreaming,",
                    "And the lamplight o'er him streaming throws his shadow on the floor;",
                    "And my soul from out that shadow that lies floating on the floor",
                    "                                          Shall be lifted--nevermore!"
                };
                return text;
            }

            std::string getRandomSentence()
            {
                const std::vector<std::string> data = getTestSentences();
                const int size = static_cast<int>(data.size());
                return data[Math::getRandom(size - 1)];
            }

            std::vector<std::string> getRandomSentences(size_t value)
            {
                const std::vector<std::string> data = getTestSentences();
                std::vector<std::string> out;
                const int size = static_cast<int>(data.size());
                int index = Math::getRandom(size - 1);
                for (size_t i = 0; i < value; ++i)
                {
                    out.push_back(data[index]);
                    ++index;
                    if (index >= size)
                    {
                        index = 0;
                    }
                }
                return out;
            }

            std::string getRandomText(size_t sentenceCount)
            {
                return String::join(getRandomSentences(sentenceCount), "\n");
            }

        } // namespace String
    } // namespace Core

    namespace
    {
        const std::vector<std::string> boolLabels =
        {
            DJV_TEXT("False"),
            DJV_TEXT("True")
        };

    } // namespace
} // namespace djv
