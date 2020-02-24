#include "LzoUtils.h"

#include <sstream>
#include <stdlib.h>
#include "minilzo.h"

#define IN_LEN      (131072u)                              /// max input block size
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)        /// max output block size

static unsigned char __LZO_MMODEL in  [ IN_LEN ];          /// input data
static unsigned char __LZO_MMODEL out [ OUT_LEN ];         /// output data

static unsigned char __LZO_MMODEL workmem [ LZO1X_1_MEM_COMPRESS ]; /// work memory

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

uint32_t DecompressLzoCompressedRawData(std::vector<char>& CompressedDataChunk, std::vector<char>& DecompressedDataChunk)
{
    /// init lzo library
    int lzo_err;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    if (lzo_init() != LZO_E_OK)
    {
        std::cerr << "LZO library internal error: lzo_init() failed!\n";
        return 0;
    }
    lzo_memset(in, 0, IN_LEN);
    /// using stream for convenience
    std::stringstream compressed_stream;
    compressed_stream.write(reinterpret_cast<char*>(CompressedDataChunk.data()), CompressedDataChunk.size());
    /// check magic
    uint32_t tag = 0;
    compressed_stream.read(reinterpret_cast<char*>(&tag), 4);
    if (tag != 0x9E2A83C1)
    {
        std::cerr << "LZO library: Missing 0x9E2A83C1 signature!\n";
        return 0;
    }
    /// read block size
    uint32_t blockSize = 0;
    compressed_stream.read(reinterpret_cast<char*>(&blockSize), 4);
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: blockSize read error!\n";
        return 0;
    }
    if (blockSize != IN_LEN)
    {
        std::cerr << "LZO library: Incorrect max block size!\n";
        return 0;
    }
    /// read data chunk sizes
    std::vector<uint32_t> sizes(2); /// compressed/uncompressed pairs
    compressed_stream.read(reinterpret_cast<char*>(sizes.data()), 4 * sizes.size());
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: data chunk sizes read error!\n";
        return 0;
    }
    size_t dataSize = sizes[1]; /// uncompressed data chunk size
    unsigned numBlocks = (dataSize + blockSize - 1) / blockSize;
    if (numBlocks < 1)
    {
        std::cerr << "LZO library: Bad data!\n";
        return 0;
    }
    sizes.resize((numBlocks + 1)*2);
    /// read and decompress data
    compressed_stream.read(reinterpret_cast<char*>(sizes.data()) + 8, 4 * sizes.size() - 8);
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: compressed data sizes read error!\n";
        return 0;
    }
    std::vector<char> dataChunk(dataSize);
    std::vector<char> compressedData(sizes[0]);
    compressed_stream.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: compressed data read error!\n";
        return 0;
    }
    size_t blockOffset = 0;
    size_t dataOffset = 0;
    for (unsigned i = 1; i <= numBlocks; ++i)
    {
        out_len = sizes[i * 2]; /// compressed size
        lzo_memcpy(out, compressedData.data() + blockOffset, out_len);
        in_len = sizes[i * 2 + 1]; /// uncompressed size
        new_len = in_len;
        lzo_err = lzo1x_decompress_safe(out, out_len, in, &new_len, NULL);
        if (lzo_err != LZO_E_OK || new_len != in_len)
        {
            std::cerr << "LZO library internal error: decompression failed!\n";
            return 0;
        }
        lzo_memcpy(dataChunk.data() + dataOffset, in, in_len);
        blockOffset += out_len;
        dataOffset += in_len;
    }
    DecompressedDataChunk = dataChunk;
    return dataChunk.size();
}

uint32_t DecompressLzoCompressedDataFromStream(std::istream& compressed_stream, std::vector<char>& DecompressedDataChunk)
{
    /// init lzo library
    int lzo_err;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    if (lzo_init() != LZO_E_OK)
    {
        std::cerr << "LZO library internal error: lzo_init() failed!\n";
        return 0;
    }
    lzo_memset(in, 0, IN_LEN);
    /// check magic
    uint32_t tag = 0;
    compressed_stream.read(reinterpret_cast<char*>(&tag), 4);
    if (tag != 0x9E2A83C1)
    {
        std::cerr << "LZO library: Missing 0x9E2A83C1 signature!\n";
        return 0;
    }
    /// read block size
    uint32_t blockSize = 0;
    compressed_stream.read(reinterpret_cast<char*>(&blockSize), 4);
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: blockSize read error!\n";
        return 0;
    }
    if (blockSize != IN_LEN)
    {
        std::cerr << "LZO library: Incorrect max block size!\n";
        return 0;
    }
    /// read data chunk sizes
    std::vector<uint32_t> sizes(2); /// compressed/uncompressed pairs
    compressed_stream.read(reinterpret_cast<char*>(sizes.data()), 4 * sizes.size());
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: data chunk sizes read error!\n";
        return 0;
    }
    size_t dataSize = sizes[1]; /// uncompressed data chunk size
    unsigned numBlocks = (dataSize + blockSize - 1) / blockSize;
    if (numBlocks < 1)
    {
        std::cerr << "LZO library: Bad data!\n";
        return 0;
    }
    sizes.resize((numBlocks + 1)*2);
    /// read and decompress data
    compressed_stream.read(reinterpret_cast<char*>(sizes.data()) + 8, 4 * sizes.size() - 8);
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: compressed data sizes read error!\n";
        return 0;
    }
    std::vector<char> dataChunk(dataSize);
    std::vector<char> compressedData(sizes[0]);
    compressed_stream.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
    if (!compressed_stream.good())
    {
        std::cerr << "LZO library: compressed data read error!\n";
        return 0;
    }
    size_t blockOffset = 0;
    size_t dataOffset = 0;
    for (unsigned i = 1; i <= numBlocks; ++i)
    {
        out_len = sizes[i * 2]; /// compressed size
        lzo_memcpy(out, compressedData.data() + blockOffset, out_len);
        in_len = sizes[i * 2 + 1]; /// uncompressed size
        new_len = in_len;
        lzo_err = lzo1x_decompress_safe(out, out_len, in, &new_len, NULL);
        if (lzo_err != LZO_E_OK || new_len != in_len)
        {
            std::cerr << "LZO library internal error: decompression failed!\n";
            return 0;
        }
        lzo_memcpy(dataChunk.data() + dataOffset, in, in_len);
        blockOffset += out_len;
        dataOffset += in_len;
    }
    DecompressedDataChunk = dataChunk;
    return dataChunk.size();
}

uint32_t FindAndDecompressLzoCompressedDataFromStream(std::istream& compressed_stream, uint32_t decompressedSize, std::vector<char>& DecompressedDataChunk)
{
    /// init lzo library
    int lzo_err;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    if (lzo_init() != LZO_E_OK)
    {
        std::cerr << "LZO library internal error: lzo_init() failed!\n";
        return 0;
    }
    lzo_memset(in, 0, IN_LEN);
    uint32_t resSize = 0;
    while (resSize != decompressedSize && compressed_stream.good())
    {
        /// check magic
        uint32_t tag = 0;
        while (tag != 0x9E2A83C1 && compressed_stream.good())
        {
            compressed_stream.read(reinterpret_cast<char*>(&tag), 4);
            if (tag != 0x9E2A83C1)
                compressed_stream.seekg(-3, std::ios::cur);
        }
        if (!compressed_stream.good())
        {
            std::cerr << "LZO library: unexpected EOF!\n";
            return 0;
        }
        if (tag != 0x9E2A83C1)
        {
            std::cerr << "LZO library: Missing 0x9E2A83C1 signature!\n";
            return 0;
        }
        /// read block size
        uint32_t blockSize = 0;
        compressed_stream.read(reinterpret_cast<char*>(&blockSize), 4);
        if (!compressed_stream.good())
        {
            std::cerr << "LZO library: blockSize read error!\n";
            return 0;
        }
        if (blockSize != IN_LEN)
        {
            std::cerr << "LZO library: Incorrect max block size!\n";
            return 0;
        }
        /// read data chunk sizes
        std::vector<uint32_t> sizes(2); /// compressed/uncompressed pairs
        compressed_stream.read(reinterpret_cast<char*>(sizes.data()), 4 * sizes.size());
        if (!compressed_stream.good())
        {
            std::cerr << "LZO library: data chunk sizes read error!\n";
            return 0;
        }
        size_t dataSize = sizes[1]; /// uncompressed data chunk size
        resSize = dataSize;
        if (resSize != decompressedSize)
            continue;
        unsigned numBlocks = (dataSize + blockSize - 1) / blockSize;
        if (numBlocks < 1)
        {
            std::cerr << "LZO library: Bad data!\n";
            return 0;
        }
        sizes.resize((numBlocks + 1)*2);
        /// read and decompress data
        compressed_stream.read(reinterpret_cast<char*>(sizes.data()) + 8, 4 * sizes.size() - 8);
        if (!compressed_stream.good())
        {
            std::cerr << "LZO library: compressed data sizes read error!\n";
            return 0;
        }
        std::vector<char> dataChunk(dataSize);
        std::vector<char> compressedData(sizes[0]);
        compressed_stream.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
        if (!compressed_stream.good())
        {
            std::cerr << "LZO library: compressed data read error!\n";
            return 0;
        }
        size_t blockOffset = 0;
        size_t dataOffset = 0;
        for (unsigned i = 1; i <= numBlocks; ++i)
        {
            out_len = sizes[i * 2]; /// compressed size
            lzo_memcpy(out, compressedData.data() + blockOffset, out_len);
            in_len = sizes[i * 2 + 1]; /// uncompressed size
            new_len = in_len;
            lzo_err = lzo1x_decompress_safe(out, out_len, in, &new_len, NULL);
            if (lzo_err != LZO_E_OK || new_len != in_len)
            {
                std::cerr << "LZO library internal error: decompression failed!\n";
                return 0;
            }
            lzo_memcpy(dataChunk.data() + dataOffset, in, in_len);
            blockOffset += out_len;
            dataOffset += in_len;
        }
        DecompressedDataChunk = dataChunk;
    }
    return DecompressedDataChunk.size();
}

uint32_t LzoCompress(std::vector<char>& DataChunk, std::vector<char>& CompressedDataChunk)
{
    /// init lzo library
    int lzo_err;
    lzo_uint in_len;
    lzo_uint out_len;
    if (lzo_init() != LZO_E_OK)
    {
        std::cerr << "LZO library internal error: lzo_init() failed!\n";
        return 0;
    }
    lzo_memset(out, 0, OUT_LEN);
    lzo_memset(workmem, 0, LZO1X_1_MEM_COMPRESS);
    /// count blocks
    uint32_t blockSize = IN_LEN;
    size_t dataSize = DataChunk.size(); /// uncompressed data chunk size
    unsigned numBlocks = (dataSize + blockSize - 1) / blockSize;
    if (numBlocks < 1)
    {
        std::cerr << "LZO library: Bad data!\n";
        return 0;
    }
    std::vector<uint32_t> sizes(numBlocks*2); /// compressed/uncompressed size
    std::vector<std::vector<char> > compressedData(numBlocks); /// compressed blocks
    size_t blockOffset = 0;
    size_t compressedSize = 0;
    /// compress blocks
    for (unsigned i = 0; i < numBlocks; ++i)
    {
        in_len = (blockOffset + blockSize) <= DataChunk.size() ? blockSize : DataChunk.size() - blockOffset;
        lzo_memcpy(in, DataChunk.data() + blockOffset, in_len);
        out_len = OUT_LEN;
        lzo_err = lzo1x_1_compress(in, in_len, out, &out_len, workmem);
        if (lzo_err != LZO_E_OK)
        {
            std::cerr << "LZO library internal error: decompression failed!\n";
            return 0;
        }
        blockOffset += in_len;
        compressedSize += out_len;
        compressedData[i].resize(out_len);
        lzo_memcpy(compressedData[i].data(), out, out_len);
        sizes[i * 2] = out_len; /// compressed size
        sizes[i * 2 + 1] = in_len; /// uncompressed size
    }
    std::vector<uint32_t> chunkSizes(2); /// compressed/uncompressed chunk sizes
    chunkSizes[0] = compressedSize; /// compressed size
    chunkSizes[1] = dataSize; /// uncompressed size
    /// using stream for convenience
    std::stringstream compressed_stream;
    uint32_t tag = 0x9E2A83C1;
    compressed_stream.write(reinterpret_cast<char*>(&tag), 4);
    compressed_stream.write(reinterpret_cast<char*>(&blockSize), 4);
    compressed_stream.write(reinterpret_cast<char*>(chunkSizes.data()), 4 * chunkSizes.size());
    compressed_stream.write(reinterpret_cast<char*>(sizes.data()), 4 * sizes.size());
    for (unsigned i = 0; i < numBlocks; ++i)
        compressed_stream.write(reinterpret_cast<char*>(compressedData[i].data()), compressedData[i].size());
    CompressedDataChunk.resize(compressed_stream.tellp());
    compressed_stream.read(reinterpret_cast<char*>(CompressedDataChunk.data()), CompressedDataChunk.size());
    return CompressedDataChunk.size();
}
