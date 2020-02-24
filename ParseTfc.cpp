#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>

#include "LzoUtils.h"
#include "dds.h"

using namespace std;

string GetFilename(uint32_t offset)
{
    char ch[255];
    sprintf(ch, "%08X", offset);
    return string(ch) + ".dds";
}

bool SaveDDS(vector<char>& DecompressedDataChunk, string filename)
{
    uint32_t guessWidth, guessHeight;

    ///assuming PF_DXT1
    guessWidth = sqrt(DecompressedDataChunk.size()*2);

    ///find the closest power of 2
    uint32_t p2 = 0x80000000;
    while(p2)
    {
        if(guessWidth & p2)
        {
            guessWidth = p2;
            break;
        }
        p2 >>= 1;
    }

    guessHeight = DecompressedDataChunk.size()*2 / guessWidth;

    DDSHeader header = MakeDDSHeader("PF_DXT1");

    /// adjust header
    header.Height = guessWidth;
    header.Width = guessHeight;
    header.Depth = 1;
    header.MipMapCount = 1;
    header.PitchOrLinearSize = DecompressedDataChunk.size();

    uint32_t ddsMagic = 0x20534444; ///DDS

    ofstream out(filename.c_str(), ios::binary);

    ///write magic
    out.write(reinterpret_cast<char*>(&ddsMagic), 4);

    ///write header
    WriteDDSHeader(out, header);

    ///write data
    out.write(DecompressedDataChunk.data(), DecompressedDataChunk.size());

    out.close();
    cout << "DDS file saved: " << filename << endl;

    return 0;
}

int main(int argN, char* argV[])
{
    cout << "ParseTfc by wghost81 aka Wasteland Ghost" << endl;

    if (argN != 2)
    {
        cerr << "Usage: ParseTfc textures.tfc" << endl;
        return 1;
    }

    ifstream in(argV[1], ios::binary);
    if (!in.is_open())
    {
        cerr << "Cannot open " << argV[1] << endl;
        return 1;
    }

    /// find magic
    while (in.good())
    {
        uint32_t tag = 0;
        while (tag != 0x9E2A83C1 && in.good())
        {
            in.read(reinterpret_cast<char*>(&tag), 4);
            if (tag != 0x9E2A83C1)
                in.seekg(-3, ios::cur);
        }
        in.seekg(-4, ios::cur);
        if (!in.good())
            return 0;
        if (tag != 0x9E2A83C1)
        {
            cerr << "Missing 0x9E2A83C1 signature!\n";
            return 0;
        }
        uint32_t offset = in.tellg();
        vector<char> DecompressedDataChunk;
        DecompressLzoCompressedDataFromStream(in, DecompressedDataChunk);
        SaveDDS(DecompressedDataChunk, GetFilename(offset));
    }
    return 0;
}

