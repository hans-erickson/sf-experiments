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

#include "c++-wrapper/sf.h"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cmath>

namespace
{
    void
    usage(const char* name)
    {
        std::cerr << "Usage: " << name << " <infile> <outfile>" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        usage(argv[0]);
    }

    sf::file::info info;
    sf::file in(argv[1], SFM_READ, info);
    sf::file out(argv[2], SFM_WRITE, info);

    std::vector<double> input(info.frames * info.channels);
    if (input.size() == 0)
    {
        input.resize(100 * 1000 * 1000);
    }

    std::vector<double> output;
    output.reserve(input.size());

    in.read(input);

    std::cout << "input.size() is " << input.size() << std::endl;

    for (int chan = 0; chan < info.channels; ++chan)
    {
        std::cerr << "Doing channel " << chan << std::endl;
        int count = 0;
        double offset = 0;
        double phase = chan * M_PI;
        double minspeed = 1.0;
        double maxspeed = 3.0;
        while (offset >= 0.0 && offset < input.size() / info.channels)
        {
            int in_index = static_cast<int>(offset) * info.channels + chan;
            int out_index = count * info.channels + chan;

            if (output.size() <= out_index)
            {
                output.resize(out_index + 1);
            }
            output[out_index] = input[in_index];
            offset += sin(static_cast<double>(count) * M_PI / (info.samplerate * 10) + phase) + (maxspeed - minspeed) / 2.0 + 1.0;

            ++count;
        }
    }

    for (size_t begin = 0; begin < output.size();)
    {
        size_t end = std::min(begin + 1024 * 1024, output.size());
        std::vector buffer(output.begin() + begin, output.begin() + end);
        out.write(buffer);
        begin = end;
    }
    std::cerr << "output size is " << output.size() << std::endl;
}
