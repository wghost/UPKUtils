#include "dds.h"

uint32_t PixelFormatNameToFourCC(std::string name)
{
    if (name == "PF_DXT1")
        return 0x31545844; ///'D' 'X' 'T' '1'
    if (name == "PF_DXT3")
        return 0x33545844; ///'D' 'X' 'T' '3'
    if (name == "PF_DXT5")
        return 0x35545844; ///'D' 'X' 'T' '5'
    return 0;
}

std::string FourCCToPixelFormatString(uint32_t fcc)
{
    switch (fcc)
    {
    case 0x31545844:
        return "PF_DXT1";
    case 0x33545844:
        return "PF_DXT3";
    case 0x35545844:
        return "PF_DXT5";
    default:
        return "";
    }
}

std::string GetPixelFormatStringFromDDSHeader(DDSHeader header)
{
    ///uncompressed grayscale data
    if ((header.spf.Flags & (uint32_t)DDSPixelFormatFlags::DDPF_ALPHAPIXELS) && (header.spf.Flags & (uint32_t)DDSPixelFormatFlags::DDPF_RGB) && header.spf.RGBBitCount == 8)
    {
        return "PF_G8";
    }
    /// uncompressed ARBG data
    if ((header.spf.Flags & (uint32_t)DDSPixelFormatFlags::DDPF_ALPHAPIXELS) && (header.spf.Flags & (uint32_t)DDSPixelFormatFlags::DDPF_RGB) && header.spf.RGBBitCount == 32)
    {
        return "PF_A8R8G8B8";
    }
    /// TC_NormalmapUncompressed
    if ((header.spf.Flags & 0x00080000) && header.spf.RGBBitCount == 16)
    {
        return "PF_V8U8";
    }
    /// compressed texels
    else if (header.spf.Flags & (uint32_t)DDSPixelFormatFlags::DDPF_FOURCC)
    {
        return FourCCToPixelFormatString(header.spf.FourCC);
    }
    else
    {
        std::cerr << "DDS header: unsupported pixel format!" << std::endl;
    }

    return "";
}

DDSHeader MakeDefaultDDSHeader()
{
    DDSHeader header;

    header.Size = 124;
    header.Flags = (uint32_t)DDSHeaderFlags::DDSD_CAPS + (uint32_t)DDSHeaderFlags::DDSD_HEIGHT + (uint32_t)DDSHeaderFlags::DDSD_WIDTH + (uint32_t)DDSHeaderFlags::DDSD_PIXELFORMAT;
    header.Height = 0;
    header.Width = 0;
    header.PitchOrLinearSize = 0;
    header.Depth = 1;
    header.MipMapCount = 1;
    header.Reserved1[0] = 0;
    header.Reserved1[1] = 0;
    header.Reserved1[2] = 0;
    header.Reserved1[3] = 0;
    header.Reserved1[4] = 0;
    header.Reserved1[5] = 0;
    header.Reserved1[6] = 0;
    header.Reserved1[7] = 0;
    header.Reserved1[8] = 0;
    header.Reserved1[9] = 0;
    header.Reserved1[10] = 0;

    header.spf.Size = 32;
    header.spf.Flags = 0;
    header.spf.FourCC = 0;
    header.spf.RGBBitCount = 0;
    header.spf.RBitMask = 0;
    header.spf.GBitMask = 0;
    header.spf.BBitMask = 0;
    header.spf.ABitMask = 0;

    header.Caps = (uint32_t)DDSCapsFlags::DDSCAPS_TEXTURE;
    header.Caps1 = 0;
    header.Caps2 = 0;
    header.Caps3 = 0;
    header.Reserved2 = 0;

    return header;
}

DDSHeader MakeDDSHeader(std::string pixelFormat)
{
    DDSHeader header = MakeDefaultDDSHeader();

    ///uncompressed grayscale data
    if (pixelFormat == "PF_G8")
    {
        header.spf.Flags = (uint32_t)DDSPixelFormatFlags::DDPF_ALPHAPIXELS + (uint32_t)DDSPixelFormatFlags::DDPF_RGB;
        header.spf.RGBBitCount = 8;
        header.spf.ABitMask = 0x00000000;
        header.spf.RBitMask = 0x00000000;
        header.spf.GBitMask = 0x0000FF00;
        header.spf.BBitMask = 0x00000000;
    }
    /// uncompressed ARBG data
    else if (pixelFormat == "PF_A8R8G8B8")
    {
        header.spf.Flags = (uint32_t)DDSPixelFormatFlags::DDPF_ALPHAPIXELS + (uint32_t)DDSPixelFormatFlags::DDPF_RGB;
        header.spf.RGBBitCount = 32;
        header.spf.ABitMask = 0xFF000000;
        header.spf.RBitMask = 0x00FF0000;
        header.spf.GBitMask = 0x0000FF00;
        header.spf.BBitMask = 0x000000FF;
    }
    /// TC_NormalmapUncompressed
    else if (pixelFormat == "PF_V8U8")
    {
        header.spf.Flags = 0x00080000;
        header.spf.RGBBitCount = 16;
        header.spf.RBitMask = 0x00FF;
        header.spf.GBitMask = 0xFF00;
        header.spf.BBitMask = 0x0000;
        header.spf.ABitMask = 0x0000;
    }
    /// compressed texels
    else if (pixelFormat == "PF_DXT1" || pixelFormat == "PF_DXT3" || pixelFormat == "PF_DXT5")
    {
        header.Flags |= (uint32_t)DDSHeaderFlags::DDSD_LINEARSIZE + (uint32_t)DDSHeaderFlags::DDSD_MIPMAPCOUNT;
        header.spf.Flags = (uint32_t)DDSPixelFormatFlags::DDPF_FOURCC;
        header.spf.FourCC = PixelFormatNameToFourCC(pixelFormat);
        header.Caps |= (uint32_t)DDSCapsFlags::DDSCAPS_MIPMAP + (uint32_t)DDSCapsFlags::DDSCAPS_COMPLEX;
    }
    else
    {
        std::cerr << "DDS header: unsupported pixel format! " << pixelFormat << std::endl;
    }

    return header;
}

std::ostream& WriteDDSHeader(std::ostream& out, DDSHeader& header)
{
    out.write(reinterpret_cast<char*>(&header.Size), 4);
    out.write(reinterpret_cast<char*>(&header.Flags), 4);
    out.write(reinterpret_cast<char*>(&header.Height), 4);
    out.write(reinterpret_cast<char*>(&header.Width), 4);
    out.write(reinterpret_cast<char*>(&header.PitchOrLinearSize), 4);
    out.write(reinterpret_cast<char*>(&header.Depth), 4);
    out.write(reinterpret_cast<char*>(&header.MipMapCount), 4);
    out.write(reinterpret_cast<char*>(&header.Reserved1), 4*11);

    out.write(reinterpret_cast<char*>(&header.spf.Size), 4);
    out.write(reinterpret_cast<char*>(&header.spf.Flags), 4);
    out.write(reinterpret_cast<char*>(&header.spf.FourCC), 4);
    out.write(reinterpret_cast<char*>(&header.spf.RGBBitCount), 4);
    out.write(reinterpret_cast<char*>(&header.spf.RBitMask), 4);
    out.write(reinterpret_cast<char*>(&header.spf.GBitMask), 4);
    out.write(reinterpret_cast<char*>(&header.spf.BBitMask), 4);
    out.write(reinterpret_cast<char*>(&header.spf.ABitMask), 4);

    out.write(reinterpret_cast<char*>(&header.Caps), 4);
    out.write(reinterpret_cast<char*>(&header.Caps1), 4);
    out.write(reinterpret_cast<char*>(&header.Caps2), 4);
    out.write(reinterpret_cast<char*>(&header.Caps3), 4);
    out.write(reinterpret_cast<char*>(&header.Reserved2), 4);

    return out;
}

std::istream& ReadDDSHeader(std::istream& in, DDSHeader& header)
{
    in.read(reinterpret_cast<char*>(&header.Size), 4);
    in.read(reinterpret_cast<char*>(&header.Flags), 4);
    in.read(reinterpret_cast<char*>(&header.Height), 4);
    in.read(reinterpret_cast<char*>(&header.Width), 4);
    in.read(reinterpret_cast<char*>(&header.PitchOrLinearSize), 4);
    in.read(reinterpret_cast<char*>(&header.Depth), 4);
    in.read(reinterpret_cast<char*>(&header.MipMapCount), 4);
    in.read(reinterpret_cast<char*>(&header.Reserved1), 4*11);

    in.read(reinterpret_cast<char*>(&header.spf.Size), 4);
    in.read(reinterpret_cast<char*>(&header.spf.Flags), 4);
    in.read(reinterpret_cast<char*>(&header.spf.FourCC), 4);
    in.read(reinterpret_cast<char*>(&header.spf.RGBBitCount), 4);
    in.read(reinterpret_cast<char*>(&header.spf.RBitMask), 4);
    in.read(reinterpret_cast<char*>(&header.spf.GBitMask), 4);
    in.read(reinterpret_cast<char*>(&header.spf.BBitMask), 4);
    in.read(reinterpret_cast<char*>(&header.spf.ABitMask), 4);

    in.read(reinterpret_cast<char*>(&header.Caps), 4);
    in.read(reinterpret_cast<char*>(&header.Caps1), 4);
    in.read(reinterpret_cast<char*>(&header.Caps2), 4);
    in.read(reinterpret_cast<char*>(&header.Caps3), 4);
    in.read(reinterpret_cast<char*>(&header.Reserved2), 4);

    return in;
}
