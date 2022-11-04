#ifndef DDS_H_INCLUDED
#define DDS_H_INCLUDED

#include <cstdint>
#include <iostream>
#include <cstring>
#include <string>

#define FOURCC_DXT1 0x31545844 ///DXT1

enum class DDSPixelFormatFlags: uint32_t
{
    DDPF_ALPHAPIXELS = 0x00000001,
    DDPF_ALPHA       = 0x00000002,
    DDPF_FOURCC      = 0x00000004,
    DDPF_RGB         = 0x00000040,
    DDPF_YUV         = 0x00000200,
    DDPF_LUMINANCE   = 0x00020000
};

struct DDSPixelFormat
{
    uint32_t Size = 32;
    uint32_t Flags;
    uint32_t FourCC;
    uint32_t RGBBitCount;
    uint32_t RBitMask;
    uint32_t GBitMask;
    uint32_t BBitMask;
    uint32_t ABitMask;
};

enum class DDSHeaderFlags: uint32_t
{
    DDSD_CAPS        = 0x00000001,
    DDSD_HEIGHT      = 0x00000002,
    DDSD_WIDTH       = 0x00000004,
    DDSD_PITCH       = 0x00000008,
    DDSD_PIXELFORMAT = 0x00001000,
    DDSD_MIPMAPCOUNT = 0x00020000,
    DDSD_LINEARSIZE  = 0x00080000,
    DDSD_DEPTH       = 0x00800000
};

enum class DDSCapsFlags: uint32_t
{
    DDSCAPS_COMPLEX     = 0x00000008,
    DDSCAPS_MIPMAP      = 0x00400000,
    DDSCAPS_TEXTURE     = 0x00001000
};

enum class DDSCaps2Flags: uint32_t
{
    DDSCAPS2_CUBEMAP             = 0x00000200,
    DDSCAPS2_CUBEMAP_POSITIVEX   = 0x00000400,
    DDSCAPS2_CUBEMAP_NEGATIVEX   = 0x00000800,
    DDSCAPS2_CUBEMAP_POSITIVEY   = 0x00001000,
    DDSCAPS2_CUBEMAP_NEGATIVEY   = 0x00002000,
    DDSCAPS2_CUBEMAP_POSITIVEZ   = 0x00004000,
    DDSCAPS2_CUBEMAP_NEGATIVEZ   = 0x00008000,
    DDSCAPS2_VOLUME              = 0x00200000
};

typedef struct
{
    uint32_t        Size = 124;
    uint32_t        Flags;              ///DDSD_CAPS + DDSD_HEIGHT + DDSD_WIDTH + DDSD_PIXELFORMAT are mandatory
    uint32_t        Height;
    uint32_t        Width;
    uint32_t        PitchOrLinearSize;
    uint32_t        Depth;
    uint32_t        MipMapCount;
    uint32_t        Reserved1[11];
    DDSPixelFormat  spf;
    uint32_t        Caps;               ///DDSCAPS_TEXTURE is mandatory
    uint32_t        Caps1;
    uint32_t        Caps2;
    uint32_t        Caps3;
    uint32_t        Reserved2;
} DDSHeader;

uint32_t PixelFormatNameToFourCC(std::string name);
std::string FourCCToPixelFormatString(uint32_t fcc);
std::string GetPixelFormatStringFromDDSHeader(DDSHeader header);
DDSHeader MakeDefaultDDSHeader();
DDSHeader MakeDDSHeader(std::string pixelFormat);
std::ostream& WriteDDSHeader(std::ostream& out, DDSHeader& header);
std::istream& ReadDDSHeader(std::istream& in, DDSHeader& header);

#endif //DDS_H_INCLUDED
