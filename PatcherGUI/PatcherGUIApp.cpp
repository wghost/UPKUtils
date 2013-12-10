/***************************************************************
 * Name:      PatcherGUIApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Wasteland Ghost (wghost81@gmail.com)
 * Created:   2013-12-09
 * Copyright: Wasteland Ghost ()
 * License:
 **************************************************************/

#include "PatcherGUIApp.h"

//(*AppHeaders
#include "PatcherGUIMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(PatcherGUIApp);

bool PatcherGUIApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	PatcherGUIFrame* Frame = new PatcherGUIFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
