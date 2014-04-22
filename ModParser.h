#ifndef MODPARSER_H
#define MODPARSER_H

#include <string>
#include <fstream>
#include <vector>

std::string Trim(std::string str);
size_t SplitAt(char ch, std::string in, std::string& out1, std::string& out2);
std::string GetFilename(std::string str);
std::string MakeTextBlock(char *data, size_t dataSize);
std::string GetStringValue(const std::string& TextBuffer);
std::vector<char> GetDataChunk(const std::string& TextBuffer);
int GetIntValue(const std::string& TextBuffer);
unsigned GetUnsignedValue(const std::string& TextBuffer);
float GetFloatValue(const std::string& TextBuffer);

class ModParser
{
public:
    ModParser(): commentBegin(0), commentEnd(0), commentLine(0), isKey(false), isSection(false), Name(""), Value(""), Index(-1), CStyleComments(true) {}
    ~ModParser() {}
    /// keys, sections and comments
    void AddKeyName(std::string name);
    void AddSectionName(std::string name);
    void SetKeyNames(std::vector<std::string> names);
    void SetSectionNames(std::vector<std::string> names);
    void ClearKeyNames() { keyNames.clear(); }
    void ClearSectionNames() { sectionNames.clear(); }
    void SetCommentMarkers(char begMarker, char endMarker, char lineMarker);
    /// init
    bool OpenModFile(const char* name);
    void UseCStyleComments(bool val) { CStyleComments = val; }
    /// find next key or section, set read pointer at the beginning of the line
    int FindNext();
    /// Getters
    std::string GetTextValue();
    std::vector<char> GetDataChunk();
    std::string GetStringValue();
    int GetIntValue();
    float GetFloatValue();
    /// Get key/section Name and text Value
    std::string GetName() { return Name; }
    std::string GetValue() { return Value; }
protected:
    std::string GetLine();
    std::string GetText();
    int FindKey(std::string str);
    int FindSection(std::string str);
    int FindKeyNameIdx(std::string name);
    int FindSectionNameIdx(std::string name);
    bool IsKey() { return isKey; }
    bool IsSection() { return isSection; }
private:
    std::ifstream modFile;
    std::vector<std::string> keyNames;
    std::vector<std::string> sectionNames;
    char commentBegin;
    char commentEnd;
    char commentLine;
    bool isKey;
    bool isSection;
    std::string Name;
    std::string Value;
    int Index;
    bool CStyleComments;
};

#endif // MODPARSER_H
