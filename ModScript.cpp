#include "ModScript.h"

#include <cstring>

void ModScript::InitStreams(std::ostream& err, std::ostream& res)
{
    ErrorMessages = &err;
    ExecutionResults = &res;
}

void ModScript::SetExecutors()
{
    Executors.clear();
    Parser.ClearKeyNames();
    Parser.ClearSectionNames();
    /// Optional keys
    Executors.insert({"MOD_NAME", &ModScript::FormatModName});
    Parser.AddKeyName("MOD_NAME");
    Executors.insert({"AUTHOR", &ModScript::FormatAuthor});
    Parser.AddKeyName("AUTHOR");
    Executors.insert({"DESCRIPTION", &ModScript::FormatDescription});
    Parser.AddKeyName("DESCRIPTION");
    /// Package keys
    Executors.insert({"UPK_FILE", &ModScript::OpenPackage});
    Parser.AddKeyName("UPK_FILE");
    Executors.insert({"GUID", &ModScript::SetGUID});
    Parser.AddKeyName("GUID");
    /// Scope keys
    Executors.insert({"OFFSET", &ModScript::SetGlobalOffset});
    Parser.AddKeyName("OFFSET");
    Executors.insert({"OBJECT", &ModScript::SetObject});
    Parser.AddKeyName("OBJECT");
    Executors.insert({"NAME_ENTRY", &ModScript::SetNameEntry});
    Parser.AddKeyName("NAME_ENTRY");
    Executors.insert({"IMPORT_ENTRY", &ModScript::SetImportEntry});
    Parser.AddKeyName("IMPORT_ENTRY");
    Executors.insert({"EXPORT_ENTRY", &ModScript::SetExportEntry});
    Parser.AddKeyName("EXPORT_ENTRY");
    /// Relative offset
    Executors.insert({"REL_OFFSET", &ModScript::SetRelOffset});
    Parser.AddKeyName("REL_OFFSET");
    Executors.insert({"FIND_HEX", &ModScript::SetDataChunkOffset}); /// is scope-aware
    Parser.AddKeyName("FIND_HEX");
    /// Actual writing
    Executors.insert({"MODDED_HEX", &ModScript::WriteModdedHEX});   /// key - new style
    Parser.AddKeyName("MODDED_HEX");
    Executors.insert({"MODDED_FILE", &ModScript::WriteModdedFile});
    Parser.AddKeyName("MODDED_FILE");
    /*Executors.insert({"MODDED_SCRIPT", &ModScript::WriteModdedScript});
    Parser.AddKeyName("MODDED_SCRIPT");
    Executors.insert({"MODDED_FLAGS", &ModScript::WriteModdedFlags});
    Parser.AddKeyName("MODDED_FLAGS");*/
    Executors.insert({"RENAME", &ModScript::WriteRename});
    Parser.AddKeyName("RENAME");
    Executors.insert({"BYTE", &ModScript::WriteByteValue});
    Parser.AddKeyName("BYTE");
    Executors.insert({"FLOAT", &ModScript::WriteFloatValue});
    Parser.AddKeyName("FLOAT");
    Executors.insert({"INTEGER", &ModScript::WriteIntValue});
    Parser.AddKeyName("INTEGER");
    Executors.insert({"UNSIGNED", &ModScript::WriteUnsignedValue});
    Parser.AddKeyName("UNSIGNED");
    Executors.insert({"NAMEIDX", &ModScript::WriteNameIdx});
    Parser.AddKeyName("NAMEIDX");
    Executors.insert({"OBJIDX", &ModScript::WriteObjectIdx});
    Parser.AddKeyName("OBJIDX");
    /// uninstaller thing
    Executors.insert({"EXPAND_UNDO", &ModScript::WriteUndoMoveResize});
    Parser.AddKeyName("EXPAND_UNDO");
    /// section-style patching
    Executors.insert({"[MODDED_HEX]", &ModScript::WriteModdedHEX}); /// section - old style
    Parser.AddSectionName("[MODDED_HEX]");
    Executors.insert({"[/MODDED_HEX]", &ModScript::Sink});
    Parser.AddSectionName("[/MODDED_HEX]");
    Executors.insert({"[BEFORE_HEX]", &ModScript::SetDataChunkOffset}); /// is scope-aware
    Parser.AddSectionName("[BEFORE_HEX]");
    Executors.insert({"[/BEFORE_HEX]", &ModScript::Sink});
    Parser.AddSectionName("[/BEFORE_HEX]");
    Executors.insert({"[AFTER_HEX]", &ModScript::WriteModdedHEX});
    Parser.AddSectionName("[AFTER_HEX]");
    Executors.insert({"[/AFTER_HEX]", &ModScript::Sink});
    Parser.AddSectionName("[/AFTER_HEX]");
    /// deprecated keys
    Executors.insert({"FUNCTION", &ModScript::SetObject}); /// legacy support - OBJECT alias
    Parser.AddKeyName("FUNCTION");                         /// legacy support - OBJECT alias
    Executors.insert({"FUNCTION_FILE", &ModScript::WriteModdedFile}); /// legacy support - MODDED_FILE alias
    Parser.AddKeyName("FUNCTION_FILE");                               /// legacy support - MODDED_FILE alias
    Executors.insert({"NAMELIST_NAME", &ModScript::WriteRename});    /// legacy support - RENAME alias
    Parser.AddKeyName("NAMELIST_NAME");                                 /// legacy support - RENAME alias
    Executors.insert({"EXPAND_FUNCTION", &ModScript::WriteMoveExpandLegacy}); /// legacy support
    Parser.AddKeyName("EXPAND_FUNCTION");                               /// legacy support
}

bool ModScript::Parse(const char* filename)
{
    BackupScript.clear();
    UPKNames.clear();
    GUIDs.clear();
    if (Parser.OpenModFile(filename) == false)
    {
        *ErrorMessages << "Can't open " << filename << " (file does not exist, or bad, or not ASCII)!" << std::endl;
        return SetBad();
    }
    SetExecutors();
    Parser.SetCommentMarkers('{', '}', 0);
    /// begin parsing mod file
    ExecutionStack.clear();
    ResetScope();
    int idx = Parser.FindNext();
    if (idx == -1)
    {
        *ErrorMessages << "Bad/unknown mod file format!\n";
        return SetBad();
    }
    while (idx != -1)
    {
        ExecutionStack.push_back({Parser.GetName(), Parser.GetValue(), Executors[Parser.GetName()]});
        idx = Parser.FindNext();
    }
    return SetGood();
}

bool ModScript::ExecuteStack()
{
    if (IsGood() == false)
    {
        *ErrorMessages << "Script state is bad, can not execute!\n";
        return SetBad();
    }
    if (ExecutionStack.size() < 1)
    {
        *ErrorMessages << "Execution stack is empty!\n";
        return SetBad();
    }
    for (unsigned i = 0; i < ExecutionStack.size(); ++i)
    {
        bool result = (this->*ExecutionStack[i].Exec)(ExecutionStack[i].Param);
        if (result == false)
        {
            *ErrorMessages << "Execution stopped at #" << i << " command named "
                           << ExecutionStack[i].Name << ".\n";
            return SetBad();
        }
    }
    return SetGood();
}

bool ModScript::FormatModName(const std::string& Param)
{
    *ExecutionResults << "Installing mod: "
                      << Param << std::endl;
    return SetGood();
}

bool ModScript::FormatAuthor(const std::string& Param)
{
    *ExecutionResults << "by "
                      << Param << std::endl;
    return SetGood();
}

bool ModScript::FormatDescription(const std::string& Param)
{
    *ExecutionResults << "Mod description:\n"
                      << Param << std::endl;
    return SetGood();
}

void ModScript::AddUPKName(std::string upkname)
{
    for (unsigned i = 0; i < UPKNames.size(); ++i)
    {
        if (UPKNames[i] == upkname)
            return;
    }
    UPKNames.push_back(upkname);
}

bool ModScript::OpenPackage(const std::string& Param)
{
    std::string UPKFileName = GetStringValue(Param);
    *ExecutionResults << "Opening package ...\n";
    if (ScriptState.UPKName == UPKFileName)
    {
        *ExecutionResults << UPKFileName << " is already opened!\n";
        return SetGood();
    }
    ScriptState.UPKName = UPKFileName;
    std::string pathName = UPKPath + UPKFileName;
    if (ScriptState.Package.Read(pathName.c_str()) == false)
    {
        *ErrorMessages << "Error reading package: " << pathName << std::endl;
        UPKReadErrors err = ScriptState.Package.GetError();
        *ErrorMessages << FormatReadErrors(err);
        if (ScriptState.Package.IsCompressed())
            *ErrorMessages << "Compression flags:\n" << FormatCompressionFlags(ScriptState.Package.GetCompressionFlags());
        return SetBad();
    }
    AddUPKName(ScriptState.UPKName);
    ResetScope();
    *ExecutionResults << "Package file: " << UPKFileName;
    if (UPKPath.length() > 0)
    {
        *ExecutionResults <<  " (" << pathName << ")";
    }
    *ExecutionResults << std::endl;
    if (GUIDs.count(UPKFileName) > 0)
    {
        bool found = false;
        std::string GUID = FormatHEX(ScriptState.Package.GetGUID());
        std::multimap<std::string, std::string>::iterator it;
        for (it = GUIDs.equal_range(UPKFileName).first; it != GUIDs.equal_range(UPKFileName).second; ++it)
        {
            if ( (*it).second == GUID)
            {
                found = true;
                break;
            }
        }
        if (found == false)
        {
            *ErrorMessages << "Package GUID " << GUID << " does not match any of GUIDs specified for package " << UPKFileName << std::endl;
            return SetBad();
        }
    }
    if (BackupScript.count(ScriptState.UPKName) < 1)
    {
        BackupScript.insert({ScriptState.UPKName, std::string("")});
    }
    *ExecutionResults << "Package opened successfully!\n";
    return SetGood();
}

bool ModScript::SetGUID(const std::string& Param)
{
    std::string PackageName, GUID;
    std::string str = GetStringValue(Param);
    size_t pos = str.find(':');
    if (pos == std::string::npos)
    {
        *ErrorMessages << "Bad GUID key format!\n";
        return SetBad();
    }
    PackageName = str.substr(pos + 1);
    GUID = str.substr(0, pos);
    GUIDs.insert({PackageName, GUID});
    *ExecutionResults << "Added allowed GUID:\n";
    *ExecutionResults << "Package: " << PackageName << " GUID: " << GUID << std::endl;
    return SetGood();
}

bool ModScript::SetGlobalOffset(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    ResetScope();
    ScriptState.Scope = UPKScope::Package;
    ScriptState.Offset = GetUnsignedValue(Param);
    *ExecutionResults << "Global offset: " << FormatHEX(ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")" << std::endl;
    if (ScriptState.Package.CheckValidFileOffset(ScriptState.Offset) == false)
    {
        *ErrorMessages << "Invalid package offset!\n";
        return SetBad();
    }
    return SetGood();
}

bool ModScript::SetObject(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    ResetScope();
    ScriptState.Scope = UPKScope::Object;
    std::string str = GetStringValue(Param);
    size_t pos = str.find(":");
    if (pos != std::string::npos)
    {
        ScriptState.Behaviour = str.substr(pos + 1);
        str = str.substr(0, pos);
    }
    else
    {
        ScriptState.Behaviour = "KEEP";
    }
    std::string ObjName = str;
    *ExecutionResults << "Searching for object named " << ObjName << " ...\n";
    UObjectReference ObjRef = ScriptState.Package.FindObject(ObjName);
    if (ObjRef == 0)
    {
        *ErrorMessages << "Can't find object named " << ObjName << std::endl;
        return SetBad();
    }
    else if (ObjRef < 0)
    {
        *ErrorMessages << "Import objects have no serial data: " << ObjName << std::endl;
        return SetBad();
    }
    *ExecutionResults << "Object found!\n";
    ScriptState.ObjIdx = (uint32_t)ObjRef;
    ScriptState.Offset = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialOffset;
    ScriptState.RelOffset = 0;
    /*
    *ExecutionResults << "Scope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nObject: " << ObjName
                      << " (" << ScriptState.Behaviour << ")"
                      << "\nIndex: " << ScriptState.ObjIdx
                      << "\nOffset (absolute): " << FormatHEX(ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")" << std::endl;
    */
    return SetGood();
}

bool ModScript::SetNameEntry(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    ResetScope();
    ScriptState.Scope = UPKScope::Name;
    std::string ObjName = GetStringValue(Param);
    *ExecutionResults << "Searching for name " << ObjName << " ...\n";
    int idx = ScriptState.Package.FindName(ObjName);
    if (idx < 0)
    {
        *ErrorMessages << "Can't find NameTable name " << ObjName << std::endl;
        return SetBad();
    }
    *ExecutionResults << "Name found!\n";
    ScriptState.ObjIdx = idx;
    ScriptState.Offset = ScriptState.Package.GetNameEntry(ScriptState.ObjIdx).EntryOffset;
    ScriptState.RelOffset = 0;
    /*
    *ExecutionResults << "Scope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nName entry: " << ObjName
                      << "\nIndex: " << ScriptState.ObjIdx
                      << "\nOffset (absolute): " << FormatHEX(ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")" << std::endl;
    */
    return SetGood();
}

bool ModScript::SetImportEntry(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    ResetScope();
    ScriptState.Scope = UPKScope::Import;
    std::string ObjName = GetStringValue(Param);
    *ExecutionResults << "Searching for import table entry " << ObjName << " ...\n";
    UObjectReference ObjRef = ScriptState.Package.FindObject(ObjName, false);
    if (ObjRef == 0)
    {
        *ErrorMessages << "Can't find ImportTable entry named " << ObjName << std::endl;
        return SetBad();
    }
    else if (ObjRef > 0)
    {
        *ErrorMessages << ObjName << " is export object, not import object!" << std::endl;
        return SetBad();
    }
    *ExecutionResults << "Import table entry found!\n";
    ScriptState.ObjIdx = (uint32_t)(-ObjRef);
    ScriptState.Offset = ScriptState.Package.GetImportEntry(ScriptState.ObjIdx).EntryOffset;
    ScriptState.RelOffset = 0;
    /*
    *ExecutionResults << "Scope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nImport entry: " << ObjName
                      << "\nIndex: " << ScriptState.ObjIdx
                      << "\nOffset (absolute): " << FormatHEX(ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")" << std::endl;
    */
    return SetGood();
}

bool ModScript::SetExportEntry(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    ResetScope();
    ScriptState.Scope = UPKScope::Export;
    std::string ObjName = GetStringValue(Param);
    *ExecutionResults << "Searching for export table entry " << ObjName << " ...\n";
    UObjectReference ObjRef = ScriptState.Package.FindObject(ObjName);
    if (ObjRef == 0)
    {
        *ErrorMessages << "Can't find ExportTable entry named " << ObjName << std::endl;
        return SetBad();
    }
    *ExecutionResults << "Export table entry found!\n";
    ScriptState.ObjIdx = (uint32_t)ObjRef;
    ScriptState.Offset = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).EntryOffset;
    ScriptState.RelOffset = 0;
    /*
    *ExecutionResults << "Scope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nExport entry: " << ObjName
                      << "\nIndex: " << ScriptState.ObjIdx
                      << "\nOffset (absolute): " << FormatHEX(ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")" << std::endl;
    */
    return SetGood();
}

bool ModScript::SetRelOffset(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    ScriptState.RelOffset = GetUnsignedValue(Param);
    *ExecutionResults << "Relative offset: " << FormatHEX(ScriptState.RelOffset)
                      << " (" << ScriptState.RelOffset << ")" << std::endl;
    if (ScriptState.Package.CheckValidOffset(ScriptState.Offset + ScriptState.RelOffset, ScriptState.Scope, ScriptState.ObjIdx) == false)
    {
        *ErrorMessages << "Invalid relative offset!\n";
        return SetBad();
    }
    return SetGood();
}

bool ModScript::WriteModdedHEX(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::vector<char> DataChunk = GetDataChunk(Param);
    if (DataChunk.size() < 1)
    {
        *ErrorMessages << "Invalid/empty data!\n";
        return SetBad();
    }
    bool wasMoved = false;
    if (ScriptState.Scope == UPKScope::Object)
    {
        if (CheckMoveResize(DataChunk.size(), wasMoved) == false)
            return SetBad();
    }
    return WriteBinaryData(DataChunk, wasMoved);
}

bool ModScript::MoveResize(size_t ObjSize)
{
    *ExecutionResults << "Moving/resizing object.\nObject size: " << ObjSize << std::endl;
    if (ScriptState.Package.MoveResizeObject(ScriptState.ObjIdx, ObjSize) == false)
    {
        *ErrorMessages << "Error moving/resizing object!\n";
        return SetBad();
    }
    *ExecutionResults << "Object moved/resized successfully.\n";
    ScriptState.Offset = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialOffset;
    /// backup info
    std::ostringstream ss;
    ss << "EXPAND_UNDO=" << ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName << "\n\n";
    ss << BackupScript[ScriptState.UPKName];
    BackupScript[ScriptState.UPKName] = ss.str();
    return SetGood();
}

bool ModScript::WriteBinaryData(const std::vector<char>& DataChunk, bool wasMoved)
{
    std::vector<char> BackupData;
    *ExecutionResults << "Writing data chunk of size " << FormatHEX(DataChunk.size())
                      << " (" << DataChunk.size() << ") at"
                      << "\nScope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nOffset (absolute): " << FormatHEX(ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")"
                      << "\nOffset (scope-relative): " << FormatHEX(ScriptState.RelOffset)
                      << " (" << ScriptState.RelOffset << ")\n";
    if (!ScriptState.Package.WriteData(ScriptState.Offset + ScriptState.RelOffset, DataChunk, &BackupData))
    {
        *ErrorMessages << "Write error!\n";
        return SetBad();
    }
    *ExecutionResults << "Write successful!" << std::endl;
    if (wasMoved == false)
    {
        /// backup info
        std::ostringstream ss;
        ss << "OFFSET=" << (ScriptState.Offset + ScriptState.RelOffset) << "\n\n"
           << "[MODDED_HEX]\n" << MakeTextBlock(BackupData.data(), BackupData.size()) << "\n\n";
        ss << BackupScript[ScriptState.UPKName];
        BackupScript[ScriptState.UPKName] = ss.str();
    }
    return SetGood();
}

bool ModScript::WriteUndoMoveResize(const std::string& Param)
{
    if (SetObject(Param) == false)
        return SetBad();
    *ExecutionResults << "Restoring object ...\n";
    if (ScriptState.Package.UndoMoveResizeObject(ScriptState.ObjIdx) == false)
    {
        *ErrorMessages << "Error restoring object " << ScriptState.ObjIdx << std::endl;
        return SetBad();
    }
    *ExecutionResults << "Move/expand undo successful!\nScope set to current object.\n";
    ScriptState.Offset = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialOffset;
    ScriptState.RelOffset = 0;
    /*
    *ExecutionResults << "Scope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nObject: " << ScriptState.ObjIdx
                      << " (" << ScriptState.Behaviour << ")"
                      << "\nIndex: " << ScriptState.ObjIdx
                      << "\nOffset (absolute): " << FormatHEX(ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")" << std::endl;
    */
    return SetGood();
}

bool ModScript::WriteModdedFile(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::string str = GetStringValue(Param);
    std::string FilePath = str;
    std::string Spec = "";
    size_t pos = str.find(':');
    if (pos != std::string::npos)
    {
        FilePath = str.substr(0, pos);
        Spec = str.substr(pos);
    }
    *ExecutionResults << "Reading binary data from file: " << FilePath << " ...\n";
    std::ifstream BinFile(FilePath.c_str(), std::ios::binary);
    if (!BinFile)
    {
        *ErrorMessages << "Can't open binary file: " << FilePath << std::endl;
        return SetBad();
    }
    BinFile.seekg(0, std::ios::end);
    size_t BinSize = BinFile.tellg();
    BinFile.seekg(0);
    std::vector<char> DataChunk(BinSize);
    BinFile.read(DataChunk.data(), DataChunk.size());
    if (DataChunk.size() < 1)
    {
        *ErrorMessages << "Invalid/empty binary file!\n";
        return SetBad();
    }
    std::string FileName = GetFilename(FilePath);
    *ExecutionResults << "Attempting to set scope by file name: " << FileName << " ...\n";
    if (FileName.find(".NameEntry") != std::string::npos)
    {
        FileName = FileName.substr(0, FileName.find(".NameEntry"));
        if (SetNameEntry(FileName) == false)
            return SetBad();
    }
    else if (FileName.find(".ImportEntry") != std::string::npos)
    {
        FileName = FileName.substr(0, FileName.find(".ImportEntry"));
        if (SetImportEntry(FileName) == false)
            return SetBad();
    }
    else if (FileName.find(".ExportEntry") != std::string::npos)
    {
        FileName = FileName.substr(0, FileName.find(".ExportEntry"));
        if (SetExportEntry(FileName) == false)
            return SetBad();
    }
    else
    {
        FileName = FileName.substr(0, FileName.find_last_of('.'));
        UObjectReference ObjRef = ScriptState.Package.FindObject(FileName);
        if (ObjRef > 0)
        {
            if (SetObject(FileName + Spec) == false)
                return SetBad();
        }
        else
        {
            *ExecutionResults << "File name does not make any sense, keeping current scope!\n";
        }
    }
    bool wasMoved = false;
    if (CheckMoveResize(DataChunk.size(), wasMoved) == false)
        return SetBad();
    return WriteBinaryData(DataChunk, wasMoved);
}

bool ModScript::CheckMoveResize(size_t DataSize, bool& wasMoved)
{
    if (ScriptState.Package.CheckValidOffset(ScriptState.Offset + ScriptState.RelOffset + DataSize - 1, ScriptState.Scope, ScriptState.ObjIdx) == false)
    {
        if (ScriptState.Behaviour == "MOVE" || ScriptState.Behaviour == "AUTO")
        {
            wasMoved = MoveResize(ScriptState.RelOffset + DataSize);
            if (wasMoved == false)
                return SetBad();
        }
        else
        {
            *ErrorMessages << "Data chunk too large for current scope!\n";
            return SetBad();
        }
    }
    else if (ScriptState.Behaviour == "MOVE")
    {
        size_t ObjSize = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize;
        if (ScriptState.RelOffset + DataSize > ObjSize)
        {
            ObjSize = ScriptState.RelOffset + DataSize;
        }
        wasMoved = MoveResize(ObjSize);
        if (wasMoved == false)
            return SetBad();
    }
    return SetGood();
}

bool ModScript::WriteByteValue(const std::string& Param)
{
    unsigned IntVal = GetUnsignedValue(Param);
    if (IntVal > 0x255)
    {
        *ErrorMessages << "Incorrect byte value: " << IntVal << std::endl;
        return SetBad();
    }
    uint8_t ByteVal = (uint8_t)IntVal;
    std::vector<char> dataChunk(1);
    memcpy(dataChunk.data(), reinterpret_cast<char*>(&ByteVal), 1);
    *ExecutionResults << "Byte value: " << (int)ByteVal << " (data chunk: " << FormatHEX(dataChunk) << ")" << std::endl;
    return WriteModdedHEX(MakeTextBlock(dataChunk.data(), dataChunk.size()));
}

bool ModScript::WriteFloatValue(const std::string& Param)
{
    float FloatVal = GetFloatValue(Param);
    std::vector<char> dataChunk(4);
    memcpy(dataChunk.data(), reinterpret_cast<char*>(&FloatVal), 4);
    *ExecutionResults << "Float value: " << FloatVal << " (data chunk: " << FormatHEX(dataChunk) << ")" << std::endl;
    return WriteModdedHEX(MakeTextBlock(dataChunk.data(), dataChunk.size()));
}

bool ModScript::WriteIntValue(const std::string& Param)
{
    int IntVal = GetIntValue(Param);
    std::vector<char> dataChunk(4);
    memcpy(dataChunk.data(), reinterpret_cast<char*>(&IntVal), 4);
    *ExecutionResults << "Int value: " << IntVal << " (data chunk: " << FormatHEX(dataChunk) << ")" << std::endl;
    return WriteModdedHEX(MakeTextBlock(dataChunk.data(), dataChunk.size()));
}

bool ModScript::WriteUnsignedValue(const std::string& Param)
{
    unsigned UVal = GetUnsignedValue(Param);
    std::vector<char> dataChunk(4);
    memcpy(dataChunk.data(), reinterpret_cast<char*>(&UVal), 4);
    *ExecutionResults << "Unsigned value: " << UVal << " (data chunk: " << FormatHEX(dataChunk) << ")" << std::endl;
    return WriteModdedHEX(MakeTextBlock(dataChunk.data(), dataChunk.size()));
}

bool ModScript::WriteNameIdx(const std::string& Param)
{
    std::string str = GetStringValue(Param);
    std::string Name = str;
    int num = 0;
    size_t pos = str.find('_');
    if (pos != std::string::npos)
    {
        Name = str.substr(0, pos);
        num = 1 + GetIntValue(str.substr(pos + 1));
    }
    *ExecutionResults << "Searching for name " << str << " ...\n";
    int idx = ScriptState.Package.FindName(Name);
    if (idx < 0)
    {
        *ErrorMessages << "Incorrect name: " << str << std::endl;
        return SetBad();
    }
    *ExecutionResults << "Name found!\n";
    UNameIndex NameIdx;
    NameIdx.NameTableIdx = idx;
    NameIdx.Numeric = num;
    std::vector<char> dataChunk(8);
    memcpy(dataChunk.data(), reinterpret_cast<char*>(&NameIdx), 8);
    *ExecutionResults << "UNameIndex: " << str << " -> " << FormatHEX(NameIdx) << std::endl;
    return WriteModdedHEX(MakeTextBlock(dataChunk.data(), dataChunk.size()));
}

bool ModScript::WriteObjectIdx(const std::string& Param)
{
    std::string FullName = GetStringValue(Param);
    *ExecutionResults << "Searching for object named " << FullName << " ...\n";
    UObjectReference ObjRef = ScriptState.Package.FindObject(FullName, false);
    *ExecutionResults << "Object found!\n";
    if (ObjRef == 0)
    {
        *ErrorMessages << "Can't find object named " << FullName << std::endl;
        return SetBad();
    }
    std::vector<char> dataChunk(4);
    memcpy(dataChunk.data(), reinterpret_cast<char*>(&ObjRef), 4);
    *ExecutionResults << "UObjectReference: " << FullName << " -> " << FormatHEX((uint32_t)ObjRef) << std::endl;
    return WriteModdedHEX(MakeTextBlock(dataChunk.data(), dataChunk.size()));
}

bool ModScript::WriteRename(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::string str = GetStringValue(Param);
    size_t pos = str.find(":");
    if (pos == std::string::npos)
    {
        *ErrorMessages << "Incorrect rename entry format: " << str << std::endl;
        return SetBad();
    }
    std::string ObjName = str.substr(0, pos);
    std::string NewName = str.substr(pos + 1);
    *ExecutionResults << "Renaming " << ObjName << " to " << NewName << " ...\n";
    if (NewName.length() != ObjName.length())
    {
        *ErrorMessages << "New name must have the same length as old name!\n";
        return SetBad();
    }
    if (SetNameEntry(ObjName) == false)
    {
        if (SetNameEntry(NewName) == false)
        {
            *ErrorMessages << "Can't find any of the names specified!\n";
            return SetBad();
        }
        else
        {
            *ExecutionResults << "Name entry already has name " << NewName << std::endl;
            return SetGood();
        }
    }
    if (ScriptState.Package.WriteNameTableName(ScriptState.ObjIdx, NewName) == false)
    {
        *ErrorMessages << "Error writing new name!\n";
        return SetBad();
    }
    *ExecutionResults << "Renamed successfully!\n";
    /// backup info
    std::ostringstream ss;
    ss << "RENAME=" << NewName << ":" << ObjName << "\n\n";
    ss << BackupScript[ScriptState.UPKName];
    BackupScript[ScriptState.UPKName] = ss.str();
    return SetGood();
}

bool ModScript::SetDataChunkOffset(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::vector<char> DataChunk = GetDataChunk(Param);
    if (DataChunk.size() < 1)
    {
        *ErrorMessages << "Invalid/empty data!\n";
        return SetBad();
    }
    *ExecutionResults << "Searching for specified data chunk ...\n";
    if (ScriptState.Scope == UPKScope::Package)
    {
        ScriptState.Offset = ScriptState.Package.FindDataChunk(DataChunk);
        *ExecutionResults << "Data found!\nGlobal offset: " << FormatHEX(ScriptState.Offset)
                          << " (" << ScriptState.Offset << ")" << std::endl;
    }
    else
    {
        ScriptState.RelOffset = ScriptState.Package.FindDataChunk(DataChunk, ScriptState.Offset);
        *ExecutionResults << "Data found!\nRelative offset: " << FormatHEX(ScriptState.RelOffset)
                          << " (" << ScriptState.RelOffset << ")" << std::endl;
    }
    if (ScriptState.Package.CheckValidFileOffset(ScriptState.Offset + ScriptState.RelOffset) == false)
    {
        *ErrorMessages << "Invalid package offset!\n";
        return SetBad();
    }
    return SetGood();
}

bool ModScript::WriteMoveExpandLegacy(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::string ObjName, str = GetStringValue(Param);
    size_t NewSize = 0;
    size_t pos = str.find(":");
    if (pos != std::string::npos)
    {
        ObjName = str.substr(0, pos);
        NewSize = GetUnsignedValue(str.substr(pos + 1));
    }
    else
    {
        ObjName = str;
        NewSize = 0;
    }
    if (SetObject(ObjName) == false)
        return SetBad();
    *ExecutionResults << "Moving/expanding function ...\n";
    if (ScriptState.Package.MoveExportData(ScriptState.ObjIdx, NewSize) == false)
    {
        *ErrorMessages << "Error expanding function!\n";
        return SetBad();
    }
    *ExecutionResults << "Function moved/expanded successfully!\n";
    /// backup info
    std::ostringstream ss;
    ss << "EXPAND_UNDO=" << ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName << "\n\n";
    ss << BackupScript[ScriptState.UPKName];
    BackupScript[ScriptState.UPKName] = ss.str();
    return SetGood();
}

bool ModScript::Sink(const std::string& Param)
{
    return SetGood();
}

std::string ModScript::GetBackupScript()
{
    std::ostringstream ss;
    for (unsigned i = 0; i < UPKNames.size(); ++i)
    {
        ss << "UPK_FILE=" << UPKNames[i] << "\n\n"
           << BackupScript[UPKNames[i]] << "\n\n";
    }
    return ss.str();
}
