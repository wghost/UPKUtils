#ifndef UPKUTILS_H
#define UPKUTILS_H

#include "UPKInfo.h"
#include "UObjectFactory.h"
#include <fstream>

enum class UPKScope
{
    Package = 0,
    Name = 1,
    Import = 2,
    Export = 3,
    Object = 4
};
std::string FormatUPKScope(UPKScope scope);

class UPKUtils: public UPKInfo
{
public:
    UPKUtils() {}
    ~UPKUtils() {}
    UPKUtils(const char* filename);
    /// Read package header
    bool Read(const char* filename);
    bool Reload();
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
    bool MoveResizeObject(uint32_t idx, int newObjectSize = -1);
    bool UndoMoveResizeObject(uint32_t idx);
    /// Deserialize
    std::string Deserialize(UObjectReference ObjRef, bool TryUnsafe = false, bool QuickMode = false);
    /// Write data
    bool CheckValidFileOffset(size_t offset);
    bool CheckValidRelOffset(size_t relOffset, UPKScope scope = UPKScope::Package, uint32_t idx = 0);
    bool CheckValidOffset(size_t offset, UPKScope scope = UPKScope::Package, uint32_t idx = 0);
    bool WriteExportData(uint32_t idx, std::vector<char> data, std::vector<char> *backupData = nullptr);
    bool WriteNameTableName(uint32_t idx, std::string name);
    bool WriteData(size_t offset, std::vector<char> data, std::vector<char> *backupData = nullptr);
    size_t FindDataChunk(std::vector<char> data, size_t beg = 0);
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
