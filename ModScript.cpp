#include "ModScript.h"

#include <cstring>
#include <cctype>

std::string FormatUPKScope(UPKScope scope)
{
    switch (scope)
    {
    case UPKScope::Package:
        return "Package";
    case UPKScope::Name:
        return "Name Table";
    case UPKScope::Import:
        return "Import Table";
    case UPKScope::Export:
        return "Export Table";
    case UPKScope::Object:
        return "Object Data";
    default:
        return "";
    }
}

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
    /// Scripting
    Executors.insert({"ALIAS", &ModScript::AddAlias});
    Parser.AddKeyName("ALIAS");
    /// Relative offset
    Executors.insert({"REL_OFFSET", &ModScript::SetRelOffset});
    Parser.AddKeyName("REL_OFFSET");
    Executors.insert({"FIND_HEX", &ModScript::SetDataChunkOffset}); /// is scope-aware
    Parser.AddKeyName("FIND_HEX");
    Executors.insert({"FIND_CODE", &ModScript::SetCodeOffset}); /// is scope-aware
    Parser.AddKeyName("FIND_CODE");
    /// Actual writing
    Executors.insert({"MODDED_HEX", &ModScript::WriteModdedHEX});   /// key - new style
    Parser.AddKeyName("MODDED_HEX");
    Executors.insert({"MODDED_CODE", &ModScript::WriteModdedCode});
    Parser.AddKeyName("MODDED_CODE");
    Executors.insert({"REPLACEMENT_CODE", &ModScript::WriteReplacementCode});
    Parser.AddKeyName("REPLACEMENT_CODE");
    Executors.insert({"INSERT_CODE", &ModScript::WriteInsertCode});
    Parser.AddKeyName("INSERT_CODE");
    Executors.insert({"MODDED_FILE", &ModScript::WriteModdedFile});
    Parser.AddKeyName("MODDED_FILE");
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
    /// add new table entry
    Executors.insert({"ADD_NAME_ENTRY", &ModScript::WriteAddNameEntry});
    Parser.AddKeyName("ADD_NAME_ENTRY");
    Executors.insert({"ADD_IMPORT_ENTRY", &ModScript::WriteAddImportEntry});
    Parser.AddKeyName("ADD_IMPORT_ENTRY");
    Executors.insert({"ADD_EXPORT_ENTRY", &ModScript::WriteAddExportEntry});
    Parser.AddKeyName("ADD_EXPORT_ENTRY");
    /// section-style patching
    Executors.insert({"[FIND_HEX]", &ModScript::SetDataChunkOffset}); /// is scope-aware
    Parser.AddSectionName("[FIND_HEX]");
    Executors.insert({"[/FIND_HEX]", &ModScript::Sink});
    Parser.AddSectionName("[/FIND_HEX]");
    Executors.insert({"[FIND_CODE]", &ModScript::SetCodeOffset}); /// is scope-aware
    Parser.AddSectionName("[FIND_CODE]");
    Executors.insert({"[/FIND_CODE]", &ModScript::Sink});
    Parser.AddSectionName("[/FIND_CODE]");
    Executors.insert({"[MODDED_HEX]", &ModScript::WriteModdedHEX}); /// section - old style
    Parser.AddSectionName("[MODDED_HEX]");
    Executors.insert({"[/MODDED_HEX]", &ModScript::Sink});
    Parser.AddSectionName("[/MODDED_HEX]");
    Executors.insert({"[MODDED_CODE]", &ModScript::WriteModdedCode});
    Parser.AddSectionName("[MODDED_CODE]");
    Executors.insert({"[/MODDED_CODE]", &ModScript::Sink});
    Parser.AddSectionName("[/MODDED_CODE]");
    Executors.insert({"[REPLACEMENT_CODE]", &ModScript::WriteReplacementCode});
    Parser.AddSectionName("[REPLACEMENT_CODE]");
    Executors.insert({"[/REPLACEMENT_CODE]", &ModScript::Sink});
    Parser.AddSectionName("[/REPLACEMENT_CODE]");
    Executors.insert({"[INSERT_CODE]", &ModScript::WriteInsertCode});
    Parser.AddSectionName("[INSERT_CODE]");
    Executors.insert({"[/INSERT_CODE]", &ModScript::Sink});
    Parser.AddSectionName("[/INSERT_CODE]");
    Executors.insert({"[ADD_NAME_ENTRY]", &ModScript::WriteAddNameEntry});
    Parser.AddSectionName("[ADD_NAME_ENTRY]");
    Executors.insert({"[/ADD_NAME_ENTRY]", &ModScript::Sink});
    Parser.AddSectionName("[/ADD_NAME_ENTRY]");
    Executors.insert({"[ADD_IMPORT_ENTRY]", &ModScript::WriteAddImportEntry});
    Parser.AddSectionName("[ADD_IMPORT_ENTRY]");
    Executors.insert({"[/ADD_IMPORT_ENTRY]", &ModScript::Sink});
    Parser.AddSectionName("[/ADD_IMPORT_ENTRY]");
    Executors.insert({"[ADD_EXPORT_ENTRY]", &ModScript::WriteAddExportEntry});
    Parser.AddSectionName("[ADD_EXPORT_ENTRY]");
    Executors.insert({"[/ADD_EXPORT_ENTRY]", &ModScript::Sink});
    Parser.AddSectionName("[/ADD_EXPORT_ENTRY]");
    /// before-after style patching
    Executors.insert({"[BEFORE_HEX]", &ModScript::SetBeforeHEXOffset}); /// is scope-aware
    Parser.AddSectionName("[BEFORE_HEX]");
    Executors.insert({"[/BEFORE_HEX]", &ModScript::Sink});
    Parser.AddSectionName("[/BEFORE_HEX]");
    Executors.insert({"[BEFORE_CODE]", &ModScript::SetBeforeCodeOffset}); /// is scope-aware
    Parser.AddSectionName("[BEFORE_CODE]");
    Executors.insert({"[/BEFORE_CODE]", &ModScript::Sink});
    Parser.AddSectionName("[/BEFORE_CODE]");
    Executors.insert({"[AFTER_HEX]", &ModScript::WriteAfterHEX});
    Parser.AddSectionName("[AFTER_HEX]");
    Executors.insert({"[/AFTER_HEX]", &ModScript::Sink});
    Parser.AddSectionName("[/AFTER_HEX]");
    Executors.insert({"[AFTER_CODE]", &ModScript::WriteAfterCode});
    Parser.AddSectionName("[AFTER_CODE]");
    Executors.insert({"[/AFTER_CODE]", &ModScript::Sink});
    Parser.AddSectionName("[/AFTER_CODE]");
    /// deprecated keys
    Executors.insert({"FUNCTION", &ModScript::SetObject}); /// legacy support - OBJECT alias
    Parser.AddKeyName("FUNCTION");                         /// legacy support - OBJECT alias
    Executors.insert({"FUNCTION_FILE", &ModScript::WriteModdedFile}); /// legacy support - MODDED_FILE alias
    Parser.AddKeyName("FUNCTION_FILE");                               /// legacy support - MODDED_FILE alias
    Executors.insert({"NAMELIST_NAME", &ModScript::WriteRename});    /// legacy support - RENAME alias
    Parser.AddKeyName("NAMELIST_NAME");                                 /// legacy support - RENAME alias
    /// relatively safe
    Executors.insert({"EXPAND_FUNCTION", &ModScript::WriteMoveExpandLegacy});
    Parser.AddKeyName("EXPAND_FUNCTION");
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
    size_t pos = SplitAt(':', str, GUID, PackageName);
    /*size_t pos = str.find(':');*/
    if (pos == std::string::npos)
    {
        *ErrorMessages << "Bad GUID key format!\n";
        return SetBad();
    }
    /*PackageName = str.substr(pos + 1);
    GUID = str.substr(0, pos);*/
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
    ScriptState.MaxOffset = ScriptState.Package.GetFileSize() - 1;
    *ExecutionResults << "Global offset: " << FormatHEX((uint32_t)ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")" << std::endl;
    if (ScriptState.Package.CheckValidFileOffset(ScriptState.Offset) == false)
    {
        *ErrorMessages << "Invalid package offset!\n";
        return SetBad();
    }
    return SetGood();
}

bool ModScript::CheckBehavior()
{
    if (ScriptState.Behavior == "")
    {
        ScriptState.Behavior = "KEEP";
        return SetGood();
    }
    else if (ScriptState.Behavior == "KEEP")
    {
        return SetGood();
    }
    else if (ScriptState.Behavior == "MOVE")
    {
        return SetGood();
    }
    else if (ScriptState.Behavior == "AUTO")
    {
        return SetGood();
    }
    return SetBad();
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
    std::string str = GetStringValue(Param), ObjName;
    SplitAt(':', str, ObjName, ScriptState.Behavior);
    if (!CheckBehavior())
    {
        *ErrorMessages << "Bad behavior modifier: " << ScriptState.Behavior << std::endl;
        return SetBad();
    }
    /*size_t pos = str.find(":");
    if (pos != std::string::npos)
    {
        ScriptState.Behavior = str.substr(pos + 1);
        str = str.substr(0, pos);
    }
    else
    {
        ScriptState.Behavior = "KEEP";
    }
    std::string ObjName = str;*/
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
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
    /*
    *ExecutionResults << "Scope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nObject: " << ObjName
                      << " (" << ScriptState.Behavior << ")"
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
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetNameEntry(ScriptState.ObjIdx).EntrySize - 1;
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
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetImportEntry(ScriptState.ObjIdx).EntrySize - 1;
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
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).EntrySize - 1;
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
    *ExecutionResults << "Relative offset: " << FormatHEX((uint32_t)ScriptState.RelOffset)
                      << " (" << ScriptState.RelOffset << ")" << std::endl;
    if (!IsInsideScope())
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
    if (ScriptState.Scope == UPKScope::Object)
    {
        if (CheckMoveResize(DataChunk.size()) == false)
            return SetBad();
    }
    return WriteBinaryData(DataChunk);
}

bool ModScript::WriteModdedCode(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    return WriteModdedHEX(ParseScript(Param));
}

bool ModScript::WriteReplacementCode(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    if (ScriptState.Scope != UPKScope::Object)
    {
        *ErrorMessages << "Replacement code only works for export object data!\n";
        return SetBad();
    }
    size_t ScriptSize = ScriptState.Package.GetScriptSize(ScriptState.ObjIdx);
    if (ScriptSize == 0)
    {
        *ErrorMessages << "Object has no script to replace!\n";
        return SetBad();
    }
    unsigned ScriptMemorySize = 0;
    std::vector<char> DataChunk = GetDataChunk(ParseScript(Param, &ScriptMemorySize));
    if (DataChunk.size() < 1)
    {
        *ErrorMessages << "Invalid/empty data!\n";
        return SetBad();
    }
    size_t ScriptRelOffset = ScriptState.Package.GetScriptRelOffset(ScriptState.ObjIdx);
    ScriptState.RelOffset = ScriptRelOffset;
    /// checking size
    if (ScriptSize != DataChunk.size())
    {
        if (ScriptState.Behavior == "KEEP")
        {
            *ErrorMessages << "Replacement code does not fit current scope!\n";
            return SetBad();
        }
        int ObjSize = (int)ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize + (int)DataChunk.size() - (int)ScriptSize;
        if (!MoveResizeAtRelOffset(ObjSize))
        {
            *ErrorMessages << "Error moving/resizing object!\n";
            return SetBad();
        }
    }
    /// resetting max offset just in case to clear any possible previous FIND_HEX calls
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
    ScriptSize = DataChunk.size();
    *ExecutionResults << "New script memory size: " << ScriptMemorySize << " (" << FormatHEX(ScriptMemorySize) << ")\n";
    *ExecutionResults << "New script serial size: " << ScriptSize << " (" << FormatHEX((uint32_t)ScriptSize) << ")\n";
    /// writing sizes
    std::vector<char> SizesDataChunk(8);
    memcpy(SizesDataChunk.data(), reinterpret_cast<char*>(&ScriptMemorySize), 4);
    memcpy(SizesDataChunk.data() + 4, reinterpret_cast<char*>(&ScriptSize), 4);
    ScriptState.RelOffset = ScriptRelOffset - 8;
    if (!WriteBinaryData(SizesDataChunk))
    {
        *ErrorMessages << "Error writing new memory/file sizes!\n";
        return SetBad();
    }
    /// writing script
    ScriptState.RelOffset = ScriptRelOffset;
    return WriteBinaryData(DataChunk);
}

bool ModScript::WriteInsertCode(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    if (ScriptState.Scope != UPKScope::Object)
    {
        *ErrorMessages << "Inserting code only works for export object data!\n";
        return SetBad();
    }
    if (ScriptState.Behavior == "KEEP")
    {
        *ErrorMessages << "Can't insert new code as current behavior is set to \"KEEP\"!\n";
        return SetBad();
    }
    std::vector<char> DataChunk = GetDataChunk(ParseScript(Param));
    if (DataChunk.size() < 1)
    {
        *ErrorMessages << "Invalid/empty data!\n";
        return SetBad();
    }
    int ObjSize = (int)ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize + (int)DataChunk.size();
    if (!MoveResizeAtRelOffset(ObjSize))
    {
        *ErrorMessages << "Error moving/resizing object!\n";
        return SetBad();
    }
    return WriteBinaryData(DataChunk);
}

bool ModScript::CheckMoveResize(size_t DataSize)
{
    if ((!IsInsideScope(DataSize) && ScriptState.Behavior == "AUTO") || ScriptState.Behavior == "MOVE")
    {
        size_t ObjSize = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize + GetDiff(DataSize);
        return MoveResizeAtRelOffset(ObjSize);
    }
    else if (!IsInsideScope(DataSize))
    {
        *ErrorMessages << "Data chunk too large for current scope!\n";
        return SetBad();
    }
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
    return SetGood();
}

bool ModScript::MoveResizeAtRelOffset(int ObjSize)
{
    *ExecutionResults << "Moving/resizing object.\nNew object size: " << ObjSize << std::endl;
    if (ScriptState.Package.MoveResizeObject(ScriptState.ObjIdx, ObjSize, ScriptState.RelOffset) == false)
    {
        *ErrorMessages << "Error moving/resizing object!\n";
        return SetBad();
    }
    *ExecutionResults << "Object moved/resized successfully.\n";
    ScriptState.Offset = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialOffset;
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
    /// backup info
    std::ostringstream ss;
    ss << "EXPAND_UNDO=" << ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName << "\n\n";
    ss << BackupScript[ScriptState.UPKName];
    BackupScript[ScriptState.UPKName] = ss.str();
    return SetGood();
}

bool ModScript::WriteBinaryData(const std::vector<char>& DataChunk)
{
    if (!IsInsideScope(DataChunk.size()))
    {
        *ErrorMessages << "Data chunk too large for current scope!\n";
        return SetBad();
    }
    std::vector<char> BackupData;
    *ExecutionResults << "Writing data chunk of size " << FormatHEX((uint32_t)DataChunk.size())
                      << " (" << DataChunk.size() << ") at"
                      << "\nScope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nOffset (absolute): " << FormatHEX((uint32_t)ScriptState.Offset)
                      << " (" << ScriptState.Offset << ")"
                      << "\nOffset (scope-relative): " << FormatHEX((uint32_t)ScriptState.RelOffset)
                      << " (" << ScriptState.RelOffset << ")\n";
    if (!ScriptState.Package.WriteData(ScriptState.Offset + ScriptState.RelOffset, DataChunk, &BackupData))
    {
        *ErrorMessages << "Write error!\n";
        return SetBad();
    }
    *ExecutionResults << "Write successful!" << std::endl;
    /// backup info
    std::ostringstream ss;
    ss << "OFFSET=" << (ScriptState.Offset + ScriptState.RelOffset) << "\n\n"
       << "[MODDED_HEX]\n" << MakeTextBlock(BackupData.data(), BackupData.size()) << "\n\n";
    ss << BackupScript[ScriptState.UPKName];
    BackupScript[ScriptState.UPKName] = ss.str();
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
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
    /*
    *ExecutionResults << "Scope: " << FormatUPKScope(ScriptState.Scope)
                      << "\nObject: " << ScriptState.ObjIdx
                      << " (" << ScriptState.Behavior << ")"
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
    SplitAt(':', str, FilePath, Spec);
    /*size_t pos = str.find(':');
    if (pos != std::string::npos)
    {
        FilePath = str.substr(0, pos);
        Spec = str.substr(pos);
    }*/
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
        if (DataChunk.size() != ScriptState.Package.GetNameEntry(ScriptState.ObjIdx).EntrySize)
        {
            *ErrorMessages << "File size is not equal to entry size!\n";
            return SetBad();
        }
    }
    else if (FileName.find(".ImportEntry") != std::string::npos)
    {
        FileName = FileName.substr(0, FileName.find(".ImportEntry"));
        if (SetImportEntry(FileName) == false)
            return SetBad();
        if (DataChunk.size() != ScriptState.Package.GetImportEntry(ScriptState.ObjIdx).EntrySize)
        {
            *ErrorMessages << "File size is not equal to entry size!\n";
            return SetBad();
        }
    }
    else if (FileName.find(".ExportEntry") != std::string::npos)
    {
        FileName = FileName.substr(0, FileName.find(".ExportEntry"));
        if (SetExportEntry(FileName) == false)
            return SetBad();
        if (DataChunk.size() != ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).EntrySize)
        {
            *ErrorMessages << "File size is not equal to entry size!\n";
            return SetBad();
        }
    }
    else
    {
        FileName = FileName.substr(0, FileName.find_last_of('.'));
        UObjectReference ObjRef = ScriptState.Package.FindObject(FileName);
        if (ObjRef > 0)
        {
            //if (SetObject(FileName + Spec) == false)
            if (SetObject(FileName + ":" + Spec) == false)
                return SetBad();
            if (DataChunk.size() != ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize)
            {
                if (ScriptState.Behavior == "AUTO" || ScriptState.Behavior == "MOVE")
                {
                    *ExecutionResults << "Moving/resizing object.\nNew object size: " << DataChunk.size() << std::endl;
                    if (ScriptState.Package.MoveResizeObject(ScriptState.ObjIdx, DataChunk.size()) == false)
                    {
                        *ErrorMessages << "Error moving/resizing object!\n";
                        return SetBad();
                    }
                    *ExecutionResults << "Object moved/resized successfully.\n";
                    ScriptState.Offset = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialOffset;
                    ScriptState.RelOffset = 0;
                    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
                    /// backup info
                    std::ostringstream ss;
                    ss << "EXPAND_UNDO=" << ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName << "\n\n";
                    ss << BackupScript[ScriptState.UPKName];
                    BackupScript[ScriptState.UPKName] = ss.str();
                }
                else
                {
                    *ErrorMessages << "File size is not equal to object serial size!\n";
                    return SetBad();
                }
            }
        }
        else
        {
            *ExecutionResults << "File name does not make any sense!\n";
            return SetBad();
        }
    }
    return WriteBinaryData(DataChunk);
}

bool ModScript::WriteByteValue(const std::string& Param)
{
    unsigned UVal = GetUnsignedValue(Param);
    if (UVal > 0xFF)
    {
        *ErrorMessages << "Incorrect byte value: " << UVal << std::endl;
        return SetBad();
    }
    uint8_t ByteVal = (uint8_t)UVal;
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
    size_t pos = str.rfind('_');
    if (pos != std::string::npos && isdigit(str[pos + 1]))
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
    std::string str = GetStringValue(Param), ObjName, NewName;
    size_t pos = SplitAt(':', str, ObjName, NewName);
    /*size_t pos = str.find(":");*/
    if (pos == std::string::npos)
    {
        *ErrorMessages << "Incorrect rename entry format: " << str << std::endl;
        return SetBad();
    }
    /*std::string ObjName = str.substr(0, pos);
    std::string NewName = str.substr(pos + 1);*/
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

bool ModScript::SetDataOffset(const std::string& Param, bool isEnd, bool isBeforeData)
{
    if (isEnd && isBeforeData)
    {
        *ErrorMessages << "Internal error: can't use isEnd and isBeforeData together!\n";
        return SetBad();
    }
    std::vector<char> DataChunk = GetDataChunk(Param);
    if (DataChunk.size() < 1)
    {
        *ErrorMessages << "Invalid/empty data!\n";
        return SetBad();
    }
    ScriptState.BeforeUsed = isBeforeData;
    *ExecutionResults << "Searching for specified data chunk ...\n";
    if (ScriptState.Scope == UPKScope::Package)
    {
        size_t offset = ScriptState.Package.FindDataChunk(DataChunk);
        if (offset != 0)
        {
            if (isEnd) /// seek to the end of specified data
            {
                offset += DataChunk.size();
            }
            ScriptState.Offset = offset;
            ScriptState.RelOffset = 0;
            if (isBeforeData) /// restrict scope for BEFORE/AFTER patching
            {
                ScriptState.MaxOffset = ScriptState.Offset + DataChunk.size() - 1;
            }
            *ExecutionResults << "Data found!\nGlobal offset: " << FormatHEX((uint32_t)ScriptState.Offset)
                              << " (" << ScriptState.Offset << ")" << std::endl;
        }
        else
        {
            *ErrorMessages << "Can't find specified data!\n";
            return SetBad();
        }
    }
    else
    {
        size_t offset = ScriptState.Package.FindDataChunk(DataChunk, ScriptState.Offset, ScriptState.MaxOffset);
        if (offset != 0)
        {
            if (isEnd) /// seek to the end of specified data
            {
                offset += DataChunk.size();
            }
            ScriptState.RelOffset = offset - ScriptState.Offset;
            if (isBeforeData) /// restrict scope for BEFORE/AFTER patching
            {
                ScriptState.MaxOffset = ScriptState.Offset + ScriptState.RelOffset + DataChunk.size() - 1;
            }
            *ExecutionResults << "Data found!\nRelative offset: " << FormatHEX((uint32_t)ScriptState.RelOffset)
                              << " (" << ScriptState.RelOffset << ")" << std::endl;
        }
        else
        {
            *ErrorMessages << "Can't find specified data!\n";
            return SetBad();
        }
    }
    if (!IsInsideScope())
    {
        *ErrorMessages << "Invalid package offset!\n";
        return SetBad();
    }
    return SetGood();
}

bool ModScript::SetDataChunkOffset(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::string DataStr = Param, SpecStr;
    bool isEnd = false;
    SplitAt(':', Param, DataStr, SpecStr);
    if (SpecStr != "")
    {
        if (SpecStr == "END")
        {
            isEnd = true;
        }
        else if (SpecStr != "BEG")
        {
            *ErrorMessages << "Bad specifier: " << SpecStr << std::endl;
            return SetBad();
        }
    }
    /*size_t pos = Param.find(':');
    if (pos != std::string::npos)
    {
        DataStr = Param.substr(0, pos);
        std::string SpecStr = Param.substr(pos + 1);
        if (SpecStr == "END")
        {
            isEnd = true;
        }
        else if (SpecStr != "BEG")
        {
            *ErrorMessages << "Unknown specifier: " << SpecStr << std::endl;
            return SetBad();
        }
    }*/
    return SetDataOffset(DataStr, isEnd, false);
}

bool ModScript::SetCodeOffset(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::string DataStr = Param, SpecStr;
    bool isEnd = false;
    SplitAt(':', Param, DataStr, SpecStr);
    if (SpecStr != "")
    {
        if (SpecStr == "END")
        {
            isEnd = true;
        }
        else if (SpecStr != "BEG")
        {
            *ErrorMessages << "Bad specifier: " << SpecStr << std::endl;
            return SetBad();
        }
    }
    /*size_t pos = Param.find(':');
    if (pos != std::string::npos)
    {
        DataStr = Param.substr(0, pos);
        std::string SpecStr = Param.substr(pos + 1);
        if (SpecStr == "END")
        {
            isEnd = true;
        }
        else if (SpecStr != "BEG")
        {
            *ErrorMessages << "Unknown specifier: " << SpecStr << std::endl;
            return SetBad();
        }
    }*/
    return SetDataOffset(ParseScript(DataStr), isEnd, false);
}

bool ModScript::SetBeforeHEXOffset(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    return SetDataOffset(Param, false, true);
}

bool ModScript::SetBeforeCodeOffset(const std::string& Param)
{
    unsigned MemSize = 0;
    std::string ParsedParam = ParseScript(Param, &MemSize);
    ScriptState.BeforeMemSize = MemSize;
    return SetBeforeHEXOffset(ParsedParam);
}

bool ModScript::WriteAfterHEX(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    if (ScriptState.BeforeUsed == false)
    {
        *ErrorMessages << "Can't use AFTER without BEFORE!\n";
        return SetBad();
    }
    std::vector<char> DataChunk = GetDataChunk(Param);
    if (DataChunk.size() < 1)
    {
        *ErrorMessages << "Invalid/empty data!\n";
        return SetBad();
    }
    size_t ScopeSize = ScriptState.MaxOffset - ScriptState.Offset - ScriptState.RelOffset + 1;
    /// checking if scope is inside script for objects with scripts
    ScriptState.isInsideScript = false;
    if (ScriptState.Scope == UPKScope::Object)
    {
        size_t ScriptSize = ScriptState.Package.GetScriptSize(ScriptState.ObjIdx);
        if (ScriptSize != 0)
        {
            size_t ScriptRelOffset = ScriptState.Package.GetScriptRelOffset(ScriptState.ObjIdx);
            if ( ScriptState.RelOffset >= ScriptRelOffset &&
                (ScriptState.RelOffset + ScopeSize) <= (ScriptRelOffset + ScriptSize) )
            {
                ScriptState.isInsideScript = true;
            }
        }
    }
    /// checking size
    if (ScopeSize != DataChunk.size())
    {
        if (ScriptState.Scope != UPKScope::Object || ScriptState.Behavior == "KEEP")
        {
            *ErrorMessages << "Replacement code does not fit current scope!\n";
            return SetBad();
        }
        int ObjSize = (int)ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize + (int)DataChunk.size() - (int)ScopeSize;
        if (!MoveResizeAtRelOffset(ObjSize))
        {
            *ErrorMessages << "Error moving/resizing object!\n";
            return SetBad();
        }
    }
    /// resetting max offset and before flag
    if (ScriptState.Scope == UPKScope::Package)
    {
        ScriptState.MaxOffset = ScriptState.Package.GetFileSize() - 1;
    }
    else
    {
        ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
    }
    ScriptState.BeforeUsed = false;
    /// trying to adjust script size for objects, which use scripts
    if (ScriptState.isInsideScript)
    {
        size_t ScriptSize = ScriptState.Package.GetScriptSize(ScriptState.ObjIdx);
        if (ScriptSize != 0)
        {
            ScriptSize = (int)ScriptSize + (int)DataChunk.size() - (int)ScopeSize;
            *ExecutionResults << "New script serial size: " << ScriptSize << " (" << FormatHEX((uint32_t)ScriptSize) << ")\n";
            std::vector<char> SizeDataChunk(4);
            memcpy(SizeDataChunk.data(), reinterpret_cast<char*>(&ScriptSize), 4);
            size_t SavedRelOffset = ScriptState.RelOffset;
            size_t ScriptRelOffset = ScriptState.Package.GetScriptRelOffset(ScriptState.ObjIdx);
            ScriptState.RelOffset = ScriptRelOffset - 4;
            if (!WriteBinaryData(SizeDataChunk))
            {
                *ErrorMessages << "Error writing new script size!\n";
                return SetBad();
            }
            ScriptState.RelOffset = SavedRelOffset;
        }
    }
    /// writing data
    return WriteBinaryData(DataChunk);
}

bool ModScript::WriteAfterCode(const std::string& Param)
{
    unsigned MemSize = 0;
    std::string ParsedParam = ParseScript(Param, &MemSize);
    if (!WriteAfterHEX(ParsedParam))
    {
        return SetBad();
    }
    /// trying to adjust script memory size for objects, which use scripts
    if (ScriptState.isInsideScript && ScriptState.Behavior != "KEEP")
    {
        size_t ScriptMemSize = ScriptState.Package.GetScriptMemSize(ScriptState.ObjIdx);
        if (ScriptMemSize != 0 && ((int)MemSize - (int)ScriptState.BeforeMemSize) != 0)
        {
            ScriptMemSize = (int)ScriptMemSize + (int)MemSize - (int)ScriptState.BeforeMemSize;
            *ExecutionResults << "New script memory size: " << ScriptMemSize << " (" << FormatHEX((uint32_t)ScriptMemSize) << ")\n";
            std::vector<char> SizeDataChunk(4);
            memcpy(SizeDataChunk.data(), reinterpret_cast<char*>(&ScriptMemSize), 4);
            size_t SavedRelOffset = ScriptState.RelOffset;
            size_t ScriptRelOffset = ScriptState.Package.GetScriptRelOffset(ScriptState.ObjIdx);
            ScriptState.RelOffset = ScriptRelOffset - 8;
            if (!WriteBinaryData(SizeDataChunk))
            {
                *ErrorMessages << "Error writing new script memory size!\n";
                return SetBad();
            }
            ScriptState.RelOffset = SavedRelOffset;
            ScriptState.BeforeMemSize = 0;
        }
    }
    /// reset inside script flag
    ScriptState.isInsideScript = false;
    return SetGood();
}

bool ModScript::WriteMoveExpandLegacy(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::string ObjName, str = GetStringValue(Param), SizeStr;
    size_t NewSize = 0;
    SplitAt(':', str, ObjName, SizeStr);
    if (SizeStr != "")
    {
        NewSize = GetUnsignedValue(SizeStr);
    }
    /*size_t pos = str.find(":");
    if (pos != std::string::npos)
    {
        ObjName = str.substr(0, pos);
    }
    else
    {
        ObjName = str;
    }*/
    if (SetObject(ObjName) == false)
        return SetBad();
    /*if (pos != std::string::npos)
    {
        NewSize = GetUnsignedValue(str.substr(pos + 1));
    }*/
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
    /// set new offset value
    ScriptState.Offset = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialOffset;
    ScriptState.RelOffset = 0;
    ScriptState.MaxOffset = ScriptState.Offset + ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).SerialSize - 1;
    return SetGood();
}

bool ModScript::IsInsideScope(size_t DataSize)
{
    return (ScriptState.Offset + ScriptState.RelOffset + DataSize - 1) <= ScriptState.MaxOffset;
}

size_t ModScript::GetDiff(size_t DataSize)
{
    if (!IsInsideScope(DataSize))
        return ((ScriptState.Offset + ScriptState.RelOffset + DataSize - 1) - ScriptState.MaxOffset);
    else
        return 0;
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

bool ModScript::WriteAddNameEntry(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    *ExecutionResults << "Adding new name entry ...\n";
    FNameEntry Entry;
    std::vector<char> data = GetDataChunk(ParseScript(Param));
    if (!ScriptState.Package.Deserialize(Entry, data))
    {
        *ErrorMessages << "Error deserializing new name entry: wrong data!\n";
        return SetBad();
    }
    if (ScriptState.Package.FindName(Entry.Name) != -1)
    {
        *ExecutionResults << "Name " << Entry.Name << " already exists, skipping...\n";
        return SetGood();
    }
    if (!ScriptState.Package.AddNameEntry(Entry))
    {
        *ErrorMessages << "Error adding new name entry!\n";
        return SetBad();
    }
    *ExecutionResults << "Name " << Entry.Name << " added successfully!\n";
    return SetGood();
}

bool ModScript::WriteAddImportEntry(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    *ExecutionResults << "Adding new import entry ...\n";
    FObjectImport Entry;
    std::vector<char> data = GetDataChunk(ParseScript(Param));
    if (!ScriptState.Package.Deserialize(Entry, data))
    {
        *ErrorMessages << "Error deserializing new import entry: wrong data!\n";
        return SetBad();
    }
    if (ScriptState.Package.FindObject(Entry.FullName, false) != 0)
    {
        *ExecutionResults << "Import object " << Entry.FullName << " already exists, skipping...\n";
        return SetGood();
    }
    if (!ScriptState.Package.AddImportEntry(Entry))
    {
        *ErrorMessages << "Error adding new import entry!\n";
        return SetBad();
    }
    *ExecutionResults << "Import object " << Entry.FullName << " added successfully!\n";
    return SetGood();
}

bool ModScript::WriteAddExportEntry(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    *ExecutionResults << "Adding new export entry ...\n";
    FObjectExport Entry;
    std::vector<char> data = GetDataChunk(ParseScript(Param));
    if (!ScriptState.Package.Deserialize(Entry, data))
    {
        *ErrorMessages << "Error deserializing new export entry: wrong data!\n";
        return SetBad();
    }
    if (ScriptState.Package.FindObject(Entry.FullName, true) != 0)
    {
        *ExecutionResults << "Export object " << Entry.FullName << " already exists, skipping...\n";
        return SetGood();
    }
    if (!ScriptState.Package.AddExportEntry(Entry))
    {
        *ErrorMessages << "Error adding new export entry!\n";
        return SetBad();
    }
    *ExecutionResults << "Export object " << Entry.FullName << " added and linked successfully!\n";
    return SetGood();
}

bool ModScript::AddAlias(const std::string& Param)
{
    if (ScriptState.Package.IsLoaded() == false)
    {
        *ErrorMessages << "Package is not opened!\n";
        return SetBad();
    }
    std::string Name, Replacement;
    size_t pos = SplitAt(':', Param, Name, Replacement);
    /*size_t pos = Param.find(':');*/
    if (pos == std::string::npos)
    {
        *ErrorMessages << "Bad key value: " << Param << std::endl;
        return SetBad();
    }
    /*Name = Param.substr(0, pos);
    Replacement = Param.substr(pos + 1);*/
    if (ScriptState.Scope == UPKScope::Object)
    {
        Name = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName + '.' + Name;
    }
    if (Alias.count(Name) != 0)
    {
        *ErrorMessages << "Alias is already defined: " << Name << std::endl;
        return SetBad();
    }
    Alias[Name] = Replacement;
    *ExecutionResults << "Alias added successfully: " << Name << std::endl;
    return SetGood();
}

std::string GetWord(std::istream& in)
{
    std::string word;
    if (!in.good())
    {
        return "";
    }
    /// discard leading white-spaces
    char ch = '\n';
    while (isspace(ch) && in.good())
    {
        ch = in.get();
    }
    /// if stream has ended, return empty string
    if (!in.good())
    {
        return "";
    }
    /// if character is not white-space and stream still good
    word += ch;
    /// extract token
    if (ch == '<')
    {
        while (ch != '>' && in.good())
        {
            ch = in.get();
            word += ch;
        }
        return word;
    }
    /// extract command
    if (ch == '[')
    {
        while (ch != ']' && in.good())
        {
            ch = in.get();
            word += ch;
        }
        return word;
    }
    /// extract HEX
    if (isxdigit(ch))
    {
        while (isxdigit(ch) && in.good())
        {
            ch = in.get();
            if (isxdigit(ch))
                word += ch;
        }
        return word;
    }
    /// extract generic word
    while (!isspace(ch) && in.good())
    {
        ch = in.get();
        if (!isspace(ch))
            word += ch;
    }
    return word;
}

std::string EatWhite(std::string str, char delim = 0)
{
    std::string ret, tail = "";
    unsigned length = str.length();
    if (delim != 0) /// use delimiter
    {
        size_t pos = str.find_first_of(delim);
        if (pos != std::string::npos)
        {
            length = pos;
            tail = str.substr(pos);
        }
    }
    for (unsigned i = 0; i < length; ++i)
    {
        if (!isspace(str[i]))
            ret += str[i];
    }
    ret += tail;
    return ret;
}

std::string ExtractString(std::string str)
{
    std::string ret;
    size_t pos = str.find_first_of("\"");
    if (pos == std::string::npos || pos + 1 >= str.length())
    {
        return "";
    }
    ret = str.substr(pos + 1);
    pos = ret.find_last_of("\"");
    if (pos == std::string::npos || pos == 0)
    {
        return "";
    }
    ret = ret.substr(0, pos);
    return ret;
}

std::string ModScript::ParseScript(std::string ScriptData, unsigned* ScriptMemSizeRef)
{
    std::ostringstream ScriptHEX;
    std::istringstream WorkingData(ScriptData);
    std::map<std::string, uint16_t> Labels;
    unsigned ScriptMemSize = 0, MemSize = 0;
    bool needSecondPass = false;
    unsigned numPasses = 0;
    do
    {
        needSecondPass = false;
        if (numPasses != 0) /// no need to parse twice
        {
            WorkingData.str(ScriptHEX.str());
            WorkingData.clear();
            ScriptHEX.str("");
            ScriptHEX.clear();
        }
        //std::cout << "Script data:\n" << WorkingData.str() << "\nScript data end.\n";
        while (!WorkingData.eof())
        {
            std::string NextWord;
            NextWord = GetWord(WorkingData);
            //WorkingData >> NextWord;
            //std::cout << "!" << NextWord << "!" << std::endl;
            if (NextWord == "")
            {
                /// skip empty lines
            }
            else if (IsHEX(NextWord))
            {
                ScriptHEX << NextWord << " ";
                if (numPasses == 0)
                    ScriptMemSize += 1;
            }
            else if (IsToken(NextWord))
            {
                ScriptHEX << TokenToHEX(NextWord, &MemSize);
                if (numPasses == 0)
                    ScriptMemSize += MemSize;
                if (!ScriptState.Good)
                {
                    *ErrorMessages << "Bad token: " << NextWord << std::endl;
                    SetBad();
                    return std::string("");
                }
            }
            else if (IsCommand(NextWord))
            {
                if (ScriptState.Scope != UPKScope::Object)
                {
                    *ErrorMessages << "You can't use code commands outside Object scope: " << NextWord << std::endl;
                    SetBad();
                    return std::string("");
                }
                std::string Command = NextWord.substr(1, NextWord.length()-2); /// remove []
                Command = EatWhite(Command); /// remove white-spaces
                if (Command[0] == '@') /// label reference
                {
                    if (Labels.count(Command.substr(1)) != 0) /// found reference
                    {
                        uint16_t LabelPos = Labels[Command.substr(1)];
                        ScriptHEX << MakeTextBlock(reinterpret_cast<char*>(&LabelPos), 2);
                    }
                    else
                    {
                        ScriptHEX << NextWord << " ";
                        needSecondPass = true;
                    }
                    if (numPasses == 0)
                        ScriptMemSize += 2;
                }
                else if (Command[0] == '#') /// label mark
                {
                    if (numPasses == 0 && Labels.count(Command.substr(1)) != 0)
                    {
                        *ErrorMessages << "Multiple labels: " << NextWord << std::endl;
                        SetBad();
                        return std::string("");
                    }
                    if (numPasses == 0)
                        Labels[Command.substr(1)] = ScriptMemSize;
                }
            }
            else
            {
                *ErrorMessages << "Bad token: " << NextWord << std::endl;
                SetBad();
                return std::string("");
            }
        }
        ++numPasses;
        //std::cout << "Script hex:\n" << ScriptHEX.str() << "\nScript hex end.\n";
    } while (needSecondPass);
    if (ScriptMemSizeRef != nullptr)
    {
        (*ScriptMemSizeRef) = ScriptMemSize;
    }
    return ScriptHEX.str();
}

bool ModScript::IsHEX(std::string word)
{
    if (word.length() != 2)
        return false;
    return (isxdigit(word.front()) && isxdigit(word.back()));
}

bool ModScript::IsToken(std::string word)
{
    if (word.length() < 3)
        return false;
    return (word.front() == '<' && word.back() == '>');
}

bool ModScript::IsCommand(std::string word)
{
    if (word.length() < 3)
        return false;
    return (word.front() == '[' && word.back() == ']');
}

std::string ModScript::TokenToHEX(std::string Token, unsigned* MemSizeRef)
{
    std::string Code = Token.substr(1, Token.length()-2); /// remove <>
    Code = EatWhite(Code, '\"'); /// remove white-spaces
    //std::cout << "!" << Code << "!" << std::endl;
    unsigned MemSize = 1;
    std::vector<char> dataChunk;
    if (Code[0] == '!') /// parse alias
    {
        std::string Name = Code.substr(1);
        if (ScriptState.Scope == UPKScope::Object)
        {
            Name = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName + '.' + Name;
            if (Alias.count(Name) == 0)
            {
                Name = Code.substr(1);
            }
        }
        if (Alias.count(Name) == 0)
        {
            *ErrorMessages << "Alias does not exist: " << Name << std::endl;
            SetBad();
            return std::string("");
        }
        std::string replacement = Alias[Name];
        std::string parsed = ParseScript(replacement, &MemSize);
        dataChunk = GetDataChunk(parsed);
    }
    else if (Code[0] == '%')
    {
        if (Code[1] == 'f')
        {
            float FloatVal = GetFloatValue(Code.substr(2));
            dataChunk.resize(4);
            memcpy(dataChunk.data(), reinterpret_cast<char*>(&FloatVal), 4);
            MemSize = 4;
        }
        else if (Code[1] == 'i')
        {
            int IntVal = GetIntValue(Code.substr(2));
            dataChunk.resize(4);
            memcpy(dataChunk.data(), reinterpret_cast<char*>(&IntVal), 4);
            MemSize = 4;
        }
        else if (Code[1] == 'u')
        {
            unsigned UnsignedVal = GetUnsignedValue(Code.substr(2));
            dataChunk.resize(4);
            memcpy(dataChunk.data(), reinterpret_cast<char*>(&UnsignedVal), 4);
            MemSize = 4;
        }
        else if (Code[1] == 's')
        {
            unsigned UnsignedVal = GetUnsignedValue(Code.substr(2));
            if (UnsignedVal > 0xFFFF)
            {
                *ErrorMessages << "Incorrect short value: " << UnsignedVal << std::endl;
                SetBad();
                return std::string("");
            }
            uint16_t ShortVal = (uint16_t)UnsignedVal;
            dataChunk.resize(2);
            memcpy(dataChunk.data(), reinterpret_cast<char*>(&ShortVal), 2);
            MemSize = 2;
        }
        else if (Code[1] == 'b')
        {
            unsigned UnsignedVal = GetUnsignedValue(Code.substr(2));
            if (UnsignedVal > 0xFF)
            {
                *ErrorMessages << "Incorrect byte value: " << UnsignedVal << std::endl;
                SetBad();
                return std::string("");
            }
            uint8_t ByteVal = (uint8_t)UnsignedVal;
            dataChunk.resize(1);
            memcpy(dataChunk.data(), reinterpret_cast<char*>(&ByteVal), 1);
            MemSize = 1;
        }
        else if (Code[1] == 't')
        {
            //std::string strVal = Code.substr(3, Code.length()-4);
            std::string strVal = ExtractString(Code);
            //std::cout << "!" << strVal << "!" << std::endl;
            dataChunk.resize(strVal.length()+1);
            memcpy(dataChunk.data(), strVal.c_str(), strVal.length()+1);
            MemSize = strVal.length()+1;
        }
        else
        {
            *ErrorMessages << "Bad token: " << Code << std::endl;
            SetBad();
            return std::string("");
        }
    }
    else if (Code[0] == '@') /// member variable reference
    {
        if (ScriptState.Scope != UPKScope::Object)
        {
            *ErrorMessages << "Can't use member variable references outside Object scope: " << Code << std::endl;
            SetBad();
            return std::string("");
        }
        std::string ObjName = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName;
        std::string ClassName = ObjName.substr(0, ObjName.find('.'));
        std::string VarName = ClassName + '.' + Code.substr(1);
        UObjectReference ObjRef = ScriptState.Package.FindObject(VarName, true);
        if (ObjRef == 0)
        {
            *ErrorMessages << "Bad object name: " << VarName << std::endl;
            SetBad();
            return std::string("");
        }
        dataChunk.resize(4);
        memcpy(dataChunk.data(), reinterpret_cast<char*>(&ObjRef), 4);
        MemSize = 8;
    }
    else if (Code.find(".") != std::string::npos)
    {
        std::string ObjName = Code;
        if (Code.front() == '.') /// local var reference
        {
            if (ScriptState.Scope != UPKScope::Object)
            {
                *ErrorMessages << "You can't use local references outside Object scope: " << Code << std::endl;
                SetBad();
                return std::string("");
            }
            else
            {
                ObjName = ScriptState.Package.GetExportEntry(ScriptState.ObjIdx).FullName + Code;
            }
        }
        else if (Code.find("Class.") == 0) /// class reference
        {
            ObjName = Code.substr(6);
        }
        UObjectReference ObjRef = ScriptState.Package.FindObject(ObjName, false);
        if (ObjRef == 0)
        {
            *ErrorMessages << "Bad object name: " << ObjName << std::endl;
            SetBad();
            return std::string("");
        }
        dataChunk.resize(4);
        memcpy(dataChunk.data(), reinterpret_cast<char*>(&ObjRef), 4);
        MemSize = 8;
    }
    else if (Code == "NullRef") /// null object
    {
        dataChunk.resize(4, 0);
        MemSize = 8;
    }
    else /// Name reference
    {
        std::string Name = Code;
        int num = 0;
        size_t pos = Code.rfind('_');
        if (pos != std::string::npos && isdigit(Code[pos + 1]))
        {
            Name = Code.substr(0, pos);
            num = 1 + GetIntValue(Code.substr(pos + 1));
        }
        int idx = ScriptState.Package.FindName(Name);
        if (idx < 0)
        {
            *ErrorMessages << "Bad name: " << Name << std::endl;
            SetBad();
            return std::string("");
        }
        UNameIndex NameIdx;
        NameIdx.NameTableIdx = idx;
        NameIdx.Numeric = num;
        dataChunk.resize(8);
        memcpy(dataChunk.data(), reinterpret_cast<char*>(&NameIdx), 8);
        MemSize = 8;
    }
    if (MemSizeRef != nullptr)
    {
        (*MemSizeRef) = MemSize;
    }
    return FormatHEX(dataChunk);
}
