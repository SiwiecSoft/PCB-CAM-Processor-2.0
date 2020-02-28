///-----------------------------------------------------------------
///
/// @file      OptDlg.h
/// @author    rado4ever
/// Created:   2019-03-04 17:20:07
/// @section   DESCRIPTION
///            OptDlg class declaration
///
///------------------------------------------------------------------

#ifndef __OPTDLG_H__
#define __OPTDLG_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/dialog.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/filedlg.h>
#include <wx/timer.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/choice.h>
#include <wx/checklst.h>
////Header Include End

////Dialog Style Start
#undef OptDlg_STYLE
#define OptDlg_STYLE wxCAPTION | wxSYSTEM_MENU | wxSTAY_ON_TOP | wxDIALOG_NO_PARENT | wxCLOSE_BOX
////Dialog Style End

#include "V2x/V2xSolTSPN.h"

#include "MySource\inc\myrequester.h"


class OptDlg : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE(); 
		
	public:
		void WxCheckListBox1CheckListBox(wxCommandEvent& event);
		OptDlg(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Optymalizacja rozwi¹zania"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = OptDlg_STYLE);
		virtual ~OptDlg();
		
		V2xSolTSPArray          *SolTSPArray;
		V2xSolTSPNArray         *SolTSPNArray;
		
		wxString MainFileName;
		
		void RefreshWindow( wxString FileName );
		void RefreshSolList();
		void RefreshParamList();
		void RefreshParamText();
		void RefreshSolNames();
		void ChangeParam();
		void Solve();
		void AppendToReport( V2xSol *Sol );
		unsigned int GetSelCS();
		unsigned int GetSelTS();
		bool GetDrawRequest();
		
		void WxCheckListBox1DoubleClicked(wxCommandEvent& event);
		void WxListBox1Selected(wxCommandEvent& event);
		void WxButton2Click(wxCommandEvent& event);
		void WxButton1Click(wxCommandEvent& event);
		void WxTimer1Timer(wxTimerEvent& event);
		void WxChoice1Selected(wxCommandEvent& event );
		void WxEdit1Enter(wxCommandEvent& event);
		void WxButton3Click(wxCommandEvent& event);
		void WxButton4Click(wxCommandEvent& event);
		void WxButton5Click(wxCommandEvent& event);
	
	private:
        unsigned int lastCheckListBoxSelection;
        bool         redrawRequest;
        
		//Do not add custom control declarations between 
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxFileDialog *WxSaveFileDialog1;
		wxTimer *WxTimer1;
		wxButton *WxButton5;
		wxButton *WxButton4;
		wxButton *WxButton3;
		wxTextCtrl *WxMemo1;
		wxButton *WxButton2;
		wxButton *WxButton1;
		wxTextCtrl *WxEdit1;
		wxListBox *WxListBox1;
		wxChoice *WxChoice1;
		wxCheckListBox *WxCheckListBox1;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_WXTIMER1 = 1007,
			ID_WXBUTTON5 = 1012,
			ID_WXBUTTON4 = 1011,
			ID_WXBUTTON3 = 1009,
			ID_WXMEMO1 = 1008,
			ID_WXBUTTON2 = 1006,
			ID_WXBUTTON1 = 1005,
			ID_WXEDIT1 = 1004,
			ID_WXLISTBOX1 = 1003,
			ID_WXCHOICE1 = 1002,
			ID_WXCHECKLISTBOX1 = 1001,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
	
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
};

#endif
