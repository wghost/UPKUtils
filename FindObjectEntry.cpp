#include <iostream>

#include "UPKUtils.h"

using namespace std;

int main(int argN, char* argV[])
{
    cout << "FindObjectEntry by wghost81 aka Wasteland Ghost" << endl;

    if (argN < 3 || argN > 4)
    {
        cerr << "Usage: FindObjectEntry UnpackedResourceFile.upk ObjectName [/d]" << endl;
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
    if (ObjRef > 0 && argN == 4 && string(argV[3]) == "/d")
    {
        package.SaveExportData((uint32_t)ObjRef);
    }
    if (ObjRef > 0)
    {
        cout << "Found Export Object:\n" << package.FormatExport(ObjRef, true);
    }
    else
    {
        cout << "Found Import Object:\n" << package.FormatImport(-ObjRef, true);
    }

    if (ObjRef > 0)
    {
        cout << "Attempting deserialization:\n" << package.Deserialize(ObjRef, true);
    }

    return 0;
}
