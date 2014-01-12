/***************************************************************
 * Name:      PatcherGUIMain.h
 * Purpose:   Defines Application Frame
 * Author:    Wasteland Ghost (wghost81@gmail.com)
 * Created:   2013-12-09
 * Copyright: Wasteland Ghost ()
 * License:
 **************************************************************/

#ifndef PATCHERGUIMAIN_H
#define PATCHERGUIMAIN_H

//(*Headers(PatcherGUIFrame)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)
#include <wx/arrstr.h>

class PatcherGUIFrame: public wxFrame
{
    public:

        PatcherGUIFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~PatcherGUIFrame();

    private:

        bool bSelectPath;
        bool bSelectFile;
        wxArrayString PatchUPKoutput;
        wxArrayString PatchUPKerrors;
        wxArrayString DecompressOutput;
        wxArrayString DecompressErrors;
        wxString BackupPathString;
        wxString PatchUPKprogram;
        wxString DecompressProgram;
        wxArrayString FilesToBackup;
        wxArrayString FilesToRemove;
        wxArrayString FilesToDecompress;
        wxString curBackupPathString;

        bool RestoreFromBackup();
        bool MakeBackups();
        bool RemoveSizeFiles();
        bool DecompressUPK();
        bool LoadCFG();
        bool SaveCFG();

        bool NeedDecompression(wxString filename);

        //(*Handlers(PatcherGUIFrame)
        void OnSaveModFile(wxCommandEvent& event);
        void OnSaveModFileAs(wxCommandEvent& event);
        void OnInstallMod(wxCommandEvent& event);
        void OnSelectDirectory(wxCommandEvent& event);
        void OnSelectModFile(wxCommandEvent& event);
        void OnChangeSettings(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        //*)

        //(*Identifiers(PatcherGUIFrame)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON8;
        static const long ID_TEXTCTRL2;
        static const long ID_BUTTON9;
        static const long ID_RICHTEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON7;
        static const long ID_BUTTON2;
        static const long ID_BUTTON5;
        static const long ID_TEXTCTRL3;
        static const long ID_PANEL1;
        //*)

        //(*Declarations(PatcherGUIFrame)
        wxRichTextCtrl* RichTextCtrl1;
        wxButton* Button1;
        wxPanel* Panel1;
        wxButton* Button2;
        wxButton* Button5;
        wxButton* Button7;
        wxButton* Button9;
        wxTextCtrl* TextCtrl2;
        wxTextCtrl* TextCtrl1;
        wxTextCtrl* TextCtrl3;
        wxButton* Button8;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // PATCHERGUIMAIN_H
