#include "ModParser.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cctype>

std::string Trim(std::string str)
{
    std::string ret = str, wspc(" \t\f\v\n\r");
    size_t pos = ret.find_first_not_of(wspc);
    if (pos != std::string::npos)
    {
        ret = ret.substr(pos);
    }
    else
    {
        return "";
    }
    pos = ret.find_last_not_of(wspc);
    ret = ret.substr(0, pos + 1);
    return ret;
}

size_t SplitAt(char ch, std::string in, std::string& out1, std::string& out2)
{
    size_t pos = in.find(ch);
    if (pos != std::string::npos)
    {
        out1 = Trim(in.substr(0, pos));
        out2 = Trim(in.substr(pos + 1));
    }
    else
    {
        out1 = Trim(in);
        out2 = "";
    }
    return pos;
}

const char chLookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

std::string MakeTextBlock(char *data, size_t dataSize)
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

std::string GetFilename(std::string str)
{
    unsigned found = str.find_last_of("/\\");
    return str.substr(found + 1);
}

std::string GetStringValue(const std::string& TextBuffer)
{
    std::string str = TextBuffer;
    if (str.length() < 1)
        return "";
    str = str.substr(0, str.find_first_of("\n")); /// get first line
    str = Trim(str); /// remove leading and trailing white-spaces
    if (str.find('\"') != std::string::npos) /// remove ""
    {
        str = str.substr(str.find_first_not_of("\""));
        str = str.substr(0, str.find_last_not_of("\"") + 1);
    }
    return str;
}

std::vector<char> GetDataChunk(const std::string& TextBuffer)
{
    std::vector<char> data;
    if (TextBuffer.length() < 1)
        return data;
    std::istringstream ss(TextBuffer);
    while (ss.good())
    {
        int byte;
        ss >> std::hex >> byte;
        if (!ss.fail() && !ss.bad())
            data.push_back(byte);
    }
    return data;
}

int GetIntValue(const std::string& TextBuffer)
{
    int val = 0;
    std::string str = ::GetStringValue(TextBuffer);
    if (str.length() < 1)
        return 0;
    //val = std::stoi(str, nullptr, 0);
    std::istringstream ss(str);
    /*if (str.find("0x") != std::string::npos)
        ss >> std::hex >> val;
    else
        ss >> std::dec >> val;*/
    ss >> val;
    return val;
}

unsigned GetUnsignedValue(const std::string& TextBuffer)
{
    unsigned val = 0;
    std::string str = ::GetStringValue(TextBuffer);
    if (str.length() < 1)
        return 0;
    std::istringstream ss(str);
    if (str.find("0x") != std::string::npos)
        ss >> std::hex >> val;
    else
        ss >> std::dec >> val;
    return val;
}

float GetFloatValue(const std::string& TextBuffer)
{
    float val = 0;
    std::string str = ::GetStringValue(TextBuffer);
    if (str.length() < 1)
        return 0;
    std::istringstream ss(str);
    ss >> val;
    return val;
}

std::string ModParser::GetText()
{
    std::string str = "";
    std::string line = "";
    size_t savedPos = modFile.tellg();
    line = GetLine(); /// read key/section line
    if (isKey)
    {
        str += line.substr(line.find("=") + 1);
        while ( modFile.good() )
        {
            savedPos = modFile.tellg();
            line = GetLine();
            if (FindKey(line) != -1 || FindSection(line) != -1)
            {
                modFile.clear(std::ios::eofbit); /// to parse last line correctly
                break;
            }
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
            {
                modFile.clear(std::ios::eofbit); /// to parse last line correctly
                break;
            }
            if (str != "") str += "\n";
            str += line;
        }
    }
    modFile.seekg(savedPos, std::ios::beg);
    return str;
}

std::string ModParser::GetTextValue()
{
    return Value;
}

std::vector<char> ModParser::GetDataChunk()
{
    return ::GetDataChunk(Value);
}

std::string ModParser::GetStringValue()
{
    return ::GetStringValue(Value);
}

int ModParser::GetIntValue()
{
    return ::GetIntValue(Value);
}

float ModParser::GetFloatValue()
{
    return ::GetFloatValue(Value);
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
        if (CStyleComments && ch == '/' && modFile.peek() == '/')
        {
            while (ch != 0x0D && ch != 0x0A && modFile.good())
                ch = modFile.get();
        }
        else if (CStyleComments && ch == '/' && modFile.peek() == '*')
        {
            while (modFile.good())
            {
                ch = modFile.get();
                if (ch == '*' && modFile.peek() == '/')
                {
                    ch = modFile.get();
                    break;
                }
            }
        }
        else if (ch == commentLine)
        {
            while (ch != 0x0D && ch != 0x0A && modFile.good())
                ch = modFile.get();
        }
        else if (ch == commentBegin)
        {
            while (ch != commentEnd && modFile.good())
                ch = modFile.get();
        }
        else if (ch != 0x0D && ch != 0x0A && modFile.good())
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
    Name = "";
    Value = "";
    Index = -1;
    int idx = -1, keyIdx = -1, sectionIdx = -1;
    std::string line = "";
    size_t savedPos = modFile.tellg();
    while ( keyIdx == -1 && sectionIdx == -1 && modFile.good() )
    {
        savedPos = modFile.tellg();
        line = GetLine();
        keyIdx = FindKey(line);
        sectionIdx = FindSection(line);
    }
    modFile.clear(std::ios::eofbit); /// to get last line value correctly
    modFile.seekg(savedPos, std::ios::beg);
    isKey = (keyIdx != -1);
    isSection = (sectionIdx != -1);
    if (keyIdx != -1)
    {
        idx = keyIdx;
        Name = keyNames[idx];
    }
    if (sectionIdx != -1)
    {
        idx = sectionIdx;
        Name = sectionNames[idx];
    }
    if (idx != -1)
    {
        Value = GetText();
    }
    Index = idx;
    return idx;
}

int ModParser::FindKey(std::string str)
{
    size_t pos = str.find("=");
    if (pos == std::string::npos)
        return -1;
    std::string name = Trim(str.substr(0, pos));
    /*std::string name = str.substr(0, pos);
    name = name.substr(name.find_first_not_of(" "));
    name = name.substr(0, name.find_last_not_of(" ") + 1);*/
    int idx = FindKeyNameIdx(name);
    return idx;
}

int ModParser::FindSection(std::string str)
{
    if (str.find("[") == std::string::npos || str.find("]") == std::string::npos)
        return -1;
    std::string name = Trim(str);
    /*std::string name = str;
    name = name.substr(name.find_first_not_of(" "));
    name = name.substr(0, name.find_last_not_of(" ") + 1);*/
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
    if (modFile.is_open())
    {
        modFile.close();
        modFile.clear();
    }
    modFile.open(name, std::ios::binary);
    if (!modFile.good())
        return false;
    /// check if file is text
    while (modFile.good())
    {
        char ch = modFile.get();
        if (modFile.eof())
            break;
        if (ch < 1 || ch > 127)
        {
            modFile.close();
            return false;
        }
    }
    modFile.clear();
    modFile.seekg(0);
    isKey = false;
    isSection = false;
    Name = "";
    Value = "";
    Index = -1;
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
