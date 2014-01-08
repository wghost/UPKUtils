#ifndef UPKUTILS_H
#define UPKUTILS_H

#include "UPKInfo.h"
#include <fstream>

class UPKUtils: public UPKInfo
{
public:
    UPKUtils() {}
    ~UPKUtils() {}
    UPKUtils(const char* filename);
    /// Read package header
    bool Read(const char* filename);
    void Reload();
    bool IsLoaded() { return (UPKFile.is_open() && UPKFile.good()); };
    /// Extract serialized data
    std::vector<char> GetExportData(uint32_t idx);
    void SaveExportData(uint32_t idx);
    /// Move/expand export object data (legacy functions for backward compatibility)
    bool MoveExportData(uint32_t idx, uint32_t newObjectSize = 0);
    bool UndoMoveExportData(uint32_t idx);
    /// Move/resize export object data (new functions)
    /// You cannot resize object without moving it first
    /// You can move object without resizing it
    bool MoveResizeExportObject(uint32_t idx, int newObjectSize = -1);
    bool UndoMoveResizeExportObject(uint32_t idx);
    /// Deserialize
    std::string Deserialize(UObjectReference ObjRef);
    /// Write data
    bool CheckValidFileOffset(size_t offset);
    bool WriteExportData(uint32_t idx, std::vector<char> data, std::vector<char> *backupData = nullptr);
    bool WriteNameTableName(uint32_t idx, std::string name);
    bool WriteData(size_t offset, std::vector<char> data, std::vector<char> *backupData = nullptr);
    size_t FindDataChunk(std::vector<char> data);
    /// Aggressive patching functions
    /*
    bool AddName(FNameEntry entry);
    bool AddImport(FObjectImport entry);
    bool AddExport(FObjectExport entry);
    bool LinkChild(UObjectReference LastChildRef, UObjectReference NewChildRef);
    bool ResizeInPlace(UObjectReference ObjRef, uint32_t newObjectSize);
    */
    /*bool AddNameListEntry(NameListEntry entry);
    bool AddObjectListEntry(ObjectListEntry entry);
    bool AddImportListEntry(ImportListEntry entry);*/
private:
    std::fstream UPKFile;
    size_t UPKFileSize;
};

#endif // UPKUTILS_H
