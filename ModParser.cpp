#include "ModParser.h"

#include <iostream>
#include <sstream>

const char chLookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

std::string hex2str(char *data, size_t dataSize)
{
    std::string out = "";
    for (unsigned i = 0; i < dataSize; ++i)
    {
        if ((i%16 == 0) && (i != 0))
            out += '\n';
        uint8_t ch = data[i];
        uint8_t up = ((ch & 0xF0) >> 4);
        uint8_t lw =  (ch & 0x0F);
        out += chLookup[up];
        out += chLookup[lw];
        out += ' ';
    }
    out += '\n';
    return out;
}

std::string ModParser::GetText()
{
    std::string str = "";
    std::string line = "";
    size_t savedPos = modFile.tellg();
    line = GetLine(); // read key/section line

    if (isKey)
    {
        str += line.substr(line.find("=") + 1);
        while ( modFile.good() )
        {
            savedPos = modFile.tellg();
            line = GetLine();
            if (FindKey(line) != -1 || FindSection(line) != -1)
                break;
            str += std::string("\n") + line;
        }
    }
    else if (isSection)
    {
        while ( modFile.good() )
        {
            savedPos = modFile.tellg();
            line = GetLine();
            if (FindKey(line) != -1 || FindSection(line) != -1)
                break;
            if (str != "") str += "\n";
            str += line;
        }
    }

    modFile.seekg(savedPos, std::ios::beg);

    return str;
}

std::vector<char> ModParser::GetDataChunk()
{
    std::vector<char> data;

    std::string str = GetText();

    if (str == "")
        return data;

    std::istringstream ss(str);

    while (ss.good())
    {
        int byte;
        ss >> std::hex >> byte;
        if (!ss.fail() && !ss.bad())
            data.push_back(byte);
    }

    return data;
}

std::string ModParser::GetStringValue()
{
    std::string str = GetText();
    if (str.length() < 1)
        return std::string("");
    str = str.substr(0, str.find_first_of("\n")); // get first line
    if (str.find("\"") != std::string::npos)
    {
        str = str.substr(str.find_first_not_of("\""));
        str = str.substr(0, str.find_first_of("\""));
    }
    else
    {
        str = str.substr(str.find_first_not_of(" "));
        str = str.substr(0, str.find_first_of(" "));
    }
    return str;
}

int ModParser::GetIntValue()
{
    int val = 0;

    std::string str = GetStringValue();

    if (str == "")
        return 0;

    //val = std::stoi(str, nullptr, 0);

    std::istringstream ss(str);

    if (str.find("0x") != std::string::npos)
        ss >> std::hex >> val;
    else
        ss >> std::dec >> val;

    return val;
}

std::string ModParser::GetLine()
{
    std::string line = "";

    int ch = 0;

    while(ch != 0x0D && ch != 0x0A && modFile.good())
    {
        ch = modFile.get();
        if (modFile.fail() || modFile.bad())
            break;
        if (ch == commentLine)
        {
            while (ch != 0x0D && ch != 0x0A && modFile.good())
                ch = modFile.get();
            break;
        }
        if (ch == commentBegin)
        {
            while (ch != commentEnd && modFile.good())
                ch = modFile.get();
        }
        else if (ch != 0x0D && ch != 0x0A)
        {
            line += ch;
        }
    }

    if (modFile.peek() == 0x0A || modFile.peek() == 0x0D)
        modFile.get();

    return line;
}

int ModParser::FindNext()
{
    if (!modFile.good())
        return -1;

    int idx = -1;
    int keyIdx = -1, sectionIdx = -1;
    std::string line = "";

    unsigned int savedPos = modFile.tellg();

    while ( keyIdx == -1 && sectionIdx == -1 && modFile.good() )
    {
        savedPos = modFile.tellg();
        line = GetLine();
        if (!modFile.fail() && !modFile.bad())
        {
            keyIdx = FindKey(line);
            sectionIdx = FindSection(line);
        }
    }

    modFile.seekg(savedPos, std::ios::beg);

    isKey = (keyIdx != -1);
    isSection = (sectionIdx != -1);

    if (keyIdx != -1)
        idx = keyIdx;
    if (sectionIdx != -1)
        idx = sectionIdx;

    return idx;
}

int ModParser::FindKey(std::string str)
{
    size_t pos = str.find("=");
    if (pos == std::string::npos)
        return -1;
    std::string name = str.substr(0, pos);
    name = name.substr(name.find_first_not_of(" "));
    name = name.substr(0, name.find_last_not_of(" ") + 1);
    int idx = FindKeyNameIdx(name);
    return idx;
}

int ModParser::FindSection(std::string str)
{
    if (str.find("[") == std::string::npos || str.find("]") == std::string::npos)
        return -1;
    std::string name = str;
    name = name.substr(name.find_first_not_of(" "));
    name = name.substr(0, name.find_last_not_of(" ") + 1);
    int idx = FindSectionNameIdx(name);
    return idx;
}

int ModParser::FindKeyNameIdx(std::string name)
{
    int idx = -1;
    for (unsigned int i = 0; i < keyNames.size(); ++i)
        if (keyNames[i] == name)
            idx = i;
    return idx;
}

int ModParser::FindSectionNameIdx(std::string name)
{
    int idx = -1;
    for (unsigned int i = 0; i < sectionNames.size(); ++i)
        if (sectionNames[i] == name)
            idx = i;
    return idx;
}

bool ModParser::OpenModFile(const char* name)
{
    modFile.open(name, std::ios::binary);

    if (!modFile.good())
        return false;

    // check if file is text
    while (modFile.good())
    {
        char ch = modFile.get();
        if (!modFile.good())
            break;
        if (ch < 1 || ch > 127)
        {
            modFile.close();
            return false;
        }
    }

    modFile.clear();
    modFile.seekg(0);

    return true;
}

void ModParser::AddKeyName(std::string name)
{
    keyNames.push_back(name);
}

void ModParser::AddSectionName(std::string name)
{
    sectionNames.push_back(name);
}

void ModParser::SetKeyNames(std::vector<std::string> names)
{
    keyNames = names;
}

void ModParser::SetSectionNames(std::vector<std::string> names)
{
    sectionNames = names;
}

void ModParser::SetCommentMarkers(char begMarker, char endMarker, char lineMarker)
{
    commentBegin = begMarker;
    commentEnd = endMarker;
    commentLine = lineMarker;
}

std::string ModParser::GetKeyName(int idx)
{
    if (idx >= 0 && uint32_t(idx) < keyNames.size())
        return keyNames[idx];
    return std::string("");
}

std::string ModParser::GetSectionName(int idx)
{
    if (idx >= 0 && uint32_t(idx) < sectionNames.size())
        return sectionNames[idx];
    return std::string("");
}
