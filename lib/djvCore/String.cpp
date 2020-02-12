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
                try
                {
                    std::regex r(expression, std::regex_constants::icase);
                    std::smatch m;
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
                    DJV_TEXT("amargosa_canyon_speckled_dace"),
                    DJV_TEXT("amargosa_river_pupfish"),
                    DJV_TEXT("american_shad"),
                    DJV_TEXT("arroyo_chub"),
                    DJV_TEXT("bigeye_marbled_sculpin"),
                    DJV_TEXT("bigscale_logperch"),
                    DJV_TEXT("black_bullhead"),
                    DJV_TEXT("black_crappie"),
                    DJV_TEXT("blue_catfish"),
                    DJV_TEXT("blue_chub"),
                    DJV_TEXT("bluefin_killifish"),
                    DJV_TEXT("bluegill"),
                    DJV_TEXT("lepomis_macrochirus"),
                    DJV_TEXT("bonytail"),
                    DJV_TEXT("brook_stickleback"),
                    DJV_TEXT("brook_trout"),
                    DJV_TEXT("brown_bullhead"),
                    DJV_TEXT("brown_trout"),
                    DJV_TEXT("bull_trout"),
                    DJV_TEXT("california_coast_fall_chinook_salmon"),
                    DJV_TEXT("california_golden_trout"),
                    DJV_TEXT("california_killifish"),
                    DJV_TEXT("california_tilapia"),
                    DJV_TEXT("central_california_coast_winter_steelhead"),
                    DJV_TEXT("central_california_roach"),
                    DJV_TEXT("central_coast_coho_salmon"),
                    DJV_TEXT("central_valley_fall_chinook_salmon"),
                    DJV_TEXT("central_valley_late_fall_chinook_salmon"),
                    DJV_TEXT("oncorhynchus_tshawytscha"),
                    DJV_TEXT("central_valley_spring_chinook_salmon"),
                    DJV_TEXT("central_valley_steelhead"),
                    DJV_TEXT("central_valley_winter_chinook_salmon"),
                    DJV_TEXT("channel_catfish"),
                    DJV_TEXT("chum_salmon"),
                    DJV_TEXT("clear_lake_hitch"),
                    DJV_TEXT("clear_lake_prickly_sculpin"),
                    DJV_TEXT("clear_lake_roach"),
                    DJV_TEXT("clear_lake_splittail"),
                    DJV_TEXT("clear_lake_tule_perch"),
                    DJV_TEXT("coastal_cutthroat_trout"),
                    DJV_TEXT("coastal_rainbow_trout"),
                    DJV_TEXT("coastal_threespine_stickleback"),
                    DJV_TEXT("coastrange_sculpin"),
                    DJV_TEXT("colorado_pikeminnow"),
                    DJV_TEXT("colorado_river_cutthroat_trout"),
                    DJV_TEXT("common_carp"),
                    DJV_TEXT("cottonball_marsh_pupfish"),
                    DJV_TEXT("cow_head_tui_chub"),
                    DJV_TEXT("delta_smelt"),
                    DJV_TEXT("desert_pupfish"),
                    DJV_TEXT("eagle_lake_rainbow_trout"),
                    DJV_TEXT("eagle_lake_tui_chub"),
                    DJV_TEXT("eulachon"),
                    DJV_TEXT("fathead_minnow"),
                    DJV_TEXT("flannelmouth_sucker"),
                    DJV_TEXT("flathead_catfish"),
                    DJV_TEXT("golden_shiner"),
                    DJV_TEXT("goldfish"),
                    DJV_TEXT("goose_lake_lamprey"),
                    DJV_TEXT("goose_lake_redband_trout"),
                    DJV_TEXT("goose_lake_sucker"),
                    DJV_TEXT("goose_lake_tui_chub"),
                    DJV_TEXT("grass_carp"),
                    DJV_TEXT("green_sunfish"),
                    DJV_TEXT("gualala_roach"),
                    DJV_TEXT("hardhead"),
                    DJV_TEXT("high_rock_springs_tui_chub"),
                    DJV_TEXT("humboldt_sucker"),
                    DJV_TEXT("inland_threespine_stickleback"),
                    DJV_TEXT("kern_brook_lamprey"),
                    DJV_TEXT("kern_river_rainbow_trout"),
                    DJV_TEXT("klamath_largescale_sucker"),
                    DJV_TEXT("klamath_mountains_province_summer_steelhead"),
                    DJV_TEXT("klamath_mountains_province_winter_steelhead"),
                    DJV_TEXT("klamath_river_lamprey"),
                    DJV_TEXT("klamath_smallscale_sucker"),
                    DJV_TEXT("klamath_speckled_dace"),
                    DJV_TEXT("klamath_tui_chub"),
                    DJV_TEXT("lahontan_cutthroat_trout"),
                    DJV_TEXT("lahontan_lake_tui_chub"),
                    DJV_TEXT("lahontan_mountain_sucker"),
                    DJV_TEXT("lahontan_redside"),
                    DJV_TEXT("lahontan_speckled_dace"),
                    DJV_TEXT("lahontan_stream_tui_chub"),
                    DJV_TEXT("lake_trout"),
                    DJV_TEXT("largemouth_bass"),
                    DJV_TEXT("little_kern_golden_trout"),
                    DJV_TEXT("long_valley_speckled_dace"),
                    DJV_TEXT("longfin_smelt"),
                    DJV_TEXT("longjaw_mudsucker"),
                    DJV_TEXT("lost_river_sucker"),
                    DJV_TEXT("lower_klamath_marbled_sculpin"),
                    DJV_TEXT("mccloud_river_redband_trout"),
                    DJV_TEXT("mississippi_silversides"),
                    DJV_TEXT("modoc_sucker"),
                    DJV_TEXT("mojave_tui_chub"),
                    DJV_TEXT("monterey_hitch"),
                    DJV_TEXT("monterey_roach"),
                    DJV_TEXT("monterey_sucker"),
                    DJV_TEXT("mountain_whitefish"),
                    DJV_TEXT("navarro_roach"),
                    DJV_TEXT("northern_pit_roach"),
                    DJV_TEXT("northern_california_brook_lamprey"),
                    DJV_TEXT("northern_california_coast_summer_steelhead"),
                    DJV_TEXT("northern_california_coast_winter_steelhead"),
                    DJV_TEXT("northern_green_sturgeon"),
                    DJV_TEXT("owens_pupfish"),
                    DJV_TEXT("owens_speckled_dace"),
                    DJV_TEXT("owens_sucker"),
                    DJV_TEXT("owens_tui_chub"),
                    DJV_TEXT("pacific_lamprey"),
                    DJV_TEXT("paiute_cutthroat_trout"),
                    DJV_TEXT("paiute_sculpin"),
                    DJV_TEXT("pink_salmon"),
                    DJV_TEXT("pit_river_tui_chub"),
                    DJV_TEXT("pit_sculpin"),
                    DJV_TEXT("pit-klamath_brook_lamprey"),
                    DJV_TEXT("porthole_livebearer"),
                    DJV_TEXT("prickly_sculpin"),
                    DJV_TEXT("pumpkinseed"),
                    DJV_TEXT("rainwater_killifish"),
                    DJV_TEXT("razorback_sucker"),
                    DJV_TEXT("red_hills_roach"),
                    DJV_TEXT("red_shiner"),
                    DJV_TEXT("redear_sunfish"),
                    DJV_TEXT("redeye_bass"),
                    DJV_TEXT("reticulate_sculpin"),
                    DJV_TEXT("riffle_sculpin"),
                    DJV_TEXT("river_lamprey"),
                    DJV_TEXT("rough_sculpin"),
                    DJV_TEXT("russian_river_roach"),
                    DJV_TEXT("russian_river_tule_perch"),
                    DJV_TEXT("sacramento_blackfish"),
                    DJV_TEXT("sacramento_hitch"),
                    DJV_TEXT("sacramento_perch"),
                    DJV_TEXT("sacramento_pikeminnow"),
                    DJV_TEXT("sacramento_speckled_dace"),
                    DJV_TEXT("sacramento_splittail"),
                    DJV_TEXT("sacramento_sucker"),
                    DJV_TEXT("sacramento_tule_perch"),
                    DJV_TEXT("sailfin_molly"),
                    DJV_TEXT("salt_creek_pupfish"),
                    DJV_TEXT("santa_ana_speckled_dace"),
                    DJV_TEXT("santa_ana_sucker"),
                    DJV_TEXT("saratoga_springs_pupfish"),
                    DJV_TEXT("shay_creek_stickleback"),
                    DJV_TEXT("shimofuri_goby"),
                    DJV_TEXT("shiner_perch"),
                    DJV_TEXT("shortfin_molly"),
                    DJV_TEXT("shortnose_sucker"),
                    DJV_TEXT("shoshone_pupfish"),
                    DJV_TEXT("smallmouth_bass"),
                    DJV_TEXT("sockeye_kokanee_salmon"),
                    DJV_TEXT("south_central_california_coast_steelhead"),
                    DJV_TEXT("southern_california_steelhead"),
                    DJV_TEXT("southern_green_sturgeon"),
                    DJV_TEXT("southern_oregon_northern_california_coast_coho_salmon"),
                    DJV_TEXT("southern_oregon_northern_california_coast_fall_chinook_salmon"),
                    DJV_TEXT("spotted_bass"),
                    DJV_TEXT("staghorn_sculpin"),
                    DJV_TEXT("starry_flounder"),
                    DJV_TEXT("striped_bass"),
                    DJV_TEXT("striped_mullet"),
                    DJV_TEXT("surf_smelt"),
                    DJV_TEXT("tahoe_sucker"),
                    DJV_TEXT("tecopa_pupfish"),
                    DJV_TEXT("tench"),
                    DJV_TEXT("thicktail_chub"),
                    DJV_TEXT("threadfin_shad"),
                    DJV_TEXT("tidewater_goby"),
                    DJV_TEXT("tomales_roach"),
                    DJV_TEXT("topsmelt"),
                    DJV_TEXT("unarmored_threespine_stickleback"),
                    DJV_TEXT("upper_klamath_marbled_sculpin"),
                    DJV_TEXT("upper_klamath-trinity_fall_chinook_salmon"),
                    DJV_TEXT("upper_klamath-trinity_spring_chinook_salmon"),
                    DJV_TEXT("wakasagi"),
                    DJV_TEXT("warmouth"),
                    DJV_TEXT("western_brook_lamprey"),
                    DJV_TEXT("western_mosquitofish"),
                    DJV_TEXT("white_bass"),
                    DJV_TEXT("white_catfish"),
                    DJV_TEXT("white_crappie"),
                    DJV_TEXT("white_sturgeon"),
                    DJV_TEXT("yellow_bullhead"),
                    DJV_TEXT("yellow_perch"),
                    DJV_TEXT("yellowfin_goby")
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
            DJV_TEXT("value_false"),
            DJV_TEXT("value_true")
        };

    } // namespace
} // namespace djv
