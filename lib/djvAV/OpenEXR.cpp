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

#include <djvAV/OpenEXR.h>

#include <ImfFramesPerSecond.h>
#include <ImfStandardAttributes.h>
#include <ImfThreading.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace OpenEXR
            {
                Channel::Channel()
                {}

                Channel::Channel(
                    const std::string& name,
                    Image::DataType    type,
                    const glm::ivec2&  sampling) :
                    name(name),
                    type(type),
                    sampling(sampling)
                {}

                Layer::Layer(
                    const std::vector<Channel>& channels,
                    bool                        luminanceChroma) :
                    channels(channels),
                    luminanceChroma(luminanceChroma)
                {
                    std::vector<std::string> names;
                    for (const auto& i : channels)
                    {
                        names.push_back(i.name);
                    }
                    name = getLayerName(names);
                }
                
                std::string getLayerName(const std::vector<std::string>& value)
                {
                    std::string out;

                    std::set<std::string> prefixes;
                    std::vector<std::string> suffixes;
                    for (const auto& i : value)
                    {
                        size_t index = i.find_last_of('.');
                        if (index != std::string::npos)
                        {
                            prefixes.insert(i.substr(0, index));
                            suffixes.push_back(i.substr(index + 1));
                        }
                        else
                        {
                            prefixes.insert(i);
                        }
                    }

                    out = String::joinSet(prefixes, ',');
                    if (!suffixes.empty())
                    {
                        out += '.';
                        out += String::join(suffixes, ',');
                    }

                    return out;
                }

                Imf::ChannelList getDefaultLayer(const Imf::ChannelList& in)
                {
                    Imf::ChannelList out;
                    for (auto i = in.begin(); i != in.end(); ++i)
                    {
                        const std::string tmp(i.name());
                        const size_t index = tmp.find_first_of('.');
                        if (index != std::string::npos)
                        {
                            if (index != 0 || index != tmp.size() - 1)
                            {
                                continue;
                            }
                        }
                        out.insert(i.name(), i.channel());
                    }
                    return out;
                }

                const Imf::Channel* find(const Imf::ChannelList& in, std::string& channel)
                {
                    const std::string channelLower = String::toLower(channel);
                    for (auto i = in.begin(); i != in.end(); ++i)
                    {
                        const std::string inName(i.name());
                        const size_t index = inName.find_last_of('.');
                        const std::string tmp =
                            (index != std::string::npos) ?
                            inName.substr(index + 1, inName.size() - index - 1) :
                            inName;
                        if (channelLower == String::toLower(tmp))
                        {
                            channel = inName;
                            return &i.channel();
                        }
                    }
                    return 0;
                }

                namespace
                {
                    bool compare(const std::vector<Imf::Channel>& in)
                    {
                        for (size_t i = 1; i < in.size(); ++i)
                        {
                            if (!(in[0] == in[i]))
                            {
                                return false;
                            }
                        }
                        return true;
                    }

                    std::vector<Layer> _getLayers(const Imf::ChannelList& in, Channels channels)
                    {
                        std::vector<Layer> out;
                        std::vector<const Imf::Channel*> reserved;
                        if (channels != Channels::None)
                        {
                            // Look for known channel configurations then convert the remainder.

                            // RGB / RGBA.
                            std::string rName = "r";
                            std::string gName = "g";
                            std::string bName = "b";
                            std::string aName = "a";
                            const Imf::Channel* r = find(in, rName);
                            const Imf::Channel* g = find(in, gName);
                            const Imf::Channel* b = find(in, bName);
                            const Imf::Channel* a = find(in, aName);
                            if (r && g && b && a && compare({ *r, *g, *b, *a }))
                            {
                                out.push_back(Layer({
                                    fromImf(rName, *r),
                                    fromImf(gName, *g),
                                    fromImf(bName, *b),
                                    fromImf(aName, *a) }));
                                reserved.push_back(r);
                                reserved.push_back(g);
                                reserved.push_back(b);
                                reserved.push_back(a);
                            }
                            else if (r && g && b && compare({ *r, *g, *b }))
                            {
                                out.push_back(Layer({
                                    fromImf(rName, *r),
                                    fromImf(gName, *g),
                                    fromImf(bName, *b) }));
                                reserved.push_back(r);
                                reserved.push_back(g);
                                reserved.push_back(b);
                            }

                            // Luminance, XYZ.
                            std::string yName = "y";
                            std::string ryName = "ry";
                            std::string byName = "by";
                            std::string xName = "x";
                            std::string zName = "z";
                            const Imf::Channel* y = find(in, yName);
                            const Imf::Channel* ry = find(in, ryName);
                            const Imf::Channel* by = find(in, byName);
                            const Imf::Channel* x = find(in, xName);
                            const Imf::Channel* z = find(in, zName);
                            if (y && a && compare({ *y, *a }))
                            {
                                out.push_back(Layer({
                                    fromImf(yName, *y),
                                    fromImf(aName, *a) }));
                                reserved.push_back(y);
                                reserved.push_back(a);
                            }
                            else if (y && ry && by &&
                                1 == y->xSampling &&
                                1 == y->ySampling &&
                                2 == ry->xSampling &&
                                2 == ry->ySampling &&
                                2 == by->xSampling &&
                                2 == by->ySampling)
                            {
                                out.push_back(Layer({
                                    fromImf(yName, *y),
                                    fromImf(ryName, *ry),
                                    fromImf(byName, *by) },
                                    true));
                                reserved.push_back(y);
                                reserved.push_back(ry);
                                reserved.push_back(by);
                            }
                            else if (x && y && z && compare({ *x, *y, *z }))
                            {
                                out.push_back(Layer({
                                    fromImf(xName, *x),
                                    fromImf(yName, *y),
                                    fromImf(zName, *z) }));
                                reserved.push_back(x);
                                reserved.push_back(y);
                                reserved.push_back(z);
                            }
                            else if (x && y && compare({ *x, *y }))
                            {
                                out.push_back(Layer({
                                    fromImf(xName, *x),
                                    fromImf(yName, *y) }));
                                reserved.push_back(x);
                                reserved.push_back(y);
                            }
                            else if (x)
                            {
                                out.push_back(Layer({ fromImf(xName, *x) }));
                                reserved.push_back(x);
                            }
                            else if (y)
                            {
                                out.push_back(Layer({ fromImf(yName, *y) }));
                                reserved.push_back(y);
                            }
                            else if (z)
                            {
                                out.push_back(Layer({ fromImf(zName, *z) }));
                                reserved.push_back(z);
                            }

                            // Colored mattes.
                            std::string arName = "ar";
                            std::string agName = "ag";
                            std::string abName = "ab";
                            const Imf::Channel* ar = find(in, arName);
                            const Imf::Channel* ag = find(in, agName);
                            const Imf::Channel* ab = find(in, abName);
                            if (ar && ag && ab && compare({ *ar, *ag, *ab }))
                            {
                                out.push_back(Layer({
                                    fromImf(arName, *ar),
                                    fromImf(agName, *ag),
                                    fromImf(abName, *ab) }));
                                reserved.push_back(ar);
                                reserved.push_back(ag);
                                reserved.push_back(ab);
                            }
                        }

                        // Convert the remainder.
                        for (auto i = in.begin(); i != in.end();)
                        {
                            std::vector<Channel> list;

                            // Add the first channel.
                            const std::string& name = i.name();
                            const Imf::Channel& channel = i.channel();
                            ++i;
                            if (std::find(reserved.begin(), reserved.end(), &channel) != reserved.end())
                            {
                                continue;
                            }
                            list.push_back(fromImf(name, channel));
                            if (Channels::All == channels)
                            {
                                // Group as many additional channels as possible.
                                for (;
                                    i != in.end() &&
                                    i.channel() == channel &&
                                    std::find(reserved.begin(), reserved.end(), &i.channel()) != reserved.end();
                                    ++i)
                                {
                                    list.push_back(fromImf(i.name(), i.channel()));
                                }
                            }

                            // Add the layer.
                            out.push_back(Layer(list));
                        }

                        return out;
                    }

                } // namespace

                std::vector<Layer> getLayers(const Imf::ChannelList& in, Channels channels)
                {
                    std::vector<Layer> out;

                    // Get the default layer.
                    for (const auto& layer : _getLayers(getDefaultLayer(in), channels))
                    {
                        out.push_back(layer);
                    }

                    // Get the additional layers.
                    std::set<std::string> layers;
                    in.layers(layers);
                    for (auto i = layers.begin(); i != layers.end(); ++i)
                    {
                        Imf::ChannelList list;
                        Imf::ChannelList::ConstIterator f, l;
                        in.channelsInLayer(*i, f, l);
                        for (auto j = f; j != l; ++j)
                        {
                            list.insert(j.name(), j.channel());
                        }
                        for (const auto& layer : _getLayers(list, channels))
                        {
                            out.push_back(layer);
                        }
                    }

                    return out;
                }

                void readTags(const Imf::Header& header, Tags& tags, Time::Speed& speed)
                {
                    if (hasOwner(header))
                    {
                        tags.setTag("Creator", ownerAttribute(header).value());
                    }
                    if (hasComments(header))
                    {
                        tags.setTag("Description", commentsAttribute(header).value());
                    }
                    if (hasCapDate(header))
                    {
                        tags.setTag("Time", capDateAttribute(header).value());
                    }
                    if (hasUtcOffset(header))
                    {
                        std::stringstream ss;
                        ss << utcOffsetAttribute(header).value();
                        tags.setTag("UTC Offset", ss.str());
                    }
                    if (hasLongitude(header))
                    {
                        std::stringstream ss;
                        ss << utcOffsetAttribute(header).value();
                        tags.setTag("Longitude", ss.str());
                    }
                    if (hasLatitude(header))
                    {
                        std::stringstream ss;
                        ss << latitudeAttribute(header).value();
                        tags.setTag("Latitude", ss.str());
                    }
                    if (hasAltitude(header))
                    {
                        std::stringstream ss;
                        ss << altitudeAttribute(header).value();
                        tags.setTag("Altitude", ss.str());
                    }
                    if (hasFocus(header))
                    {
                        std::stringstream ss;
                        ss << focusAttribute(header).value();
                        tags.setTag("Focus", ss.str());
                    }
                    if (hasExpTime(header))
                    {
                        std::stringstream ss;
                        ss << expTimeAttribute(header).value();
                        tags.setTag("Exposure Time", ss.str());
                    }
                    if (hasAperture(header))
                    {
                        std::stringstream ss;
                        ss << apertureAttribute(header).value();
                        tags.setTag("Aperture", ss.str());
                    }
                    if (hasIsoSpeed(header))
                    {
                        std::stringstream ss;
                        ss << isoSpeedAttribute(header).value();
                        tags.setTag("ISO Speed", ss.str());
                    }
                    if (hasChromaticities(header))
                    {
                        const Imf::Chromaticities data = chromaticitiesAttribute(header).value();
                        std::stringstream ss;
                        ss << data.red.x << " " << data.red.y << " ";
                        ss << data.green.x << " " << data.green.y << " ";
                        ss << data.blue.x << " " << data.blue.y << " ";
                        ss << data.white.x << " " << data.white.y << " ";
                        tags.setTag("Chromaticities", ss.str());
                    }
                    if (hasWhiteLuminance(header))
                    {
                        std::stringstream ss;
                        ss << whiteLuminanceAttribute(header).value();
                        tags.setTag("White Luminance", ss.str());
                    }
                    if (hasXDensity(header))
                    {
                        std::stringstream ss;
                        ss << xDensityAttribute(header).value();
                        tags.setTag("X Density", ss.str());
                    }
                    if (hasKeyCode(header))
                    {
                        const Imf::KeyCode data = keyCodeAttribute(header).value();
                        tags.setTag(
                            "Keycode",
                            Time::keycodeToString(
                                data.filmMfcCode(),
                                data.filmType(),
                                data.prefix(),
                                data.count(),
                                data.perfOffset()));
                    }
                    if (hasTimeCode(header))
                    {
                        tags.setTag("Timecode", Time::timecodeToString(timeCodeAttribute(header).value().timeAndFlags()));
                    }
                    if (hasFramesPerSecond(header))
                    {
                        const Imf::Rational data = framesPerSecondAttribute(header).value();
                        speed = Time::Speed(data.n, data.d);
                    }
                }

                void writeTags(const Tags& tags, const Time::Speed& speed, Imf::Header& header)
                {
                    if (tags.hasTag("Owner"))
                    {
                        addOwner(header, tags.getTag("Owner"));
                    }
                    if (tags.hasTag("Description"))
                    {
                        addComments(header, tags.getTag("Description"));
                    }
                    if (tags.hasTag("Time"))
                    {
                        addCapDate(header, tags.getTag("Time"));
                    }
                    if (tags.hasTag("UTC Offset"))
                    {
                        addUtcOffset(header, std::stof(tags.getTag("UTC Offset")));
                    }
                    if (tags.hasTag("Longitude"))
                    {
                        addLongitude(header, std::stof(tags.getTag("Longitude")));
                    }
                    if (tags.hasTag("Latitude"))
                    {
                        addLatitude(header, std::stof(tags.getTag("Latitude")));
                    }
                    if (tags.hasTag("Altitude"))
                    {
                        addAltitude(header, std::stof(tags.getTag("Altitude")));
                    }
                    if (tags.hasTag("Focus"))
                    {
                        addFocus(header, std::stof(tags.getTag("Focus")));
                    }
                    if (tags.hasTag("Exposure Time"))
                    {
                        addExpTime(header, std::stof(tags.getTag("Exposure Time")));
                    }
                    if (tags.hasTag("Aperture"))
                    {
                        addAperture(header, std::stof(tags.getTag("Aperture")));
                    }
                    if (tags.hasTag("ISO Speed"))
                    {
                        addIsoSpeed(header, std::stof(tags.getTag("ISO Speed")));
                    }
                    if (tags.hasTag("Chromaticities"))
                    {
                        std::stringstream ss(tags.getTag("Chromaticities"));
                        std::vector<Imath::V2f> chromaticities;
                        chromaticities.resize(8);
                        for (size_t i = 0; i < 4; ++i)
                        {
                            ss >> chromaticities[i].x;
                            ss >> chromaticities[i].y;
                        }
                        addChromaticities(header,
                            Imf::Chromaticities(
                                chromaticities[0],
                                chromaticities[1],
                                chromaticities[2],
                                chromaticities[3]));
                    }
                    if (tags.hasTag("White Luminance"))
                    {
                        addWhiteLuminance(header, std::stof(tags.getTag("White Luminance")));
                    }
                    if (tags.hasTag("X Density"))
                    {
                        addXDensity(header, std::stof(tags.getTag("X Desnity")));
                    }
                    if (tags.hasTag("Keycode"))
                    {
                        int id = 0, type = 0, prefix = 0, count = 0, offset = 0;
                        Time::stringToKeycode(tags.getTag("Keycode"), id, type, prefix, count, offset);
                        addKeyCode(header, Imf::KeyCode(id, type, prefix, count, offset));
                    }
                    if (tags.hasTag("Timecode"))
                    {
                        uint32_t timecode = 0;
                        Time::stringToTimecode(tags.getTag("Timecode"), timecode);
                        addTimeCode(header, timecode);
                    }
                    addFramesPerSecond(
                        header,
                        Imf::Rational(speed.getNum(), speed.getDen()));
                }

                BBox2i fromImath(const Imath::Box2i& value)
                {
                    return BBox2i(glm::ivec2(value.min.x, value.min.y), glm::ivec2(value.max.x, value.max.y));
                }

                Imf::PixelType toImf(Image::DataType value)
                {
                    Imf::PixelType out = Imf::PixelType::HALF;
                    switch (value)
                    {
                    case Image::DataType::U32: out = Imf::PixelType::UINT;  break;
                    case Image::DataType::F32: out = Imf::PixelType::FLOAT; break;
                    default: break;
                    }
                    return out;
                }

                Image::DataType fromImf(Imf::PixelType in)
                {
                    Image::DataType out = Image::DataType::None;
                    switch (in)
                    {
                    case Imf::UINT:  out = Image::DataType::U32; break;
                    case Imf::HALF:  out = Image::DataType::F16; break;
                    case Imf::FLOAT: out = Image::DataType::F32; break;
                    default: break;
                    }
                    return out;
                }

                Channel fromImf(const std::string& name, const Imf::Channel& channel)
                {
                    return Channel(
                        name,
                        fromImf(channel.type),
                        glm::ivec2(channel.xSampling, channel.ySampling));
                }

                struct Plugin::Private
                {
                    Options options;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                std::shared_ptr<Plugin> Plugin::create(
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    Imf::setGlobalThreadCount(out->_p->options.threadCount);
                    out->_init(
                        pluginName,
                        DJV_TEXT("This plugin provides OpenEXR file I/O."),
                        fileExtensions,
                        resourceSystem,
                        logSystem);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->options);
                }

                void Plugin::setOptions(const picojson::value & value)
                {
                    DJV_PRIVATE_PTR();
                    fromJSON(value, p.options);
                    Imf::setGlobalThreadCount(p.options.threadCount);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo, const ReadOptions& options) const
                {
                    return Read::create(fileInfo, options, _p->options, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const FileSystem::FileInfo& fileInfo, const Info & info, const WriteOptions& options) const
                {
                    return Write::create(fileInfo, info, options, _p->options, _resourceSystem, _logSystem);
                }

            } // namespace OpenEXR
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::OpenEXR,
        Channels,
        DJV_TEXT("None"),
        DJV_TEXT("Known"),
        DJV_TEXT("All"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::OpenEXR,
        Compression,
        DJV_TEXT("None"),
        DJV_TEXT("RLE"),
        DJV_TEXT("ZIPS"),
        DJV_TEXT("ZIP"),
        DJV_TEXT("PIZ"),
        DJV_TEXT("PXR24"),
        DJV_TEXT("B44"),
        DJV_TEXT("B44A"),
        DJV_TEXT("DWAA"),
        DJV_TEXT("DWAB"));

    picojson::value toJSON(const AV::IO::OpenEXR::Options & value)
    {
        picojson::value out(picojson::object_type, true);
        {
            out.get<picojson::object>()["ThreadCount"] = toJSON(value.threadCount);
            {
                std::stringstream ss;
                ss << value.channels;
                out.get<picojson::object>()["Channels"] = picojson::value(ss.str());
            }
            {
                std::stringstream ss;
                ss << value.compression;
                out.get<picojson::object>()["Compression"] = picojson::value(ss.str());
            }
            out.get<picojson::object>()["DWACompressionLevel"] = toJSON(value.dwaCompressionLevel);
            out.get<picojson::object>()["ColorSpace"] = toJSON(value.colorSpace);
        }
        return out;
    }

    void fromJSON(const picojson::value & value, AV::IO::OpenEXR::Options & out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto & i : value.get<picojson::object>())
            {
                if ("ThreadCount" == i.first)
                {
                    fromJSON(i.second, out.threadCount);
                }
                else if ("Channels" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.channels;
                }
                else if ("Compression" == i.first)
                {
                    std::stringstream ss(i.second.get<std::string>());
                    ss >> out.compression;
                }
                else if ("DWACompressionLevel" == i.first)
                {
                    fromJSON(i.second, out.dwaCompressionLevel);
                }
                else if ("ColorSpace" == i.first)
                {
                    fromJSON(i.second, out.colorSpace);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv

