#include <iostream>
#include <sstream>

#include "UPKUtils.h"
#include "ModParser.h"

using namespace std;

int string2int(string str)
{
    int val = 0;
    if (str == "")
        return val;
    istringstream ss(str);
    if (str.find("0x") != string::npos)
        ss >> hex >> val;
    else
        ss >> dec >> val;
    return val;
}

int main(int argN, char* argV[])
{
    cout << "PatchUPK" << endl;

    if (argN < 2 || argN > 3)
    {
        cerr << "Usage: PatchUPK modfile.txt [PATH_TO_UPK]" << endl;
        return 1;
    }

    ModParser parser;

    if (!parser.OpenModFile(argV[1]))
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }

    string upkPath = "";

    if (argN == 3)
    {
        upkPath = argV[2];
        if (upkPath.length() < 1)
        {
            cerr << "Incorrect package path!" << endl;
            return 1;
        }
        if (upkPath[upkPath.length() - 1] != '\\')
            upkPath +=  "\\";
    }

    parser.AddKeyName(string("MOD_NAME"));
    parser.AddKeyName(string("AUTHOR"));
    parser.AddKeyName(string("DESCRIPTION"));
    parser.AddKeyName(string("UPK_FILE"));
    parser.AddKeyName(string("OFFSET"));
    parser.AddKeyName(string("FUNCTION"));
    parser.AddKeyName(string("REL_OFFSET"));
    parser.AddKeyName(string("FUNCTION_FILE"));
    parser.AddKeyName(string("NAMELIST_NAME"));
    parser.AddKeyName(string("EXPAND_FUNCTION"));

    parser.AddSectionName(string("[MODDED_HEX]"));
    parser.AddSectionName(string("[BEFORE_HEX]"));
    parser.AddSectionName(string("[/BEFORE_HEX]"));
    parser.AddSectionName(string("[AFTER_HEX]"));
    parser.AddSectionName(string("[/AFTER_HEX]"));

    parser.SetCommentMarkers('{', '}', 0);

    int idx = parser.FindNext();

    UPKUtils package;

    string upkFileName = "", upkFileNameSaved = "";
    size_t offset = 0, rel_offset = 0;
    string functionName = "", functionFile = "", namelistName = "", newName = "", stringData = "";
    int functionIdx = -1, namelistIdx = -1, functionSize = 0;
    size_t colonPos = string::npos;

    std::vector<char> dataChunk;
    fstream binFile;
    size_t binFileSize = 0;

    while (idx != -1)
    {
        if (parser.IsKey())
        {
            switch(idx)
            {
            case 0:
                cout << "\nInstalling mod:" << endl
                     << parser.GetText() << endl;
                break;
            case 1:
                cout << "by " << parser.GetText() << endl;
                break;
            case 2:
                cout << "Mod description:" << endl
                     << parser.GetText() << endl;
                break;
            case 3:
                upkFileName = parser.GetStringValue();
                cout << "\nPackage file: " << upkFileName << endl;
                if (upkFileName != upkFileNameSaved)
                {
                    upkFileNameSaved = upkFileName;
                    string fullPath = upkPath + upkFileName;
                    cout << "Full path: " << fullPath << endl;
                    if (!package.open(fullPath.c_str()))
                    {
                        cerr << "Can't open " << fullPath.c_str() << endl;
                        return 1;
                    }
                }
                offset = 0;
                rel_offset = 0;
                functionName = "";
                functionFile = "";
                break;
            case 4:
                if (!package.good())
                {
                    cerr << "Package file not set!" << endl;
                    return 1;
                }
                offset = parser.GetIntValue();
                cout << "Data offset: " << hex << showbase << offset << dec << endl;
                if (!package.CheckValidFileOffset(offset))
                {
                    cerr << "Incorrect offset value!" << endl;
                    return 1;
                }
                rel_offset = 0;
                functionName = "";
                functionFile = "";
                break;
            case 5:
                if (!package.good())
                {
                    cerr << "Package file not set!" << endl;
                    return 1;
                }
                functionName = parser.GetStringValue();
                cout << "Function to find: " << functionName << endl;
                if (functionName.length() < 1)
                {
                    cerr << "Incorrect function name!" << endl;
                    return 1;
                }
                functionIdx = package.FindObjectListEntryByName(functionName);
                if (functionIdx <= 0)
                {
                    cerr << "Can't find function " << functionName << " in package " << upkFileName << endl;
                    return 1;
                }
                offset = package.GetObjectListEntryByIdx(functionIdx).DataOffset;
                cout << "Function offset: " << hex << showbase << offset << dec << endl;
                rel_offset = 0;
                functionFile = "";
                break;
            case 6:
                if (offset == 0)
                {
                    cerr << "Error attempting to set relative offset while base offset is not set!" << endl;
                    return 1;
                }
                rel_offset = parser.GetIntValue();
                cout << "Relative offset: " << hex << showbase << rel_offset << dec << endl;
                if (!package.CheckValidFileOffset(offset + rel_offset))
                {
                    cerr << "Incorrect offset value!" << endl;
                    return 1;
                }
                functionFile = "";
                break;
            case 7:
                if (!package.good())
                {
                    cerr << "Package file not set!" << endl;
                    return 1;
                }
                functionFile = parser.GetStringValue();
                cout << "Binary function file to read: " << functionFile << endl;
                if (functionFile.length() < 1)
                {
                    cerr << "Incorrect function file name!" << endl;
                    return 1;
                }
                if (binFile.is_open())
                {
                    binFile.close();
                    binFile.clear();
                }
                binFile.open(functionFile.c_str(), ios::binary | ios::in | ios::out);
                if (!binFile.is_open())
                {
                    cerr << "Can't open " << functionFile.c_str() << endl;
                    return 1;
                }
                binFile.seekg(0, ios::end);
                binFileSize = binFile.tellg();
                binFile.seekg(0, ios::beg);
                dataChunk.clear();
                dataChunk.resize(binFileSize);
                binFile.read(dataChunk.data(), binFileSize);
                if (!binFile.good())
                {
                    cerr << "Error reading input file: " << functionFile.c_str() << endl;
                    return 1;
                }
                if (offset == 0)
                {
                    cout << "Attempting to write data by function name" << endl;
                    functionName = functionFile.substr(functionFile.find_last_of("\\") + 1);
                    if (functionName.find(".Function") != std::string::npos)
                    {
                        functionName = functionName.substr(0, functionName.find(".Function"));
                    }
                    cout << "Function name from file name: " << functionName << endl;
                    functionIdx = package.FindObjectListEntryByName(functionName);
                    if (functionIdx <= 0)
                    {
                        cerr << "Can't find function " << functionName << " in package " << upkFileName << endl;
                        return 1;
                    }
                    offset = package.GetObjectListEntryByIdx(functionIdx).DataOffset;
                    cout << "Writing new data at " << hex << showbase << offset << dec << " in " << upkFileName << endl;
                    if (!package.WriteObjectData(functionIdx, dataChunk))
                    {
                        cerr << "Error writing to upk file: " << upkFileName << endl;
                        return 1;
                    }
                }
                else
                {
                    cout << "Writing new data at " << hex << showbase << offset << dec << " in " << upkFileName << endl;
                    if (!package.WriteData(offset, dataChunk))
                    {
                        cerr << "Error writing to upk file: " << upkFileName << endl;
                        return 1;
                    }
                }
                cout << "Write successfull!" << endl;
                dataChunk.clear();
                offset = 0;
                rel_offset = 0;
                functionName = "";
                functionFile = "";
                break;
            case 8:
                if (!package.good())
                {
                    cerr << "Package file not set!" << endl;
                    return 1;
                }
                stringData = parser.GetStringValue();
                colonPos = stringData.find(":");
                if (colonPos == string::npos)
                {
                    cerr << "Incorrect data format!" << endl;
                    return 1;
                }
                namelistName = stringData.substr(0, colonPos);
                newName = stringData.substr(colonPos + 1);
                if (namelistName.length() < 1 || newName.length() < 1)
                {
                    cerr << "Incorrect namelist name!" << endl;
                    return 1;
                }
                cout << "Attempting to determine data offset by namelist name: " << namelistName << endl;
                namelistIdx = package.FindNamelistEntryByName(namelistName);
                if (namelistIdx < 0)
                {
                    cerr << "Can't find namelist name " << namelistName << " in package " << upkFileName << endl;
                    return 1;
                }
                cout << "Attempting to write new namelist name: " << newName << endl;
                if (!package.WriteNamelistName(namelistIdx, newName))
                {
                    cerr << "Error writing to upk file: " << upkFileName << endl;
                    return 1;
                }
                offset = 0;
                rel_offset = 0;
                functionName = "";
                functionFile = "";
                break;
            case 9:
                if (!package.good())
                {
                    cerr << "Package file not set!" << endl;
                    return 1;
                }
                stringData = parser.GetStringValue();
                colonPos = stringData.find(":");
                if (colonPos == string::npos)
                {
                    cerr << "Incorrect data format!" << endl;
                    return 1;
                }
                functionName = stringData.substr(0, colonPos);
                functionSize = string2int(stringData.substr(colonPos + 1));
                cout << "Function name: " << functionName << endl;
                cout << "New function size: " << functionSize << endl;
                functionIdx = package.FindObjectListEntryByName(functionName);
                if (functionIdx <= 0)
                {
                    cerr << "Can't find function " << functionName << " in package " << upkFileName << endl;
                    return 1;
                }
                if (uint32_t(functionSize) <= package.GetObjectListEntryByIdx(functionIdx).ObjectFileSize)
                {
                    cerr << "Existing function size is greater than specified value!" << endl;
                    return 1;
                }
                package.MoveObject(functionIdx, functionSize, true);
                cout << "Function moved successfully!" << endl;
                offset = 0;
                rel_offset = 0;
                functionName = "";
                functionFile = "";
                break;
            default:
                cerr << "Unsupported key type!" << endl;
                return 1;
            }
        }
        else if (parser.IsSection())
        {
            switch(idx)
            {
            case 0:
            case 3:
                if (!package.good())
                {
                    cerr << "Package file not set!" << endl;
                    return 1;
                }
                if (offset == 0)
                {
                    cerr << "Offset value not set!" << endl;
                    return 1;
                }
                dataChunk = parser.GetDataChunk();
                if (dataChunk.size() == 0)
                {
                    cerr << "Data chunk is empty/invalid!" << endl;
                    return 1;
                }
                if (!package.CheckValidFileOffset(offset + rel_offset + dataChunk.size()))
                {
                    cerr << "Incorrect offset value or data chunk size!" << endl;
                    return 1;
                }
                cout << "Writing new data at " << hex << showbase << offset + rel_offset << dec << " in " << upkFileName << endl;
                if (!package.WriteData(offset + rel_offset, dataChunk))
                {
                    cerr << "Error writing to upk file: " << upkFileName << endl;
                    return 1;
                }
                cout << "Write successfull!" << endl;
                if (rel_offset == 0)
                    offset = 0;
                rel_offset = 0;
                functionFile = "";
                break;
            case 4:
                // ignore [/AFTER_HEX]
                parser.GetText();
                break;
            case 1:
                if (!package.good())
                {
                    cerr << "Package file not set!" << endl;
                    return 1;
                }
                dataChunk = parser.GetDataChunk();
                if (dataChunk.size() == 0)
                {
                    cerr << "Data chunk is empty/invalid!" << endl;
                    return 1;
                }
                offset = package.FindDataChunk(dataChunk);
                if (offset == 0)
                {
                    cerr << "Can't find specified data chunk in " << upkFileName << endl;
                    return 1;
                }
                cout << "Found data at: " << hex << showbase << offset << dec << endl;
                rel_offset = 0;
                functionName = "";
                functionFile = "";
                break;
            case 2:
                // ignore [/BEFORE_HEX]
                parser.GetText();
                break;
            default:
                cerr << "Unsupported section type!" << endl;
                return 1;
            }
        }
        idx = parser.FindNext();
    }

    return 0;
}
