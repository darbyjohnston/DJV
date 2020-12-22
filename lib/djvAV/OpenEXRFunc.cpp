// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAV/OpenEXRFunc.h>

#include <djvAV/TimeFunc.h>

#include <djvCore/StringFunc.h>

#include <ImfDoubleAttribute.h>
#include <ImfFloatVectorAttribute.h>
#include <ImfFramesPerSecond.h>
#include <ImfIntAttribute.h>
#include <ImfStandardAttributes.h>
#include <ImfThreading.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace OpenEXR
            {
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
                    return nullptr;
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
                            if (!r)
                            {
                                rName = "red";
                                r = find(in, rName);
                            }
                            if (!g)
                            {
                                gName = "green";
                                g = find(in, gName);
                            }
                            if (!b)
                            {
                                bName = "blue";
                                b = find(in, bName);
                            }
                            if (!a)
                            {
                                aName = "alpha";
                                a = find(in, aName);
                            }
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
                                    i.channel() == channel;
                                    ++i)
                                {
                                    if (std::find(reserved.begin(), reserved.end(), &i.channel()) != reserved.end())
                                    {
                                        continue;
                                    }
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

                namespace
                {
                    const std::vector<std::string> knownAttributes =
                    {
                        // Predefined attributes.
                        "displayWindow",
                        "dataWindow",
                        "pixelAspectRatio",
                        "screenWindowCenter",
                        "screenWindowWidth",
                        "channels",
                        "lineOrder",
                        "compression",

                        // Multipart attributes.
                        "name",
                        "type",
                        "version",
                        "chunkCount",
                        "view",

                        // Tile description.
                        "tileDescription",

                        // Standard attributes.
                        "chromaticities",
                        "whiteLuminance",
                        "adoptedNeutral",
                        "renderingTransform",
                        "lookModTransform",
                        "xDensity",
                        "owner",
                        "comments",
                        "capDate",
                        "utcOffset",
                        "longitude",
                        "latitude",
                        "altitude",
                        "focus",
                        "expTime",
                        "aperture",
                        "isoSpeed",
                        "envMap",
                        "keyCode",
                        "timeCode",
                        "wrapModes",
                        "framesPerSecond",
                        "multiView",
                        "worldToCamera",
                        "worldToNDC",
                        "deepImageState",
                        "originalDataWindow",
                        "dwaCompressionLevel"
                    };

                    template<typename T>
                    std::string serialize(const T& value)
                    {
                        std::stringstream ss;
                        ss << value;
                        return ss.str();
                    }

                    template<typename T>
                    std::string serialize(const std::vector<T>& value)
                    {
                        std::vector<std::string> list;
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << serialize(i);
                            list.push_back(ss.str());
                        }
                        return String::join(list, " ");
                    }

                    template<typename T>
                    std::string serialize(const Imath::Vec2<T>& value)
                    {
                        std::stringstream ss;
                        ss << value.x << " " << value.y;
                        return ss.str();
                    }

                    template<typename T>
                    std::string serialize(const Imath::Vec3<T>& value)
                    {
                        std::stringstream ss;
                        ss << value.x << " " << value.y << " " << value.z;
                        return ss.str();
                    }

                    template<typename T>
                    std::string serialize(const Imath::Box<Imath::Vec2<T> >& value)
                    {
                        std::stringstream ss;
                        ss << value.min.x << " " << value.min.y << " " <<
                            value.max.x << " " << value.max.y;
                        return ss.str();
                    }

                    template<typename T>
                    std::string serialize(const Imath::Box<Imath::Vec3<T> >& value)
                    {
                        std::stringstream ss;
                        ss << value.min.x << " " << value.min.y << " " << value.min.z << " " <<
                            value.max.x << " " << value.max.y << " " << value.max.z;
                        return ss.str();
                    }

                    template<>
                    std::string serialize(const Imf::Compression& value)
                    {
                        const std::vector<std::string> text =
                        {
                            "None",
                            "RLE",
                            "ZIPS",
                            "ZIP",
                            "PIZ",
                            "PXR24",
                            "B44",
                            "B44A",
                            "DWAA",
                            "DWAB"
                        };
                        return text[value];
                    }

                    template<>
                    std::string serialize(const Imf::LineOrder& value)
                    {
                        const std::vector<std::string> text =
                        {
                            "Increasing Y",
                            "Decreasing Y",
                            "Random Y"
                        };
                        return text[value];
                    }

                    template<>
                    std::string serialize(const Imf::LevelMode& value)
                    {
                        const std::vector<std::string> text =
                        {
                            "One Level",
                            "Mipmap Levels",
                            "Ripmap Levels"
                        };
                        return text[value];
                    }

                    template<>
                    std::string serialize(const Imf::LevelRoundingMode& value)
                    {
                        const std::vector<std::string> text =
                        {
                            "Round Down",
                            "Round Up"
                        };
                        return text[value];
                    }

                    template<>
                    std::string serialize(const Imf::DeepImageState& value)
                    {
                        const std::vector<std::string> text =
                        {
                            "Messy",
                            "Sorted",
                            "Non Overlapping",
                            "Tidy"
                        };
                        return text[value];
                    }

                    template<>
                    std::string serialize(const Imf::TimeCode& value)
                    {
                        return Time::timecodeToString(value.timeAndFlags());
                    }

                    template<>
                    std::string serialize(const Imf::KeyCode& value)
                    {
                        return Time::keycodeToString(
                            value.filmMfcCode(),
                            value.filmType(),
                            value.prefix(),
                            value.count(),
                            value.perfOffset());
                    }

                    template<>
                    std::string serialize(const Imf::Chromaticities& value)
                    {
                        std::stringstream ss;
                        ss << serialize(value.red) << " ";
                        ss << serialize(value.green) << " ";
                        ss << serialize(value.blue) << " ";
                        ss << serialize(value.white);
                        return ss.str();
                    }

                    template<>
                    std::string serialize(const Imf::Rational& value)
                    {
                        std::stringstream ss;
                        ss << value.n << " " << value.d;
                        return ss.str();
                    }

                } // namespace

                void readTags(const Imf::Header& header, Image::Tags& tags, Math::IntRational& speed)
                {
                    // Predefined attributes.
                    tags.set("Display Window", serialize(header.displayWindow()));
                    tags.set("Data Window", serialize(header.dataWindow()));
                    tags.set("Pixel Aspect Ratio", serialize(header.pixelAspectRatio()));
                    tags.set("Screen Window Center", serialize(header.screenWindowCenter()));
                    tags.set("Screen Window Width", serialize(header.screenWindowWidth()));
                    {
                        std::vector<std::string> values;
                        for (auto i = header.channels().begin(); i != header.channels().end(); ++i)
                        {
                            values.push_back(i.name());
                        }
                        tags.set("Channels", String::join(values, " "));
                    }
                    tags.set("Line Order", serialize(header.lineOrder()));
                    tags.set("Compression", serialize(header.compression()));

                    // Multipart attributes.
                    if (header.hasName())
                    {
                        tags.set("Name", header.name());
                    }
                    if (header.hasType())
                    {
                        tags.set("Type", header.type());
                    }
                    if (header.hasVersion())
                    {
                        tags.set("Version", serialize(header.version()));
                    }
                    if (header.hasChunkCount())
                    {
                        tags.set("Chunk Count", serialize(header.chunkCount()));
                    }
                    if (header.hasView())
                    {
                        tags.set("View", header.view());
                    }

                    // Tile description.
                    if (header.hasTileDescription())
                    {
                        const auto& value = header.tileDescription();
                        {
                            std::stringstream ss;
                            ss << value.xSize << " " << value.ySize;
                            tags.set("Tile Size", ss.str());
                        }
                        tags.set("Tile Level Mode", serialize(value.mode));
                        tags.set("Tile Level Rounding Mode", serialize(value.roundingMode));
                    }

                    // Standard attributes.
                    if (hasChromaticities(header))
                    {
                        tags.set("Chromaticities", serialize(chromaticitiesAttribute(header).value()));
                    }
                    if (hasWhiteLuminance(header))
                    {
                        tags.set("White Luminance", serialize(whiteLuminanceAttribute(header).value()));
                    }
                    if (hasAdoptedNeutral(header))
                    {
                        tags.set("Adopted Neutral", serialize(adoptedNeutralAttribute(header).value()));
                    }
                    if (hasRenderingTransform(header))
                    {
                        tags.set("Rendering Transform", renderingTransformAttribute(header).value());
                    }
                    if (hasLookModTransform(header))
                    {
                        tags.set("Look Modification Transform", lookModTransformAttribute(header).value());
                    }
                    if (hasXDensity(header))
                    {
                        tags.set("X Density", serialize(xDensityAttribute(header).value()));
                    }
                    if (hasOwner(header))
                    {
                        tags.set("Owner", ownerAttribute(header).value());
                    }
                    if (hasComments(header))
                    {
                        tags.set("Comments", commentsAttribute(header).value());
                    }
                    if (hasCapDate(header))
                    {
                        tags.set("Capture Date", capDateAttribute(header).value());
                    }
                    if (hasUtcOffset(header))
                    {
                        tags.set("UTC Offset", serialize(utcOffsetAttribute(header).value()));
                    }
                    if (hasLongitude(header))
                    {
                        tags.set("Longitude", serialize(longitudeAttribute(header).value()));
                    }
                    if (hasLatitude(header))
                    {
                        tags.set("Latitude", serialize(latitudeAttribute(header).value()));
                    }
                    if (hasAltitude(header))
                    {
                        tags.set("Altitude", serialize(altitudeAttribute(header).value()));
                    }
                    if (hasFocus(header))
                    {
                        tags.set("Focus", serialize(focusAttribute(header).value()));
                    }
                    if (hasExpTime(header))
                    {
                        tags.set("Exposure Time", serialize(expTimeAttribute(header).value()));
                    }
                    if (hasAperture(header))
                    {
                        tags.set("Aperture", serialize(apertureAttribute(header).value()));
                    }
                    if (hasIsoSpeed(header))
                    {
                        tags.set("ISO Speed", serialize(isoSpeedAttribute(header).value()));
                    }
                    if (hasEnvmap(header))
                    {
                        tags.set("Environment Map", serialize(envmapAttribute(header).value()));
                    }
                    if (hasKeyCode(header))
                    {
                        tags.set("Keycode", serialize(keyCodeAttribute(header).value()));
                    }
                    if (hasTimeCode(header))
                    {
                        tags.set("Timecode", serialize(timeCodeAttribute(header).value()));
                    }
                    if (hasWrapmodes(header))
                    {
                        tags.set("Wrap Modes", wrapmodesAttribute(header).value());
                    }
                    if (hasFramesPerSecond(header))
                    {
                        const Imf::Rational data = framesPerSecondAttribute(header).value();
                        speed = Math::IntRational(data.n, data.d);
                    }
                    if (hasMultiView(header))
                    {
                        tags.set("Multi-View", serialize(multiViewAttribute(header).value()));
                    }
                    if (hasWorldToCamera(header))
                    {
                        tags.set("World To Camera", serialize(worldToCameraAttribute(header).value()));
                    }
                    if (hasWorldToNDC(header))
                    {
                        tags.set("World To NDC", serialize(worldToNDCAttribute(header).value()));
                    }
                    if (hasDeepImageState(header))
                    {
                        tags.set("Deep Image State", serialize(deepImageStateAttribute(header).value()));
                    }
                    if (hasOriginalDataWindow(header))
                    {
                        tags.set("Original Data Window", serialize(originalDataWindowAttribute(header).value()));
                    }
                    if (hasDwaCompressionLevel(header))
                    {
                        tags.set("DWA Compression Level", serialize(dwaCompressionLevelAttribute(header).value()));
                    }

                    // Other attributes.
                    for (auto i = header.begin(); i != header.end(); ++i)
                    {
                        const auto j = std::find(knownAttributes.begin(), knownAttributes.end(), i.name());
                        if (j == knownAttributes.end())
                        {
                            if ("string" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::StringAttribute>(i.name()))
                                {
                                    tags.set(i.name(), ta->value());
                                }
                            }
                            else if ("stringVector" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::StringVectorAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("int" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::IntAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("float" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::FloatAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("floatVector" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::FloatVectorAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("double" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::DoubleAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("v2i" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::V2iAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("v2f" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::V2fAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("v2d" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::V2dAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("v3i" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::V3iAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("v3f" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::V3fAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("v3d" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::V3dAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("box2i" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::Box2iAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("box2f" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::Box2fAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("m33f" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::M33fAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("m33d" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::M33dAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("m44f" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::M44fAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("m44d" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::M44dAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                            else if ("rational" == std::string(i.attribute().typeName()))
                            {
                                if (const auto ta = header.findTypedAttribute<Imf::RationalAttribute>(i.name()))
                                {
                                    tags.set(i.name(), serialize(ta->value()));
                                }
                            }
                        }
                    }
                }

                void writeTags(const Image::Tags& tags, const Math::IntRational& speed, Imf::Header& header)
                {
                    if (tags.contains("Chromaticities"))
                    {
                        std::stringstream ss(tags.get("Chromaticities"));
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
                    if (tags.contains("White Luminance"))
                    {
                        addWhiteLuminance(header, std::stof(tags.get("White Luminance")));
                    }
                    if (tags.contains("X Density"))
                    {
                        addXDensity(header, std::stof(tags.get("X Desnity")));
                    }
                    if (tags.contains("Owner"))
                    {
                        addOwner(header, tags.get("Owner"));
                    }
                    if (tags.contains("Comments"))
                    {
                        addComments(header, tags.get("Comments"));
                    }
                    if (tags.contains("Capture Date"))
                    {
                        addCapDate(header, tags.get("Capture Date"));
                    }
                    if (tags.contains("UTC Offset"))
                    {
                        addUtcOffset(header, std::stof(tags.get("UTC Offset")));
                    }
                    if (tags.contains("Longitude"))
                    {
                        addLongitude(header, std::stof(tags.get("Longitude")));
                    }
                    if (tags.contains("Latitude"))
                    {
                        addLatitude(header, std::stof(tags.get("Latitude")));
                    }
                    if (tags.contains("Altitude"))
                    {
                        addAltitude(header, std::stof(tags.get("Altitude")));
                    }
                    if (tags.contains("Focus"))
                    {
                        addFocus(header, std::stof(tags.get("Focus")));
                    }
                    if (tags.contains("Exposure Time"))
                    {
                        addExpTime(header, std::stof(tags.get("Exposure Time")));
                    }
                    if (tags.contains("Aperture"))
                    {
                        addAperture(header, std::stof(tags.get("Aperture")));
                    }
                    if (tags.contains("ISO Speed"))
                    {
                        addIsoSpeed(header, std::stof(tags.get("ISO Speed")));
                    }
                    if (tags.contains("Keycode"))
                    {
                        int id     = 0;
                        int type   = 0;
                        int prefix = 0;
                        int count  = 0;
                        int offset = 0;
                        Time::stringToKeycode(tags.get("Keycode"), id, type, prefix, count, offset);
                        addKeyCode(header, Imf::KeyCode(id, type, prefix, count, offset));
                    }
                    if (tags.contains("Timecode"))
                    {
                        uint32_t timecode = 0;
                        Time::stringToTimecode(tags.get("Timecode"), timecode);
                        addTimeCode(header, timecode);
                    }
                    addFramesPerSecond(
                        header,
                        Imf::Rational(speed.getNum(), speed.getDen()));
                }

                Math::BBox2i fromImath(const Imath::Box2i& value)
                {
                    return Math::BBox2i(glm::ivec2(value.min.x, value.min.y), glm::ivec2(value.max.x, value.max.y));
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
                    
                DJV_ENUM_HELPERS_IMPLEMENTATION(Channels);
                DJV_ENUM_HELPERS_IMPLEMENTATION(Compression);

            } // namespace OpenEXR
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::OpenEXR,
        Channels,
        DJV_TEXT("exr_channel_grouping_none"),
        DJV_TEXT("exr_channel_grouping_known"),
        DJV_TEXT("exr_channel_grouping_all"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::IO::OpenEXR,
        Compression,
        DJV_TEXT("exr_compression_none"),
        DJV_TEXT("exr_compression_rle"),
        DJV_TEXT("exr_compression_zips"),
        DJV_TEXT("exr_compression_zip"),
        DJV_TEXT("exr_compression_piz"),
        DJV_TEXT("exr_compression_pxr24"),
        DJV_TEXT("exr_compression_b44"),
        DJV_TEXT("exr_compression_b44a"),
        DJV_TEXT("exr_compression_dwaa"),
        DJV_TEXT("exr_compression_dwab"));

    rapidjson::Value toJSON(const AV::IO::OpenEXR::Options& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
            out.AddMember("ThreadCount", toJSON(value.threadCount, allocator), allocator);
            {
                std::stringstream ss;
                ss << value.channels;
                const std::string& s = ss.str();
                out.AddMember("Channels", rapidjson::Value(s.c_str(), s.size(), allocator), allocator);
            }
            {
                std::stringstream ss;
                ss << value.compression;
                const std::string& s = ss.str();
                out.AddMember("Compression", rapidjson::Value(s.c_str(), s.size(), allocator), allocator);
            }
            out.AddMember("DWACompressionLevel", toJSON(value.dwaCompressionLevel, allocator), allocator);
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, AV::IO::OpenEXR::Options& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("ThreadCount", i.name.GetString()))
                {
                    fromJSON(i.value, out.threadCount);
                }
                else if (0 == strcmp("Channels", i.name.GetString()) && i.value.IsString())
                {
                    std::stringstream ss(i.value.GetString());
                    ss >> out.channels;
                }
                else if (0 == strcmp("Compression", i.name.GetString()) && i.value.IsString())
                {
                    std::stringstream ss(i.value.GetString());
                    ss >> out.compression;
                }
                else if (0 == strcmp("DWACompressionLevel", i.name.GetString()))
                {
                    fromJSON(i.value, out.dwaCompressionLevel);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv
