#ifndef UPKUTILS_H
#define UPKUTILS_H

#include "UPKInfo.h"
#include "UObjectFactory.h"
#include <sstream>

class UPKUtils: public UPKInfo
{
public:
    UPKUtils() {}
    ~UPKUtils();
    UPKUtils(const char* filename);
    /// Read package header
    bool Read(const char* filename);
    bool SaveFileAs(const char* filename);
    bool SaveOnDisk();
    bool Reload();
    bool IsLoaded() { return (UPKFile.str() != "" && UPKFile.good()); };
    size_t GetFileSize() { return UPKFileSize; }
    void SetAutosave (bool val) { AutosaveOn = val; }
    /// Decompress and reload
    bool DecompressPackage(const char* decompFileName = "");
    /// Extract serialized data
    std::vector<char> GetExportData(uint32_t idx);
    std::string GetUObjectSerializedData(uint32_t idx);
    void SaveExportData(uint32_t idx);
    size_t GetScriptSize(uint32_t idx);
    size_t GetScriptMemSize(uint32_t idx);
    size_t GetScriptRelOffset(uint32_t idx);
    /// Modify serialized data in memory
    bool ReplacePropertyValue(UDefaultProperty prop, uint32_t idx, std::string& exportDataStr);
    bool ReplaceProperty(UDefaultProperty oldProp, UDefaultProperty newProp, uint32_t idx, std::string& exportDataStr);
    bool RemoveProperty(UDefaultProperty prop, uint32_t idx, std::string& exportDataStr);
    bool InsertProperty(UDefaultProperty prop, UDefaultProperty beforeProp, uint32_t idx, std::string& exportDataStr);
    bool InsertProperty(UDefaultProperty prop, uint32_t idx, std::string& exportDataStr);
    /// Move/expand export object data (legacy functions for backward compatibility)
    bool MoveExportData(uint32_t idx, uint32_t newObjectSize = 0);
    bool UndoMoveExportData(uint32_t idx);
    /// Move/resize export object data (new functions)
    /// You cannot resize object without moving it first
    /// You can move object without resizing it
    std::vector<char> GetResizedDataChunk(uint32_t idx, int newObjectSize = -1, int resizeAt = -1);
    bool ResizeInPlace(uint32_t idx, int newObjectSize = -1, int resizeAt = -1);
    bool MoveResizeObject(uint32_t idx, int newObjectSize = -1, int resizeAt = -1);
    bool UndoMoveResizeObject(uint32_t idx);
    /// Deserialize
    std::string Deserialize(UObjectReference ObjRef, bool TryUnsafe = false, bool QuickMode = false);
    UObject* DeserializeObjectByRef(UObjectReference ObjRef, bool DoNotReadTFC = false);
    bool Deserialize(FNameEntry& entry, std::vector<char>& data);
    bool Deserialize(FObjectImport& entry, std::vector<char>& data);
    bool Deserialize(FObjectExport& entry, std::vector<char>& data);
    /// Write data
    bool CheckValidFileOffset(size_t offset);
    bool WriteExportData(uint32_t idx, std::vector<char> data, std::vector<char> *backupData = nullptr);
    bool WriteNameTableName(uint32_t idx, std::string name);
    bool WriteData(size_t offset, std::vector<char> data, std::vector<char> *backupData = nullptr);
    size_t FindDataChunk(std::vector<char> data, size_t beg = 0, size_t limit = 0);
    std::vector<char> GetBulkData(size_t offset, std::vector<char> data);
    /// UPK serialization
    std::vector<char> SerializeSummary();
    std::vector<char> SerializeHeader();
    /// Aggressive patching functions
    bool AddNameEntry(FNameEntry Entry);
    bool AddImportEntry(FObjectImport Entry);
    bool AddExportEntry(FObjectExport Entry);
    bool LinkChild(UObjectReference OwnerRef, UObjectReference ChildRef);
    /*
    bool ResizeInPlace(UObjectReference ObjRef, uint32_t newObjectSize);
    */
private:
    std::string UPKFileName;
    std::stringstream UPKFile;
    size_t UPKFileSize;
    bool AutosaveOn = false;
};

#endif // UPKUTILS_H
