#include <iostream>

#include "UPKInfo.h"
#include <fstream>
#include <sstream>

using namespace std;

int main(int argN, char* argV[])
{
    cout << "FindObjectByOffset by wghost81 aka Wasteland Ghost" << endl;

    if (argN != 3)
    {
        cerr << "Usage: FindObjectByOffset UnpackedResourceFile.upk offset" << endl;
        return 1;
    }

    ifstream package(argV[1], ios::binary);
    if (!package.is_open())
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }

    size_t Offset = 0;
    string str(argV[2]);
    istringstream ss(str);
    if (str.find("0x") != string::npos)
        ss >> hex >> Offset;
    else
        ss >> dec >> Offset;

    cout << "Offset = " << FormatHEX((uint32_t)Offset) << " (" << Offset << ")\n";

    UPKInfo PackageInfo(package);
    UPKReadErrors err = PackageInfo.GetError();
    if (err != UPKReadErrors::NoErrors)
    {
        cerr << "Error reading package:\n" << FormatReadErrors(err);
        if (PackageInfo.IsCompressed())
        {
            cout << PackageInfo.FormatSummary();
        }
        return 1;
    }

    UObjectReference ObjRef = PackageInfo.FindObjectByOffset(Offset);
    if (ObjRef <= 0)
    {
        cerr << "Can't find object by specified offset!\n";
    }
    cout << "Found object: " << PackageInfo.GetExportEntry(ObjRef).FullName << std::endl;

    return 0;
}
