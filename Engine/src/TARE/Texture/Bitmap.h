#pragma once

#include <string.h>
#include <glm/glm.hpp>
#include "TAGE/Common/TEnums.h"

namespace TARE {
    static int GetBytesPerComponent(BitmapFormat Fmt)
    {
        int BytesPerComponent = 0;

        switch (Fmt) {
        case BitmapFormat::UBYTE:
            BytesPerComponent = 1;
            break;

        case BitmapFormat::FLOAT:
            BytesPerComponent = 4;
            break;
        };

        return BytesPerComponent;
    }


    class Bitmap {

    public:
        Bitmap() {};

        void Init(int w, int h, int comp, BitmapFormat fmt)
        {
            w_ = w;
            h_ = h;
            comp_ = comp;
            fmt_ = fmt;
            data_.resize(w * h * comp * GetBytesPerComponent(fmt));

            initGetSetFuncs();
        }

        Bitmap(int w, int h, int comp, BitmapFormat fmt)
            :w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w* h* comp* GetBytesPerComponent(fmt))
        {
            initGetSetFuncs();
        }

        Bitmap(int w, int h, int d, int comp, BitmapFormat fmt)
            :w_(w), h_(h), d_(d), comp_(comp), fmt_(fmt), data_(w* h* d* comp* GetBytesPerComponent(fmt))
        {
            initGetSetFuncs();
        }

        Bitmap(int w, int h, int comp, BitmapFormat fmt, void* ptr)
            :w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w* h* comp* GetBytesPerComponent(fmt))
        {
            initGetSetFuncs();
            memcpy(data_.data(), ptr, data_.size());
        }

        int w_ = 0;
        int h_ = 0;
        int d_ = 1;
        int comp_ = 3;
        BitmapFormat fmt_ = BitmapFormat::UBYTE;
        BitmapType type_ = BitmapType::T2D;
        std::vector<uint8_t> data_;

        void setPixel(int x, int y, glm::vec4& c)
        {
            (*this.*setPixelFunc)(x, y, c);
        }

        glm::vec4 getPixel(int x, int y) const
        {
            return ((*this.*getPixelFunc)(x, y));
        }

    private:

        using setPixel_t = void(Bitmap::*)(int, int, glm::vec4&);
        using getPixel_t = glm::vec4(Bitmap::*)(int, int) const;
        setPixel_t setPixelFunc = &Bitmap::setPixelUnsignedByte;
        getPixel_t getPixelFunc = &Bitmap::getPixelUnsignedByte;

        void initGetSetFuncs()
        {
            switch (fmt_)
            {
            case BitmapFormat::UBYTE:
                setPixelFunc = &Bitmap::setPixelUnsignedByte;
                getPixelFunc = &Bitmap::getPixelUnsignedByte;
                break;
            case BitmapFormat::FLOAT:
                setPixelFunc = &Bitmap::setPixelFloat;
                getPixelFunc = &Bitmap::getPixelFloat;
                break;
            }
        }

        void setPixelFloat(int x, int y, glm::vec4& c)
        {
            int ofs = comp_ * (y * w_ + x);
            float* data = (float*)(data_.data());
            if (comp_ > 0) data[ofs + 0] = c.x;
            if (comp_ > 1) data[ofs + 1] = c.y;
            if (comp_ > 2) data[ofs + 2] = c.z;
            if (comp_ > 3) data[ofs + 3] = c.w;
        }

        glm::vec4 getPixelFloat(int x, int y) const
        {
            int ofs = comp_ * (y * w_ + x);
            const float* data = (const float*)(data_.data());
            return glm::vec4(
                comp_ > 0 ? data[ofs + 0] : 0.0f,
                comp_ > 1 ? data[ofs + 1] : 0.0f,
                comp_ > 2 ? data[ofs + 2] : 0.0f,
                comp_ > 3 ? data[ofs + 3] : 0.0f);
        }

        void setPixelUnsignedByte(int x, int y, glm::vec4& c)
        {
            int ofs = comp_ * (y * w_ + x);
            if (comp_ > 0) data_[ofs + 0] = uint8_t(c.x * 255.0f);
            if (comp_ > 1) data_[ofs + 1] = uint8_t(c.y * 255.0f);
            if (comp_ > 2) data_[ofs + 2] = uint8_t(c.z * 255.0f);
            if (comp_ > 3) data_[ofs + 3] = uint8_t(c.w * 255.0f);
        }

        glm::vec4 getPixelUnsignedByte(int x, int y) const
        {
            int ofs = comp_ * (y * w_ + x);
            return glm::vec4(
                comp_ > 0 ? float(data_[ofs + 0]) / 255.0f : 0.0f,
                comp_ > 1 ? float(data_[ofs + 1]) / 255.0f : 0.0f,
                comp_ > 2 ? float(data_[ofs + 2]) / 255.0f : 0.0f,
                comp_ > 3 ? float(data_[ofs + 3]) / 255.0f : 0.0f);
        }
    };
}