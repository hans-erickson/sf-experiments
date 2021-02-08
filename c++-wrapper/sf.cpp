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

#include "sf.h"

#include <stdexcept>
#include <string>
#include <type_traits>

namespace sf
{
    struct file::Implementation
    {
        void
        throw_error(const std::string& msg = "")
        {
            std::string err = sf_strerror(sndfile);
            if (!msg.empty())
            {
                err = msg + ": " + err;
            }
            throw std::runtime_error(err);
        }

        // Wrapper to verify preconditions (sndfile not null) and postconditions
        // (integral values are greater than zero)
        template<typename Callable, typename... Arg,
                 typename Result = std::invoke_result_t<Callable, SNDFILE*, Arg...>>
        Result
        wrap(Callable callable, Arg... args)
        {
            if (sndfile == nullptr)
            {
                throw std::runtime_error("No sound file is open.");
            }

            if constexpr (!std::is_same_v<Result, void>)
            {
                Result result = callable(sndfile, args...);
                if constexpr (std::is_integral_v<Result>)
                {
                    if (result < 0)
                    {
                        throw_error();
                    }
                }
                return result;
            }
            else
            {
                callable(sndfile, args...);
            }
        }

        template<typename Callable, typename NumberType>
        void
        wrap_read(Callable callable, std::vector<NumberType>& buffer)
        {
            if (!buffer.empty())
            {
                sf_count_t result = wrap(callable, buffer.data(), buffer.size());
                if (result != 0)
                {
                    buffer.resize(result);
                }
            }
        }

        template<typename Callable, typename NumberType>
        void
        wrap_write(Callable callable, const std::vector<NumberType>& buffer)
        {
            wrap(callable, buffer.data(), buffer.size());
        }

        SNDFILE* sndfile = nullptr;
        sf::file::info info;
    };

    file::file(const std::string& path, int mode, info& info)
        : impl_(std::make_unique<Implementation>())
    {
        impl_->info = info;
        open(path, mode, info);
    }

    file::~file()
    {
        if (impl_->sndfile)
        {
            write_sync();
            sf_close(impl_->sndfile);
        }
    }

    void
    file::command(int cmd, void *data, int datasize)
    {
        impl_->wrap(sf_command, cmd, data, datasize);
    }

    std::string
    file::get_string(int str_type)
    {
        return impl_->wrap(sf_get_string, str_type);
    }
        
    void
    file::open(const std::string& path, int mode, info& info)
    {
        if (impl_->sndfile != nullptr)
        {
            sf_close(impl_->sndfile);
        }
        impl_->sndfile = sf_open(path.c_str(), mode, &info);
        if (impl_->sndfile == nullptr)
        {
            impl_->throw_error(path);
        }
    }

    void
    file::read(std::vector<short>& buffer)
    {
        impl_->wrap_read(sf_read_short, buffer);
    }

    void
    file::read(std::vector<int>& buffer)
    {
        impl_->wrap_read(sf_read_int, buffer);
    }

    void
    file::read(std::vector<float>& buffer)
    {
        impl_->wrap_read(sf_read_float, buffer);
    }

    void
    file::read(std::vector<double>& buffer)
    {
        impl_->wrap_read(sf_read_double, buffer);
    }

    count_t
    file::seek(count_t frames, int whence)
    {
        return impl_->wrap(sf_seek, frames, whence);
    }

    void
    file::set_string(int str_type, const std::string& str)
    {
        impl_->wrap(sf_set_string, str_type, str.c_str());
    }

    void
    file::write(const std::vector<short>& buffer)
    {
        impl_->wrap_write(sf_write_short, buffer);
    }

    void
    file::write(const std::vector<int>& buffer)
    {
        impl_->wrap_write(sf_write_int, buffer);
    }

    void
    file::write(const std::vector<float>& buffer)
    {
        impl_->wrap_write(sf_write_float, buffer);
    }

    void
    file::write(const std::vector<double>& buffer)
    {
        impl_->wrap_write(sf_write_double, buffer);
    }

    void
    file::write_sync()
    {
        impl_->wrap(sf_write_sync);
    }
}

