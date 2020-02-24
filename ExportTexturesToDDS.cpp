#include <iostream>
#include <sstream>
#include <map>

#include "UPKUtils.h"
#include "FileUtils.h"
#include "dds.h"

#include <wx/cmdline.h>

bool SaveDDS(UObjectReference ObjRef, UPKUtils& package, std::string filename);
std::string FormatMap(std::map<wxString, wxArrayString> aMap);

int main(int argN, char* argV[])
{
    wxMessageOutputStderr OutputStderr;
    wxMessageOutput::Set(&OutputStderr);
    /// static logo text
    static const std::string myLogo = "ExportTexturesToDDS by wghost81 aka Wasteland Ghost";
    /// parse command line
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_PARAM,  NULL, NULL,        "<input file or pattern>", wxCMD_LINE_VAL_STRING },
        { wxCMD_LINE_PARAM,  NULL, NULL,        "object name or pattern", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_SWITCH, "h", "help",       "get usage help (this text)", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_OPTION, "i", "input",      "set input dir" },
        { wxCMD_LINE_OPTION, "o", "output",     "set output dir" },
        { wxCMD_LINE_SWITCH, "k", "keep",       "keep input dir structure for output dir" },
        { wxCMD_LINE_SWITCH, "w", "overwrite",  "overwrite existing dds files" },
        { wxCMD_LINE_OPTION, "t", "tfc",        "set tfc dir (by default input dir is used)" },
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
    wxFileName tmpFname;
    /// directories
    wxString inputDirName;
    if (!cmdLineParser.Found("input", &inputDirName))
        inputDirName = wxFileName(cmdLineParser.GetParam()).GetPath();
    if (inputDirName == "")
        inputDirName = ".";
    tmpFname = inputDirName; tmpFname.MakeAbsolute();
    inputDirName = tmpFname.GetFullPath();
    std::cout << "Input dir name: " << inputDirName << std::endl;
    wxString outputDirName;
    if (!cmdLineParser.Found("output", &outputDirName))
        outputDirName = ".";
    tmpFname = outputDirName; tmpFname.MakeAbsolute();
    outputDirName = tmpFname.GetFullPath();
    if (!wxDirExists(outputDirName) && !wxMkdir(outputDirName))
    {
        std::cerr << "Output directory does not exist: " + outputDirName << std::endl;
        return 1;
    }
    std::cout << "Output dir name: " << outputDirName << std::endl;
    wxString tfcDirName;
    if (!cmdLineParser.Found("tfc", &tfcDirName))
        tfcDirName = inputDirName;
    tmpFname = tfcDirName; tmpFname.MakeAbsolute();
    tfcDirName = tmpFname.GetFullPath();
    std::cout << "tfc dir name: " << tfcDirName << std::endl;
    /// upk file name
    wxString upkFileName = wxFileName(inputDirName + "/" + wxFileName(cmdLineParser.GetParam()).GetFullName()).GetFullPath();
    /// get the names of all files in the array
    wxString fileMask = wxFileName(upkFileName).GetFullName();
    wxArrayString filesToProcess;
    if (wxIsWild(fileMask))
    {
        std::cout << "File mask: " << fileMask << std::endl;
        wxDirTraverserSimple traverser(filesToProcess, fileMask);
        wxDir dir(inputDirName);
        dir.Traverse(traverser);
    }
    else
    {
        std::cout << "Upk file full path: " << upkFileName << std::endl;
        filesToProcess.Add(upkFileName);
    }
    /// get the name/mask of object(s) to find
    wxString objectName = "*";
    if (cmdLineParser.GetParamCount() > 1)
        objectName = cmdLineParser.GetParam(1);
    if (wxIsWild(objectName))
        std::cout << "Object mask: " << objectName << std::endl;
    else
        std::cout << "Object to find: " << objectName << std::endl;
    /// set cwd for proper tfc path
    wxSetWorkingDirectory(tfcDirName);
    /// process packages
    wxArrayString failedToProcess, failedToExportObjs;
    std::map<wxString, wxArrayString> texturesInventory;
    for (unsigned i = 0; i < filesToProcess.size(); ++i)
    {
        wxString nextFileName = filesToProcess[i];
        if (!wxFileExists(nextFileName))
        {
            wxString found = wxDir::FindFirst(inputDirName, wxFileName(nextFileName).GetFullName());
            if (found != "")
                nextFileName = found;
            else
            {
                std::cerr << "Cannot find upk file: " + nextFileName << std::endl;
                failedToProcess.Add(nextFileName);
                continue;
            }
        }
        std::cout << "Next upk file: " << nextFileName << std::endl;
        wxFileName absDirName;
        if (cmdLineParser.Found("keep"))
        {
            wxFileName relFileName(nextFileName);
            relFileName.MakeRelativeTo(inputDirName);
            absDirName = wxFileName(outputDirName + "/" + relFileName.GetPath() + "/" + relFileName.GetName(), "");
        }
        else
            absDirName = wxFileName(outputDirName, "");
        UPKUtils Package(nextFileName.c_str());
        UPKReadErrors err = Package.GetError();
        if (err != UPKReadErrors::NoErrors && !Package.IsCompressed())
        {
            std::cerr << "Error reading package:\n" << FormatReadErrors(err);
            failedToProcess.Add(nextFileName);
            continue;
        }
        if (Package.IsCompressed())
        {
            std::cout << "Package is compressed, decompressing...\n";
            if (!Package.DecompressPackage())
            {
                std::cerr << "Error decompressing package!\n";
                failedToProcess.Add(nextFileName);
                continue;
            }
            std::cout << "Done.\n";
        }
        std::vector<FObjectExport> ExportTable = Package.GetExportTable();
        for (unsigned j = 1; j < ExportTable.size(); ++j)
        {
            if (ExportTable[j].Type != "Texture2D" || Package.IsPropertiesObject(j))
                continue;
            if (wxIsWild(objectName) && !wxMatchWild(objectName, ExportTable[j].FullName, false))
                continue;
            if (!wxIsWild(objectName) && ExportTable[j].FullName != objectName)
                continue;
            std::cout << "Texture2D object found: " << ExportTable[j].FullName << std::endl;
            if (!absDirName.DirExists())
                absDirName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            wxString absFileName = wxFileName(absDirName.GetPath() + "/" + ExportTable[j].FullName + ".dds").GetFullPath();
            texturesInventory[ExportTable[j].FullName].Add(wxFileName(nextFileName).GetName());
            if (!cmdLineParser.Found("overwrite") && wxFile::Exists(absFileName) && wxFileName::GetSize(absFileName) != wxInvalidSize)
            {
                std::cout << "Already exported, skipping." << std::endl;
                continue;
            }
            if (!SaveDDS(j, Package, absFileName.ToStdString()))
                failedToExportObjs.Add(ExportTable[j].FullName + "(in " + nextFileName + ")");
        }
    }
    if (failedToProcess.size() > 0)
    {
        std::cout << "Failed to process packages (" << failedToProcess.size() << "):\n";
        for (unsigned i = 0; i < failedToProcess.size(); ++i)
            std::cout << failedToProcess[i] << std::endl;
    }
    if (failedToExportObjs.size() > 0)
    {
        std::cout << "Failed to export textures (" << failedToExportObjs.size() << "):\n";
        for (unsigned i = 0; i < failedToExportObjs.size(); ++i)
            std::cout << failedToExportObjs[i] << std::endl;
    }
    if (texturesInventory.size() > 0 && (wxIsWild(fileMask) || wxIsWild(objectName)))
    {
        std::cout << texturesInventory.size() << " textures found.\n";
        wxFileName invFileName(outputDirName, "inventory.csv");
        unsigned idx = 1;
        while (invFileName.Exists())
        {
            invFileName.SetName("inventory (" + std::to_string(idx) + ")");
            ++idx;
        }
        std::ofstream invFile(invFileName.GetFullPath().ToStdString());
        invFile << FormatMap(texturesInventory);
        invFile.close();
        std::cout << "Inventory saved to " << invFileName.GetFullPath() << std::endl;
    }
    return 0;
}

bool SaveDDS(UObjectReference ObjRef, UPKUtils& package, std::string filename)
{
    ///extract the texture data
    UTexture2D* texture = dynamic_cast<UTexture2D*>(package.DeserializeObjectByRef(ObjRef));
    if (texture == nullptr)
    {
        std::cerr << "Error deserializing Texture2D object!\n";
        return false;
    }

    ///make dds header
    DDSHeader header = MakeDDSHeader(texture->GetPixelFormat());

    /// adjust header
    header.Height = texture->GetHeight();
    header.Width = texture->GetWidth();
    header.Depth = 1;
    header.MipMapCount = texture->GetMipMapCount();
    header.PitchOrLinearSize = texture->GetPitchOrLinearSize();

    ///extract mipmaps
    std::vector<UTexture2DMipMap> mipMaps = texture->GetMipMaps(), mipMapsToSave;

    ///check for empty bulk data
    mipMapsToSave.reserve(mipMaps.size());
    for (unsigned i = 0; i < mipMaps.size(); ++i)
    {
        if(!(mipMaps[i].GetSavedBulkDataFlags() & (uint32_t)UBulkDataFlags::EmptyData || mipMaps[i].GetBulkData().size() == 0))
            mipMapsToSave.push_back(mipMaps[i]);
    }

    if (mipMapsToSave.size() == 0)
    {
        std::cerr << "No mipmas to save to dds!\n";
        delete texture;
        return false;
    }

    if (mipMapsToSave.size() != header.MipMapCount)
    {
        header.MipMapCount = mipMapsToSave.size();
        header.Width = mipMapsToSave[0].GetSizeX();
        header.Height = mipMapsToSave[0].GetSizeY();
        header.PitchOrLinearSize = mipMapsToSave[0].GetSavedElementCount();
    }

    if (mipMapsToSave.size() > 1)
    {
        header.Flags |= (uint32_t)DDSHeaderFlags::DDSD_MIPMAPCOUNT;
        header.Caps |= (uint32_t)DDSCapsFlags::DDSCAPS_MIPMAP;
    }

    uint32_t ddsMagic = 0x20534444; ///DDS

    std::ofstream out(filename.c_str(), std::ios::binary);
    if (!out.is_open())
    {
        std::cerr << "Error opening " << filename << std::endl;
        return false;
    }

    ///write magic
    out.write(reinterpret_cast<char*>(&ddsMagic), 4);

    ///write header
    WriteDDSHeader(out, header);

    ///write chunks
    for (unsigned i = 0; i < mipMapsToSave.size(); ++i)
    {
        std::vector<char> BulkData = mipMapsToSave[i].GetBulkData();
        out.write(BulkData.data(), BulkData.size());
    }

    out.close();
    std::cout << "DDS file saved: " << filename << std::endl;

    delete texture;

    return true;
}

std::string FormatMap(std::map<wxString, wxArrayString> aMap)
{
    std::stringstream res;
    for (std::map<wxString, wxArrayString>::iterator it = aMap.begin(); it != aMap.end(); ++it)
    {
        res << it->first;
        wxArrayString second = it->second;
        for (unsigned i = 0; i < second.size(); ++i)
            res << ";" << second[i];
        res << std::endl;
    }
    return res.str();
}
