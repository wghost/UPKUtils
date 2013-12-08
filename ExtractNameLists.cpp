#include <iostream>

#include "UPKUtils.h"

using namespace std;

int main(int argN, char* argV[])
{
    cout << "ExtractNameLists" << endl;

    if (argN != 2)
    {
        cerr << "Usage: ExtractNameLists UnpackedResourceFile.upk" << endl;
        return 1;
    }

    UPKUtils package;

    if (!package.open(argV[1]))
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }

    cout << showbase << hex << "Package signature: " << package.GetHeader().Signature << endl
         << "Package version: " << hex << package.GetHeader().Version << dec << " (" << package.GetHeader().Version << ")" << endl
         << "License version: " << hex << package.GetHeader().LicenseVersion << dec << " (" << package.GetHeader().LicenseVersion << ")" << endl
         << "package.GetHeader() size: " << hex << package.GetHeader().HeaderSize << dec << " (" << package.GetHeader().HeaderSize << ")" << endl
         << "Folder name length: " << hex << package.GetHeader().FolderNameLength << dec << " (" << package.GetHeader().FolderNameLength << ")" << endl
         << "Folder name: " << package.GetHeader().FolderName << endl
         << hex << "Package flags: " << package.GetHeader().PackageFlags << endl
         << "Name count: " << hex << package.GetHeader().NameCount << dec << " (" << package.GetHeader().NameCount << ")" << endl
         << "Name offset: " << hex << package.GetHeader().NameOffset << endl
         << "Export count: " << hex << package.GetHeader().ExportCount << dec << " (" << package.GetHeader().ExportCount << ")" << endl
         << "Export offset: " << hex << package.GetHeader().ExportOffset << endl
         << "Import count: " << hex << package.GetHeader().ImportCount << dec << " (" << package.GetHeader().ImportCount << ")"  << endl
         << "Import offset: " << hex << package.GetHeader().ImportOffset << endl;

    cout << "Name list table:" << endl;

    for (int i = 0; i < package.GetNameListSize(); ++i)
    {
        cout << hex << showbase << "Index: " << i << " (" << dec << i << ")"
             << ", Name: " << package.GetNameByIdx(i) << endl;
    }

    cout << "Object list table:" << endl;

    for (int i = 1; i < package.GetObjectListSize(); ++i)
    {
        ObjectListEntry EntryToRead = package.GetObjectListEntryByIdx(i);

        cout << hex << showbase << "Index: " << i << " (" << dec << i << ")"
             << ", Name: " << package.GetObjectNameByIdx(i) << endl;

        cout << hex << "Object type: " << EntryToRead.ObjType << endl
             << "Parent class reference: " << EntryToRead.ParentClassRef << endl
             << "Owner reference: " << EntryToRead.OwnerRef << endl
             << "Index to name list table: " << EntryToRead.NameListIdx << endl
             << "Property flags: " << EntryToRead.PropertyFlags << endl
             << "Object file size: " << EntryToRead.ObjectFileSize << endl
             << "Object data offset: " << EntryToRead.DataOffset << endl
             << "Num additional fields: " << EntryToRead.NumAdditionalFields << endl;
    }

    cout << "Import list table:" << endl;

    for (int i = 1; i < package.GetImportListSize(); ++i)
    {
        ImportListEntry EntryToRead = package.GetImportListEntryByIdx(i);

        cout << hex << showbase << "Index: " << i << " (" << dec << i << ")"
             << ", Name: " << package.GetImportNameByIdx(i) << endl;

        cout << hex << "Object type: " << EntryToRead.ObjType << endl
             << "Package ID: " << EntryToRead.PackageID << endl
             << "Owner reference: " << EntryToRead.OwnerRef << endl
             << "Index to name list table: " << EntryToRead.NameListIdx << endl;
    }

    return 0;
}
