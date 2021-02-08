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

#include <sndfile.h>

#include <memory>
#include <string>
#include <vector>

namespace sf
{
    using count_t = sf_count_t;
    
    class file final
    {
    public:
        using info = SF_INFO;

        file(const std::string& path, int mode, info& info);

        ~file();

        void
        command(int cmd, void *data, int datasize);

        std::string
        get_string(int str_type);
        
        void
        open(const std::string& path, int mode, info& info);

        void
        read(std::vector<short>& buffer);

        void
        read(std::vector<int>& buffer);

        void
        read(std::vector<float>& buffer);

        void
        read(std::vector<double>& buffer);

        count_t
        seek(count_t frames, int whence);

        void
        set_string(int str_type, const std::string& str);

        void
        write(const std::vector<short>& buffer);

        void
        write(const std::vector<int>& buffer);

        void
        write(const std::vector<float>& buffer);

        void
        write(const std::vector<double>& buffer);

        void
        write_sync();

    private:
        struct Implementation;
        std::unique_ptr<Implementation> impl_;
    };
}
