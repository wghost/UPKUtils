#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//(*Headers(SettingsDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class SettingsDialog: public wxDialog
{
	public:
		SettingsDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~SettingsDialog();

		//(*Declarations(SettingsDialog)
		wxTextCtrl* TextCtrl4;
		wxButton* Button4;
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* Button2;
		wxButton* Button3;
		wxTextCtrl* TextCtrl2;
		wxTextCtrl* TextCtrl1;
		wxStaticText* StaticText4;
		wxTextCtrl* TextCtrl3;
		//*)

	protected:

		//(*Identifiers(SettingsDialog)
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL3;
		static const long ID_BUTTON3;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL4;
		static const long ID_BUTTON4;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(SettingsDialog)
		void OnSaveSettings(wxCommandEvent& event);
		void OnOK(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void OnSetGamePath(wxCommandEvent& event);
		void OnSetBackupPath(wxCommandEvent& event);
		void OnSetPatchUPKprogram(wxCommandEvent& event);
		void OnLoadSettings(wxCommandEvent& event);
		void OnSetDecompressProgramm(wxCommandEvent& event);
		void OnSetXshapeProgram(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
