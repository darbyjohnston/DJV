// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OpenEXRTest.h>

#include <djvAV/OpenEXR.h>

#include <djvCore/Error.h>

#include <ImfStandardAttributes.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::IO;

namespace djv
{
    namespace AVTest
    {
        OpenEXRTest::OpenEXRTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::AVTest::OpenEXRTest", tempPath, context)
        {}
        
        void OpenEXRTest::run()
        {
            _enum();
            _data();
            _serialize();
        }

        void OpenEXRTest::_enum()
        {
            for (const auto& i : OpenEXR::getChannelsEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Channels: " + _getText(ss.str()));
            }
            
            for (const auto& i : OpenEXR::getCompressionEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Compression: " + _getText(ss.str()));
            }
        }
        
        void OpenEXRTest::_data()
        {
            {
                const OpenEXR::Channel channel;
                DJV_ASSERT(channel.name.empty());
                DJV_ASSERT(Image::DataType::None == channel.type);
                DJV_ASSERT(glm::ivec2(1, 1) == channel.sampling);
            }

            {
                const OpenEXR::Channel channel(
                    "name",
                    Image::DataType::F32,
                    glm::ivec2(2, 3));
                DJV_ASSERT("name" == channel.name);
                DJV_ASSERT(Image::DataType::F32 == channel.type);
                DJV_ASSERT(glm::ivec2(2, 3) == channel.sampling);
            }
            
            {
                const OpenEXR::Layer layer;
                DJV_ASSERT(layer.name.empty());
                DJV_ASSERT(layer.channels.empty());
                DJV_ASSERT(false == layer.luminanceChroma);
            }

            {
                const OpenEXR::Channel channel("name", Image::DataType::F32);
                const OpenEXR::Layer layer({ channel }, true);
                DJV_ASSERT(!layer.channels.empty());
                DJV_ASSERT("name" == layer.channels[0].name);
                DJV_ASSERT(Image::DataType::F32 == layer.channels[0].type);
                DJV_ASSERT(true == layer.luminanceChroma);
            }
            
            {
                DJV_ASSERT("diffuse.g,b" == OpenEXR::getLayerName({
                    "diffuse",
                    "diffuse.g",
                    "diffuse.b" }));
            }
            
            {
                Imf::ChannelList in;
                in.insert("r", Imf::Channel());
                in.insert("g", Imf::Channel());
                in.insert("b", Imf::Channel());
                in.insert("diffuse.r", Imf::Channel());
                in.insert("diffuse.g", Imf::Channel());
                in.insert("diffuse.b", Imf::Channel());
                const Imf::ChannelList out = OpenEXR::getDefaultLayer(in);
                DJV_ASSERT(out.findChannel("r"));
                DJV_ASSERT(out.findChannel("g"));
                DJV_ASSERT(out.findChannel("b"));
                DJV_ASSERT(!out.findChannel("diffuse.r"));
                DJV_ASSERT(!out.findChannel("diffuse.g"));
                DJV_ASSERT(!out.findChannel("diffuse.b"));
            }
            
            {
                Imf::ChannelList list;
                list.insert("diffuse.r", Imf::Channel());
                list.insert("diffuse.g", Imf::Channel());
                list.insert("diffuse.b", Imf::Channel());
                std::string name = "r";
                DJV_ASSERT(OpenEXR::find(list, name));
                DJV_ASSERT("diffuse.r" == name);
                DJV_ASSERT(!OpenEXR::find(list, name));
            }
            
            {
                Imf::ChannelList list;
                list.insert("r", Imf::Channel());
                list.insert("diffuse.r", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::None);
                size_t found = 0;
                for (const auto& i : layers)
                {
                    if ("r" == i.name) ++found;
                    if ("diffuse.r" == i.name) ++found;
                }
                DJV_ASSERT(2 == found);
            }
            
            {
                Imf::ChannelList list;
                list.insert("r", Imf::Channel());
                list.insert("g", Imf::Channel());
                list.insert("b", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("b,g,r" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("red", Imf::Channel());
                list.insert("green", Imf::Channel());
                list.insert("blue", Imf::Channel());
                list.insert("alpha", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("alpha,blue,green,red" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("y", Imf::Channel());
                list.insert("a", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("a,y" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("y", Imf::Channel());
                list.insert("ry", Imf::Channel(Imf::PixelType::HALF, 2, 2));
                list.insert("by", Imf::Channel(Imf::PixelType::HALF, 2, 2));
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("by,ry,y" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("x", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("x" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("y", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("y" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("z", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("z" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("x", Imf::Channel());
                list.insert("y", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("x,y" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("x", Imf::Channel());
                list.insert("y", Imf::Channel());
                list.insert("z", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("x,y,z" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("ar", Imf::Channel());
                list.insert("ag", Imf::Channel());
                list.insert("ab", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(1 == layers.size());
                DJV_ASSERT("ab,ag,ar" == layers[0].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("r", Imf::Channel());
                list.insert("g", Imf::Channel());
                list.insert("b", Imf::Channel());
                list.insert("c", Imf::Channel());
                list.insert("d", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(3 == layers.size());
                DJV_ASSERT("b,g,r" == layers[0].name);
                DJV_ASSERT("c" == layers[1].name);
                DJV_ASSERT("d" == layers[2].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("r", Imf::Channel());
                list.insert("g", Imf::Channel());
                list.insert("b", Imf::Channel());
                list.insert("c", Imf::Channel());
                list.insert("d", Imf::Channel());
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::All);
                DJV_ASSERT(2 == layers.size());
                DJV_ASSERT("b,g,r" == layers[0].name);
                DJV_ASSERT("c,d" == layers[1].name);
            }
            
            {
                Imf::ChannelList list;
                list.insert("r", Imf::Channel());
                list.insert("g", Imf::Channel());
                list.insert("b", Imf::Channel(Imf::PixelType::FLOAT));
                const auto layers = OpenEXR::getLayers(list, OpenEXR::Channels::Known);
                DJV_ASSERT(3 == layers.size());
                DJV_ASSERT("b" == layers[0].name);
                DJV_ASSERT("g" == layers[1].name);
                DJV_ASSERT("r" == layers[2].name);
            }
            
            {
                Imf::Header header;
                const IMATH_NAMESPACE::Box2i box2i(
                    IMATH_NAMESPACE::V2i(1, 2),
                    IMATH_NAMESPACE::V2i(3, 4));
                header.insert("box2i", Imf::Box2iAttribute(box2i));
                
                Math::IntRational speed;
                Image::Tags tags;
                OpenEXR::readTags(header, tags, speed);
                
                DJV_ASSERT(tags.contains("box2i"));
            }
        }
        
        void OpenEXRTest::_serialize()
        {
            {
                OpenEXR::Options options;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(options, allocator);
                OpenEXR::Options options2;
                fromJSON(json, options2);
                DJV_ASSERT(options == options2);
            }
            
            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                OpenEXR::Options options;
                fromJSON(json, options);
                DJV_ASSERT(options == options);
            }
            catch (const std::exception& e)
            {
                _print(Error::format(e.what()));
            }
        }
        
    } // namespace AVTest
} // namespace djv

