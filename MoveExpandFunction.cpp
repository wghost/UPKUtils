#include <iostream>
#include <sstream>
#include <cstring>

#include "UPKUtils.h"

using namespace std;

int main(int argN, char* argV[])
{
    cout << "MoveExpandFunction" << endl;

    if (argN < 3 || argN > 4)
    {
        cerr << "Usage: FindObjectEntry UnpackedResourceFile.upk FunctionName [NewFunctionSize or /u]" << endl;
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

    cout << "Function size: " << hex << showbase << EntryToRead.ObjectFileSize << endl;
    cout << "Function offset: " << hex << showbase << EntryToRead.DataOffset << endl;

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

            cout << "Resize function to: " << hex << showbase << newFunctionSize << endl;

            if (newFunctionSize <= EntryToRead.ObjectFileSize)
            {
                cerr << "Can't expand function: existing function size is greater than specified value!" << endl;
                return 1;
            }
        }
    }

    if (!bUndoMove)
    {
        package.MoveObject(idx, newFunctionSize);
        cout << "Object moved successfully!" << endl;
    }
    else
    {
        package.UndoMoveObject(idx);
        cout << "Object restored successfully!" << endl;
    }

    return 0;
}

