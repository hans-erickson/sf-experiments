//
//  MIT License
//  
//  Copyright (c) 2021 Hans Erickson
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//  

#include <gtest/gtest.h>

#include "../sf.h"

namespace
{
    std::string
    get_sf_path(const std::string& fname)
    {
        return std::string(SF_TEST_SOUND_DIR) + "/" + fname;
    }

    std::string
    get_tmp_path(const std::string& fname)
    {
        return std::string("/tmp/") + fname;
    }
}

TEST(WrapperTest, BellTest)
{
    sf::file::info rinfo;
    sf::file in(get_sf_path("bell.oga"), SFM_READ, rinfo);
    EXPECT_EQ(rinfo.frames, 6151);
    EXPECT_EQ(rinfo.samplerate, 44100);
    EXPECT_EQ(rinfo.channels, 2);
    EXPECT_EQ(rinfo.format, SF_FORMAT_OGG | SF_FORMAT_VORBIS);
    EXPECT_EQ(rinfo.sections, 1);
    EXPECT_EQ(rinfo.seekable, 1);

    std::vector<float> buffer(rinfo.frames * rinfo.channels);
    in.read(buffer);

    EXPECT_EQ(buffer.size(), rinfo.frames * rinfo.channels);
    std::vector<float> reverse(buffer.rbegin(), buffer.rend());

    sf::file::info winfo;
    winfo.samplerate = rinfo.samplerate;
    winfo.channels = rinfo.channels;
    winfo.format = rinfo.format;
    
    EXPECT_EQ(reverse.size(), rinfo.frames * rinfo.channels);
    sf::file out(get_tmp_path("reverse-bell.ogg"), SFM_WRITE, winfo);
    out.write(reverse);
}
