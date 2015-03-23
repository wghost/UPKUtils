#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

#define MAGIC (uint32_t)747443441

int main(int argN, char* argV[])
{
    cout << "UENativeTablesReader" << endl;

    if (argN != 2)
    {
        cerr << "Usage: UENativeTablesReader NativeTable.NTL" << endl;
        return 1;
    }

    ifstream table(argV[1], ios::binary);
    if (!table.is_open())
    {
        cerr << "Can't open " << argV[1] << endl;
        return 1;
    }

    uint32_t magic;
    table.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != MAGIC)
    {
        cerr << "Input file is not a NTL file!\n";
        return 1;
    }

    uint32_t num;
    table.read(reinterpret_cast<char*>(&num), sizeof(num));
    if (num < 0 || num > 0xFFF)
    {
        cerr << "Input file is not a NTL file!\n";
        return 1;
    }

    cout << "Num = " << num << endl;

    cout << "HEX\tName\t\t\t\tOpPrec\tType\tToken\n";

    for (unsigned i = 0; i < num; ++i)
    {
        uint8_t NameLen;
        string Name;
        uint8_t OperPrecedence;
        uint8_t Type;
        uint32_t ByteToken;
        char ch[255];

        table.read(reinterpret_cast<char*>(&NameLen), sizeof(NameLen));
        table.read(ch, NameLen);
        Name = string(ch, NameLen);
        table.read(reinterpret_cast<char*>(&OperPrecedence), sizeof(OperPrecedence));
        table.read(reinterpret_cast<char*>(&Type), sizeof(Type));
        table.read(reinterpret_cast<char*>(&ByteToken), sizeof(ByteToken));

        cout << "0x" << setfill('0') << setw(2) << hex << ByteToken << "\t" << Name
             << "\t\t\t\t"
             << dec << (int)OperPrecedence << "\t" << (int)Type << "\t" << ByteToken << endl;
    }

    return 0;
}
