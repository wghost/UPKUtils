#ifndef LZOUTILS_H
#define LZOUTILS_H

#include <iostream>
#include <fstream>
#include <vector>

uint32_t DecompressLzoCompressedRawData(std::vector<char>& CompressedDataChunk, std::vector<char>& DecompressedDataChunk);
uint32_t DecompressLzoCompressedDataFromStream(std::istream& compressed_stream, std::vector<char>& DecompressedDataChunk);
uint32_t FindAndDecompressLzoCompressedDataFromStream(std::istream& compressed_stream, uint32_t decompressedSize, std::vector<char>& DecompressedDataChunk);
uint32_t LzoCompress(std::vector<char>& DataChunk, std::vector<char>& CompressedDataChunk);

#endif //LZOUTILS_H
