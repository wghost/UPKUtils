#include <iostream>
#include <iomanip>

#include "UPKInfo.h"
#include <fstream>
#include <sstream>
#include "minilzo.h"

using namespace std;

#define IN_LEN      (131072u)                              /// max input block size
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)        /// max output block size

static unsigned char __LZO_MMODEL in  [ IN_LEN ];          /// input data
static unsigned char __LZO_MMODEL out [ OUT_LEN ];         /// output data

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

//static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);           /// lzo working memory

void SerializeSummary(FPackageFileSummary Summary, ostream& decompressedPackage);

int main(int argN, char* argV[])
{
    cout << "DecompressLZO" << endl;

    if (argN < 2 || argN > 3)
    {
        cerr << "Usage: DecompressLZO CompressedResourceFile.upk [DecompressedResourceFile.upk]" << endl;
        return 1;
    }

    ifstream package(argV[1], ios::binary);
    if (!package.is_open())
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }
    stringstream package_stream;
    package_stream << package.rdbuf();
    package.close();

    UPKInfo PackageInfo(package_stream);

    UPKReadErrors err = PackageInfo.GetError();

    if (err != UPKReadErrors::NoErrors && !PackageInfo.IsCompressed())
    {
        cerr << "Error reading package:\n" << FormatReadErrors(err);
        return 1;
    }

    if (!PackageInfo.IsCompressed())
    {
        cerr << "Package is already decompressed!\n";
        return 1;
    }

    cout << PackageInfo.FormatSummary() << std::endl;

    FPackageFileSummary Summary = PackageInfo.GetSummary();

    if (Summary.CompressionFlags != (uint32_t)UCompressionFlags::LZO && !PackageInfo.IsFullyCompressed())
    {
        cerr << "Can't decompress non-LZO compressed packages!\n";
        return 1;
    }

    /// init lzo library
    int lzo_err;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;

    if (lzo_init() != LZO_E_OK)
    {
        cout << "LZO library internal error: lzo_init() failed!!!\n";
        return 1;
    }

    lzo_memset(in, 0, IN_LEN);

    stringstream decompressed_stream;
    ofstream decompressedPackage;
    if (argN == 3)
    {
        decompressedPackage.open(argV[2], ios::binary);
    }
    else
    {
        decompressedPackage.open((string(argV[1]) + ".uncompr").c_str(), ios::binary);
    }
    if (!decompressedPackage.is_open())
    {
        cerr << "Can't open output file!!!\n";
        return 1;
    }

    unsigned int NumCompressedChunks = Summary.NumCompressedChunks;

    if (PackageInfo.IsFullyCompressed())
    {
        NumCompressedChunks = 1;
    }
    else
    {
        cout << "Writing package summary...\n";
        SerializeSummary(Summary, decompressed_stream);
    }

    cout << "Decompressing package...\n";

    for (unsigned int i = 0; i < NumCompressedChunks; ++i)
    {
        if (PackageInfo.IsFullyCompressed())
        {
            package_stream.seekg(0);
        }
        else
        {
            package_stream.seekg(Summary.CompressedChunks[i].CompressedOffset);
        }

        cout << "Decompressing chunk #" << i << endl;

        uint32_t tag = 0;
        package_stream.read(reinterpret_cast<char*>(&tag), 4);
        if (tag != 0x9E2A83C1)
        {
            cerr << "Missing magic number!\n";
            return 1;
        }
        uint32_t blockSize = 0;
        package_stream.read(reinterpret_cast<char*>(&blockSize), 4);
        if (blockSize != IN_LEN)
        {
            cerr << "Incorrect max block size!\n";
            return 1;
        }
        vector<uint32_t> sizes(2); /// compressed/uncompressed pairs
        package_stream.read(reinterpret_cast<char*>(sizes.data()), 4 * sizes.size());
        size_t dataSize = sizes[1]; /// uncompressed data chunk size
        unsigned numBlocks = (dataSize + blockSize - 1) / blockSize;
        cout << "Num blocks: " << numBlocks << endl;
        if (numBlocks < 1)
        {
            cerr << "Bad data!\n";
            return 1;
        }
        sizes.resize((numBlocks + 1)*2);
        package_stream.read(reinterpret_cast<char*>(sizes.data()) + 8, 4 * sizes.size() - 8);
        for (unsigned i = 0; i <= numBlocks; ++i)
        {
            cout << "Compressed size: " << sizes[i * 2]
                 << "\tUncompressed size: " << sizes[i * 2 + 1] << endl;
        }
        vector<unsigned char> dataChunk(dataSize);
        vector<unsigned char> compressedData(sizes[0]);
        package_stream.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
        size_t blockOffset = 0;
        size_t dataOffset = 0;
        for (unsigned i = 1; i <= numBlocks; ++i)
        {
            out_len = sizes[i * 2]; /// compressed size
            lzo_memcpy(out, compressedData.data() + blockOffset, out_len);
            in_len = sizes[i * 2 + 1]; /// uncompressed size
            new_len = in_len;
            lzo_err = lzo1x_decompress(out, out_len, in, &new_len, NULL);
            if (lzo_err == LZO_E_OK && new_len == in_len)
                cout << "decompressed " << (unsigned long) out_len << " bytes back into "
                     << (unsigned long) in_len << endl;
            else
            {
                cout << "LZO library internal error: decompression failed!!!\n";
                return 1;
            }
            lzo_memcpy(dataChunk.data() + dataOffset, in, in_len);
            blockOffset += out_len;
            dataOffset += in_len;
        }
        decompressed_stream.write(reinterpret_cast<char*>(dataChunk.data()), dataSize);
    }

    decompressedPackage << decompressed_stream.str();

    return 0;
}

void SerializeSummary(FPackageFileSummary Summary, ostream& stream)
{
    /// reset compression flags
    Summary.CompressionFlags = 0;
    Summary.PackageFlags ^= (uint32_t)UPackageFlags::Compressed;
    Summary.NumCompressedChunks = 0;
    /// serialize summary
    stream.seekp(0);
    stream.write(reinterpret_cast<char*>(&Summary.Signature), 4);
    int32_t tmpVer = Summary.Version + (Summary.LicenseeVersion << 16);
    stream.write(reinterpret_cast<char*>(&tmpVer), 4);
    stream.write(reinterpret_cast<char*>(&Summary.HeaderSize), 4);
    stream.write(reinterpret_cast<char*>(&Summary.FolderNameLength), 4);
    if (Summary.FolderNameLength > 0)
    {
        stream.write(Summary.FolderName.c_str(), Summary.FolderNameLength);
    }
    stream.write(reinterpret_cast<char*>(&Summary.PackageFlags), 4);
    stream.write(reinterpret_cast<char*>(&Summary.NameCount), 4);
    stream.write(reinterpret_cast<char*>(&Summary.NameOffset), 4);
    stream.write(reinterpret_cast<char*>(&Summary.ExportCount), 4);
    stream.write(reinterpret_cast<char*>(&Summary.ExportOffset), 4);
    stream.write(reinterpret_cast<char*>(&Summary.ImportCount), 4);
    stream.write(reinterpret_cast<char*>(&Summary.ImportOffset), 4);
    stream.write(reinterpret_cast<char*>(&Summary.DependsOffset), 4);
    stream.write(reinterpret_cast<char*>(&Summary.SerialOffset), 4);
    stream.write(reinterpret_cast<char*>(&Summary.Unknown2), 4);
    stream.write(reinterpret_cast<char*>(&Summary.Unknown3), 4);
    stream.write(reinterpret_cast<char*>(&Summary.Unknown4), 4);
    stream.write(reinterpret_cast<char*>(&Summary.GUID), sizeof(Summary.GUID));
    stream.write(reinterpret_cast<char*>(&Summary.GenerationsCount), 4);
    for (unsigned i = 0; i < Summary.GenerationsCount; ++i)
    {
        stream.write(reinterpret_cast<char*>(&Summary.Generations[i].ExportCount), 4);
        stream.write(reinterpret_cast<char*>(&Summary.Generations[i].NameCount), 4);
        stream.write(reinterpret_cast<char*>(&Summary.Generations[i].NetObjectCount), 4);
    }
    stream.write(reinterpret_cast<char*>(&Summary.EngineVersion), 4);
    stream.write(reinterpret_cast<char*>(&Summary.CookerVersion), 4);
    stream.write(reinterpret_cast<char*>(&Summary.CompressionFlags), 4);
    stream.write(reinterpret_cast<char*>(&Summary.NumCompressedChunks), 4);
    if (Summary.UnknownDataChunk.size() > 0)
    {
        stream.write(Summary.UnknownDataChunk.data(), Summary.UnknownDataChunk.size());
    }
}
