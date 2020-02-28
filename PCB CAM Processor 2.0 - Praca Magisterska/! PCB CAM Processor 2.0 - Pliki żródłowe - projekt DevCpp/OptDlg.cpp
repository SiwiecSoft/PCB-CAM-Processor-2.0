///-----------------------------------------------------------------
///
/// @file      OptDlg.cpp
/// @author    rado4ever
/// Created:   2019-03-04 17:20:07
/// @section   DESCRIPTION
///            OptDlg class implementation
///
///------------------------------------------------------------------

#include "OptDlg.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// OptDlg
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(OptDlg,wxDialog)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(OptDlg::OnClose)
	EVT_TIMER(ID_WXTIMER1,OptDlg::WxTimer1Timer)
	EVT_BUTTON(ID_WXBUTTON5,OptDlg::WxButton5Click)
	EVT_BUTTON(ID_WXBUTTON4,OptDlg::WxButton4Click)
	EVT_BUTTON(ID_WXBUTTON3,OptDlg::WxButton3Click)
	EVT_BUTTON(ID_WXBUTTON2,OptDlg::WxButton2Click)
	EVT_BUTTON(ID_WXBUTTON1,OptDlg::WxButton1Click)
	EVT_TEXT_ENTER(ID_WXEDIT1,OptDlg::WxEdit1Enter)
	EVT_LISTBOX(ID_WXLISTBOX1,OptDlg::WxListBox1Selected)
	EVT_CHOICE(ID_WXCHOICE1,OptDlg::WxChoice1Selected)
	EVT_CHECKLISTBOX(ID_WXCHECKLISTBOX1,OptDlg::WxCheckListBox1CheckListBox)
	EVT_LISTBOX_DCLICK(ID_WXCHECKLISTBOX1,OptDlg::WxCheckListBox1DoubleClicked)
END_EVENT_TABLE()
////Event Table End

OptDlg::OptDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

OptDlg::~OptDlg()
{
} 

void OptDlg::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	WxSaveFileDialog1 =  new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("*.log*"), wxSAVE);

	WxTimer1 = new wxTimer();
	WxTimer1->SetOwner(this, ID_WXTIMER1);
	WxTimer1->Start(50);

	WxButton5 = new wxButton(this, ID_WXBUTTON5, wxT("Zapisz"), wxPoint(1075, 510), wxSize(75, 30), 0, wxDefaultValidator, wxT("WxButton5"));

	WxButton4 = new wxButton(this, ID_WXBUTTON4, wxT("Wyczyœæ"), wxPoint(995, 510), wxSize(75, 30), 0, wxDefaultValidator, wxT("WxButton4"));

	WxButton3 = new wxButton(this, ID_WXBUTTON3, wxT("Raport ->"), wxPoint(365, 510), wxSize(75, 30), 0, wxDefaultValidator, wxT("WxButton3"));

	WxMemo1 = new wxTextCtrl(this, ID_WXMEMO1, wxEmptyString, wxPoint(450, 5), wxSize(701, 496), wxTE_DONTWRAP | wxTE_MULTILINE, wxDefaultValidator, wxT("WxMemo1"));
	WxMemo1->SetMaxLength(0);
	WxMemo1->AppendText(wxT("WxMemo1"));
	WxMemo1->SetFocus();
	WxMemo1->SetInsertionPointEnd();

	WxButton2 = new wxButton(this, ID_WXBUTTON2, wxT("Rozwi¹¿ wybranymi metodami"), wxPoint(5, 510), wxSize(350, 30), 0, wxDefaultValidator, wxT("WxButton2"));

	WxButton1 = new wxButton(this, ID_WXBUTTON1, wxT("Zmieñ"), wxPoint(365, 475), wxSize(75, 25), 0, wxDefaultValidator, wxT("WxButton1"));

	WxEdit1 = new wxTextCtrl(this, ID_WXEDIT1, wxT(""), wxPoint(5, 475), wxSize(351, 24), 0, wxDefaultValidator, wxT("WxEdit1"));

	wxArrayString arrayStringFor_WxListBox1;
	WxListBox1 = new wxListBox(this, ID_WXLISTBOX1, wxPoint(5, 365), wxSize(436, 101), arrayStringFor_WxListBox1, wxLB_SINGLE);

	wxArrayString arrayStringFor_WxChoice1;
	arrayStringFor_WxChoice1.Add(wxT("Optymalizacja wykonania konturów"));
	arrayStringFor_WxChoice1.Add(wxT("Optymalizacja wykonania otworów"));
	WxChoice1 = new wxChoice(this, ID_WXCHOICE1, wxPoint(5, 5), wxSize(435, 23), arrayStringFor_WxChoice1, 0, wxDefaultValidator, wxT("WxChoice1"));
	WxChoice1->SetSelection(0);

	wxArrayString arrayStringFor_WxCheckListBox1;
	WxCheckListBox1 = new wxCheckListBox(this, ID_WXCHECKLISTBOX1, wxPoint(5, 35), wxSize(436, 321), arrayStringFor_WxCheckListBox1, wxLB_SINGLE, wxDefaultValidator, wxT("WxCheckListBox1"));

	SetTitle(wxT("Optymalizacja rozwi¹zania"));
	SetIcon(wxNullIcon);
	SetSize(8,8,1172,582);
	Center();
	
	////GUI Items Creation End
	
	SetTitle(wxT("Mened¿er rozwi¹zañ"));
	SetSize( 455, 575 );
	WxMemo1->Clear();
}

void OptDlg::OnClose(wxCloseEvent& /*event*/)
{
	Show(false);
}

void OptDlg::RefreshWindow( wxString FileName )
{
    MainFileName = FileName;
    WxMemo1->Clear();
    WxMemo1->AppendText( MainFileName + "\n\n" );
    
    RefreshSolList();
}
void OptDlg::RefreshSolList()
{
    WxCheckListBox1->Clear();
    
    if ( WxChoice1->GetSelection() == 0 )
    {
        for ( int i = 0; i < SolTSPNArray->GetCount(); i ++ )
        {
            WxCheckListBox1->Append( SolTSPNArray->Item(i)->GetName() );
            WxCheckListBox1->Check( i, SolTSPNArray->Item(i)->IsDefaultActive() );
        }
    }
    else
    {
        for ( int i = 0; i < SolTSPArray->GetCount(); i ++ )
        {
            WxCheckListBox1->Append( SolTSPArray->Item(i)->GetName() );
            WxCheckListBox1->Check( i, SolTSPArray->Item(i)->IsDefaultActive() );
        }
    }
    
    WxCheckListBox1->SetSelection(0);
    RefreshParamList();
}

void OptDlg::RefreshParamList()
{
    wxArrayString ParamsNames;
    
    WxListBox1->Clear();
    
    if ( WxChoice1->GetSelection() == 0 ) SolTSPNArray->Item( WxCheckListBox1->GetSelection() )->GetParamsNames( &ParamsNames );
    else                                  SolTSPArray ->Item( WxCheckListBox1->GetSelection() )->GetParamsNames( &ParamsNames );
    
	WxListBox1->InsertItems( ParamsNames, 0 );
	WxListBox1->SetSelection(0);
	RefreshParamText();
}

void OptDlg::RefreshParamText()
{
    V2xParam Param;
    
    if ( WxChoice1->GetSelection() == 0 ) Param = SolTSPNArray->Item( WxCheckListBox1->GetSelection() )->GetParam( WxListBox1->GetSelection() );
    else                                  Param = SolTSPArray ->Item( WxCheckListBox1->GetSelection() )->GetParam( WxListBox1->GetSelection() );
    
	WxEdit1->SetValue( wxString::Format( "%1.3f", Param.GetValue() ) );
}

void OptDlg::RefreshSolNames()
{
    if ( WxChoice1->GetSelection() == 0 )
    {
        for ( int i = 0; ( i < WxCheckListBox1->GetCount() && i < SolTSPNArray->GetCount() ); i ++ )
            WxCheckListBox1->SetString( i, SolTSPNArray->Item(i)->GetName() );
    }
    else
    {
        for ( int i = 0; ( i < WxCheckListBox1->GetCount() && i < SolTSPArray->GetCount() ); i ++ )
            WxCheckListBox1->SetString( i, SolTSPArray->Item(i)->GetName() );
    }
}

void OptDlg::ChangeParam()
{
    if ( WxChoice1->GetSelection() == 0 ) SolTSPNArray->Item( WxCheckListBox1->GetSelection() )->SetParamValue( WxListBox1->GetStringSelection(), WxEdit1->GetValue() );
    else                                  SolTSPArray ->Item( WxCheckListBox1->GetSelection() )->SetParamValue( WxListBox1->GetStringSelection(), WxEdit1->GetValue() );
    
	RefreshParamText();
}

void OptDlg::Solve()
{
    if ( WxChoice1->GetSelection() == 0 )
    {
        for ( int i = 0; ( i < WxCheckListBox1->GetCount() && i < SolTSPNArray->GetCount() ); i ++ )
        {
            if ( WxCheckListBox1->IsChecked(i) ) 
            {
                SolTSPNArray->Item(i)->Solve();
                WxCheckListBox1->SetString( i, SolTSPNArray->Item(i)->GetName() );
                
                AppendToReport( SolTSPNArray->Item(i) );
            }
        }
    }
    else
    {
        for ( int i = 0; ( i < WxCheckListBox1->GetCount() && i < SolTSPArray->GetCount() ); i ++ )
        {
            if ( WxCheckListBox1->IsChecked(i) ) 
            {
                SolTSPArray->Item(i)->Solve();
                WxCheckListBox1->SetString( i, SolTSPArray->Item(i)->GetName() );
            
                AppendToReport( SolTSPArray->Item(i) );
            }
        }
    }
}

void OptDlg::AppendToReport( V2xSol *Sol )
{
    V2xParam Param;
    
    WxMemo1->AppendText( Sol->GetName() + "\n" );
    for ( int i = 0; i < Sol->GetParamCount(); i++ )
    {
        Param = Sol->GetParam(i);
        WxMemo1->AppendText( wxString::Format( "%1.3f\t - ", Param.GetValue() ) + Param.GetName() + "\n" );
    }
    WxMemo1->AppendText( Sol->GetOutData() );
    WxMemo1->AppendText( "\n" );
}

unsigned int OptDlg::GetSelCS()
{
    static int Sel = 0;
    
    if ( WxChoice1->GetSelection() == 0 ) 
        Sel = WxCheckListBox1->GetSelection();
    
    return Sel;
}

unsigned int OptDlg::GetSelTS()
{
    static int Sel = 0;
    
    if ( WxChoice1->GetSelection() == 1 ) 
        Sel = WxCheckListBox1->GetSelection();
    
    return Sel;
}

bool OptDlg::GetDrawRequest() 
{ 
    if ( redrawRequest ) 
    {
        redrawRequest = false;
        return true; 
    }
    else return false;
}
	
/*
 * WxCheckListBox1CheckListBox
 */
void OptDlg::WxCheckListBox1CheckListBox(wxCommandEvent& event)
{
	
}

/*
 * WxCheckListBox1DoubleClicked
 */
void OptDlg::WxCheckListBox1DoubleClicked(wxCommandEvent& event)
{
	int Sel = WxCheckListBox1->GetSelection();
	int Count;
	double Len, Time, LenAvg = 0.0, TimeAvg = 0.0;
	double MinSolLen = 10e100, MinSolTime = 10e100;
	double MaxSolLen = 0.0, MaxSolTime = 0.0;
	wxString Name, Str = "\nWyniki: \n\n";
    MyRequester MyRequester1(this);
    
    Name = WxChoice1->GetSelection() == 0 ? SolTSPNArray->Item(Sel)->GetName() : SolTSPArray->Item(Sel)->GetName();
    
    if ( MyRequester1.AskUserInt( "10", "Zapêtliæ obliczenia(" + Name + ") wybran¹ iloœæ razy?" ) )
	{
        Count = MyRequester1.GetInt();
        
        for ( int i = 0; i < Count; i++ )
        {
            if ( WxChoice1->GetSelection() == 0 ) 
            {
                SolTSPNArray->Item(Sel)->Solve();
                AppendToReport( SolTSPNArray->Item(Sel) );
                SolTSPNArray->Item(Sel)->GetResults( &Len, &Time );
                Name = SolTSPNArray->Item(Sel)->GetName();
                
                Str += Name + "\n";
                if ( Len < MinSolLen ) 
                { 
                    MinSolLen = Len; 
                    MinSolTime = Time; 
                    SolTSPNArray->Item(Sel)->wtfBackUpSol();
                    WxCheckListBox1->SetString( Sel, Name );
                }
            }
            else
            {
                SolTSPArray->Item(Sel)->Solve();
                AppendToReport( SolTSPArray->Item(Sel) );
                SolTSPArray->Item(Sel)->GetResults( &Len, &Time );
                Name = SolTSPArray->Item(Sel)->GetName();
                
                Str += Name + "\n";
                if ( Len < MinSolLen ) 
                { 
                    MinSolLen = Len; 
                    MinSolTime = Time; 
                    SolTSPArray->Item(Sel)->wtfBackUpSol();
                    WxCheckListBox1->SetString( Sel, Name );
                }
            }
            
            if ( Len > MaxSolLen ) { MaxSolLen = Len; MaxSolTime = Time; };

            LenAvg  += Len;
            TimeAvg += Time;
        }
        
        LenAvg  /= Count;
        TimeAvg /= Count;
        
        Str += wxString::Format(  "\nMin wynik:\t%1.1f mm (%1.0f ms)\n",   MinSolLen, MinSolTime );
        Str += wxString::Format(    "Max wynik:\t%1.1f mm (%1.0f ms)\n",   MaxSolLen, MaxSolTime );
        Str += wxString::Format( "Œredni wynik:\t%1.1f mm (%1.0f ms)\n\n", LenAvg,    TimeAvg );
        
        WxMemo1->AppendText( Str );
        //wxMessageBox( Str );
    }
    
    if ( WxChoice1->GetSelection() == 0 ) SolTSPNArray->Item(Sel)->wtfRestoreSol();
    else                                  SolTSPArray ->Item(Sel)->wtfRestoreSol();
    
    RefreshParamList(); 
}

/*
 * WxListBox1Selected
 */
void OptDlg::WxListBox1Selected(wxCommandEvent& event)
{
	RefreshParamText();
}

/*
 * WxButton1Click // Change param
 */
void OptDlg::WxButton1Click(wxCommandEvent& event)
{
	ChangeParam();
}

/*
 * WxEdit1Enter
 */
void OptDlg::WxEdit1Enter(wxCommandEvent& event)
{
	ChangeParam();
}


/*
 * WxButton2Click // Recalc
 */
void OptDlg::WxButton2Click(wxCommandEvent& event)
{
	Solve();
	redrawRequest = true;
}

/*
 * WxTimer1Timer
 */
void OptDlg::WxTimer1Timer(wxTimerEvent& event)
{
	if ( WxCheckListBox1->GetSelection() != lastCheckListBoxSelection )
	{
        lastCheckListBoxSelection = WxCheckListBox1->GetSelection();
        RefreshParamList();
        redrawRequest = true;
    }
}

/*
 * WxChoice1Selected
 */
void OptDlg::WxChoice1Selected(wxCommandEvent& event )
{
	RefreshSolList();
}

/*
 * WxButton3Click
 */
void OptDlg::WxButton3Click(wxCommandEvent& event)
{
	static bool Clicked = false;
	
	if ( Clicked )
	{
        SetSize( 455, 575 );
        Clicked = false;
    }
    else
    {
        SetSize( 1165, 575 );
        Clicked = true;
    }
}

/*
 * WxButton4Click // wyczysc
 */
void OptDlg::WxButton4Click(wxCommandEvent& event)
{
	// insert your code here
	WxMemo1->Clear();
    WxMemo1->AppendText( MainFileName + "\n\n" );
}

/*
 * WxButton5Click zapisz
 */
void OptDlg::WxButton5Click(wxCommandEvent& event)
{
    wxString FileName = MainFileName.AfterLast('/');
    WxSaveFileDialog1->SetFilename( FileName.BeforeLast('.') + ".log" ); 
    
    if ( WxSaveFileDialog1->ShowModal() == wxID_OK )
    {
        WxMemo1->SaveFile( WxSaveFileDialog1->GetPath() );
    }
}
