#include <iostream>
#include <sstream>
#include <cstring>

#include "UPKUtils.h"

using namespace std;

int main(int argN, char* argV[])
{
    cout << "MoveExpandFunction by wghost81 aka Wasteland Ghost" << endl;

    if (argN < 3 || argN > 4)
    {
        cerr << "Usage: MoveExpandFunction UnpackedResourceFile.upk FunctionName [NewFunctionSize or /u]" << endl;
        return 1;
    }

    UPKUtils package(argV[1]);

    string NameToFind = argV[2];

    cout << "Name to find: " << NameToFind << endl;

    UObjectReference ObjRef = package.FindObject(NameToFind, true);

    if (ObjRef == 0)
    {
        cerr << "Can't find object entry by name " << NameToFind << endl;
        return 1;
    }

    FObjectExport ExportEntry = package.GetExportEntry((uint32_t)ObjRef);

    cout << "Function size: " << ExportEntry.SerialSize << endl;
    cout << "Function offset: " << FormatHEX(ExportEntry.SerialOffset) << endl;

    uint32_t newFunctionSize = 0;
    bool bUndoMove = false;

    if (argN == 4)
    {
        if (strcmp(argV[3], "/u") == 0)
        {
            bUndoMove = true;
        }
        else
        {
            string str(argV[3]);

            istringstream ss(str);

            if (str.find("0x") != string::npos)
                ss >> hex >> newFunctionSize;
            else
                ss >> dec >> newFunctionSize;

            cout << "Resize function to: " << newFunctionSize << endl;

            if (newFunctionSize <= ExportEntry.SerialSize)
            {
                cerr << "Can't expand function: existing function size is greater than specified value!" << endl;
                return 1;
            }
        }
    }

    if (!bUndoMove)
    {
        package.MoveExportData((uint32_t)ObjRef, newFunctionSize);
        cout << "Object moved successfully!" << endl;
    }
    else
    {
        package.UndoMoveExportData((uint32_t)ObjRef);
        cout << "Object restored successfully!" << endl;
    }

    package.SaveOnDisk();

    return 0;
}

