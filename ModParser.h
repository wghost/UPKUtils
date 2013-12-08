#ifndef MODPARSER_H
#define MODPARSER_H

#include <string>
#include <fstream>
#include <vector>

class ModParser
{
public:
    ModParser(): commentBegin(0), commentEnd(0), commentLine(0), isKey(false), isSection(false) {}
    ~ModParser() {}

    void AddKeyName(std::string name);
    void AddSectionName(std::string name);
    void SetKeyNames(std::vector<std::string> names);
    void SetSectionNames(std::vector<std::string> names);
    void SetCommentMarkers(char begMarker, char endMarker, char lineMarker);
    std::string GetKeyName(int idx);
    std::string GetSectionName(int idx);

    bool OpenModFile(const char* name);

    int FindNext(); // find next key or section, set read pointer at the beginning of the line

    bool IsKey() { return isKey; }
    bool IsSection() { return isSection; }

    std::string GetText();
    std::vector<char> GetDataChunk();
    std::string GetStringValue();
    int GetIntValue();

protected:
    std::string GetLine();

    int FindKey(std::string str);
    int FindSection(std::string str);

    int FindKeyNameIdx(std::string name);
    int FindSectionNameIdx(std::string name);

private:
    std::ifstream modFile;
    std::vector<std::string> keyNames;
    std::vector<std::string> sectionNames;
    char commentBegin;
    char commentEnd;
    char commentLine;
    bool isKey;
    bool isSection;
};

#endif // MODPARSER_H
