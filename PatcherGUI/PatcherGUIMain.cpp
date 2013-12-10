/***************************************************************
 * Name:      PatcherGUIMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Wasteland Ghost (wghost81@gmail.com)
 * Created:   2013-12-09
 * Copyright: Wasteland Ghost ()
 * License:
 **************************************************************/

#include "PatcherGUIMain.h"
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/time.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include "SettingsDialog.h"

#include <fstream>

//(*InternalHeaders(PatcherGUIFrame)
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(PatcherGUIFrame)
const long PatcherGUIFrame::ID_TEXTCTRL1 = wxNewId();
const long PatcherGUIFrame::ID_BUTTON8 = wxNewId();
const long PatcherGUIFrame::ID_TEXTCTRL2 = wxNewId();
const long PatcherGUIFrame::ID_BUTTON9 = wxNewId();
const long PatcherGUIFrame::ID_RICHTEXTCTRL1 = wxNewId();
const long PatcherGUIFrame::ID_BUTTON1 = wxNewId();
const long PatcherGUIFrame::ID_BUTTON7 = wxNewId();
const long PatcherGUIFrame::ID_BUTTON2 = wxNewId();
const long PatcherGUIFrame::ID_BUTTON5 = wxNewId();
const long PatcherGUIFrame::ID_TEXTCTRL3 = wxNewId();
const long PatcherGUIFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PatcherGUIFrame,wxFrame)
    //(*EventTable(PatcherGUIFrame)
    //*)
END_EVENT_TABLE()

PatcherGUIFrame::PatcherGUIFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(PatcherGUIFrame)
    wxFlexGridSizer* FlexGridSizer2;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer3;

    Create(parent, wxID_ANY, _("XCOM UPK Patcher"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("D:\\CodeBlocksProj\\UPKUtils\\PatcherGUI\\favicon.ico"))));
    	SetIcon(FrameIcon);
    }
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer2->AddGrowableCol(0);
    FlexGridSizer2->AddGrowableRow(2);
    FlexGridSizer2->AddGrowableRow(3);
    TextCtrl1 = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("Path to XCOM:EU or XCOM:EW"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    FlexGridSizer2->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button8 = new wxButton(Panel1, ID_BUTTON8, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    FlexGridSizer2->Add(Button8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl2 = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("Mod file"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    FlexGridSizer2->Add(TextCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button9 = new wxButton(Panel1, ID_BUTTON9, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON9"));
    FlexGridSizer2->Add(Button9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    RichTextCtrl1 = new wxRichTextCtrl(Panel1, ID_RICHTEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(575,353), wxRE_MULTILINE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
    	wxRichTextAttr rchtxtAttr_1;
    FlexGridSizer2->Add(RichTextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button7 = new wxButton(Panel1, ID_BUTTON7, _("Save as..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    BoxSizer3->Add(Button7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(Panel1, ID_BUTTON2, _("Install"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer3->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button5 = new wxButton(Panel1, ID_BUTTON5, _("Settings"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    BoxSizer3->Add(Button5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(BoxSizer3, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    TextCtrl3 = new wxTextCtrl(Panel1, ID_TEXTCTRL3, _("PatchUPK output stream"), wxDefaultPosition, wxSize(575,88), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    FlexGridSizer2->Add(TextCtrl3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Panel1->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel1);
    BoxSizer2->SetSizeHints(Panel1);
    BoxSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSelectDirectory);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSelectModFile);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSaveModFile);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSaveModFileAs);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnInstallMod);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnChangeSettings);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&PatcherGUIFrame::OnClose);
    //*)
    bSelectPath = false;
    bSelectFile = false;
    if (!LoadCFG())
    {
        BackupPathString = wxGetCwd() + "\\Backup";
        PatchUPKprogram = wxGetCwd() + "\\PatchUPK.exe";
        DecompressProgram = wxGetCwd() + "\\Decompress.exe";
    }
    curBackupPathString = "";
}

PatcherGUIFrame::~PatcherGUIFrame()
{
    //(*Destroy(PatcherGUIFrame)
    //*)
}

void PatcherGUIFrame::OnClose(wxCloseEvent& event)
{
    SaveCFG();

    if (RichTextCtrl1->IsModified() && RichTextCtrl1->GetValue() != wxEmptyString)
    {
        if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
            wxICON_QUESTION | wxYES_NO, this) == wxNO)
            {
                event.Skip(false);
                return;
            }
    }

    event.Skip();
}

bool PatcherGUIFrame::LoadCFG()
{
    std::ifstream cfg("PatcherGUI.cfg");
    if (cfg.is_open())
    {
        std::string str = "";
        cfg >> str;
        if (wxDirExists(str + "\\XComGame\\CookedPCConsole"))
        {
            TextCtrl1->SetValue(str);
            bSelectPath = true;
        }
        cfg >> str;
        BackupPathString = str;
        cfg >> str;
        PatchUPKprogram = str;
        cfg >> str;
        DecompressProgram = str;
        return true;
    }
    return false;
}

bool PatcherGUIFrame::SaveCFG()
{
    std::ofstream cfg("PatcherGUI.cfg");
    if (cfg.is_open())
    {
        if (!wxDirExists(TextCtrl1->GetValue()))
            TextCtrl1->SetValue(wxGetCwd());
        cfg << TextCtrl1->GetValue() << std::endl;
        if (!wxDirExists(BackupPathString))
            BackupPathString = wxGetCwd() + "\\Backup";
        cfg << BackupPathString << std::endl;
        if (!wxFileExists(PatchUPKprogram))
            PatchUPKprogram = wxGetCwd() + "\\PatchUPK.exe";
        cfg << PatchUPKprogram << std::endl;
        if (!wxFileExists(DecompressProgram))
            DecompressProgram = wxGetCwd() + "\\Decompress.exe";
        cfg << DecompressProgram << std::endl;
        return true;
    }
    return false;
}

void PatcherGUIFrame::OnSaveModFile(wxCommandEvent& event)
{
    if (RichTextCtrl1->GetFilename() != wxEmptyString)
        RichTextCtrl1->SaveFile(RichTextCtrl1->GetFilename(), wxRICHTEXT_TYPE_TEXT);
}

void PatcherGUIFrame::OnSaveModFileAs(wxCommandEvent& event)
{
    wxFileDialog saveFileDialog(this, _("Select a file"), "", "", "Text files (*.txt)|*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    TextCtrl2->SetValue(saveFileDialog.GetPath());
    RichTextCtrl1->SetFilename(saveFileDialog.GetPath());
    RichTextCtrl1->SaveFile(RichTextCtrl1->GetFilename(), wxRICHTEXT_TYPE_TEXT);
    bSelectFile = true;
}

void PatcherGUIFrame::OnSelectDirectory(wxCommandEvent& event)
{
    wxDirDialog dirDialog(NULL, _("Select a directory"), "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dirDialog.ShowModal() == wxID_CANCEL)
        return;

    TextCtrl1->SetValue(dirDialog.GetPath());
    bSelectPath = true;
}

void PatcherGUIFrame::OnSelectModFile(wxCommandEvent& event)
{
    if (RichTextCtrl1->IsModified() && RichTextCtrl1->GetValue() != wxEmptyString)
    {
        if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
            wxICON_QUESTION | wxYES_NO, this) == wxNO)
            return;
    }

    wxFileDialog openFileDialog(this, _("Select mod file"), "", "", "Text files (*.txt)|*.txt", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    TextCtrl2->SetValue(openFileDialog.GetPath());
    RichTextCtrl1->SetFilename(openFileDialog.GetPath());
    RichTextCtrl1->LoadFile(RichTextCtrl1->GetFilename(), wxRICHTEXT_TYPE_TEXT);
    bSelectFile = true;
}

void PatcherGUIFrame::OnInstallMod(wxCommandEvent& event)
{
    if (RichTextCtrl1->IsModified() && RichTextCtrl1->GetValue() != wxEmptyString)
    {
        wxMessageBox(_("You have to save mod file before installing mod!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }
    if (!bSelectFile)
    {
        wxMessageBox(_("No mod file has been selected!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }
    if (!wxDirExists(TextCtrl1->GetValue() + "\\XComGame\\CookedPCConsole"))
    {
        wxMessageBox(_("Incorrect path to XCOM directory!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }
    if (!wxFileExists(PatchUPKprogram))
    {
        wxMessageBox(_("Can't find PatchUPK in ") + PatchUPKprogram, _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    FilesToBackup.Clear();
    FilesToRemove.Clear();
    FilesToDecompress.Clear();

    wxString str = RichTextCtrl1->GetValue();
    int pos = str.find("UPK_FILE");
    while (pos != wxNOT_FOUND)
    {
        int eqPos = str.find_first_of('=', pos);
        wxString upkFile = str.substr(eqPos + 1, str.find_first_of('.', pos) + 3 - eqPos);
        //upkFile.Trim();
        upkFile = upkFile.substr(upkFile.find_first_not_of(" "));
        upkFile = upkFile.substr(0, upkFile.find_last_not_of(" ") + 1);
        FilesToBackup.Add(upkFile);
        wxString sizeFile = TextCtrl1->GetValue() + "\\XComGame\\CookedPCConsole\\" + upkFile + ".uncompressed_size";
        //TextCtrl3->AppendText(sizeFile + "\n");
        if (wxFileExists(sizeFile))
        {
            FilesToBackup.Add(upkFile + ".uncompressed_size");
            FilesToRemove.Add(upkFile + ".uncompressed_size");
            FilesToDecompress.Add(upkFile);
        }
        str = str.substr(pos + 8);
        pos = str.find("UPK_FILE");
    }

    TextCtrl3->SetValue("");

    if (!MakeBackups())
        return;
    if (!DecompressUPK())
        return;
    if (!RemoveSizeFiles())
        return;

    long retVal = 0;
    wxString executePatchUPKcommandLineString = "\"" + PatchUPKprogram.c_str() + "\"";
    executePatchUPKcommandLineString += " \"" + TextCtrl2->GetValue().c_str() + "\"";
    if (bSelectPath)
        executePatchUPKcommandLineString += " \"" + TextCtrl1->GetValue().c_str() + "\\XComGame\\CookedPCConsole\"";

    wxExecuteEnv env;
    env.cwd = wxPathOnly(TextCtrl2->GetValue());

    PatchUPKoutput.Clear();
    PatchUPKerrors.Clear();

    retVal = wxExecute(executePatchUPKcommandLineString, PatchUPKoutput, PatchUPKerrors, wxEXEC_SYNC, &env);

    TextCtrl3->AppendText("Executing external PatchUPK program:\n" + executePatchUPKcommandLineString + "\n\n");

    for (unsigned i = 0; i < PatchUPKoutput.GetCount(); ++i)
        TextCtrl3->AppendText(PatchUPKoutput[i] + "\n");
    for (unsigned i = 0; i < PatchUPKerrors.GetCount(); ++i)
        TextCtrl3->AppendText(PatchUPKerrors[i] + "\n");

    if (retVal != 0)
    {
        wxMessageBox(_("Error patching UPK!"), _("Error"), wxICON_ERROR | wxOK, this);
        RestoreFromBackup();
        return;
    }
    else
        wxMessageBox(_("Patched successfully!"), _("Success"), wxICON_INFORMATION | wxOK, this);

    TextCtrl3->AppendText("\nMod applied successfully\n\n");
}

bool PatcherGUIFrame::RestoreFromBackup()
{
    if (curBackupPathString == wxEmptyString || !wxDirExists(curBackupPathString))
        return false;

    TextCtrl3->AppendText("Restoring from backup in " + curBackupPathString + "\n");

    for (unsigned i = 0; i < FilesToBackup.GetCount(); ++i)
    {
        wxString copyFromPath = curBackupPathString + wxString("\\") + FilesToBackup[i];
        wxString copyToPath = TextCtrl1->GetValue() + wxString("\\XComGame\\CookedPCConsole\\") + FilesToBackup[i];
        if (!wxCopyFile(copyFromPath, copyToPath, true))
        {
            wxMessageBox(_("Can't restore from backup!"), _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }
        TextCtrl3->AppendText(FilesToBackup[i] + " restored from backup dir\n");
    }

    TextCtrl3->AppendText("Restoring from backup completed successfully\n\n");

    return true;
}

bool PatcherGUIFrame::MakeBackups()
{
    if (FilesToBackup.GetCount() == 0)
        return true;

    if (!wxDirExists(BackupPathString))
        if (!wxMkdir(BackupPathString))
        {
            wxMessageBox(_("Can't create backup dir!"), _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }

    wxString currBackupSubdir = "";
    currBackupSubdir << wxGetUTCTime();

    curBackupPathString = BackupPathString + wxString("\\") + currBackupSubdir;

    if (!wxDirExists(curBackupPathString))
        if(!wxMkdir(curBackupPathString))
        {
            wxMessageBox(_("Can't create backup subdir!"), _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }

    TextCtrl3->AppendText("Writing backups to " + curBackupPathString + "\n");

    for (unsigned i = 0; i < FilesToBackup.GetCount(); ++i)
    {
        wxString copyToPath = curBackupPathString + wxString("\\") + FilesToBackup[i];
        wxString copyFromPath = TextCtrl1->GetValue() + wxString("\\XComGame\\CookedPCConsole\\") + FilesToBackup[i];
        if (!wxFileExists(copyToPath))
        {
            if (!wxCopyFile(copyFromPath, copyToPath))
            {
                wxMessageBox(_("Can't make backups!"), _("Error"), wxICON_ERROR | wxOK, this);
                return false;
            }
            TextCtrl3->AppendText(FilesToBackup[i] + " saved to backup dir\n");
        }
    }

    TextCtrl3->AppendText("Backup completed successfully\n\n");

    return true;
}

bool PatcherGUIFrame::DecompressUPK()
{
    if (FilesToDecompress.GetCount() == 0)
        return true;

    if (!wxFileExists(DecompressProgram))
    {
        wxMessageBox(_("Can't find decompress in ") + DecompressProgram, _("Error"), wxICON_ERROR | wxOK, this);
        return false;
    }

    for (unsigned i = 0; i < FilesToDecompress.GetCount(); ++i)
    {
        long retVal = 0;
        wxString executeDecompressCommandLineString = "\"" + DecompressProgram + "\"";
        executeDecompressCommandLineString += " -lzo -ps3";
        executeDecompressCommandLineString += " -out=\"" + TextCtrl1->GetValue() + "\\XComGame\\CookedPCConsole\"";
        executeDecompressCommandLineString += " " + FilesToDecompress[i];

        wxExecuteEnv env;
        env.cwd = curBackupPathString;

        DecompressOutput.Clear();
        DecompressErrors.Clear();

        retVal = wxExecute(executeDecompressCommandLineString, DecompressOutput, DecompressErrors, wxEXEC_SYNC, &env);

        TextCtrl3->AppendText("Executing external decompress program:\n" + executeDecompressCommandLineString + "\n\n");

        for (unsigned k = 0; k < DecompressOutput.GetCount(); ++k)
            TextCtrl3->AppendText(DecompressOutput[k] + "\n");
        for (unsigned k = 0; k < DecompressErrors.GetCount(); ++k)
            TextCtrl3->AppendText(DecompressErrors[k] + "\n");

        if (retVal != 0)
        {
            wxMessageBox(_("Error decompressing ") + FilesToDecompress[i], _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }
    }

    TextCtrl3->AppendText("Decompression completed successfully\n\n");

    return true;
}

bool PatcherGUIFrame::RemoveSizeFiles()
{
    if (FilesToRemove.GetCount() == 0)
        return true;

    TextCtrl3->AppendText("Deleting size files\n");

    for (unsigned i = 0; i < FilesToRemove.GetCount(); ++i)
    {
        wxString removePath = TextCtrl1->GetValue() + wxString("\\XComGame\\CookedPCConsole\\") + FilesToRemove[i];
        if (!wxFileExists(removePath))
        {
            wxMessageBox(_("File doesn't exist ") + removePath, _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }
        if (!wxRemoveFile(removePath))
        {
            wxMessageBox(_("Can't delete ") + removePath, _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }
        TextCtrl3->AppendText(FilesToRemove[i] + " deleted\n");
    }

    TextCtrl3->AppendText("Files deleted successfully\n\n");

    return true;
}

void PatcherGUIFrame::OnChangeSettings(wxCommandEvent& event)
{
    SettingsDialog dlg(this);

    dlg.TextCtrl2->SetValue(BackupPathString);
    dlg.TextCtrl3->SetValue(PatchUPKprogram);
    dlg.TextCtrl1->SetValue(DecompressProgram);

    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    bSelectPath = true;
    BackupPathString = dlg.TextCtrl2->GetValue();
    PatchUPKprogram = dlg.TextCtrl3->GetValue();
    DecompressProgram = dlg.TextCtrl1->GetValue();
    //SaveCFG();
}
