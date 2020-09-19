// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/StringFunc.h>

#include <djvCore/RandomFunc.h>

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
            std::vector<std::string> split(const char * s, size_t size, char delimeter, bool keepEmpty)
            {
                std::vector<std::string> out;
                bool word = false;
                size_t wordStart = 0;
                size_t i = 0;
                for (; i < size; ++i)
                {
                    if (s[i] != delimeter)
                    {
                        if (!word)
                        {
                            word = true;
                            wordStart = i;
                        }
                    }
                    else
                    {
                        if (word)
                        {
                            word = false;
                            out.push_back(std::string(s + wordStart, i - wordStart));
                        }
                        if (keepEmpty && i > 0 && s[i - 1] == delimeter)
                        {
                            out.push_back(std::string());
                        }
                    }
                }
                if (word)
                {
                    out.push_back(std::string(s + wordStart, i - wordStart));
                }
                return out;
            }

            std::vector<std::string> split(const std::string& s, char delimeter, bool keepEmpty)
            {
                std::vector<std::string> out;
                bool word = false;
                size_t wordStart = 0;
                size_t i = 0;
                for (; i < s.size(); ++i)
                {
                    if (s[i] != delimeter)
                    {
                        if (!word)
                        {
                            word = true;
                            wordStart = i;
                        }
                    }
                    else
                    {
                        if (word)
                        {
                            word = false;
                            out.push_back(s.substr(wordStart, i - wordStart));
                        }
                        if (keepEmpty && i > 0 && s[i - 1] == delimeter)
                        {
                            out.push_back(std::string());
                        }
                    }
                }
                if (word)
                {
                    out.push_back(s.substr(wordStart, i - wordStart));
                }
                return out;
            }

            std::vector<std::string> split(const std::string& s, const std::vector<char>& delimeters, bool keepEmpty)
            {
                std::vector<std::string> out;
                bool word = false;
                size_t wordStart = 0;
                size_t i = 0;
                for (; i < s.size(); ++i)
                {
                    if (std::find(delimeters.begin(), delimeters.end(), s[i]) == delimeters.end())
                    {
                        if (!word)
                        {
                            word = true;
                            wordStart = i;
                        }
                    }
                    else
                    {
                        if (word)
                        {
                            word = false;
                            out.push_back(s.substr(wordStart, i - wordStart));
                        }
                        if (keepEmpty && i > 0 && std::find(delimeters.begin(), delimeters.end(), s[i - 1]) != delimeters.end())
                        {
                            out.push_back(std::string());
                        }
                    }
                }
                if (word)
                {
                    out.push_back(s.substr(wordStart, i - wordStart));
                }
                return out;
            }

            std::string join(const std::vector<std::string>& value)
            {
                std::string out;
                for (const auto& s : value)
                {
                    out += s;
                }
                return out;
            }

            std::string join(const std::vector<std::string>& value, char delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            std::string join(const std::vector<std::string>& value, const std::string& delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            std::string joinList(const std::list<std::string>& value)
            {
                std::string out;
                for (const auto& s : value)
                {
                    out += s;
                }
                return out;
            }

            std::string joinList(const std::list<std::string>& value, char delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            std::string joinList(const std::list<std::string>& value, const std::string& delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            std::string joinSet(const std::set<std::string>& value)
            {
                std::string out;
                for (const auto& s : value)
                {
                    out += s;
                }
                return out;
            }

            std::string joinSet(const std::set<std::string>& value, char delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

            std::string joinSet(const std::set<std::string>& value, const std::string& delimeter)
            {
                std::string out;
                if (const size_t size = value.size())
                {
                    size_t j = 0;
                    for (auto i = value.begin(); i != value.end(); ++i, ++j)
                    {
                        out += *i;
                        if (j < size - 1)
                        {
                            out += delimeter;
                        }
                    }
                }
                return out;
            }

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

            bool match(const std::string& value, const std::string& expression)
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

            void fromString(const char * s, size_t size, int& out)
            {
                out = 0;

                // Find the sign.
                bool negativeSign = false;
                if ('-' == s[0])
                {
                    negativeSign = true;
                    ++s;
                    --size;
                }
                else if ('+' == s[0])
                {
                    ++s;
                    --size;
                }

                // Find the end.
                size_t end = 0;
                for (; end < size && s[end]; ++end)
                    ;

                // Add up the digits.
                int tens = 1;
                for (int i = int(end) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (s[i] - 48) * tens;
                }

                // Apply the sign.
                if (negativeSign)
                {
                    out = -out;
                }
            }

            void fromString(const char * s, size_t size, int64_t& out)
            {
                out = 0;

                // Find the sign.
                bool negativeSign = false;
                if ('-' == s[0])
                {
                    negativeSign = true;
                    ++s;
                    --size;
                }
                else if ('+' == s[0])
                {
                    ++s;
                    --size;
                }

                // Find the end.
                size_t end = 0;
                for (; end < size && s[end]; ++end)
                    ;

                // Add up the digits.
                int64_t tens = 1;
                for (int i = int(end) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (static_cast<int64_t>(s[i]) - 48) * tens;
                }

                // Apply the sign.
                if (negativeSign)
                {
                    out = -out;
                }
            }

            void fromString(const char * s, size_t size, size_t& out)
            {
                out = 0;

                // Add up the digits.
                size_t tens = 1;
                for (int i = int(size) - 1; i >= 0; --i, tens *= 10)
                {
                    out += (static_cast<size_t>(s[i]) - 48) * tens;
                }
            }

            void fromString(const char * s, size_t size, float& out)
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

            std::wstring toWide(const std::string& value)
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.from_bytes(value);
            }

            std::string fromWide(const std::wstring& value)
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.to_bytes(value);
            }

            std::string escape(const std::string& value)
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

            std::string unescape(const std::string& value)
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
                    DJV_TEXT("fish_amargosa_canyon_speckled_dace"),
                    DJV_TEXT("fish_amargosa_river_pupfish"),
                    DJV_TEXT("fish_american_shad"),
                    DJV_TEXT("fish_arroyo_chub"),
                    DJV_TEXT("fish_bigeye_marbled_sculpin"),
                    DJV_TEXT("fish_bigscale_logperch"),
                    DJV_TEXT("fish_black_bullhead"),
                    DJV_TEXT("fish_black_crappie"),
                    DJV_TEXT("fish_blue_catfish"),
                    DJV_TEXT("fish_blue_chub"),
                    DJV_TEXT("fish_bluefin_killifish"),
                    DJV_TEXT("fish_bluegill"),
                    DJV_TEXT("fish_lepomis_macrochirus"),
                    DJV_TEXT("fish_bonytail"),
                    DJV_TEXT("fish_brook_stickleback"),
                    DJV_TEXT("fish_brook_trout"),
                    DJV_TEXT("fish_brown_bullhead"),
                    DJV_TEXT("fish_brown_trout"),
                    DJV_TEXT("fish_bull_trout"),
                    DJV_TEXT("fish_california_coast_fall_chinook_salmon"),
                    DJV_TEXT("fish_california_golden_trout"),
                    DJV_TEXT("fish_california_killifish"),
                    DJV_TEXT("fish_california_tilapia"),
                    DJV_TEXT("fish_central_california_coast_winter_steelhead"),
                    DJV_TEXT("fish_central_california_roach"),
                    DJV_TEXT("fish_central_coast_coho_salmon"),
                    DJV_TEXT("fish_central_valley_fall_chinook_salmon"),
                    DJV_TEXT("fish_central_valley_late_fall_chinook_salmon"),
                    DJV_TEXT("fish_oncorhynchus_tshawytscha"),
                    DJV_TEXT("fish_central_valley_spring_chinook_salmon"),
                    DJV_TEXT("fish_central_valley_steelhead"),
                    DJV_TEXT("fish_central_valley_winter_chinook_salmon"),
                    DJV_TEXT("fish_channel_catfish"),
                    DJV_TEXT("fish_chum_salmon"),
                    DJV_TEXT("fish_clear_lake_hitch"),
                    DJV_TEXT("fish_clear_lake_prickly_sculpin"),
                    DJV_TEXT("fish_clear_lake_roach"),
                    DJV_TEXT("fish_clear_lake_splittail"),
                    DJV_TEXT("fish_clear_lake_tule_perch"),
                    DJV_TEXT("fish_coastal_cutthroat_trout"),
                    DJV_TEXT("fish_coastal_rainbow_trout"),
                    DJV_TEXT("fish_coastal_threespine_stickleback"),
                    DJV_TEXT("fish_coastrange_sculpin"),
                    DJV_TEXT("fish_colorado_pikeminnow"),
                    DJV_TEXT("fish_colorado_river_cutthroat_trout"),
                    DJV_TEXT("fish_common_carp"),
                    DJV_TEXT("fish_cottonball_marsh_pupfish"),
                    DJV_TEXT("fish_cow_head_tui_chub"),
                    DJV_TEXT("fish_delta_smelt"),
                    DJV_TEXT("fish_desert_pupfish"),
                    DJV_TEXT("fish_eagle_lake_rainbow_trout"),
                    DJV_TEXT("fish_eagle_lake_tui_chub"),
                    DJV_TEXT("fish_eulachon"),
                    DJV_TEXT("fish_fathead_minnow"),
                    DJV_TEXT("fish_flannelmouth_sucker"),
                    DJV_TEXT("fish_flathead_catfish"),
                    DJV_TEXT("fish_golden_shiner"),
                    DJV_TEXT("fish_goldfish"),
                    DJV_TEXT("fish_goose_lake_lamprey"),
                    DJV_TEXT("fish_goose_lake_redband_trout"),
                    DJV_TEXT("fish_goose_lake_sucker"),
                    DJV_TEXT("fish_goose_lake_tui_chub"),
                    DJV_TEXT("fish_grass_carp"),
                    DJV_TEXT("fish_green_sunfish"),
                    DJV_TEXT("fish_gualala_roach"),
                    DJV_TEXT("fish_hardhead"),
                    DJV_TEXT("fish_high_rock_springs_tui_chub"),
                    DJV_TEXT("fish_humboldt_sucker"),
                    DJV_TEXT("fish_inland_threespine_stickleback"),
                    DJV_TEXT("fish_kern_brook_lamprey"),
                    DJV_TEXT("fish_kern_river_rainbow_trout"),
                    DJV_TEXT("fish_klamath_largescale_sucker"),
                    DJV_TEXT("fish_klamath_mountains_province_summer_steelhead"),
                    DJV_TEXT("fish_klamath_mountains_province_winter_steelhead"),
                    DJV_TEXT("fish_klamath_river_lamprey"),
                    DJV_TEXT("fish_klamath_smallscale_sucker"),
                    DJV_TEXT("fish_klamath_speckled_dace"),
                    DJV_TEXT("fish_klamath_tui_chub"),
                    DJV_TEXT("fish_lahontan_cutthroat_trout"),
                    DJV_TEXT("fish_lahontan_lake_tui_chub"),
                    DJV_TEXT("fish_lahontan_mountain_sucker"),
                    DJV_TEXT("fish_lahontan_redside"),
                    DJV_TEXT("fish_lahontan_speckled_dace"),
                    DJV_TEXT("fish_lahontan_stream_tui_chub"),
                    DJV_TEXT("fish_lake_trout"),
                    DJV_TEXT("fish_largemouth_bass"),
                    DJV_TEXT("fish_little_kern_golden_trout"),
                    DJV_TEXT("fish_long_valley_speckled_dace"),
                    DJV_TEXT("fish_longfin_smelt"),
                    DJV_TEXT("fish_longjaw_mudsucker"),
                    DJV_TEXT("fish_lost_river_sucker"),
                    DJV_TEXT("fish_lower_klamath_marbled_sculpin"),
                    DJV_TEXT("fish_mccloud_river_redband_trout"),
                    DJV_TEXT("fish_mississippi_silversides"),
                    DJV_TEXT("fish_modoc_sucker"),
                    DJV_TEXT("fish_mojave_tui_chub"),
                    DJV_TEXT("fish_monterey_hitch"),
                    DJV_TEXT("fish_monterey_roach"),
                    DJV_TEXT("fish_monterey_sucker"),
                    DJV_TEXT("fish_mountain_whitefish"),
                    DJV_TEXT("fish_navarro_roach"),
                    DJV_TEXT("fish_northern_pit_roach"),
                    DJV_TEXT("fish_northern_california_brook_lamprey"),
                    DJV_TEXT("fish_northern_california_coast_summer_steelhead"),
                    DJV_TEXT("fish_northern_california_coast_winter_steelhead"),
                    DJV_TEXT("fish_northern_green_sturgeon"),
                    DJV_TEXT("fish_owens_pupfish"),
                    DJV_TEXT("fish_owens_speckled_dace"),
                    DJV_TEXT("fish_owens_sucker"),
                    DJV_TEXT("fish_owens_tui_chub"),
                    DJV_TEXT("fish_pacific_lamprey"),
                    DJV_TEXT("fish_paiute_cutthroat_trout"),
                    DJV_TEXT("fish_paiute_sculpin"),
                    DJV_TEXT("fish_pink_salmon"),
                    DJV_TEXT("fish_pit_river_tui_chub"),
                    DJV_TEXT("fish_pit_sculpin"),
                    DJV_TEXT("fish_pit-klamath_brook_lamprey"),
                    DJV_TEXT("fish_porthole_livebearer"),
                    DJV_TEXT("fish_prickly_sculpin"),
                    DJV_TEXT("fish_pumpkinseed"),
                    DJV_TEXT("fish_rainwater_killifish"),
                    DJV_TEXT("fish_razorback_sucker"),
                    DJV_TEXT("fish_red_hills_roach"),
                    DJV_TEXT("fish_red_shiner"),
                    DJV_TEXT("fish_redear_sunfish"),
                    DJV_TEXT("fish_redeye_bass"),
                    DJV_TEXT("fish_reticulate_sculpin"),
                    DJV_TEXT("fish_riffle_sculpin"),
                    DJV_TEXT("fish_river_lamprey"),
                    DJV_TEXT("fish_rough_sculpin"),
                    DJV_TEXT("fish_russian_river_roach"),
                    DJV_TEXT("fish_russian_river_tule_perch"),
                    DJV_TEXT("fish_sacramento_blackfish"),
                    DJV_TEXT("fish_sacramento_hitch"),
                    DJV_TEXT("fish_sacramento_perch"),
                    DJV_TEXT("fish_sacramento_pikeminnow"),
                    DJV_TEXT("fish_sacramento_speckled_dace"),
                    DJV_TEXT("fish_sacramento_splittail"),
                    DJV_TEXT("fish_sacramento_sucker"),
                    DJV_TEXT("fish_sacramento_tule_perch"),
                    DJV_TEXT("fish_sailfin_molly"),
                    DJV_TEXT("fish_salt_creek_pupfish"),
                    DJV_TEXT("fish_santa_ana_speckled_dace"),
                    DJV_TEXT("fish_santa_ana_sucker"),
                    DJV_TEXT("fish_saratoga_springs_pupfish"),
                    DJV_TEXT("fish_shay_creek_stickleback"),
                    DJV_TEXT("fish_shimofuri_goby"),
                    DJV_TEXT("fish_shiner_perch"),
                    DJV_TEXT("fish_shortfin_molly"),
                    DJV_TEXT("fish_shortnose_sucker"),
                    DJV_TEXT("fish_shoshone_pupfish"),
                    DJV_TEXT("fish_smallmouth_bass"),
                    DJV_TEXT("fish_sockeye_kokanee_salmon"),
                    DJV_TEXT("fish_south_central_california_coast_steelhead"),
                    DJV_TEXT("fish_southern_california_steelhead"),
                    DJV_TEXT("fish_southern_green_sturgeon"),
                    DJV_TEXT("fish_southern_oregon_northern_california_coast_coho_salmon"),
                    DJV_TEXT("fish_southern_oregon_northern_california_coast_fall_chinook_salmon"),
                    DJV_TEXT("fish_spotted_bass"),
                    DJV_TEXT("fish_staghorn_sculpin"),
                    DJV_TEXT("fish_starry_flounder"),
                    DJV_TEXT("fish_striped_bass"),
                    DJV_TEXT("fish_striped_mullet"),
                    DJV_TEXT("fish_surf_smelt"),
                    DJV_TEXT("fish_tahoe_sucker"),
                    DJV_TEXT("fish_tecopa_pupfish"),
                    DJV_TEXT("fish_tench"),
                    DJV_TEXT("fish_thicktail_chub"),
                    DJV_TEXT("fish_threadfin_shad"),
                    DJV_TEXT("fish_tidewater_goby"),
                    DJV_TEXT("fish_tomales_roach"),
                    DJV_TEXT("fish_topsmelt"),
                    DJV_TEXT("fish_unarmored_threespine_stickleback"),
                    DJV_TEXT("fish_upper_klamath_marbled_sculpin"),
                    DJV_TEXT("fish_upper_klamath-trinity_fall_chinook_salmon"),
                    DJV_TEXT("fish_upper_klamath-trinity_spring_chinook_salmon"),
                    DJV_TEXT("fish_wakasagi"),
                    DJV_TEXT("fish_warmouth"),
                    DJV_TEXT("fish_western_brook_lamprey"),
                    DJV_TEXT("fish_western_mosquitofish"),
                    DJV_TEXT("fish_white_bass"),
                    DJV_TEXT("fish_white_catfish"),
                    DJV_TEXT("fish_white_crappie"),
                    DJV_TEXT("fish_white_sturgeon"),
                    DJV_TEXT("fish_yellow_bullhead"),
                    DJV_TEXT("fish_yellow_perch"),
                    DJV_TEXT("fish_yellowfin_goby")
                };
                return fishes;
            }

            std::vector<std::string> getRandomNames(size_t value)
            {
                const std::vector<std::string>& data = getTestNames();
                std::vector<std::string> out;
                for (size_t i = 0; i < value; ++i)
                {
                    out.push_back(Random::getRandom(data));
                }
                return out;
            }

            std::string getRandomName()
            {
                const std::vector<std::string>& data = getTestNames();
                return Random::getRandom(data);
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
                return data[Random::getRandom(size - 1)];
            }

            std::vector<std::string> getRandomSentences(size_t value)
            {
                const std::vector<std::string> data = getTestSentences();
                std::vector<std::string> out;
                const int size = static_cast<int>(data.size());
                int index = Random::getRandom(size - 1);
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
            DJV_TEXT("boolean_false"),
            DJV_TEXT("boolean_true")
        };

    } // namespace
} // namespace djv
