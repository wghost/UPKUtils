#include <iostream>
#include <cstdlib>

#include <wx/string.h>
#include <wx/filefn.h>
#include <wx/filename.h>

#include "UPKUtils.h"

using namespace std;

string CreatePath(string fullName, string dirName)
{
    string str = fullName;
    vector<string> names;
    unsigned pos = str.find('.');
    if (pos == string::npos)
        return (dirName + "\\" + str + ".txt");
    while (pos != string::npos)
    {
        names.push_back(str.substr(0, pos));
        str = str.substr(pos + 1);
        pos = str.find('.');
    }
    string ret = str;
    str = dirName;
    for (unsigned i = 0; i < names.size(); ++i)
    {
        str += "\\" + names[i];
        if (!wxDirExists(str))
            wxMkdir(str);
    }
    return (str + "\\" + ret + ".txt");
}

int main(int argN, char* argV[])
{
    cout << "DeserializeAll" << endl;

    if (argN < 2 || argN > 3)
    {
        cerr << "Usage: DeserializeAll UnpackedResourceFile.upk [NameMask]" << endl;
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

    string NameMask = "";
    if (argN == 3)
        NameMask = argV[2];

    wxString dirName;
    wxFileName::SplitPath(argV[1], nullptr, nullptr, &dirName, nullptr);

    if (!wxDirExists(dirName))
        wxMkdir(dirName);

    string listPath = dirName.ToStdString() + ".txt";
    ofstream out(listPath);
    out << package.FormatSummary() << std::endl
        << package.FormatNames(false) << std::endl
        << package.FormatImports(false) << std::endl
        << package.FormatExports(false);
    out.close();

    vector<FObjectExport> ExportTable = package.GetExportTable();

    for (unsigned i = 1; i < ExportTable.size(); ++i)
    {
        cout << ExportTable[i].FullName <<endl;
        if (NameMask != "")
        {
            if (ExportTable[i].FullName.find(NameMask) == string::npos)
                continue;
        }
        string filePath = CreatePath(ExportTable[i].FullName, dirName.ToStdString());
        ofstream out(filePath);
        out << package.FormatExport(i, true);
        out << package.Deserialize(i, true);
        out.close();
    }

    return 0;
}
