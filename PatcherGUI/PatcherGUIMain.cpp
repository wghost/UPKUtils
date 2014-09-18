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
#include <wx/filename.h>
#include <wx/mimetype.h>
#include <wx/textdlg.h>
#include "SettingsDialog.h"
#include "ShowDebugLogDialog.h"
#include "ViewLog.h"

#include <fstream>
#include <sstream>
#include <functional>

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
const long PatcherGUIFrame::ID_BUTTON4 = wxNewId();
const long PatcherGUIFrame::ID_PANEL1 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM9 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM4 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM5 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM6 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM2 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM3 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM13 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM10 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM11 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM1 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM12 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM7 = wxNewId();
const long PatcherGUIFrame::ID_MENUITEM8 = wxNewId();
const long PatcherGUIFrame::ID_STATUSBAR1 = wxNewId();
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
    	FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("favicon.ico"))));
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
    TextCtrl1->SetToolTip(_("Path to XCOM:EU or XCOM:EW"));
    FlexGridSizer2->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button8 = new wxButton(Panel1, ID_BUTTON8, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    Button8->SetToolTip(_("Set path to XCOM:EU or XCOM:EW"));
    FlexGridSizer2->Add(Button8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl2 = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("Mod file"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    TextCtrl2->SetToolTip(_("Path to mod file"));
    FlexGridSizer2->Add(TextCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button9 = new wxButton(Panel1, ID_BUTTON9, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON9"));
    Button9->SetToolTip(_("Open mod file"));
    FlexGridSizer2->Add(Button9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    RichTextCtrl1 = new wxRichTextCtrl(Panel1, ID_RICHTEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(575,353), wxRE_MULTILINE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
    	wxRichTextAttr rchtxtAttr_1;
    RichTextCtrl1->SetToolTip(_("View and edit mod file"));
    FlexGridSizer2->Add(RichTextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    Button1->SetToolTip(_("Save mod file"));
    BoxSizer3->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button7 = new wxButton(Panel1, ID_BUTTON7, _("Save as..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    Button7->SetToolTip(_("Save mod file as..."));
    BoxSizer3->Add(Button7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(Panel1, ID_BUTTON2, _("Apply"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    Button2->SetToolTip(_("Apply mod"));
    BoxSizer3->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button4 = new wxButton(Panel1, ID_BUTTON4, _("Show log"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    Button4->SetToolTip(_("View install log"));
    BoxSizer3->Add(Button4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(BoxSizer3, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer2->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Panel1->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel1);
    BoxSizer2->SetSizeHints(Panel1);
    BoxSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu5 = new wxMenu();
    MenuItem6 = new wxMenuItem(Menu5, ID_MENUITEM9, _("Load\tCTRL+L"), _("Load mod file"), wxITEM_NORMAL);
    Menu5->Append(MenuItem6);
    MenuItem1 = new wxMenuItem(Menu5, ID_MENUITEM4, _("Save\tCTRL+S"), _("Save current mod file"), wxITEM_NORMAL);
    Menu5->Append(MenuItem1);
    MenuItem2 = new wxMenuItem(Menu5, ID_MENUITEM5, _("Save as..."), _("Save current mod file as..."), wxITEM_NORMAL);
    Menu5->Append(MenuItem2);
    MenuItem3 = new wxMenuItem(Menu5, ID_MENUITEM6, _("Quit\tCTRL+X"), _("Quit program"), wxITEM_NORMAL);
    Menu5->Append(MenuItem3);
    MenuBar1->Append(Menu5, _("&File"));
    Menu4 = new wxMenu();
    Menu2 = new wxMenuItem(Menu4, ID_MENUITEM2, _("Disable hash check"), _("Patch game executable to disable hash check"), wxITEM_NORMAL);
    Menu4->Append(Menu2);
    Menu3 = new wxMenuItem(Menu4, ID_MENUITEM3, _("Enable ini loading"), _("Patch game executable to enable ini loading"), wxITEM_NORMAL);
    Menu4->Append(Menu3);
    MenuItem10 = new wxMenuItem(Menu4, ID_MENUITEM13, _("Disable phoning home"), _("Patch game executable to disable ini re-downloading"), wxITEM_NORMAL);
    Menu4->Append(MenuItem10);
    MenuItem7 = new wxMenuItem(Menu4, ID_MENUITEM10, _("Open config folder"), _("Open default config folder in explorer"), wxITEM_NORMAL);
    Menu4->Append(MenuItem7);
    MenuItem8 = new wxMenuItem(Menu4, ID_MENUITEM11, _("Show debug log"), _("Show program debug messages"), wxITEM_NORMAL);
    Menu4->Append(MenuItem8);
    MenuBar1->Append(Menu4, _("&Tools"));
    Menu7 = new wxMenu();
    Menu1 = new wxMenuItem(Menu7, ID_MENUITEM1, _("Settings"), _("Change program settings"), wxITEM_NORMAL);
    Menu7->Append(Menu1);
    MenuItem9 = new wxMenuItem(Menu7, ID_MENUITEM12, _("Disable hash..."), _("Disable specific package hash check"), wxITEM_NORMAL);
    Menu7->Append(MenuItem9);
    MenuBar1->Append(Menu7, _("&Advanced"));
    Menu6 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu6, ID_MENUITEM7, _("About"), _("Show about info"), wxITEM_NORMAL);
    Menu6->Append(MenuItem4);
    MenuItem5 = new wxMenuItem(Menu6, ID_MENUITEM8, _("Readme\tF1"), _("Open readme file"), wxITEM_NORMAL);
    Menu6->Append(MenuItem5);
    MenuBar1->Append(Menu6, _("&Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -10 };
    int __wxStatusBarStyles_1[1] = { wxSB_FLAT };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSelectDirectory);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSelectModFile);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSaveModFile);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnSaveModFileAs);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnInstallMod);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PatcherGUIFrame::OnShowLog);
    Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnSelectModFile);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnSaveModFile);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnSaveModFileAs);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnQuit);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnDisableHashCheck);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnEnableINI);
    Connect(ID_MENUITEM13,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnDisablePhoneHome);
    Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnOpenConfigFolder);
    Connect(ID_MENUITEM11,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnShowDebugLog);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnChangeSettings);
    Connect(ID_MENUITEM12,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::DisableSpecificPackageHashCheck);
    Connect(ID_MENUITEM7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnAbout);
    Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&PatcherGUIFrame::OnShowReadme);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&PatcherGUIFrame::OnClose);
    //*)
    bSelectPath = false;
    bSelectFile = false;
    if (!LoadCFG())
    {
        BackupPathString = wxGetCwd() + "\\Backup";
        PatchUPKprogram = wxGetCwd() + "\\Binaries\\PatchUPK.exe";
        DecompressProgram = wxGetCwd() + "\\Binaries\\Decompress.exe";
    }
    curBackupPathString = "";
    ProgLog = "";
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

void PatcherGUIFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

bool PatcherGUIFrame::LoadCFG()
{
    std::ifstream cfg("PatcherGUI.cfg");
    if (cfg.is_open())
    {
        std::string str = "";
        getline(cfg, str);
        if (wxDirExists(str + "\\XComGame\\CookedPCConsole"))
        {
            TextCtrl1->SetValue(str);
            bSelectPath = true;
            LoadLogs();
        }
        getline(cfg, str);
        BackupPathString = str;
        getline(cfg, str);
        PatchUPKprogram = str;
        getline(cfg, str);
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
            BackupPathString = wxGetCwd() + "\\Binaries\\Backup";
        cfg << BackupPathString << std::endl;
        if (!wxFileExists(PatchUPKprogram))
            PatchUPKprogram = wxGetCwd() + "\\Binaries\\PatchUPK.exe";
        cfg << PatchUPKprogram << std::endl;
        if (!wxFileExists(DecompressProgram))
            DecompressProgram = wxGetCwd() + "\\Binaries\\Decompress.exe";
        cfg << DecompressProgram << std::endl;
        return true;
    }
    return false;
}

void PatcherGUIFrame::OnSaveModFile(wxCommandEvent& event)
{
    if (RichTextCtrl1->GetFilename() != wxEmptyString)
        RichTextCtrl1->SaveFile(RichTextCtrl1->GetFilename(), wxRICHTEXT_TYPE_TEXT);
    else
        SaveModFileAs();
    StatusBar1->PushStatusText("Mod file saved");
}

void PatcherGUIFrame::SaveModFileAs()
{
    wxString defDir = "";

    wxFileName::SplitPath(TextCtrl2->GetValue(), &defDir, NULL, NULL);

    if (!wxDirExists(defDir))
        defDir = "";

    wxFileDialog saveFileDialog(this, _("Select a file"), defDir, "", "Text files (*.txt)|*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    TextCtrl2->SetValue(saveFileDialog.GetPath());
    RichTextCtrl1->SetFilename(saveFileDialog.GetPath());
    RichTextCtrl1->SaveFile(RichTextCtrl1->GetFilename(), wxRICHTEXT_TYPE_TEXT);
    bSelectFile = true;
}

void PatcherGUIFrame::OnSaveModFileAs(wxCommandEvent& event)
{
    SaveModFileAs();
    StatusBar1->PushStatusText("Mod file saved");
}

void PatcherGUIFrame::OnSelectDirectory(wxCommandEvent& event)
{
    wxString defDir = TextCtrl1->GetValue();

    if (!wxDirExists(defDir))
        defDir = "";

    wxDirDialog dirDialog(NULL, _("Select a directory"), defDir, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dirDialog.ShowModal() == wxID_CANCEL)
        return;

    TextCtrl1->SetValue(dirDialog.GetPath());
    bSelectPath = true;
    LoadLogs();

    StatusBar1->PushStatusText("Game directory changed");
}

void PatcherGUIFrame::LoadLogs()
{
    InstList.Clear();
    UninstList.Clear();
    InstLogName = "";

    std::hash<std::string> StrHash;
    size_t HashVal = StrHash(TextCtrl1->GetValue().ToStdString());

    InstLogName << wxGetCwd() << "\\Logs\\" << HashVal << "-log.txt";
    if (wxFileExists(InstLogName) == false)
        return;

    std::ifstream instlog(InstLogName.c_str());
    while (instlog.good())
    {
        std::string str = "";
        getline(instlog, str);
        if(instlog.eof() || str == "")
            break;
        InstList.Add(str);
        getline(instlog, str);
        UninstList.Add(str);
    }
}

void PatcherGUIFrame::SaveLogs()
{
    if (!wxDirExists(wxGetCwd() + "\\Logs"))
        if (!wxMkdir(wxGetCwd() + "\\Logs"))
        {
            wxMessageBox(_("Can't create logs dir!"), _("Error"), wxICON_ERROR | wxOK, this);
            return;
        }

    std::ofstream instlog(InstLogName.c_str());
    for (unsigned i = 0; i < InstList.GetCount(); ++i)
    {
        instlog << InstList[i] << std::endl;
        instlog << UninstList[i] << std::endl;
    }
}

void PatcherGUIFrame::OnSelectModFile(wxCommandEvent& event)
{
    if (RichTextCtrl1->IsModified() && RichTextCtrl1->GetValue() != wxEmptyString)
    {
        if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
            wxICON_QUESTION | wxYES_NO, this) == wxNO)
            return;
    }

    wxString defDir = wxGetCwd() + "\\Mods";

    if (RichTextCtrl1->GetFilename() != wxEmptyString)
        wxFileName::SplitPath(RichTextCtrl1->GetFilename(), &defDir, NULL, NULL);

    if (!wxDirExists(defDir))
        defDir = "";

    wxFileDialog openFileDialog(this, _("Select mod file"), defDir, "", "Text files (*.txt)|*.txt", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    OpenModFile(openFileDialog.GetPath());
    StatusBar1->PushStatusText("Mod file loaded");
}

void PatcherGUIFrame::OpenModFile(wxString FilePath)
{
    TextCtrl2->SetValue(FilePath);
    RichTextCtrl1->SetFilename(FilePath);
    RichTextCtrl1->LoadFile(FilePath, wxRICHTEXT_TYPE_TEXT);
    bSelectFile = true;
}

bool PatcherGUIFrame::NeedDecompression(wxString filename)
{
    wxString sizeFile = filename + ".uncompressed_size";
    if (wxFileExists(sizeFile))
        return true;
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in.is_open())
        return false;
    in.seekg(-3, std::ios::end);
    char val[3];
    in.read(val, 3);
    in.close();
    if (val[0] == 0x11 && val[1] == 0x00 && val[2] == 0x00)
        return true;
    return false;
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
        upkFile = upkFile.substr(upkFile.find_first_not_of(" "));
        upkFile = upkFile.substr(0, upkFile.find_last_not_of(" ") + 1);
        FilesToBackup.Add(upkFile);
        wxString sizeFile = TextCtrl1->GetValue() + "\\XComGame\\CookedPCConsole\\" + upkFile + ".uncompressed_size";
        if (NeedDecompression(TextCtrl1->GetValue() + "\\XComGame\\CookedPCConsole\\" + upkFile))
        {
            FilesToDecompress.Add(upkFile);
            if (wxFileExists(sizeFile))
            {
                FilesToBackup.Add(upkFile + ".uncompressed_size");
                FilesToRemove.Add(upkFile + ".uncompressed_size");
            }
        }
        str = str.substr(pos + 8);
        pos = str.find("UPK_FILE");
    }

    ProgLog = "";

    if (!MakeBackups())
        return;
    if (!DecompressUPK())
        return;
    if (!RemoveSizeFiles())
        return;

    StatusBar1->PushStatusText("Applying mod file, please, wait...");

    long retVal = 0;
    wxString executePatchUPKcommandLineString = PatchUPKprogram.c_str();
    executePatchUPKcommandLineString += " " + TextCtrl2->GetValue().c_str();
    if (bSelectPath)
        executePatchUPKcommandLineString += " " + TextCtrl1->GetValue().c_str() + "\\XComGame\\CookedPCConsole\\";

    wxExecuteEnv env;
    env.cwd = wxPathOnly(TextCtrl2->GetValue());

    PatchUPKoutput.Clear();
    PatchUPKerrors.Clear();

    retVal = wxExecute(executePatchUPKcommandLineString, PatchUPKoutput, PatchUPKerrors, wxEXEC_SYNC, &env);

    ProgLog << "Executing external PatchUPK program:\n" << executePatchUPKcommandLineString << "\n\n";

    for (unsigned i = 0; i < PatchUPKoutput.GetCount(); ++i)
        ProgLog << PatchUPKoutput[i] << "\n";
    for (unsigned i = 0; i < PatchUPKerrors.GetCount(); ++i)
        ProgLog << PatchUPKerrors[i] << "\n";

    if (retVal != 0)
    {
        wxMessageBox(_("Error patching UPK!"), _("Error"), wxICON_ERROR | wxOK, this);
        RestoreFromBackup();
        return;
    }

    ProgLog << "\nMod applied successfully\n\n";

    StatusBar1->PushStatusText("Mod file successfully applied");

    wxMessageBox(_("Patched successfully!"), _("Success"), wxICON_INFORMATION | wxOK, this);

    if (TextCtrl2->GetValue().Find(".uninstall") != wxNOT_FOUND)
    {
        unsigned idx = FindUninstallFileIdx(TextCtrl2->GetValue());
        if (idx < UninstList.GetCount())
        {
            UninstList.RemoveAt(idx);
            InstList.RemoveAt(idx);
        }
    }
    else
    {
        unsigned idx = FindInstallFileIdx(TextCtrl2->GetValue());
        if (idx == InstList.GetCount())
        {
            InstList.Add(TextCtrl2->GetValue());
            UninstList.Add(GetLatestUninstallFile(TextCtrl2->GetValue()));
        }
    }
    SaveLogs();
}

unsigned PatcherGUIFrame::FindUninstallFileIdx(wxString UninstallFile)
{
    for (unsigned i = 0; i < UninstList.GetCount(); ++i)
    {
        if (UninstList[i] == UninstallFile)
            return i;
    }
    return UninstList.GetCount();
}

unsigned PatcherGUIFrame::FindInstallFileIdx(wxString InstallFile)
{
    for (unsigned i = 0; i < InstList.GetCount(); ++i)
    {
        if (InstList[i] == InstallFile)
            return i;
    }
    return InstList.GetCount();
}

wxString PatcherGUIFrame::GetLatestUninstallFile(wxString InstallFile)
{
    wxString Name, prevName;
    unsigned i = 0;
    Name << InstallFile << ".uninstall.txt";
    prevName = "Error.uninstall.txt";
    while (wxFileExists(Name) == true)
    {
        ++i;
        prevName = Name;
        Name = "";
        Name << InstallFile << ".uninstall" << i << ".txt";
    }
    return prevName;
}

bool PatcherGUIFrame::RestoreFromBackup()
{
    if (curBackupPathString == wxEmptyString || !wxDirExists(curBackupPathString))
        return false;

    ProgLog << "Restoring from backup in " << curBackupPathString << "\n";

    for (unsigned i = 0; i < FilesToBackup.GetCount(); ++i)
    {
        wxString copyFromPath = curBackupPathString + wxString("\\") + FilesToBackup[i];
        wxString copyToPath = TextCtrl1->GetValue() + wxString("\\XComGame\\CookedPCConsole\\") + FilesToBackup[i];
        if (!wxCopyFile(copyFromPath, copyToPath, true))
        {
            wxMessageBox(_("Can't restore from backup!"), _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }
        ProgLog << FilesToBackup[i] << " restored from backup dir\n";
    }

    ProgLog << "Restoring from backup completed successfully\n\n";

    StatusBar1->PushStatusText("Backup files restored");

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

    ProgLog << "Writing backups to " << curBackupPathString << "\n";

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
            ProgLog << FilesToBackup[i] << " saved to backup dir\n";
        }
    }

    ProgLog << "Backup completed successfully\n\n";

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
        wxString executeDecompressCommandLineString = DecompressProgram;
        executeDecompressCommandLineString += " -lzo -ps3";
        executeDecompressCommandLineString += " -out=" + TextCtrl1->GetValue() + "\\XComGame\\CookedPCConsole";
        executeDecompressCommandLineString += " " + FilesToDecompress[i];

        wxExecuteEnv env;
        env.cwd = curBackupPathString;

        DecompressOutput.Clear();
        DecompressErrors.Clear();

        retVal = wxExecute(executeDecompressCommandLineString, DecompressOutput, DecompressErrors, wxEXEC_SYNC, &env);

        ProgLog << "Executing external decompress program:\n" << executeDecompressCommandLineString << "\n\n";

        for (unsigned k = 0; k < DecompressOutput.GetCount(); ++k)
            ProgLog << DecompressOutput[k] << "\n";
        for (unsigned k = 0; k < DecompressErrors.GetCount(); ++k)
            ProgLog << DecompressErrors[k] << "\n";

        if (retVal != 0)
        {
            wxMessageBox(_("Error decompressing ") + FilesToDecompress[i], _("Error"), wxICON_ERROR | wxOK, this);
            return false;
        }
    }

    ProgLog << "Decompression completed successfully\n\n";

    return true;
}

bool PatcherGUIFrame::RemoveSizeFiles()
{
    if (FilesToRemove.GetCount() == 0)
        return true;

    ProgLog << "Deleting size files\n";

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
        ProgLog << FilesToRemove[i] << " deleted\n";
    }

    ProgLog << "Files deleted successfully\n\n";

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

void PatcherGUIFrame::OnShowLog(wxCommandEvent& event)
{
    ViewLog dlg(this);

    dlg.SetParams(TextCtrl1->GetValue(), InstList);

    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    wxString FileToOpen;

    if (dlg.IsInstaller())
        FileToOpen = InstList[dlg.GetSelection()];
    else
        FileToOpen = UninstList[dlg.GetSelection()];

    OpenModFile(FileToOpen);
}

void PatcherGUIFrame::OnDisableHashCheck(wxCommandEvent& event)
{
    wxString exePath = TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComGame.exe";

    if (!wxFileExists(exePath))
    {
        if (wxFileExists(TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComEW.exe"))
        {
            wxMessageBox(_("XCOM:EW already has hash check removed by developers!"), _("Information"), wxICON_INFORMATION | wxOK, this);
            return;
        }
        wxMessageBox(_("Can't find game executable!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    if (!wxFileExists(exePath + ".bak"))
    {
        if (!wxCopyFile(exePath, exePath + ".bak"))
        {
            wxMessageBox(_("Failed to write backup!"), _("Error"), wxICON_ERROR | wxOK, this);
            return;
        }
    }

    std::ifstream exeFile(exePath.c_str(), std::ios::binary);
    std::stringstream ss;
    ss << exeFile.rdbuf();

    size_t pos = std::string::npos;

    pos = ss.str().find("core.upk");
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('y');
    }
    pos = ss.str().find("engine.upk");
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('y');
    }
    pos = ss.str().find("xcomgame.upk");
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('y');
    }
    pos = ss.str().find("xcomstrategygame.upk");
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('y');
    }

    exeFile.close();

    std::ofstream exeFleOut(exePath.c_str(), std::ios::binary);
    exeFleOut << ss.str();

    StatusBar1->PushStatusText("Hash check successfully disabled");

    wxMessageBox(_("Has check disabled successfully!"), _("Success"), wxICON_INFORMATION | wxOK, this);
}

void PatcherGUIFrame::OnEnableINI(wxCommandEvent& event)
{
    wxString exePath = TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComGame.exe";

    if (!wxFileExists(exePath))
        exePath = TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComEW.exe";

    if (!wxFileExists(exePath))
    {
        wxMessageBox(_("Can't find game executable!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    if (!wxFileExists(exePath + ".bak"))
    {
        if (!wxCopyFile(exePath, exePath + ".bak"))
        {
            wxMessageBox(_("Failed to write backup!"), _("Error"), wxICON_ERROR | wxOK, this);
            return;
        }
    }

    std::ifstream exeFile(exePath.c_str(), std::ios::binary);
    std::stringstream ss;
    ss << exeFile.rdbuf();

    size_t pos = std::string::npos;

    char chdgc[] = {0x58 , 0x00 , 0x43 , 0x00 , 0x6F , 0x00 , 0x6D , 0x00 , 0x47 , 0x00 , 0x61 , 0x00 , 0x6D , 0x00 , 0x65 , 0x00 , 0x5C , 0x00 , 0x43 , 0x00 , 0x6F , 0x00 , 0x6E , 0x00 , 0x66 , 0x00 , 0x69 , 0x00 , 0x67 , 0x00 , 0x5C , 0x00 , 0x44 , 0x00 , 0x65 , 0x00 , 0x66 , 0x00 , 0x61 , 0x00 , 0x75 , 0x00 , 0x6C , 0x00 , 0x74 , 0x00 , 0x47 , 0x00 , 0x61 , 0x00 , 0x6D , 0x00 , 0x65 , 0x00 , 0x43 , 0x00 , 0x6F , 0x00 , 0x72 , 0x00 , 0x65 , 0x00 , 0x2E , 0x00 , 0x69 , 0x00 , 0x6E , 0x00 , 0x69 , 0x00};
    char chdl[]  = {0x58 , 0x00 , 0x43 , 0x00 , 0x6F , 0x00 , 0x6D , 0x00 , 0x47 , 0x00 , 0x61 , 0x00 , 0x6D , 0x00 , 0x65 , 0x00 , 0x5C , 0x00 , 0x43 , 0x00 , 0x6F , 0x00 , 0x6E , 0x00 , 0x66 , 0x00 , 0x69 , 0x00 , 0x67 , 0x00 , 0x5C , 0x00 , 0x44 , 0x00 , 0x65 , 0x00 , 0x66 , 0x00 , 0x61 , 0x00 , 0x75 , 0x00 , 0x6C , 0x00 , 0x74 , 0x00 , 0x4C , 0x00 , 0x6F , 0x00 , 0x61 , 0x00 , 0x64 , 0x00 , 0x6F , 0x00 , 0x75 , 0x00 , 0x74 , 0x00 , 0x73 , 0x00 , 0x2E , 0x00 , 0x69 , 0x00 , 0x6E , 0x00 , 0x69 , 0x00};

    std::string dgc(chdgc, 70), dl(chdl, 70);

    pos = ss.str().find(dgc);
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('Y');
        pos = ss.str().find(dgc, pos + 70);
        if (pos != std::string::npos)
        {
            ss.seekp(pos);
            ss.put('Y');
        }
    }
    pos = ss.str().find(dl);
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('Y');
        pos = ss.str().find(dl, pos + 70);
        if (pos != std::string::npos)
        {
            ss.seekp(pos);
            ss.put('Y');
        }
    }

    exeFile.close();

    std::ofstream exeFleOut(exePath.c_str(), std::ios::binary);
    exeFleOut << ss.str();

    StatusBar1->PushStatusText("INI loading successfully enabled");

    wxMessageBox(_("INI loading enabled successfully!"), _("Success"), wxICON_INFORMATION | wxOK, this);
}

void PatcherGUIFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg;
    msg << "PatcherGUI — a tool to install and maintain modifications for XCOM:EU and EW.\n\n"
        << "Current version: 5.2\n\n"
        << "Author: wghost81 aka Wasteland Ghost\n\n"
        << "All trademarks, mentioned herein, are properties of their respective owners.";
    wxMessageBox(msg, _("About PatcherGUI"), wxICON_INFORMATION | wxOK, this);
}

void PatcherGUIFrame::OnShowReadme(wxCommandEvent& event)
{
    wxString pathToReadmeFile = wxGetCwd() + "\\PatcherGUIReadme.txt";

    if (!wxFileExists(pathToReadmeFile))
    {
        wxMessageBox(_("Can't find readme file!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension("txt");
    wxString cmd;
    if (ft)
    {
        ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(pathToReadmeFile));
        wxExecute(cmd, wxEXEC_ASYNC);
        delete ft;
    }
}

void PatcherGUIFrame::OnOpenConfigFolder(wxCommandEvent& event)
{
    wxString configPath = TextCtrl1->GetValue() + "\\XComGame\\Config";

    if (!wxDirExists(configPath))
    {
        wxMessageBox(_("Can't find config folder!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    wxLaunchDefaultBrowser(wxString("file:") + configPath, wxBROWSER_NEW_WINDOW);
}

void PatcherGUIFrame::OnShowDebugLog(wxCommandEvent& event)
{
    ShowDebugLogDialog dlg(this);

    dlg.TextCtrl1->SetValue(ProgLog);

    dlg.ShowModal();
}

void PatcherGUIFrame::DisableSpecificPackageHashCheck(wxCommandEvent& event)
{
    wxString exePath = TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComGame.exe";

    if (!wxFileExists(exePath))
    {
        if (wxFileExists(TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComEW.exe"))
        {
            wxMessageBox(_("XCOM:EW already has hash check removed by developers!"), _("Information"), wxICON_INFORMATION | wxOK, this);
            return;
        }
        wxMessageBox(_("Can't find game executable!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    if (!wxFileExists(exePath + ".bak"))
    {
        if (!wxCopyFile(exePath, exePath + ".bak"))
        {
            wxMessageBox(_("Failed to write backup!"), _("Error"), wxICON_ERROR | wxOK, this);
            return;
        }
    }

    wxTextEntryDialog dlg(this, "Input package name", "Disable hash check");

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    wxString nameToFind = dlg.GetValue().Lower();

    if (nameToFind.Find(".upk") == wxNOT_FOUND)
    {
        wxMessageBox(nameToFind + " is not a package!", _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    std::ifstream exeFile(exePath.c_str(), std::ios::binary);
    std::stringstream ss;
    ss << exeFile.rdbuf();

    size_t pos = std::string::npos;

    pos = ss.str().find(nameToFind);
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('y');
    }
    else
    {
        nameToFind[0] = 'y';
        pos = ss.str().find(nameToFind);
        if (pos != std::string::npos)
        {
            wxMessageBox(_("Hash check for this package already disabled!"), _("Error"), wxICON_ERROR | wxOK, this);
        }
        else
        {
            wxMessageBox(_("Can't find specified package reference!"), _("Error"), wxICON_ERROR | wxOK, this);
        }
        return;
    }

    exeFile.close();

    std::ofstream exeFleOut(exePath.c_str(), std::ios::binary);
    exeFleOut << ss.str();

    StatusBar1->PushStatusText("Hash check successfully disabled");

    wxMessageBox(_("Has check disabled successfully!"), _("Success"), wxICON_INFORMATION | wxOK, this);
}

void PatcherGUIFrame::OnDisablePhoneHome(wxCommandEvent& event)
{
    wxString exePath = TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComGame.exe";

    if (!wxFileExists(exePath))
        exePath = TextCtrl1->GetValue() + "\\Binaries\\Win32\\XComEW.exe";

    if (!wxFileExists(exePath))
    {
        wxMessageBox(_("Can't find game executable!"), _("Error"), wxICON_ERROR | wxOK, this);
        return;
    }

    if (!wxFileExists(exePath + ".bak"))
    {
        if (!wxCopyFile(exePath, exePath + ".bak"))
        {
            wxMessageBox(_("Failed to write backup!"), _("Error"), wxICON_ERROR | wxOK, this);
            return;
        }
    }

    std::ifstream exeFile(exePath.c_str(), std::ios::binary);
    std::stringstream ss;
    ss << exeFile.rdbuf();

    size_t pos = std::string::npos;

    char chAddr[] = {0x66, 0x00, 0x69, 0x00, 0x72, 0x00, 0x61, 0x00, 0x78, 0x00, 0x69, 0x00, 0x73, 0x00, 0x2E, 0x00, 0x63, 0x00, 0x6F, 0x00, 0x6D, 0x00};

    std::string addr(chAddr, 22);

    pos = ss.str().find(addr);
    if (pos != std::string::npos)
    {
        ss.seekp(pos);
        ss.put('y');
    }
    exeFile.close();

    std::ofstream exeFleOut(exePath.c_str(), std::ios::binary);
    exeFleOut << ss.str();

    StatusBar1->PushStatusText("INI downloading successfully disabled");

    wxMessageBox(_("INI downloading disabled successfully!"), _("Success"), wxICON_INFORMATION | wxOK, this);
}
