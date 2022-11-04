#include <iostream>
#include <string>
#include <map>

#include "UPKUtils.h"
#include "dds.h"
#include "FileUtils.h"

#include <wx/cmdline.h>

#define CUSTOM_TFC_NAME "Texture2D"

//using namespace std;

bool ReadInventory(wxString filename, std::map<wxString, wxArrayString>& inv);
bool CatalogueFilesToProcess(wxString ddsDir, wxString ddsMask, wxString upkDir, wxString upkMask, std::map<wxString, wxArrayString>& ddsInv, std::map<wxString, wxArrayString>& upkInv);
bool ReadDDSFile(wxString nextDDSFileName, DDSHeader& header, std::vector<UTexture2DMipMap>& mipMaps);
bool WriteTextureObject(UObjectReference ObjRef, UPKUtils& package, CustomTFC& tfc, DDSHeader& header, std::vector<UTexture2DMipMap>& mipMaps, int32_t LODBias = LOD_BIAS_BAD, bool resetLOD = false);

//void ReadDDS(UObjectReference &ObjRef, UPKUtils &package);

int main(int argN, char* argV[])
{
    wxMessageOutputStderr OutputStderr;
    wxMessageOutput::Set(&OutputStderr);
    /// static logo text
    static const std::string myLogo = "ImportTexturesFromDDS by wghost81 aka Wasteland Ghost";
    /// parse command line
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_PARAM,  NULL, NULL,        "<input file or pattern>", wxCMD_LINE_VAL_STRING },
        { wxCMD_LINE_PARAM,  NULL, NULL,        "dds name or pattern", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_SWITCH, "h", "help",       "get usage help (this text)", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_OPTION, "i", "input",      "set input dir (input packages)" },
        { wxCMD_LINE_OPTION, "o", "output",     "set output dir (output packages)" },
        { wxCMD_LINE_OPTION, "d", "dds",        "set dds dir (input dds, inventory.csv)" },
        { wxCMD_LINE_OPTION, "t", "tfc",        "set tfc name and dir (by default [output dir]/Texture2D.tfc is used)" },
        { wxCMD_LINE_OPTION, NULL, "LODBias",   "new LODBias value (ignored for Batman AC)", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_SWITCH, NULL, "resetLOD",  "reset LODBias (ignored for Batman AC)" },
        { wxCMD_LINE_SWITCH, "x", "exit",       "exit on error" },
        { wxCMD_LINE_SWITCH, "w", "overwrite",  "overwrite existing files without backup" },
        { wxCMD_LINE_NONE }
    };
    wxCmdLineParser cmdLineParser(cmdLineDesc, argN, argV);
    cmdLineParser.SetLogo(myLogo);
    cmdLineParser.SetSwitchChars("-");
    cmdLineParser.EnableLongOptions();
    if (cmdLineParser.Parse() != 0)
        return 1;
    std::cout << myLogo << std::endl;
    /// handle command line params
    /// directories
    wxString upkDirName;
    if (!cmdLineParser.Found("input", &upkDirName))
        upkDirName = wxFileName(cmdLineParser.GetParam()).GetPath();
    if (upkDirName == "")
        upkDirName = ".";
    upkDirName = wxFileName(upkDirName).GetFullPath();
    std::cout << "Input dir name (input packages): " << upkDirName << std::endl;
    wxString ddsDirName;
    if (!cmdLineParser.Found("dds", &ddsDirName))
        ddsDirName = wxFileName(cmdLineParser.GetParam()).GetPath();
    if (ddsDirName == "")
        ddsDirName = ".";
    ddsDirName = wxFileName(ddsDirName).GetFullPath();
    std::cout << "DDS dir name (input dds files): " << ddsDirName << std::endl;
    wxString outputDirName;
    if (!cmdLineParser.Found("output", &outputDirName))
        outputDirName = ".";
    outputDirName = wxFileName(outputDirName).GetFullPath();
    if (!wxDirExists(outputDirName) && !wxMkdir(outputDirName))
    {
        std::cerr << "Error: output directory does not exist: " + outputDirName << std::endl;
        return 1;
    }
    std::cout << "Output dir name (output packages): " << outputDirName << std::endl;
    wxString customTFCFileName;
    if (!cmdLineParser.Found("tfc", &customTFCFileName))
        customTFCFileName = wxFileName(outputDirName + "/" + CUSTOM_TFC_NAME + ".tfc").GetFullPath();
    std::cout << "Custom tfc name: " << customTFCFileName << std::endl;
    /// upk file name
    wxString upkFileName = wxFileName(cmdLineParser.GetParam()).GetFullName();
    /// get the names of all files in the array
    wxString upkFileMask = wxFileName(upkFileName).GetFullName();
    if (wxIsWild(upkFileMask))
        std::cout << "Package file mask: " << upkFileName << std::endl;
    else
        std::cout << "Package file path: " << upkFileName << std::endl;
    /// get the name/mask of object(s) to find
    wxString ddsFileName = "*.dds";
    if (cmdLineParser.GetParamCount() > 1)
        ddsFileName = wxFileName(cmdLineParser.GetParam(1)).GetFullName();
    if (wxIsWild(ddsFileName))
        std::cout << "DDS file mask: " << ddsFileName << std::endl;
    else
        std::cout << "DDS file path: " << ddsFileName << std::endl;
    /// LODBias
    long LODBias = LOD_BIAS_BAD;
    if (cmdLineParser.Found("LODBias", &LODBias))
        std::cout << "New LODBias: " << LODBias << std::endl;
    bool resetLOD = false;
    if (cmdLineParser.FoundSwitch("resetLOD"))
    {
        resetLOD = true;
        std::cout << "LODBias will be reset!" << std::endl;
    }
    bool exitOnError = cmdLineParser.FoundSwitch("exit");
    bool overwriteFiles = cmdLineParser.FoundSwitch("overwrite");

    /// textures inventory contains texture object names and corresponding package names
    std::map<wxString, wxArrayString> texturesInventory;
    if (wxIsWild(upkFileMask) || wxIsWild(ddsFileName))
    {
        /// check for multiple inventory files from multiple exports
        wxArrayString invCsvFound = TraverseDir(ddsDirName, "*inventory*.csv");
        if (invCsvFound.size() > 1)
        {
            std::cerr << "Error: multiple inventory files found in " << ddsDirName << std::endl;
            return 1;
        }
        if (invCsvFound.size() == 0)
        {
            std::cerr << "Error: inventory file is missing from " << ddsDirName << std::endl;
            return 1;
        }
        std::cout << "Inventory file found: " << invCsvFound[0] << std::endl;
        /// read the inventory file
        wxFileName texturesInvFile(invCsvFound[0]);
        if (!ReadInventory(texturesInvFile.GetFullPath(), texturesInventory))
        {
            std::cerr << "Error reading inventory file: " << texturesInvFile.GetFullPath() << std::endl;
            return 1;
        }
        std::cout << texturesInventory.size();
        if (texturesInventory.size() > 1)
            std::cout << " entries";
        else
            std::cout << " entry";
        std::cout << " found in " << invCsvFound[0] << std::endl;
    }

    /// packages inventory contains package paths (relative to upk dir) and corresponding
    /// dds paths (relative to dds dir)
    std::map<wxString, wxArrayString> packagesInventory;
    if (!CatalogueFilesToProcess(ddsDirName, ddsFileName, upkDirName, upkFileName, texturesInventory, packagesInventory))
    {
        std::cerr << "Error cataloging files to process!" << std::endl;
        return 1;
    }
    std::cout << "Files cataloged: " << packagesInventory.size();
    if (packagesInventory.size() > 1)
        std::cout << " packages";
    else
        std::cout << " package";
    std::cout << " to process." << std::endl;

    /// prepare custom tfc
    /// backup the existing custom tfc
    if (wxFileName::FileExists(customTFCFileName) && !overwriteFiles)
    {
        wxString backupFileName = wxFileName(customTFCFileName + ".bak").GetFullPath();
        if (!wxCopyFile(customTFCFileName, backupFileName))
            std::cerr << "Error writing backup to: " << backupFileName << std::endl;
        std::cout << "tfc file backup saved to: " << backupFileName << std::endl;
    }
    /// init custom tfc
    CustomTFC T2DFile;
    if (!T2DFile.Read(customTFCFileName.ToStdString().c_str()))
    {
        std::cerr << "Error reading tfc file: " << customTFCFileName << std::endl;
        return 1;
    }

    wxArrayString failedToProcess, failedToExportObjs;

    for (std::map<wxString, wxArrayString>::iterator it = packagesInventory.begin(); it != packagesInventory.end(); ++it)
    {
        /// find the next file on disk
        wxString nextFileName = wxFileName(upkDirName + "/" + it->first).GetFullPath();
        if (!wxFileExists(nextFileName))
        {
            wxString found = wxDir::FindFirst(upkDirName, wxFileName(nextFileName).GetFullName());
            if (found != "")
                nextFileName = found;
            else
            {
                std::cerr << "Error: cannot find package file: " << nextFileName << std::endl;
                failedToProcess.Add(nextFileName);
                if (exitOnError)
                    return 1;
                continue;
            }
        }
        std::cout << "Next package file: " << nextFileName << std::endl;
        /// process the package
        UPKUtils Package(nextFileName.c_str());
        UPKReadErrors err = Package.GetError();
        if (err != UPKReadErrors::NoErrors && !Package.IsCompressed())
        {
            std::cerr << "Error reading package: " << nextFileName << "\n" << FormatReadErrors(err);
            failedToProcess.Add(nextFileName);
            if (exitOnError)
                return 1;
            continue;
        }
        if (Package.IsCompressed())
        {
            std::cout << "Package is compressed, decompressing...\n";
            if (!Package.DecompressPackage())
            {
                std::cerr << "Error decompressing package: " << nextFileName << "\n";
                failedToProcess.Add(nextFileName);
                if (exitOnError)
                    return 1;
                continue;
            }
            std::cout << "Done.\n";
        }
        /// make sure the specified tfc name exists in nametable
        if (Package.FindName(wxFileName(T2DFile.GetFilename()).GetName().ToStdString()) == -1)
        {
            std::cerr << "Error: the tfc name specified does not exist for this package!\n";
            failedToProcess.Add(nextFileName);
            if (exitOnError)
                return 1;
            continue;
        }
        /// process dds files for the package
        wxArrayString ddsFilesList = it->second;
        for (unsigned i = 0; i < ddsFilesList.size(); ++i)
        {
            wxString nextDDSFileName = wxFileName(ddsDirName + "/" + ddsFilesList[i]).GetFullPath();
            if (!wxFileExists(nextDDSFileName))
            {
                wxString found = wxDir::FindFirst(ddsDirName, wxFileName(nextDDSFileName).GetFullName());
                if (found != "")
                    nextDDSFileName = found;
                else
                {
                    std::cerr << "Error: cannot find dds file: " << nextDDSFileName << std::endl;
                    failedToExportObjs.Add(nextDDSFileName);
                    if (exitOnError)
                        return 1;
                    continue;
                }
            }
            DDSHeader header;
            std::vector<UTexture2DMipMap> mipMaps;
            if (!ReadDDSFile(nextDDSFileName, header, mipMaps))
            {
                std::cerr << "Error reading dds file: " << nextDDSFileName << std::endl;
                failedToExportObjs.Add(nextDDSFileName);
                if (exitOnError)
                    return 1;
                continue;
            }
            std::string fullObjName = wxFileName(nextDDSFileName).GetName().ToStdString();
            UObjectReference objRef = Package.FindObjectOfType(fullObjName, "Texture2D", true);
            if (objRef == 0)
            {
                std::cerr << "Error: export object not found: " << fullObjName << " (in " << nextFileName << ")" << std::endl;
                failedToExportObjs.Add(fullObjName + "(in " + nextFileName + ")");
                if (exitOnError)
                    return 1;
                continue;
            }
            std::cout << "Texture2D object found: " << fullObjName << std::endl;
            if (!WriteTextureObject(objRef, Package, T2DFile, header, mipMaps, LODBias, resetLOD))
            {
                std::cerr << "Error importing texture data: " << fullObjName << " (in " << nextFileName << ")" << std::endl;
                failedToExportObjs.Add(fullObjName + "(in " + nextFileName + ")");
                if (exitOnError)
                    return 1;
                continue;
            }
        }
        ///time to save the package
        if (ddsFilesList.size() > 0)
        {
            wxFileName relFileName(nextFileName);
            relFileName.MakeRelativeTo(upkDirName);
            wxFileName absFileName(outputDirName + "/" + relFileName.GetFullPath());
            //if (upkDirName == outputDirName)
            if (absFileName.FileExists() && !overwriteFiles)
            {
                wxString backupFileName = wxFileName(absFileName.GetFullPath() + ".bak").GetFullPath();
                if (!wxCopyFile(nextFileName, backupFileName))
                {
                    std::cerr << "Error writing backup to: " << backupFileName << std::endl;
                    failedToProcess.Add(nextFileName);
                    if (exitOnError)
                        return 1;
                    continue;
                }
                std::cout << "Backup saved to: " << backupFileName << std::endl;
            }
            else if (!absFileName.DirExists())
            {
                absFileName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }
            wxString modifiedPackageName = absFileName.GetFullPath();
            if (Package.SaveFileAs(modifiedPackageName.ToStdString().c_str()))
                std::cout << "Modified package saved to: " << modifiedPackageName << std::endl;
            else
            {
                std::cerr << "Error writing modified package to disk: " << modifiedPackageName << std::endl;
                failedToProcess.Add(nextFileName);
                if (exitOnError)
                    return 1;
            }
        }
    }
    ///save the custom texture file on disk
    /*if (wxFileName::FileExists(customTFCFileName))
    {
        wxString backupFileName = wxFileName(customTFCFileName + ".bak").GetFullPath();
        if (!wxCopyFile(customTFCFileName, backupFileName) && !overwriteFiles)
            std::cerr << "Error writing backup to: " << backupFileName << std::endl;
        std::cout << "Backup saved to: " << backupFileName << std::endl;
    }*/
    if (T2DFile.SaveOnDisk())
        std::cout << "Custom tfc saved to: " << customTFCFileName << std::endl;
    else
        std::cerr << "Error writing custom tfc to disk: " << customTFCFileName << std::endl;
    ///report on failed imports if any
    if (failedToProcess.size() > 0)
    {
        std::cout << "Failed to process packages (" << failedToProcess.size() << "):\n";
        for (unsigned i = 0; i < failedToProcess.size(); ++i)
            std::cout << failedToProcess[i] << std::endl;
    }
    if (failedToExportObjs.size() > 0)
    {
        std::cout << "Failed to import textures (" << failedToExportObjs.size() << "):\n";
        for (unsigned i = 0; i < failedToExportObjs.size(); ++i)
            std::cout << failedToExportObjs[i] << std::endl;
    }
    return 0;

    /*cout << "ImportTexturesFromDDS" << endl;

    if (argN != 3)
    {
        cerr << "Usage: ImportTexturesFromDDS UnpackedResourceFile.upk Full.Texture.Name" << endl;
        return 1;
    }

    UPKUtils package(argV[1]);

    UPKReadErrors err = package.GetError();

    if (err != UPKReadErrors::NoErrors)
    {
        cerr << "Error reading package:\n" << FormatReadErrors(err);
        if (package.IsCompressed())
            cerr << "Compression flags:\n" << FormatCompressionFlags(package.GetCompressionFlags());
        return 1;
    }

    string NameToFind = argV[2];
    cout << "Name to find: " << NameToFind << endl;

    UObjectReference ObjRef = package.FindObject(NameToFind, false);
    if (ObjRef == 0)
    {
        cerr << "Can't find object entry by name " << NameToFind << endl;
        return 1;
    }
    if (ObjRef < 0)
    {
        cerr << "Found Import Object: cannot deserialize import objects!\n";
        return 1;
    }
    if (package.GetExportEntry(ObjRef).Type != "Texture2D")
    {
        cerr << "Object is not of Texture2D type!\n";
        return 1;
    }
    cout << "Export Object found!\n";

    ReadDDS(ObjRef, package);

    return 0;*/
}

bool ReadInventory(wxString filename, std::map<wxString, wxArrayString>& inv)
{
    std::ifstream invFile(filename.ToStdString());
    if (!invFile.is_open())
    {
        std::cerr << "Error: cannot open " << filename << std::endl;
        return false;
    }
    inv.clear();
    while (invFile.good())
    {
        wxString key;
        std::string line;
        std::getline(invFile, line);
        if (!invFile.good())
            break;
        size_t pos = 0;
        while (pos < line.size())
        {
            std::string word;
            size_t found = line.find(';', pos);
            if (found != std::string::npos)
            {
                word = line.substr(pos, found - pos);
                pos = found + 1;
            }
            else
            {
                word = line.substr(pos, line.size() - pos);
                pos = line.size() + 1;
            }
            if (word.size() == 0)
                continue;
            if (key.size() == 0)
                key = word;
            else
                inv[key].Add(word);
        }
    }
    return true;
}

bool CatalogueFilesToProcess(wxString ddsDir, wxString ddsMask, wxString upkDir, wxString upkMask, std::map<wxString, wxArrayString>& ddsInv, std::map<wxString, wxArrayString>& upkInv)
{
    wxArrayString ddsToProcess = TraverseDir(ddsDir, ddsMask);
    if (ddsToProcess.size() == 0)
    {
        std::cerr << "Error: missing dds file(s) to process!" << std::endl;
        return false;
    }
    wxArrayString upkToProcess = TraverseDir(upkDir, upkMask);
    if (upkToProcess.size() == 0)
    {
        std::cerr << "Error: missing package(s) to process!" << std::endl;
        return false;
    }
    upkInv.clear();
    /// just one file and one package, skipping the testing
    if (ddsToProcess.size() == 1 && upkToProcess.size() == 1)
    {
        wxFileName relUpkName(upkToProcess[0]);
        relUpkName.MakeRelativeTo(upkDir);
        wxFileName relDdsName(ddsToProcess[0]);
        relDdsName.MakeRelativeTo(ddsDir);
        upkInv[relUpkName.GetFullPath()].Add(relDdsName.GetFullPath());
        return true;
    }
    if (ddsInv.size() == 0)
    {
        std::cerr << "Error: bad inventory!" << std::endl;
        return false;
    }
    std::map<wxString, wxString> upkPathInv;
    for (unsigned i = 0; i < upkToProcess.size(); ++i)
    {
        wxString upkName = wxFileName(upkToProcess[i]).GetName();
        upkPathInv[upkName] = upkToProcess[i];
    }
    /// for each package to process build a list of objects based on dds inventory
    for (unsigned i = 0; i < ddsToProcess.size(); ++i)
    {
        /// derive texture object name from full path
        wxString objName = wxFileName(ddsToProcess[i]).GetName();
        if (ddsInv.count(objName) == 0)
        {
            std::cerr << "Error: bad texture object name or bad inventory: " << objName << std::endl;
            return false;
        }
        /// extract package names from the inventory
        wxArrayString upkNames = ddsInv[objName];
        for (unsigned j = 0; j < upkNames.size(); ++j)
        {
            /// check if this package is present in processing queue and add it to the inventory
            if (upkPathInv.count(upkNames[j]) > 0)
            {
                wxFileName relUpkName(upkPathInv[upkNames[j]]);
                relUpkName.MakeRelativeTo(upkDir);
                wxFileName relDdsName(ddsToProcess[i]);
                relDdsName.MakeRelativeTo(ddsDir);
                upkInv[relUpkName.GetFullPath()].Add(relDdsName.GetFullPath());
            }
        }
    }
    if (upkInv.size() == 0)
    {
        std::cerr << "Error: cannot match textures to packages!" << std::endl;
        return false;
    }
    return true;
}

bool ReadDDSFile(wxString nextDDSFileName, DDSHeader& header, std::vector<UTexture2DMipMap>& mipMaps)
{
    std::ifstream ddsIn(nextDDSFileName.ToStdString(), std::ios::binary);
    if (!ddsIn.is_open())
    {
        std::cerr << "Error opening dds file: " << nextDDSFileName << std::endl;
        return false;
    }

    ///read magic
    uint32_t ddsMagic = 0; ///DDS
    ddsIn.read(reinterpret_cast<char*>(&ddsMagic), 4);
    if (ddsMagic != 0x20534444)
    {
        std::cerr << "Error: missing DDS magic!" << std::endl;
        return false;
    }

    header = MakeDefaultDDSHeader();
    ReadDDSHeader(ddsIn, header);
    if (!ddsIn.good())
    {
        std::cerr << "Error reading header from " << nextDDSFileName << std::endl;
        return false;
    }

    std::string pixelFormat = GetPixelFormatStringFromDDSHeader(header);
    ///calculate linear size if not set
    if (header.PitchOrLinearSize == 0)
    {
        ///uncompressed grayscale data: 1 byte per pixel
        if (pixelFormat == "PF_G8")
            header.PitchOrLinearSize = header.Width * header.Height;
        /// uncompressed ARBG data: 4 bytes per pixel
        else if (pixelFormat == "PF_A8R8G8B8")
            header.PitchOrLinearSize = header.Width * header.Height * 4;
        /// compressed texels
        else if (pixelFormat == "PF_DXT1") ///8 bytes per texel (4x4 pixel block)
            header.PitchOrLinearSize = header.Width * header.Height / (4 * 4) * 8;
        else if (pixelFormat == "PF_DXT3" || pixelFormat == "PF_DXT5") ///16 bytes per texel (4x4 pixel block)
            header.PitchOrLinearSize = header.Width * header.Height / (4 * 4) * 16;
    }

    ///min mipmap size
    uint32_t minSize = 1;
    if (pixelFormat == "PF_G8" || pixelFormat == "PF_A8R8G8B8")
        minSize = 1;
    else if (pixelFormat == "PF_DXT1" || pixelFormat == "PF_DXT3" || pixelFormat == "PF_DXT5")
        minSize = 4;

    ///read chunks from the dds
    uint32_t curW = header.Width, curH = header.Height, curSize = header.PitchOrLinearSize;
    mipMaps.clear();
    mipMaps.resize(header.MipMapCount);
    for (unsigned i = 0; i < mipMaps.size(); ++i)
    {
        std::vector<char> BulkData(curSize);
        ddsIn.read(BulkData.data(), BulkData.size());
        if (!ddsIn.good())
        {
            std::cerr << "Error reading mipmaps from " << nextDDSFileName << std::endl;
            return false;
        }
        mipMaps[i].SetBulkDataRaw(BulkData);
        mipMaps[i].SetSizeX(curW);
        mipMaps[i].SetSizeY(curH);
        if (curW > minSize)
        {
            curW >>= 1;
            curSize >>= 1;
        }
        if (curH > minSize)
        {
            curH >>= 1;
            curSize >>= 1;
        }
    }
    return true;
}

bool WriteTextureObject(UObjectReference ObjRef, UPKUtils& package, CustomTFC& tfc, DDSHeader& header, std::vector<UTexture2DMipMap>& mipMaps, int32_t LODBias, bool resetLOD)
{
    /// find the existing texture
    UTexture2D* texture = dynamic_cast<UTexture2D*>(package.DeserializeObjectByRef(ObjRef, true));
    if (texture == nullptr)
    {
        std::cerr << "Error deserializing Texture2D object: " << package.ResolveFullName(ObjRef) << "!\n";
        return false;
    }
    /// make a new texture object
    UTexture2D* newTexture = new UTexture2D;
    if (newTexture == nullptr)
    {
        std::cerr << "Memory error!\n";
        delete texture;
        return false;
    }
    ///make a copy of the old texture
    (*newTexture) = (*texture);
    ///adjust texture params
    bool texturesAreIdentical = true;
    newTexture->SetPixelFormat(GetPixelFormatStringFromDDSHeader(header));
    std::cout << "Pixel format for " << package.ResolveFullName(ObjRef) << ": " << texture->GetPixelFormat() << std::endl;
    if (texture->GetPixelFormat() != newTexture->GetPixelFormat())
    {
        std::cerr << "Error: PixelFormat mismatch! Old PixelFormat = " << texture->GetPixelFormat() << ". New PixelFormat = " << newTexture->GetPixelFormat() << std::endl;
        delete texture;
        delete newTexture;
        return false;
    }
    newTexture->SetWidth(header.Width);
    if (texture->GetWidth() != newTexture->GetWidth())
        texturesAreIdentical = false;
    newTexture->SetHeight(header.Height);
    if (texture->GetHeight() != newTexture->GetHeight())
        texturesAreIdentical = false;
    newTexture->SetMipMapCount(header.MipMapCount);
    if (texture->GetMipMapCount() != newTexture->GetMipMapCount())
        texturesAreIdentical = false;
    ///set mipmaps
    newTexture->SetMipMaps(mipMaps);
    ///get export entry data
    FObjectExport textureExportEntry = package.GetExportEntry(ObjRef);
    ///get original serialized data
    std::string originalExportDataStr = package.GetUObjectSerializedData(ObjRef);
    ///make new export data
    std::string newExportDataStr = originalExportDataStr;
    ///adjust defaultproperties
    if (!texturesAreIdentical)
    {
        ///for BatmanAC def props are serialized by offset, so searching for a def prop entry by type + offset (4 bytes) is unsafe (not unique enough combination)
        ///this is why for that one full prop is reconstructed and replaced old -> new
        if (texture->GetWidth() != newTexture->GetWidth())
        {
            UDefaultProperty newProp;
            newProp.MakeIntProperty("SizeX", newTexture->GetWidth(), package);
            if (package.GetVersion() == VER_BATMAN_CITY)
            {
                UDefaultProperty oldProp;
                oldProp.MakeIntProperty("SizeX", texture->GetWidth(), package);
                package.ReplaceProperty(oldProp, newProp, ObjRef, newExportDataStr);
            }
            else
                package.ReplacePropertyValue(newProp, ObjRef, newExportDataStr);
        }
        if (texture->GetHeight() != newTexture->GetHeight())
        {
            UDefaultProperty newProp;
            newProp.MakeIntProperty("SizeY", newTexture->GetHeight(), package);
            if (package.GetVersion() == VER_BATMAN_CITY)
            {
                UDefaultProperty oldProp;
                oldProp.MakeIntProperty("SizeY", texture->GetHeight(), package);
                package.ReplaceProperty(oldProp, newProp, ObjRef, newExportDataStr);
            }
            else
                package.ReplacePropertyValue(newProp, ObjRef, newExportDataStr);
        }
        if (texture->GetMipMapCount() != newTexture->GetMipMapCount())
        {
            UDefaultProperty newProp;
            newProp.MakeIntProperty("MipTailBaseIdx", newTexture->GetMipMapCount() - 1, package);
            if (package.GetVersion() == VER_BATMAN_CITY)
            {
                UDefaultProperty oldProp;
                oldProp.MakeIntProperty("MipTailBaseIdx", texture->GetMipMapCount() - 1, package);
                package.ReplaceProperty(oldProp, newProp, ObjRef, newExportDataStr);
            }
            else
                package.ReplacePropertyValue(newProp, ObjRef, newExportDataStr);
        }
    }
    ///adjust LODBias
    if (package.GetVersion() == VER_BATMAN_CITY)
    {
        ///Doesn't seem to have LODBias var
    }
    else
    {
        if (resetLOD)
        {
            UDefaultProperty prop;
            prop.MakeIntProperty("LODBias", LODBias, package);
            package.RemoveProperty(prop, ObjRef, newExportDataStr);
        }
        else if (LODBias != LOD_BIAS_BAD)
        {
            UDefaultProperty prop;
            prop.MakeIntProperty("LODBias", LODBias, package);
            if (!package.ReplacePropertyValue(prop, ObjRef, newExportDataStr))
                package.InsertProperty(prop, ObjRef, newExportDataStr);
        }
    }
    ///mipmaps were in an external tfc originally
    if (texture->GetTextureFileCacheName() != "")
    {
        ///set new tfc name
        newTexture->SetTextureFileCacheName(wxFileName(tfc.GetFilename()).GetName().ToStdString());
        ///adjust default properties
        UDefaultProperty newProp;
        newProp.MakeNameProperty("TextureFileCacheName", newTexture->GetTextureFileCacheName(), package);
        if (package.GetVersion() == VER_BATMAN_CITY)
        {
            UDefaultProperty oldProp;
            oldProp.MakeNameProperty("TextureFileCacheName", texture->GetTextureFileCacheName(), package);
            package.ReplaceProperty(oldProp, newProp, ObjRef, newExportDataStr);
        }
        else
            package.ReplacePropertyValue(newProp, ObjRef, newExportDataStr);
        ///try compression
        if (texture->GetHasCompressedMipMaps())
        {
            int minResForCompression = newTexture->GetMinObservedResForCompression();
            std::cout << "Min observed res for mipmap compression for " << package.ResolveFullName(ObjRef) << ": " << minResForCompression << std::endl;
            if (minResForCompression > -1)
            {
                if (newTexture->TryLzoCompression(minResForCompression))
                {
                    ///try exporting compressed textures to the new tfc
                    if (!newTexture->ExportToExternalFile(tfc, package, true))
                    {
                        std::cerr << "Error writing external texture file " << tfc.GetFilename() << std::endl;
                        delete texture;
                        delete newTexture;
                        return false;
                    }
                }
            }
        }
        if (package.GetVersion() == VER_BATMAN_CITY && texture->GetNumTFCMipMaps() > -1)
        {
            UDefaultProperty newProp, oldProp;
            if (newTexture->GetNumTFCMipMaps() > -1)
                newProp.MakeIntProperty("NumTFCMipMaps", newTexture->GetNumTFCMipMaps(), package);
            else
            {
                newProp.MakeIntProperty("NumTFCMipMaps", 0, package);
                std::cerr << "Warning: potential issues with texture compression and/or tfc! Texture2D object: " << package.ResolveFullName(ObjRef) << std::endl;
            }
            oldProp.MakeIntProperty("NumTFCMipMaps", texture->GetNumTFCMipMaps(), package);
            package.ReplaceProperty(oldProp, newProp, ObjRef, newExportDataStr);
        }
    }
    ///check if export object resize is needed
    uint32_t t2DataSize = newExportDataStr.size() + newTexture->CalculateTexture2DDataSize();
    if (textureExportEntry.SerialSize != t2DataSize)
    {
        package.MoveResizeObject(ObjRef, t2DataSize);
        textureExportEntry = package.GetExportEntry(ObjRef);
    }
    ///get serialized data
    size_t offset = textureExportEntry.SerialOffset + newExportDataStr.size();
    std::string t2dData = newExportDataStr + newTexture->SerializeTexture2DData(offset);
    ///rewriting actual data in the package
    std::vector<char> dataToWrite(t2dData.begin(), t2dData.end());
    if (!package.WriteExportData(ObjRef, dataToWrite))
    {
        std::cerr << "Error writing texture data for " << package.ResolveFullName(ObjRef) << "!" << std::endl;
        delete texture;
        delete newTexture;
        return false;
    }
    ///cleanup
    delete texture;
    delete newTexture;
    return true;
}

/*void ReadDDS(UObjectReference &ObjRef, UPKUtils &package)
{
    /// find the existing texture
    UTexture2D* texture = dynamic_cast<UTexture2D*>(package.DeserializeObjectByRef(ObjRef));
    if (texture == nullptr)
    {
        cerr << "Error deserializing Texture2D object!\n";
        return;
    }

    /// read the replacement one from the dds
    std::string filename = package.GetExportEntry(ObjRef).FullName + ".dds";
    ifstream ddsIn(filename, ios::binary);
    if (!ddsIn.is_open())
    {
        cerr << "Error opening dds file: " << filename << endl;
        delete texture;
        return;
    }

    uint32_t ddsMagic = 0; ///DDS

    ///read magic
    ddsIn.read(reinterpret_cast<char*>(&ddsMagic), 4);
    if (ddsMagic != 0x20534444)
    {
        cerr << "Missing DDS magic!" << endl;
        delete texture;
        return;
    }

    DDSHeader header;

    ReadDDSHeader(ddsIn, header);

    string pixelFormat = GetPixelFormatStringFromDDSHeader(header);

    ///calculate linear size if not set
    if (header.PitchOrLinearSize == 0)
    {
        ///uncompressed grayscale data: 1 byte per pixel
        if (pixelFormat == "PF_G8")
        {
            header.PitchOrLinearSize = header.Width * header.Height;
        }
        /// uncompressed ARBG data: 4 bytes per pixel
        else if (pixelFormat == "PF_A8R8G8B8")
        {
            header.PitchOrLinearSize = header.Width * header.Height * 4;
        }
        /// compressed texels
        else if (pixelFormat == "PF_DXT1") ///8 bytes per texel (4x4 pixel block)
        {
            header.PitchOrLinearSize = header.Width * header.Height / (4 * 4) * 8;
        }
        else if (pixelFormat == "PF_DXT3" || pixelFormat == "PF_DXT5") ///16 bytes per texel (4x4 pixel block)
        {
            header.PitchOrLinearSize = header.Width * header.Height / (4 * 4) * 16;
        }
    }

    /// make a new texture object
    UTexture2D* newTexture = new UTexture2D;
    if (newTexture == nullptr)
    {
        cerr << "Memory error!\n";
        delete texture;
        return;
    }

    ///make a copy of the old texture
    (*newTexture) = (*texture);

    bool texturesAreIdentical = true;

    ///adjust texture params
    newTexture->SetPixelFormat(pixelFormat);
    cout << "PixelFormat = " << newTexture->GetPixelFormat() << endl;
    if (texture->GetPixelFormat() != newTexture->GetPixelFormat())
    {
        cerr << "PixelFormat mismatch! Old PixelFormat = " << texture->GetPixelFormat() << endl;
        delete texture;
        delete newTexture;
        return;
    }
    newTexture->SetWidth(header.Width);
    cout << "Width = " << newTexture->GetWidth() << endl;
    if (texture->GetWidth() != newTexture->GetWidth())
    {
        texturesAreIdentical = false;
        cout << "Old Width = " << texture->GetWidth() << endl;
    }
    newTexture->SetHeight(header.Height);
    cout << "Height = " << newTexture->GetHeight() << endl;
    if (texture->GetHeight() != newTexture->GetHeight())
    {
        texturesAreIdentical = false;
        cout << "Old Height = " << texture->GetHeight() << endl;
    }
    newTexture->SetMipMapCount(header.MipMapCount);
    cout << "MipMapCount = " << newTexture->GetMipMapCount() << endl;
    if (texture->GetMipMapCount() != newTexture->GetMipMapCount())
    {
        texturesAreIdentical = false;
        cout << "Old MipMapCount = " << texture->GetMipMapCount() << endl;
    }

    ///read chunks from the dds
    uint32_t curW = header.Width, curH = header.Height, curSize = header.PitchOrLinearSize;
    vector<UTexture2DMipMap> mipMaps(newTexture->GetMipMapCount());
    for (unsigned i = 0; i < mipMaps.size(); ++i)
    {
        vector<char> BulkData(curSize);
        ddsIn.read(BulkData.data(), BulkData.size());
        mipMaps[i].SetBulkDataRaw(BulkData);
        mipMaps[i].SetSizeX(curW);
        mipMaps[i].SetSizeY(curH);
        if (curW > 4)
        {
            curW >>= 1;
            curSize >>= 1;
        }
        if (curH > 4)
        {
            curH >>= 1;
            curSize >>= 1;
        }
        ///// testing compression
        //vector<char> CompressedBulkData, DecompressedBulkData;
        //cout << "raw = " << BulkData.size() << " compr = " << LzoCompress(BulkData, CompressedBulkData) << endl;
        //cout << "compr = " << CompressedBulkData.size() << " decompr = " << DecompressLzoCompressedRawData(CompressedBulkData, DecompressedBulkData) << endl;
    }
    newTexture->SetMipMaps(mipMaps);

    ///write the new texture to package
    ///get export entry data
    FObjectExport textureExportEntry = package.GetExportEntry(ObjRef);
    ///get original serialized data
    string originalExportDataStr = package.GetUObjectSerializedData(ObjRef);
    ///make new export data
    string newExportDataStr = originalExportDataStr;

    if (!texturesAreIdentical)
    {
        cout << "Texture params have changed, adjusting.\n";
        ///different texture params, need to adjust defaultproperties
        //if (texture->GetPixelFormat() != newTexture->GetPixelFormat())
        //{
        //    cout << "Adjusting pixel format.\n";
        //    UDefaultProperty prop;
        //    prop.MakeByteProperty("Format", "EPixelFormat", newTexture->GetPixelFormat(), package);
        //    package.ReplacePropertyValue(prop, ObjRef, newExportDataStr);
        //}
        if (texture->GetWidth() != newTexture->GetWidth())
        {
            cout << "Adjusting width.\n";
            UDefaultProperty prop;
            prop.MakeIntProperty("SizeX", newTexture->GetWidth(), package);
            package.ReplacePropertyValue(prop, ObjRef, newExportDataStr);
        }
        if (texture->GetHeight() != newTexture->GetHeight())
        {
            cout << "Adjusting height.\n";
            UDefaultProperty prop;
            prop.MakeIntProperty("SizeY", newTexture->GetHeight(), package);
            package.ReplacePropertyValue(prop, ObjRef, newExportDataStr);
        }
        if (texture->GetMipMapCount() != newTexture->GetMipMapCount())
        {
            cout << "Adjusting mipmap count.\n";
            UDefaultProperty prop;
            prop.MakeIntProperty("MipTailBaseIdx", newTexture->GetMipMapCount() - 1, package);
            package.ReplacePropertyValue(prop, ObjRef, newExportDataStr);
        }
    }

    ///temporarily: all the mipmaps are embedded, remove the tfc name, add NeverStream
    //if (texture->GetTextureFileCacheName() != "")
    //{
    //    cout << "Adjusting tfc name.\n";
    //    UDefaultProperty prop1, prop2, prop3;
    //    prop1.MakeNameProperty("TextureFileCacheName", "Textures", package);
    //    package.RemoveProperty(prop1, ObjRef, newExportDataStr);
    //    prop2.MakeByteProperty("LODGroup", "TextureGroup", "", package);
    //    prop3.MakeBoolProperty("NeverStream", true, package);
    //    package.InsertProperty(prop3, prop2, ObjRef, newExportDataStr);
    //    texture->SetTextureFileCacheName("");
    //    texture->SetNeverStream(true);
    //}

    ///mipmaps were in an external tfc originally
    if (texture->GetTextureFileCacheName() != "")
    {
        ///set new tfc name
        newTexture->SetTextureFileCacheName("Texture2D");
        ///adjust default properties
        cout << "Adjusting tfc name.\n";
        UDefaultProperty prop1;
        prop1.MakeNameProperty("TextureFileCacheName", newTexture->GetTextureFileCacheName(), package);
        package.ReplacePropertyValue(prop1, ObjRef, newExportDataStr);
        ///try compression
        newTexture->TryLzoCompression(256);
        ///try exporting to the new tfc
        CustomTFC T2DFile;
        if (!T2DFile.Read("Texture2D.tfc"))
        {
            cerr << "Export to Texture2D.tfc failed!" << endl;
        }
        newTexture->ExportToExternalFile(T2DFile, package, true);
    }

    ///check if export object resize is needed
    uint32_t t2DataSize = newExportDataStr.size() + newTexture->CalculateTexture2DDataSize();
    if (textureExportEntry.SerialSize != t2DataSize)
    {
        cout << "Export data moved: oldOffset = " << textureExportEntry.SerialOffset;
        package.MoveResizeObject(ObjRef, t2DataSize);
        textureExportEntry = package.GetExportEntry(ObjRef);
        cout << "; newOffset = " << textureExportEntry.SerialOffset << endl;
    }

    ///get serialized data
    size_t offset = textureExportEntry.SerialOffset + newExportDataStr.size();
    string t2dData = newExportDataStr + newTexture->SerializeTexture2DData(offset);

    ///rewriting actual data in the package
    std::vector<char> dataToWrite(t2dData.begin(), t2dData.end());
    if (package.WriteExportData(ObjRef, dataToWrite))
        cout << "Data write successful." << endl;
    else
        cerr << "Error writing texture data!" << endl;

    ///testing

    //filename = textureExportEntry.FullName + "-copy" + ".Texture2D";
    //ofstream out2(filename.c_str(), ios::binary);
    //out2.write(dataToWrite.data(), dataToWrite.size());

    //filename = textureExportEntry.FullName + "-copy" + ".dds";
    //ofstream out(filename.c_str(), ios::binary);
    ///write magic
    //out.write(reinterpret_cast<char*>(&ddsMagic), 4);
    ///write header
    //WriteDDSHeader(out, header);
    ///write chunks
    //mipMaps = newTexture->GetMipMaps();
    //for (unsigned i = 0; i < mipMaps.size(); ++i)
    //{
    //    vector<char> BulkData = mipMaps[i].GetBulkData();
    //    out.write(BulkData.data(), BulkData.size());
    //}
    //out.close();
    //cout << "DDS file saved: " << filename << endl;

    delete texture;
    delete newTexture;
}*/
