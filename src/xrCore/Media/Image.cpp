#include "stdafx.h"
#include "Image.hpp"

#include <cstdio>

using namespace XRay::Media;

Image::Image(u32 w, u32 h, void* dataPtr, ImageDataFormat fmt)
    : format(fmt),
      channelCount(format == ImageDataFormat::RGB8 ? 3 : 4),
      width(w), height(h), data(dataPtr) {}

Image::~Image()
{
    if (ownsData)
        xr_free(data);
}

void Image::SaveTGA(const char* name, ImageDataFormat format, bool align)
{
    FILE* file = std::fopen(name, "wb");
    auto writerFunc = [&](void* data, size_t dataSize) { std::fwrite(data, dataSize, 1, file); };
    SaveTGA(writerFunc, format, align);
    std::fclose(file);
}

void Image::SaveTGA(IWriter& writer, bool align) { SaveTGA(writer, format, align); }
void Image::SaveTGA(IWriter& writer, ImageDataFormat format, bool align)
{
    auto writerFunc = [&](void* data, size_t dataSize) { writer.w(data, dataSize); };
    SaveTGA(writerFunc, format, align);
}

void Image::SaveTGA(const char* name, bool align) { SaveTGA(name, format, align); }
template <typename TWriter>
void Image::SaveTGA(TWriter& writerFunc, ImageDataFormat format, bool align)
{
    R_ASSERT(data);
    R_ASSERT(width);
    R_ASSERT(height);
    TGAHeader hdr = {};
    hdr.ImageType = 2; // uncompressed true-color image
    hdr.Width = width;
    hdr.Height = height;
    int scanLength = width * channelCount;
    switch (format)
    {
    case ImageDataFormat::RGB8:
    {
        hdr.BPP = 24;
        // XXX: generally should be set to zero
        hdr.ImageDesc = 32;
        writerFunc(&hdr, sizeof(hdr));
        int paddingBuf = 0;
        int paddingSize = align ? 4 - (width * 3 & 3) : 0;
        for (u32 j = 0; j < height; j++)
        {
            u8* p = (u8*)data + scanLength * j;
            for (u32 i = 0; i < width; i++)
            {
                u8 buffer[3] = {p[0], p[1], p[2]};
                writerFunc(buffer, sizeof(buffer));
                p += channelCount;
            }
            if (paddingSize)
                writerFunc(&paddingBuf, paddingSize);
        }
        break;
    }
    case ImageDataFormat::RGBA8:
    {
        hdr.BPP = 32;
        hdr.ImageDesc = 0x0f | 32;
        writerFunc(&hdr, sizeof(hdr));
        if (this->format == format)
            writerFunc(data, width * height * channelCount);
        else
        {
            for (u32 j = 0; j < height; j++)
            {
                u8* p = (u8*)data + scanLength * j;
                for (u32 i = 0; i < width; i++)
                {
                    u8 buffer[4] = {p[0], p[1], p[2], 0xff};
                    writerFunc(buffer, sizeof(buffer));
                    p += channelCount;
                }
            }
        }
        break;
    }
    default: FATAL("Unsupported TGA image format");
    }
}
