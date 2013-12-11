#include <iostream>
#include <iomanip>

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
         << "Header size: " << hex << package.GetHeader().HeaderSize << dec << " (" << package.GetHeader().HeaderSize << ")" << endl
         << "Folder name length: " << hex << package.GetHeader().FolderNameLength << dec << " (" << package.GetHeader().FolderNameLength << ")" << endl
         << "Folder name: " << package.GetHeader().FolderName << endl
         << hex << "Package flags: " << package.GetHeader().PackageFlags << endl
         << "Name count: " << hex << package.GetHeader().NameCount << dec << " (" << package.GetHeader().NameCount << ")" << endl
         << "Name offset: " << hex << package.GetHeader().NameOffset << endl
         << "Export count: " << hex << package.GetHeader().ExportCount << dec << " (" << package.GetHeader().ExportCount << ")" << endl
         << "Export offset: " << hex << package.GetHeader().ExportOffset << endl
         << "Import count: " << hex << package.GetHeader().ImportCount << dec << " (" << package.GetHeader().ImportCount << ")"  << endl
         << "Import offset: " << hex << package.GetHeader().ImportOffset << endl
         << "Depends offset: " << hex << package.GetHeader().DependsOffset << endl
         << "Unknown1: " << hex << package.GetHeader().Unknown1 << endl
         << "Unknown2: " << hex << package.GetHeader().Unknown2 << endl
         << "Unknown3: " << hex << package.GetHeader().Unknown3 << endl
         << "Unknown4: " << hex << package.GetHeader().Unknown4 << endl
         << "GUID: ";
    for (uint32_t i = 0; i < package.GetHeader().GUID.size(); ++i)
        cout << hex << noshowbase << uppercase << setfill('0') << setw(2) << unsigned(package.GetHeader().GUID[i]);
    cout << showbase << nouppercase << endl;
    cout << "Generations count: " << hex << package.GetHeader().GenerationsCount << endl;
    for (uint32_t i =0; i < package.GetHeader().GenerationsCount; ++i)
    {
        cout << "Generation [" << i << "]:" << endl
             << "\tExport count: " << hex << package.GetHeader().generations[i].ExportCount << endl
             << "\tName count: " << hex << package.GetHeader().generations[i].NameCount << endl
             << "\tNet object count: " << hex << package.GetHeader().generations[i].NetObjectCount << endl;
    }
    cout << "Engine version: " << hex << package.GetHeader().EngineVersion << endl
         << "Cooker version: " << hex << package.GetHeader().CookerVersion << endl
         << "Compression flags: " << hex << package.GetHeader().CompressionFlags << endl;

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
             << "; Name: " << package.GetObjectNameByIdx(i)
             << "; Type: " << package.GetObjectOrImportNameByIdx(EntryToRead.ObjTypeRef) << endl;
    }

    cout << "Import list table:" << endl;

    for (int i = 1; i < package.GetImportListSize(); ++i)
    {
        ImportListEntry EntryToRead = package.GetImportListEntryByIdx(i);

        cout << hex << showbase << "Index: " << i << " (" << dec << i << ")"
             << "; Name: " << package.GetImportNameByIdx(i)
             << "; Type: " << package.GetNameByIdx(EntryToRead.ObjTypeIdx) << endl;
    }

    return 0;
}
