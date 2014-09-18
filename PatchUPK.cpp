#include <iostream>
#include <sstream>
#include <cstring>

#include "ModScript.h"

using namespace std;

bool FileExists(string str)
{
    ifstream in(str);
    return in.good();
}

string int2fstr(int val)
{
    if (val <= 0)
        return string("");
    ostringstream ss;
    ss << dec << val;
    return ss.str();
}

int main(int argN, char* argV[])
{
    cout << "PatchUPK" << endl;

    if (argN < 2 || argN > 3)
    {
        cerr << "Usage: PatchUPK modfile.txt [PATH_TO_UPK]" << endl;
        return 1;
    }

    string upkPath = "";

    if (argN == 3)
    {
        upkPath = argV[2];
        if (upkPath.length() < 1)
        {
            cerr << "Incorrect package path!" << endl;
            return 1;
        }
        // add trailing path separator to command line parameter
        /*if (upkPath[upkPath.length() - 1] != '\\')
            upkPath +=  "\\";*/
    }

    ModScript script(argV[1], upkPath.c_str());
    script.InitStreams(std::cerr, std::cout);
    if (script.IsGood() == false)
    {
        return 1;
    }

    bool ExecResult = script.ExecuteStack();

    if (string(argV[1]).find(".uninstall") == string::npos)
    {
        unsigned i = 0;
        string nextName = "";
        do
        {
            nextName = string(argV[1]) + string(".uninstall") + int2fstr(i) + string(".txt");
            ++i;
        } while (FileExists(nextName));

        ofstream uninstFile(nextName);
        if (!uninstFile.good())
        {
            cerr << "Error saving uninstall script!" << endl;
            return 1;
        }
        uninstFile << "MOD_NAME=" << argV[1] << " uninstall script\n"
                   << "AUTHOR=PatchUPK\n"
                   << "DESCRIPTION=This is automatically generated uninstall script. Do not change anything!\n\n"
                   << script.GetBackupScript() << "\n{ backup script end }\n";
        cout << "Uninstall script saved to " << nextName << endl;
    }

    if (!ExecResult)
        return 1;

    return 0;
}
