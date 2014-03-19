#include <iostream>
#include <iomanip>

#include "UPKInfo.h"
#include <fstream>

using namespace std;

int main(int argN, char* argV[])
{
    cout << "CompareUPK" << endl;

    if (argN != 3)
    {
        cerr << "Usage: CompareUPK OldPackage.upk NewPackage.upk" << endl;
        return 1;
    }

    ifstream OldFile(argV[1], ios::binary);
    if (!OldFile.is_open())
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }
    ifstream NewFile(argV[2], ios::binary);
    if (!NewFile.is_open())
    {
        cerr << "Can't open " << argV[2] << endl;
        return 1;
    }

    UPKInfo OldPackageInfo(OldFile), NewPackageInfo(NewFile);

    UPKReadErrors OldErr = OldPackageInfo.GetError(), NewErr = NewPackageInfo.GetError();

    if (OldErr != UPKReadErrors::NoErrors)
    {
        cerr << "Error reading package: " << argV[1] << std::endl
             << FormatReadErrors(OldErr);
        return 1;
    }
    if (NewErr != UPKReadErrors::NoErrors)
    {
        cerr << "Error reading package: " << argV[2] << std::endl
             << FormatReadErrors(NewErr);
        return 1;
    }

    if (OldPackageInfo.GetSummary().Version != NewPackageInfo.GetSummary().Version)
    {
        cout << "Old Version: " << OldPackageInfo.GetSummary().Version
             << "\tNew Version: " << NewPackageInfo.GetSummary().Version << std::endl;
    }
    if (OldPackageInfo.GetSummary().NameCount != NewPackageInfo.GetSummary().NameCount)
    {
        cout << "Old NameCount: " << OldPackageInfo.GetSummary().NameCount
             << "\tNew NameCount: " << NewPackageInfo.GetSummary().NameCount
             << "\tNew-Old = " << ((int)NewPackageInfo.GetSummary().NameCount - (int)OldPackageInfo.GetSummary().NameCount) << std::endl;
    }
    if (OldPackageInfo.GetSummary().ExportCount != NewPackageInfo.GetSummary().ExportCount)
    {
        cout << "Old ExportCount: " << OldPackageInfo.GetSummary().ExportCount
             << "\tNew ExportCount: " << NewPackageInfo.GetSummary().ExportCount
             << "\tNew-Old = " << ((int)NewPackageInfo.GetSummary().ExportCount - (int)OldPackageInfo.GetSummary().ExportCount) << std::endl;
    }
    if (OldPackageInfo.GetSummary().ImportCount != NewPackageInfo.GetSummary().ImportCount)
    {
        cout << "Old ImportCount: " << OldPackageInfo.GetSummary().ImportCount
             << "\tNew ImportCount: " << NewPackageInfo.GetSummary().ImportCount
             << "\tNew-Old = " << ((int)NewPackageInfo.GetSummary().ImportCount - (int)OldPackageInfo.GetSummary().ImportCount) << std::endl;
    }

    cout << "Analyzing names:\n";

    int numDeletedNames = 0, numNewNames = 0;

    for (unsigned i = 0; i < OldPackageInfo.GetSummary().NameCount; ++i)
    {
        int foundIdx = NewPackageInfo.FindName(OldPackageInfo.GetNameEntry(i).Name);
        if (foundIdx < 0)
        {
            cout << "Deleted name: " << OldPackageInfo.GetNameEntry(i).Name
                 << " (index = " << i << ")\n";
            ++numDeletedNames;
        }
    }

    cout << "Number of deleted names = " << numDeletedNames << std::endl;

    for (unsigned i = 0; i < NewPackageInfo.GetSummary().NameCount; ++i)
    {
        int foundIdx = OldPackageInfo.FindName(NewPackageInfo.GetNameEntry(i).Name);
        if (foundIdx < 0)
        {
            cout << "New name: " << NewPackageInfo.GetNameEntry(i).Name
                 << " (index = " << i << ")\n";
            ++numNewNames;
        }
    }

    cout << "Number of new names = " << numNewNames << std::endl;

    cout << "Analyzing imports:\n";

    int numDeletedImports = 0, numNewImports = 0;

    for (unsigned i = 1; i <= OldPackageInfo.GetSummary().ImportCount; ++i)
    {
        UObjectReference foundIdx = NewPackageInfo.FindObject(OldPackageInfo.GetImportEntry(i).FullName, false);
        if (foundIdx == 0)
        {
            cout << "Deleted import: " << OldPackageInfo.GetImportEntry(i).FullName
                 << " (index = " << i << ")\n";
            ++numDeletedImports;
        }
    }

    cout << "Number of deleted imports = " << numDeletedImports << std::endl;

    for (unsigned i = 1; i <= NewPackageInfo.GetSummary().ImportCount; ++i)
    {
        UObjectReference foundIdx = OldPackageInfo.FindObject(NewPackageInfo.GetImportEntry(i).FullName, false);
        if (foundIdx == 0)
        {
            cout << "New import: " << NewPackageInfo.GetImportEntry(i).FullName
                 << " (index = " << i << ")\n";
            ++numNewImports;
        }
    }

    cout << "Number of new imports = " << numNewImports << std::endl;

    cout << "Analyzing exports:\n";

    int numDeletedExports = 0, numNewExports = 0, numResizedExports = 0;

    for (unsigned i = 1; i <= OldPackageInfo.GetSummary().ExportCount; ++i)
    {
        UObjectReference foundIdx = NewPackageInfo.FindObject(OldPackageInfo.GetExportEntry(i).FullName);
        if (foundIdx == 0)
        {
            cout << "Deleted export: " << OldPackageInfo.GetExportEntry(i).FullName
                 << " (index = " << i << ")\n";
            ++numDeletedExports;
        }
    }

    cout << "Number of deleted exports = " << numDeletedExports << std::endl;

    for (unsigned i = 1; i <= NewPackageInfo.GetSummary().ExportCount; ++i)
    {
        UObjectReference foundIdx = OldPackageInfo.FindObject(NewPackageInfo.GetExportEntry(i).FullName);
        if (foundIdx == 0)
        {
            cout << "New export: " << NewPackageInfo.GetExportEntry(i).FullName
                 << " (index = " << i << ")\n";
            ++numNewExports;
        }
        else
        {
            if (NewPackageInfo.GetExportEntry(i).SerialSize != OldPackageInfo.GetExportEntry(foundIdx).SerialSize)
            {
                cout << "SerialSize changed: " << NewPackageInfo.GetExportEntry(i).FullName
                     << " (index = " << i << ")\n"
                     << "\tOld SerialSize: " << OldPackageInfo.GetExportEntry(foundIdx).SerialSize
                     << "\tNew SerialSize: " << NewPackageInfo.GetExportEntry(i).SerialSize
                     << "\tNew-Old = " << (NewPackageInfo.GetExportEntry(i).SerialSize - OldPackageInfo.GetExportEntry(foundIdx).SerialSize) << std::endl;
                ++numResizedExports;
            }
        }
    }

    cout << "Number of new exports = " << numNewExports << std::endl;
    cout << "Number of resized exports = " << numResizedExports << std::endl;

    return 0;
}
