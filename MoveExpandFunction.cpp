#include <iostream>
#include <sstream>

#include "UPKUtils.h"

using namespace std;

int main(int argN, char* argV[])
{
    cout << "MoveExpandFunction" << endl;

    if (argN < 3 || argN > 4)
    {
        cerr << "Usage: FindObjectEntry UnpackedResourceFile.upk FunctionName [NewFunctionSize]" << endl;
        return 1;
    }

    UPKUtils package;

    if (!package.open(argV[1]))
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }

    string NameToFind = argV[2];

    cout << "Object name: " << NameToFind << endl;

    int idx = package.FindObjectListEntryByName(NameToFind);

    if (idx < 1)
    {
        cerr << "Can't find object entry by name " << NameToFind << endl;
        return 1;
    }

    ObjectListEntry EntryToRead = package.GetObjectListEntryByIdx(idx);

    uint32_t newFunctionSize = 0;

    cout << "Function size: " << EntryToRead.ObjectFileSize << endl;

    if (argN == 4)
    {
        string str(argV[3]);

        istringstream ss(str);

        if (str.find("0x") != string::npos)
            ss >> hex >> newFunctionSize;
        else
            ss >> dec >> newFunctionSize;

        cout << "Resize function to: " << newFunctionSize << endl;

        if (newFunctionSize <= EntryToRead.ObjectFileSize)
        {
            cerr << "Can't expand function: existing function size is greater than specified value!" << endl;
            return 1;
        }
    }

    package.MoveObject(idx, newFunctionSize, true);

    cout << "Object moved successfully!" << endl;

    return 0;
}

