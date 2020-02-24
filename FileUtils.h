#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <wx/string.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/dir.h>

int _CRT_glob = 0; /// prevent wildcard expansion of command line arguments

class wxDirTraverserSimple : public wxDirTraverser
{
public:
    wxDirTraverserSimple(wxArrayString& files, wxString fileMask = "*") : m_files(files), match(fileMask) {}
    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
        if (wxMatchWild(match, filename, false))
            m_files.Add(filename);
        return wxDIR_CONTINUE;
    }
    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
    {
        return wxDIR_CONTINUE;
    }
private:
    wxArrayString& m_files;
    wxString match = "*";
};

wxArrayString TraverseDir(wxString dirName, wxString mask)
{
    wxArrayString res;
    if (wxIsWild(mask))
    {
        wxDirTraverserSimple traverser(res, mask);
        wxDir dir(dirName);
        dir.Traverse(traverser);
    }
    else
        res.Add(mask);
    return res;
}

#endif //FILEUTILS_H
