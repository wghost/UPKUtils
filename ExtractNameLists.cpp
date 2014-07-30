#include <iostream>
#include <iomanip>

#include "UPKInfo.h"
#include <fstream>

using namespace std;

int main(int argN, char* argV[])
{
    cout << "ExtractNameLists" << endl;

    if (argN < 2 || argN > 3)
    {
        cerr << "Usage: ExtractNameLists UnpackedResourceFile.upk [/v]" << endl;
        return 1;
    }

    ifstream package(argV[1], ios::binary);
    if (!package.is_open())
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }

    bool verbose = false;

    if (argN == 3 && string(argV[2]) == "/v")
    {
        verbose = true;
    }

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

    cout << PackageInfo.FormatSummary() << std::endl
         << PackageInfo.FormatNames(verbose) << std::endl
         << PackageInfo.FormatImports(verbose) << std::endl
         << PackageInfo.FormatExports(verbose);

    return 0;
}
