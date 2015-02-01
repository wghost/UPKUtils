#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "minilzo.h"

using namespace std;

#define IN_LEN      (131072u)                              /// max input block size
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)        /// max output block size

static unsigned char __LZO_MMODEL in  [ IN_LEN ];          /// input data
static unsigned char __LZO_MMODEL out [ OUT_LEN ];         /// output data

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);           /// lzo working memory

#define TAG 0x9E2A83C1

int main(int argN, char* argV[])
{
    cout << "XComLZO Packer/Unpacker" << endl;

    if (argN != 3)
    {
        cerr << "Usage: XComLZO p inputFileName" << endl
             << "    or XComLZO u inputFileName" << endl
             << "where p = pack and u = unpack" << endl;
        return 1;
    }

    bool isPacking = false;

    if (string(argV[1]) == "p" || string(argV[1]) == "P")
        isPacking = true;
    else if (string(argV[1]) == "u" || string(argV[1]) == "U")
        isPacking = false;
    else
    {
        cerr << "Unknown option " << argV[1] << endl;
        return 1;
    }

    ifstream inFile(argV[2], ios::binary);
    if (!inFile.is_open())
    {
        cerr << "Can't open " << argV[2] << endl;
        return 1;
    }

    int err;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;

    if (lzo_init() != LZO_E_OK)
    {
        cout << "LZO library internal error: lzo_init() failed!!!\n";
        return 1;
    }

    lzo_memset(in, 0, IN_LEN);

    if (isPacking)
    {
        size_t dataSize = 0;
        inFile.seekg(0, ios::end);
        dataSize = inFile.tellg();
        inFile.seekg(0);
        vector<unsigned char> dataChunk(dataSize);
        inFile.read(reinterpret_cast<char*>(dataChunk.data()), dataSize);
        uint32_t blockSize = IN_LEN;
        unsigned numBlocks = (dataSize + blockSize - 1) / blockSize;
        vector<uint32_t> sizes((numBlocks + 1)*2); /// compressed/uncompressed pairs
        vector<unsigned char> compressedData;
        sizes[1] = dataSize; /// uncompressed file size
        for (unsigned i = 1; i < numBlocks; ++i)
        {
            sizes[i * 2 + 1] = blockSize; /// block uncompressed sizes, always the same
        }
        sizes[numBlocks * 2 + 1] = dataSize - (numBlocks - 1) * blockSize;
        size_t blockOffset = 0;
        for (unsigned i = 1; i <= numBlocks; ++i)
        {
            in_len = sizes[i * 2 + 1];
            lzo_memcpy(in, dataChunk.data() + blockOffset, in_len);
            err = lzo1x_1_compress(in, in_len, out, &out_len, wrkmem);
            if (err == LZO_E_OK)
                cout << "compressed " << (unsigned long) in_len << " bytes into "
                     << (unsigned long) out_len << " bytes\n";
            else
            {
                cout << "LZO library internal error: compression failed!!!\n";
                return 1;
            }
            sizes[i * 2] = out_len; /// block compressed size
            sizes[0] += out_len; /// overall compressed data size
            size_t compressedOffset = compressedData.size();
            compressedData.resize(compressedData.size() + out_len);
            lzo_memcpy(compressedData.data() + compressedOffset, out, out_len);
            blockOffset += in_len;
        }
        uint32_t tag = TAG;
        ofstream outFile((string(argV[2]) + ".packed").c_str(), ios::binary);
        if (!outFile.is_open())
        {
            cerr << "Can't open output file!!!\n";
            return 1;
        }
        outFile.write(reinterpret_cast<char*>(&tag), 4);
        outFile.write(reinterpret_cast<char*>(&blockSize), 4);
        outFile.write(reinterpret_cast<char*>(sizes.data()), 4 * sizes.size());
        outFile.write(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
        return 0;
    }
    else
    {
        ofstream outFile((string(argV[2]) + ".unpacked").c_str(), ios::binary);
        if (!outFile.is_open())
        {
            cerr << "Can't open output file!!!\n";
            return 1;
        }
        uint32_t tag = 0;
        while (inFile.read(reinterpret_cast<char*>(&tag), 4).good())
        {
            cout << "Next block:\n";
            if (tag != TAG)
            {
                cerr << "Missing magic number!\n";
                return 1;
            }
            uint32_t blockSize = 0;
            inFile.read(reinterpret_cast<char*>(&blockSize), 4);
            if (blockSize != IN_LEN)
            {
                cerr << "Incorrect max block size!\n";
                return 1;
            }
            vector<uint32_t> sizes(2); /// compressed/uncompressed pairs
            inFile.read(reinterpret_cast<char*>(sizes.data()), 4 * sizes.size());
            size_t dataSize = sizes[1]; /// uncompressed data chunk size
            unsigned numBlocks = (dataSize + blockSize - 1) / blockSize;
            //cout << "Num blocks: " << numBlocks << endl;
            if (numBlocks < 1)
            {
                cerr << "Bad data!\n";
                return 1;
            }
            sizes.resize((numBlocks + 1)*2);
            inFile.read(reinterpret_cast<char*>(sizes.data()) + 8, 4 * sizes.size() - 8);
            /*for (unsigned i = 0; i <= numBlocks; ++i)
            {
                cout << "Compressed size: " << sizes[i * 2]
                     << "\tUncompressed size: " << sizes[i * 2 + 1] << endl;
            }*/
            vector<unsigned char> dataChunk(dataSize);
            vector<unsigned char> compressedData(sizes[0]);
            inFile.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
            size_t blockOffset = 0;
            size_t dataOffset = 0;
            for (unsigned i = 1; i <= numBlocks; ++i)
            {
                out_len = sizes[i * 2]; /// compressed size
                lzo_memcpy(out, compressedData.data() + blockOffset, out_len);
                in_len = sizes[i * 2 + 1]; /// uncompressed size
                new_len = in_len;
                err = lzo1x_decompress(out, out_len, in, &new_len, NULL);
                if (err == LZO_E_OK && new_len == in_len)
                    cout << "decompressed " << (unsigned long) out_len << " bytes back into "
                         << (unsigned long) in_len << endl;
                else
                {
                    cout << "LZO library internal error: decompression failed!!!\n";
                    return 3;
                }
                lzo_memcpy(dataChunk.data() + dataOffset, in, in_len);
                blockOffset += out_len;
                dataOffset += in_len;
            }
            outFile.write(reinterpret_cast<char*>(dataChunk.data()), dataSize);
        }
        return 0;
    }
    return 0;
}
