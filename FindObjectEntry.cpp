#include <iostream>

#include "UPKUtils.h"

using namespace std;

int main(int argN, char* argV[])
{
    cout << "FindObjectEntry" << endl;

    if (argN < 3 || argN > 4)
    {
        cerr << "Usage: FindObjectEntry UnpackedResourceFile.upk ObjectName [/d]" << endl;
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
    cout << hex << showbase << "Object table index: " << idx << " (" << dec << idx << ")" << endl
         << hex << "Object type: " << EntryToRead.ObjType << endl
         << "Parent class reference: " << EntryToRead.ParentClassRef << endl
         << "Owner reference: " << EntryToRead.OwnerRef << endl
         << "Index to name list table: " << EntryToRead.NameListIdx << endl
         << "Unknown Field 5: " << EntryToRead.Field5 << endl
         << "Unknown Field 6: " << EntryToRead.Field5 << endl
         << "Property flags: " << EntryToRead.PropertyFlags << endl
         << "Unknown Field 6: " << EntryToRead.Field6 << endl
         << "Object file size: " << EntryToRead.ObjectFileSize << endl
         << "Object data offset: " << EntryToRead.DataOffset << endl
         << "Unknown Field 11: " << EntryToRead.Field11 << endl
         << "NumAdditionalFields: " << EntryToRead.NumAdditionalFields << endl
         << "Unknown Field 13: " << EntryToRead.Field11 << endl
         << "Unknown Field 14: " << EntryToRead.Field11 << endl
         << "Unknown Field 15: " << EntryToRead.Field11 << endl
         << "Unknown Field 16: " << EntryToRead.Field11 << endl
         << "Unknown Field 17: " << EntryToRead.Field11 << endl;

    if (argN == 4 && string(argV[3]) == "/d")
    {
        vector<char> dataChunk = package.GetObjectData(idx);
        string filename = NameToFind + ".Function";
        ofstream out(filename.c_str(), ios::binary);
        out.write(dataChunk.data(), dataChunk.size());
    }

    return 0;
}
