#ifndef CUSTOMTFC_H
#define CUSTOMTFC_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>

#define CUSTOM_TFC_MAGIC 0x5391422B ///2B 42 91 53
#define ALIGN_TO_BLOCK_SIZE 0x8000

struct TFCBlockHeader
{
    uint32_t Magic = CUSTOM_TFC_MAGIC;
    uint32_t BlockOffset = 0;
    uint32_t BlockSize = 16;
    uint32_t NextBlockOffset = 0xFFFFFFFF;
};

struct TFCInventoryEntry
{
    uint32_t    SavedBulkDataSizeOnDisk = 0;
    uint32_t    SavedBulkDataOffsetInFile = 0xFFFFFFFF;
    uint32_t    ObjectNameLength = 0;
    std::string ObjectName;
};

class CustomTFC
{
public:
    ///constructors-destructors
    CustomTFC() {}
    ~CustomTFC() {}
    CustomTFC(const char* filename);
    ///file access
    bool SaveOnDisk();
    bool Read(const char* filename);
    bool Reload();
    bool IsLoaded() { return TFCFile.good(); }
    bool WriteData(TFCInventoryEntry& DataDescr, std::vector<char> DataToWrite);
    ///getters
    std::string GetFilename() { return TFCFileName; }
private:
    std::map<std::string, TFCInventoryEntry>    TFCInventory;
    std::vector<TFCBlockHeader>                 BlockHeaders;
    std::string                                 TFCFileName;
    //std::iostream                               TFCFile;
    std::fstream                                TFCFile;
    uint32_t                                    LastEntryEndOffset = 0;
    ///internal operations
    bool InternalWriteData(TFCInventoryEntry& DataDescr, std::vector<char> DataToWrite);
    bool InternalWriteNewBlock();
};

#endif //CUSTOMTFC_H
