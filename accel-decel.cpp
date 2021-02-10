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
#include <cmath>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

#include <iostream> // TODO: Remove!

namespace
{
    // This is only a struct instead of just a namespace in order
    // to avoid all the forward declarations that a namespace would
    // require.
    struct impl
    {
        // Range between which speed is normal
        struct normal_range
        {
            double start = 0.0;
            double stop  = 0.0;
        };

        static std::vector<normal_range>
        get_normal_ranges(char* const* begin, char* const* end, double sample_rate)
        {
            std::vector<normal_range> result;

            normal_range nr;
            for (auto iter = begin; iter != end; ++iter)
            {
                if (sscanf(*iter, "%lf,%lf", &nr.start, &nr.stop) != 2)
                {
                    usage();
                }
                nr.start *= sample_rate;
                nr.stop  *= sample_rate;
                result.push_back(nr);
            }

            return result;
        }

        static std::string&
        program_name()
        {
            static std::string pn;
            return pn;
        }

        static void
        usage()
        {
            std::cerr << "Usage: " << program_name() << " <infile> <outfile> <accel> <normal-range> [<normal-range>...]" << std::endl;
            exit(EXIT_FAILURE);
        }

    };
}

int main(int argc, char** argv)
    try
    {
        impl::program_name() = argv[0];
        if (argc < 5)
        {
            impl::usage();
        }

        sf::file::info info;
        sf::file in(argv[1], SFM_READ, info);
        sf::file out(argv[2], SFM_WRITE, info);

        double acceleration = std::stod(argv[3]);
        auto normal_ranges = impl::get_normal_ranges(argv + 4, argv + argc, info.samplerate);

        std::vector<double> input(info.frames * info.channels);
        if (input.size() == 0)
        {
            input.resize(100 * 1000 * 1000);
        }

        std::vector<double> output;
        output.reserve(input.size());

        in.read(input);

        std::cout << "input.size() is " << input.size() << std::endl;

        constexpr double normal_speed = 1.0;
        
        // Find velocity at start of song. We do this by ramping backward
        // to the beginning of the song from the start of the normal speed
        // range. (Since we're doing this backwards, we call the final velocity
        // function.)
        double speed = normal_speed;
        std::cerr << "Initial speed is " << speed << std::endl;
        double src_t = 0;
        int dest_t = 0;
        int next_range = 0;
        double next_peak = normal_ranges[0].start / 2;

        std::cout << "Normal start: " << normal_ranges[next_range].start << std::endl;
        std::cout << "Normal stop: " << normal_ranges[next_range].stop << std::endl;
        while (src_t >= 0.0 && src_t < input.size() / info.channels)
        {
            for (int chan = 0; chan < info.channels; ++chan)
            {
                int in_index = static_cast<int>(src_t) * info.channels + chan;
                int out_index = dest_t * info.channels + chan;

                if (output.size() <= out_index)
                {
                    output.resize(out_index + 1);
                }
                output[out_index] = input[in_index];

                if (src_t >= normal_ranges[next_range].start &&
                    src_t <= normal_ranges[next_range].stop)
                {
                    speed = normal_speed;
                }
                else
                {
                    if (src_t > normal_ranges[next_range].stop)
                    {
                        if (next_range + 1 < normal_ranges.size())
                        {
                            double first = normal_ranges[next_range].stop;
                            ++next_range;
                            double last = normal_ranges[next_range].start;
                            next_peak = (last - first) / 2 + first;
                        }
                        else
                        {
                            double first = normal_ranges[next_range].stop;
                            double last = input.size() / info.channels;
                            next_peak = (last - first) / 2 + first;
                        }
                    }
                    if (src_t > next_peak)
                    {
                        // Decelerate
                        speed -= acceleration;
                    }
                    else
                    {
                        // Accelerate
                        speed += acceleration;
                    }
                }
                
                src_t += speed;
                dest_t += 1;

                if (dest_t % info.samplerate == 0)
                {
                    std::cout << "source time: " << src_t << ", destination time: " << dest_t << ", speed: " << speed << std::endl;
                    std::cout << "    Normal start: " << normal_ranges[next_range].start << std::endl;
                    std::cout << "    Normal stop: " << normal_ranges[next_range].stop << std::endl;

                }
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
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

