#include <iostream>

#include "CustomTFC.h"

CustomTFC::CustomTFC(const char* filename)
{
    CustomTFC::Read(filename);
}

bool CustomTFC::SaveOnDisk()
{
    /*std::ofstream file(TFCFileName, std::ios::binary);
    if (!file)
        return false;
    //file.write(TFCFile.str().data(), TFCFile.str().size());
    file << TFCFile.rdbuf();
    file.close();*/
    return true;
}

bool CustomTFC::Read(const char* filename)
{
    TFCFileName = filename;
    //TFCFile.str("");
    /*std::ifstream file(TFCFileName, std::ios::binary);
    if (file)
    {
        file.seekg(0, std::ios::end);
        if (file.tellg() != 0)
        {
            file.seekg(0);
            TFCFile << file.rdbuf();
        }
        file.close();
    }*/
    if (TFCFile.is_open())
        TFCFile.close();
    TFCFile.open(TFCFileName.c_str(), std::ios::binary | std::ios::in | std::ios::out);
    if (!TFCFile)
    {
        std::ofstream tmpout(TFCFileName.c_str(), std::ios::binary);
        tmpout.close();
        TFCFile.open(TFCFileName.c_str(), std::ios::binary | std::ios::in | std::ios::out);
        if (!TFCFile)
        {
            std::cerr << "Cannot open custom texture file: " << TFCFileName << std::endl;
            return false;
        }
    }
    return Reload();
}

bool CustomTFC::Reload()
{
    if (!IsLoaded())
    {
        std::cerr << "Custom tfc is not loaded!\n";
        return false;
    }
    TFCFile.clear();
    TFCFile.seekg(0);
    TFCInventory.clear();
    BlockHeaders.clear();
    ///test if the file is empty
    TFCFile.seekg(0, std::ios::end);
    if (TFCFile.tellg() == 0)
        return true;
    else
        TFCFile.seekg(0);
    ///attempt to read the info
    bool stop = false;
    while (!stop)
    {
        TFCBlockHeader header;
        TFCFile.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (header.Magic != CUSTOM_TFC_MAGIC || !TFCFile.good())
        {
            std::cerr << "Missing custom tfc magic!\n";
            return false;
        }
        BlockHeaders.push_back(header);
        uint32_t maxOffset = header.BlockOffset + header.BlockSize;
        while (TFCFile.tellg() < maxOffset)
        {
            TFCInventoryEntry entry;
            TFCFile.read(reinterpret_cast<char*>(&entry.SavedBulkDataSizeOnDisk), 4);
            TFCFile.read(reinterpret_cast<char*>(&entry.SavedBulkDataOffsetInFile), 4);
            TFCFile.read(reinterpret_cast<char*>(&entry.ObjectNameLength), 4);
            if (!TFCFile.good())
            {
                std::cerr << "Error reading custom tfc entry!\n";
                return false;
            }
            if ((uint32_t)TFCFile.tellg() + (uint32_t)entry.ObjectNameLength > (uint32_t)maxOffset)
            {
                std::cerr << "Error reading custom tfc entry!\n";
                return false;
            }
            std::vector<char> tmpVec(entry.ObjectNameLength);
            TFCFile.read(tmpVec.data(), tmpVec.size());
            if (!TFCFile.good())
            {
                std::cerr << "Error reading custom tfc entry!\n";
                return false;
            }
            entry.ObjectName = std::string(tmpVec.data(), tmpVec.size());
            std::string id = entry.ObjectName + "_" + std::to_string(entry.SavedBulkDataSizeOnDisk);
            TFCInventory[id] = entry;
        }
        LastEntryEndOffset = TFCFile.tellg();
        if (header.NextBlockOffset != 0xFFFFFFFF)
            TFCFile.seekg(header.NextBlockOffset);
        else
            stop = true;
    }
    return true;
}

bool CustomTFC::WriteData(TFCInventoryEntry& DataDescr, std::vector<char> DataToWrite)
{
    if (!IsLoaded())
    {
        std::cerr << "Custom tfc is not loaded!\n";
        return false;
    }
    if (DataToWrite.size() == 0 || DataDescr.SavedBulkDataSizeOnDisk != DataToWrite.size())
    {
        std::cerr << "Custom tfc: wrong data size!\n";
        return false;
    }
    std::string id = DataDescr.ObjectName + "_" + std::to_string(DataDescr.SavedBulkDataSizeOnDisk);
    if (TFCInventory.count(id) > 0)
    {
        DataDescr.SavedBulkDataOffsetInFile = TFCInventory[id].SavedBulkDataOffsetInFile;
        return true;
    }
    return InternalWriteData(DataDescr, DataToWrite);
}

bool CustomTFC::InternalWriteData(TFCInventoryEntry& DataDescr, std::vector<char> DataToWrite)
{
    ///new file -> make the first header
    if (BlockHeaders.size() == 0 && !InternalWriteNewBlock())
    {
        std::cerr << "Error writing custom tfc header!\n";
        return false;
    }
    if (DataToWrite.size() == 0)
    {
        std::cerr << "Error writing custom tfc data: data array is empty!\n";
        return false;
    }
    ///write the data
    TFCFile.seekg(0, std::ios::end);
    uint32_t newDataOffset = TFCFile.tellg();
    DataDescr.SavedBulkDataOffsetInFile = newDataOffset;
    TFCFile.seekp(newDataOffset);
    TFCFile.write(DataToWrite.data(), DataToWrite.size());
    if (!TFCFile.good())
    {
        std::cerr << "Error writing custom tfc data!\n";
        std::cerr << "Stream flags: EOF = " << TFCFile.eof() << " FAIL = " << TFCFile.fail() << " BAD = " << TFCFile.bad() << " RDSTATE = " << TFCFile.rdstate() << std::endl;
        return false;
    }
    ///zero-padding
    if (TFCFile.tellp() % ALIGN_TO_BLOCK_SIZE)
    {
        std::vector<char> alignmentZeros(ALIGN_TO_BLOCK_SIZE - TFCFile.tellp() % ALIGN_TO_BLOCK_SIZE, 0x00);
        TFCFile.write(reinterpret_cast<char*>(alignmentZeros.data()), alignmentZeros.size());
        if (!TFCFile.good())
        {
            std::cerr << "Custom tfc: zero-padding failed!\n";
            return false;
        }
    }
    ///add new descriptor into the tfc
    uint32_t newEntryOffset = LastEntryEndOffset;
    DataDescr.ObjectNameLength = DataDescr.ObjectName.size();
    uint32_t newEntrySize = 12 + DataDescr.ObjectNameLength;
    TFCBlockHeader LastHeader = BlockHeaders[BlockHeaders.size() - 1];
    ///not enough space inside the current block -> add a new one
    if (newEntryOffset + newEntrySize >= LastHeader.BlockOffset + ALIGN_TO_BLOCK_SIZE)
    {
        if (!InternalWriteNewBlock())
        {
            std::cerr << "Error writing new custom tfc block!\n";
            return false;
        }
        LastHeader = BlockHeaders[BlockHeaders.size() - 1];
    }
    ///write a new entry
    TFCFile.seekp(LastEntryEndOffset);
    TFCFile.write(reinterpret_cast<char*>(&DataDescr.SavedBulkDataSizeOnDisk), 4);
    TFCFile.write(reinterpret_cast<char*>(&DataDescr.SavedBulkDataOffsetInFile), 4);
    TFCFile.write(reinterpret_cast<char*>(&DataDescr.ObjectNameLength), 4);
    TFCFile.write(DataDescr.ObjectName.data(), DataDescr.ObjectName.size());
    if (!TFCFile.good())
    {
        std::cerr << "Error writing new custom tfc entry!\n";
        return false;
    }
    LastEntryEndOffset = TFCFile.tellp();
    ///add the entry to the inventory
    std::string id = DataDescr.ObjectName + "_" + std::to_string(DataDescr.SavedBulkDataSizeOnDisk);
    TFCInventory[id] = DataDescr;
    ///update current block size
    LastHeader.BlockSize += newEntrySize;
    TFCFile.seekp(LastHeader.BlockOffset + 8);
    TFCFile.write(reinterpret_cast<char*>(&LastHeader.BlockSize), 4);
    BlockHeaders[BlockHeaders.size() - 1] = LastHeader;
    return TFCFile.good();
}

bool CustomTFC::InternalWriteNewBlock()
{
    TFCFile.seekg(0, std::ios::end);
    uint32_t newBlockOffset = TFCFile.tellg();
    ///no headers and file is not empty -> not a custom tfc, won't do anything!
    if (BlockHeaders.size() == 0 && newBlockOffset != 0)
    {
        std::cerr << "The existing custom tfc file has no header!\n";
        return false;
    }
    ///write a new block of zeros
    TFCFile.seekp(newBlockOffset);
    std::vector<char> newBlock(ALIGN_TO_BLOCK_SIZE, 0x00);
    TFCFile.write(newBlock.data(), newBlock.size());
    if (!TFCFile.good())
    {
        std::cerr << "Custom tfc file: failed to make a new block!\n";
        return false;
    }
    ///set NextBlockOffset for the previous block
    if (BlockHeaders.size() > 0)
    {
        TFCFile.seekp(BlockHeaders[BlockHeaders.size() - 1].BlockOffset + 12);
        TFCFile.write(reinterpret_cast<char*>(&newBlockOffset), 4);
        if (!TFCFile.good())
        {
            std::cerr << "Custom tfc file: failed to update previous block link!\n";
            return false;
        }
    }
    ///make and write the new block header
    TFCBlockHeader NewHeader;
    NewHeader.BlockOffset = newBlockOffset;
    TFCFile.seekp(newBlockOffset);
    TFCFile.write(reinterpret_cast<char*>(&NewHeader), sizeof(NewHeader));
    if (!TFCFile.good())
    {
        std::cerr << "Custom tfc file: failed to write new block header!\n";
        return false;
    }
    ///set the offset for the new entry
    LastEntryEndOffset = TFCFile.tellp();
    ///add the new block to the list
    BlockHeaders.push_back(NewHeader);
    return true;
}
