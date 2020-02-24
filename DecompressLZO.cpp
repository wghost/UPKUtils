#include "UPKUtils.h"
#include "FileUtils.h"

#include <wx/cmdline.h>

int main(int argN, char* argV[])
{
    wxMessageOutputStderr OutputStderr;
    wxMessageOutput::Set(&OutputStderr);
    /// static logo text
    static const std::string myLogo = "DecompressLZO by wghost81 aka Wasteland Ghost";
    /// parse command line
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_PARAM,  NULL, NULL,        "<input file or pattern>", wxCMD_LINE_VAL_STRING },
        { wxCMD_LINE_SWITCH, "h", "help",       "get usage help (this text)", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_OPTION, "i", "input",      "set input dir" },
        { wxCMD_LINE_OPTION, "o", "output",     "set output dir" },
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
    wxString inputDirName;
    if (!cmdLineParser.Found("input", &inputDirName))
        inputDirName = wxFileName(cmdLineParser.GetParam()).GetPath();
    if (inputDirName == "")
        inputDirName = ".";
    inputDirName = wxFileName(inputDirName).GetFullPath();
    std::cout << "Input dir name: " << inputDirName << std::endl;
    wxString outputDirName;
    if (!cmdLineParser.Found("output", &outputDirName))
        outputDirName = ".";
    outputDirName = wxFileName(outputDirName).GetFullPath();
    if (!wxDirExists(outputDirName) && !wxMkdir(outputDirName))
    {
        std::cerr << "Output directory does not exist: " + outputDirName << std::endl;
        return 1;
    }
    std::cout << "Output dir name: " << outputDirName << std::endl;
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
        std::cout << "Package file full path: " << upkFileName << std::endl;
        filesToProcess.Add(upkFileName);
    }
    wxArrayString failedToProcess;
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
                std::cerr << "Cannot find package file: " << nextFileName << std::endl;
                failedToProcess.Add(nextFileName);
                continue;
            }
        }
        std::cout << "Next package file: " << nextFileName << std::endl;
        UPKUtils Package(nextFileName.c_str());
        UPKReadErrors err = Package.GetError();
        if (err != UPKReadErrors::NoErrors && !Package.IsCompressed())
        {
            std::cerr << "Error reading package:\n" << FormatReadErrors(err);
            failedToProcess.Add(nextFileName);
            continue;
        }
        if (!Package.IsCompressed())
        {
            std::cerr << "Package is not compressed!\n";
            failedToProcess.Add(nextFileName);
            continue;
        }
        std::cout << "Decompressing package...\n";
        if (!Package.DecompressPackage())
        {
            std::cerr << "Error decompressing package!\n";
            failedToProcess.Add(nextFileName);
            continue;
        }
        std::cout << "Done.\n";
        wxFileName relFileName(nextFileName);
        relFileName.MakeRelativeTo(inputDirName);
        wxFileName absFileName(outputDirName + "/" + relFileName.GetFullPath());
        /// writing to the same dir, need to make backups
        if (inputDirName == outputDirName)
        {
            wxString backupFileName = wxFileName(absFileName.GetFullPath() + ".bak").GetFullPath();
            if (!wxCopyFile(nextFileName, backupFileName))
            {
                std::cerr << "Error writing backup to: " + backupFileName << std::endl;
                failedToProcess.Add(nextFileName);
                continue;
            }
            std::cout << "Backup saved to: " << backupFileName << std::endl;
        }
        else
        {
            absFileName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        wxString decompressedFileName = absFileName.GetFullPath();
        if (Package.SaveFileAs(decompressedFileName.c_str()))
            std::cout << "Decompressed package saved to: " << decompressedFileName << std::endl;
        else
        {
            std::cerr << "Error writing decompressed package to disk: " << decompressedFileName << std::endl;
            failedToProcess.Add(decompressedFileName);
        }
    }
    if (failedToProcess.size() > 0)
    {
        std::cout << "Failed to process packages:\n";
        for (unsigned i = 0; i < failedToProcess.size(); ++i)
            std::cout << failedToProcess[i] << std::endl;
    }
    return 0;
}
