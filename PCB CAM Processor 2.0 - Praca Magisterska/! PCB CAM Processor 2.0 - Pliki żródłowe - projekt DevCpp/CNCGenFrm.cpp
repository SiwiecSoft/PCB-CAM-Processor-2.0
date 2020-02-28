///-----------------------------------------------------------------
///
/// @file      CNCGenFrm.cpp
/// @author    rado4ever
/// Created:   2016-02-15 21:18:13
/// @section   DESCRIPTION
///            CNCGenFrm class implementation
///
///------------------------------------------------------------------

#include "CNCGenFrm.h"
#include "PCB CAM ProcessorFrm.h"

extern PCB_CAM_ProcessorFrm* MainFrame;
extern PCBHoleArray HolesToMake;

/*static int CompareDistance(DpointID *first, DpointID *second);
void MakeCommonPointsArray();

extern PCBNodeArray PCBNodes;
extern PCBHoleArray PCBHoles;
extern PCBPadArray  PCBPads;
extern PCBPathArray PCBPaths;

extern DpointIDArray DpointIDs; 
extern DpointIDSortedArray LineGroup( CompareDistance );

extern LineIDArray     LineIDs;*/


//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// CNCGenFrm
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(CNCGenFrm,wxFrame)
	////Manual Code Start
	EVT_BUTTON(100000,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100001,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100002,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100003,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100004,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100005,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100006,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100007,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100008,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100009,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100010,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100011,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100012,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100013,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100014,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100015,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100016,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100017,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100018,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100019,CNCGenFrm::TemplateButtonClick)
	EVT_BUTTON(100020,CNCGenFrm::TemplateButtonClick)
	////Manual Code End
	
	EVT_CLOSE(CNCGenFrm::OnClose)
	EVT_SIZE(CNCGenFrm::CNCGenFrmSize)
	EVT_MENU(ID_MNU_NOWY_1002, CNCGenFrm::TemplateNew)
	EVT_MENU(ID_MNU_OTW_RZ_1003, CNCGenFrm::TemplateOpen)
	EVT_MENU(ID_MNU_ZAPISZ_1004, CNCGenFrm::TemplateSave)
	EVT_MENU(ID_MNU_ZAPISZJAKO_1005, CNCGenFrm::TemplateSaveAs)
	EVT_MENU(ID_MNU_WYGENERUJKODCNC_1007, CNCGenFrm::GenCNC)
	EVT_MENU(ID_MNU_PODGL_D_1008, CNCGenFrm::ReView)
	EVT_TIMER(ID_WXTIMER1,CNCGenFrm::WxTimer1Timer)
END_EVENT_TABLE()
////Event Table End

CNCGenFrm::CNCGenFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreatingDone = false;
    CreateGUIControls();
    CreatingDone = true;
}

CNCGenFrm::~CNCGenFrm()
{
}

void CNCGenFrm::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	WxOpenFileDialog1 =  new wxFileDialog(this, wxT("Wybierz plik"), wxT(""), wxT(""), wxT("*.tmp*"), wxOPEN);

	WxTimer1 = new wxTimer();
	WxTimer1->SetOwner(this, ID_WXTIMER1);

	WxSaveFileDialog1 =  new wxFileDialog(this, wxT("Wybierz plik"), wxT(""), wxT(""), wxT("*.tmp*"), wxSAVE);

	WxMenuBar1 = new wxMenuBar();
	wxMenu *ID_MNU_SZABLON_1001_Mnu_Obj = new wxMenu(0);
	ID_MNU_SZABLON_1001_Mnu_Obj->Append(ID_MNU_NOWY_1002, wxT("Nowy"), wxT(""), wxITEM_NORMAL);
	ID_MNU_SZABLON_1001_Mnu_Obj->Append(ID_MNU_OTW_RZ_1003, wxT("Otw�rz"), wxT(""), wxITEM_NORMAL);
	ID_MNU_SZABLON_1001_Mnu_Obj->Append(ID_MNU_ZAPISZ_1004, wxT("Zapisz"), wxT(""), wxITEM_NORMAL);
	ID_MNU_SZABLON_1001_Mnu_Obj->Append(ID_MNU_ZAPISZJAKO_1005, wxT("Zapisz jako"), wxT(""), wxITEM_NORMAL);
	WxMenuBar1->Append(ID_MNU_SZABLON_1001_Mnu_Obj, wxT("Szablon"));
	
	wxMenu *ID_MNU_GENEROWANIE_1006_Mnu_Obj = new wxMenu(0);
	ID_MNU_GENEROWANIE_1006_Mnu_Obj->Append(ID_MNU_WYGENERUJKODCNC_1007, wxT("Wygeneruj kod CNC"), wxT(""), wxITEM_NORMAL);
	ID_MNU_GENEROWANIE_1006_Mnu_Obj->Append(ID_MNU_PODGL_D_1008, wxT("Podgl�d"), wxT(""), wxITEM_CHECK);
	ID_MNU_GENEROWANIE_1006_Mnu_Obj->Check(ID_MNU_PODGL_D_1008,false);
	WxMenuBar1->Append(ID_MNU_GENEROWANIE_1006_Mnu_Obj, wxT("Generowanie"));
	SetMenuBar(WxMenuBar1);

	WxNotebook1 = new wxNotebook(this, ID_WXNOTEBOOK1, wxPoint(78, 62), wxSize(623, 452), wxNB_DEFAULT);

	SetTitle(wxT("Generator CNC"));
	SetIcon(wxNullIcon);
	SetSize(8,8,958,785);
	Center();
	
	////GUI Items Creation End
	
	SetSize(0,0,440,600);
	SetMinSize( wxSize(440,580) );
	Center();
	
	Review = false;
	
	GenMenu = ID_MNU_GENEROWANIE_1006_Mnu_Obj;

	// tworzenie paneli szablon�w
	
	cncTemplateArray1 = new cncArrayTemplate();
	
	wxString Params, Codes, G2G3ToolTip;
	
	Params  = "Ext;Rozszerzenie pliku wyj�ciowego:;np. txt;";
	Params += "X;Pocz�tek uk�adu wsp�rz�dnych - o� X:;Przesuni�cie punktu zerowego - o� X;";
	Params += "Y;Pocz�tek uk�adu wsp�rz�dnych - o� Y:;Przesuni�cie punktu zerowego - o� Y;";
	//V2x
	Params += "StartPointX;Punkt startowy narz�dzia - o� X:;Pocz�tek programu obr�bki - o� X;";
	Params += "StartPointY;Punkt startowy narz�dzia - o� Y:;Pocz�tek programu obr�bki - o� Y;";

	Codes  = "Start;Pocz�tek programu:;Powinien zawiera� pocz�tkowe ustawienia, dane itp.\nDodawany jest tylko raz na pocz�tku pliku.;";
	Codes += "Drill;Podprogram wiercenia:;Powinien zawiera�:\n - najazd nad otw�r (@X@,@Y@)\n - wykonanie wiercenia\n - wyjazd - powr�t na p�aszczyzn� ruchu, czyli przygotowanie do przejazdu nad kolejny otw�r;";
	Codes += "Tool;Podprogram r�cznej wymiany wiert�a:;Otwory wykonywane s� w kolejno�ci rosn�cej. Wiert�a musz� mie� te same d�ugo�ci.;";
	Codes += "End;Zako�czenie programu:;Powinno zawiera� ko�cowe czynno�ci, przygotowanie do kolejnych operacji itp.\nDodawany jest tylko raz na ko�cu pliku.;";
	
	cncTemplateArray1->Add( new cncTemplate( WxNotebook1, "Wiercenie", Params, Codes ) );
	
	Params  = "Ext;Rozszerzenie pliku wyj�ciowego:;np. txt;";
    Params += "X;Pocz�tek uk�adu wsp�rz�dnych - o� X:;Przesuni�cie punktu zerowego - o� X;";
	Params += "Y;Pocz�tek uk�adu wsp�rz�dnych - o� Y:;Przesuni�cie punktu zerowego - o� Y;";
	
	//V2x
	Params += "StartPointX;Punkt startowy narz�dzia - o� X:;Pocz�tek programu obr�bki - o� X;";
	Params += "StartPointY;Punkt startowy narz�dzia - o� Y:;Pocz�tek programu obr�bki - o� Y;";
	
	Params += "R;Promie� narz�dzia grawerskiego:;Podanie prawid�owego promienia narz�dzia jest niezb�dne dla dok�adnego odwzorowania wymiar�w.;";
	
	Codes  = "Start;Pocz�tek programu:;Powinien zawiera� pocz�tkowe ustawienia, dane itp.\nDodawany jest tylko raz na pocz�tku pliku.;";
	Codes += "In;Dojazd do materia�u:;Powinien zawiera�:\n - najazd nad punkt startowy konturu (@X@,@Y@)\n - zag��bienie si� w materia�;";
	Codes += "Lin;Ruch roboczy - interpolacja liniowa:;Wykonanie przejazdu roboczego liniowego do punku (@X@,@Y@);";
	
	G2G3ToolTip  = "Wykonanie przejazdu roboczego po �uku - dost�pne parametry interpolacji ko�owej:\n";
	G2G3ToolTip += " - @X@, @Y@ - wsp. punktu ko�cowego\n";
	G2G3ToolTip += " - @AX@, @AY@ - wsp. punktu po�redniego\n";
	G2G3ToolTip += " - @I@, @J@ - wsp. �rodka �uku wzgl�dem punktu pocz�tkowego (G-code)\n";
	G2G3ToolTip += " - @R@ - promie� �uku\n";
	G2G3ToolTip += " - @CR@ - promie� �uku ( Sinumerik - ujemny dla rozwarcia > 180* )\n";
	G2G3ToolTip += " - @P@ - k�t rozwarcia �uku;";
	
	Codes += "G2;Ruch roboczy - interpolacja ko�owa zgodna z ruchem wsk. zegara:;" + G2G3ToolTip;
	Codes += "G3;Ruch roboczy - interpolacja ko�owa przeciwna do ruchu wsk. zegara:;" + G2G3ToolTip;
	Codes += "Out;Wyjazd z materia�u:;Wycofanie narz�dzia, czyli przygotowanie do przejazdu nad kolejny kont�r;";
	Codes += "End;Zako�czenie programu:;Powinno zawiera� ko�cowe czynno�ci, przygotowanie do kolejnych operacji itp.\nDodawany jest tylko raz na ko�cu pliku.;";
    
    cncTemplateArray1->Add( new cncTemplate( WxNotebook1, "Grawerowanie", Params, Codes ) );
}

void CNCGenFrm::OnClose(wxCloseEvent& event)
{
	Show(false);
}

void CNCGenFrm::Resize()
{
    if ( CreatingDone )
    {
        WxNotebook1->SetPosition( wxPoint(0,0) );
    	WxNotebook1->SetSize(  GetClientSize() );
    	
    	for ( int i = 0; i < cncTemplateArray1->GetCount(); i++ ) cncTemplateArray1->Item(i)->Resize();	
    }	
}

void CNCGenFrm::SetReView( bool State )
{
    GenMenu->FindItem( ID_MNU_PODGL_D_1008 )->Check(State);
    Review = State;
}

/*
 * TemplateNew
 */
void CNCGenFrm::TemplateNew(wxCommandEvent& event)
{
	for ( int i = 0; i < cncTemplateArray1->GetCount(); i++ ) cncTemplateArray1->Item(i)->Clear();
	MainFileName.Clear();
	MainFilePath.Clear();
	SetTitle( "Generator CNC" );
}

/*
 * TemplateOpen
 */
void CNCGenFrm::TemplateOpen(wxCommandEvent& event)
{
    MyConfigFile TemplateFile;
   
    if ( WxOpenFileDialog1->ShowModal() == wxID_OK )
    {
        MainFilePath = WxOpenFileDialog1->GetPath();
        MainFileName = WxOpenFileDialog1->GetFilename();
    
        SetTitle( "Generator CNC - [ " + MainFileName + " ] - " + MainFilePath );
        
        TemplateFile.MyOpen( MainFilePath );
	
    	for ( int i = 0; i < cncTemplateArray1->GetCount(); i++ )
            cncTemplateArray1->Item(i)->LoadDataFromString( TemplateFile.GetFile() );
        
        TemplateFile.Close();
    }
}

/*
 * TemplateSave
 */
void CNCGenFrm::TemplateSave(wxCommandEvent& event)
{
	MyConfigFile TemplateFile;
    bool MakeSave = false;
    
    if ( MainFileName.IsEmpty() )
    {
        if ( WxSaveFileDialog1->ShowModal() == wxID_OK )
        {
            MainFilePath = WxSaveFileDialog1->GetPath();
            MainFileName = WxSaveFileDialog1->GetFilename();
            if ( MainFileName.AfterLast('.') != "tmp" ) { MainFileName += ".tmp"; MainFilePath += ".tmp"; }
            SetTitle( "Generator CNC - [ " + MainFileName + " ] - " + MainFilePath );
            MakeSave = true;
        }
    }
    else MakeSave = true;

    if ( MakeSave )
    {
    	TemplateFile.MyOpen( MainFilePath );
    	TemplateFile.Clear();
    
    	for ( int i = 0; i < cncTemplateArray1->GetCount(); i++ )
            TemplateFile.AddLine( cncTemplateArray1->Item(i)->GetDataString() );
        
        TemplateFile.MyClose();	
    }
}

/*
 * TemplateSaveAs
 */
void CNCGenFrm::TemplateSaveAs(wxCommandEvent& event)
{
	MyConfigFile TemplateFile;
    
    if ( WxSaveFileDialog1->ShowModal() == wxID_OK )
    {
        MainFilePath = WxSaveFileDialog1->GetPath();
        MainFileName = WxSaveFileDialog1->GetFilename();
        if ( MainFileName.AfterLast('.') != "tmp" ) { MainFileName += ".tmp"; MainFilePath += ".tmp"; }
        SetTitle( "Generator CNC - [ " + MainFileName + " ] - " + MainFilePath );
        
        TemplateFile.MyOpen( MainFilePath );
    	TemplateFile.Clear();
    
    	for ( int i = 0; i < cncTemplateArray1->GetCount(); i++ )
            TemplateFile.AddLine( cncTemplateArray1->Item(i)->GetDataString() );
        
        TemplateFile.MyClose();	
    }
}

/*
 * GenCNC
 */
void CNCGenFrm::GenCNC(wxCommandEvent& event)
{
    //double CuterRadius;
    
    //cncTemplateArray1->Item(1)->Get( "R", CuterRadius );
    //MainFrame->CalcContour( CuterRadius );
    //MainFrame->PrepareDrillSim();
    //MainFrame->PrepareMillSim();
    
    MainFrame->MakeDrillFile();
	MainFrame->MakeMillFile();
	
	// V2x
    //MainFrame->V2xPrepareContourArray();
	
	
	
	//MainFrame->SetSimView(Review);
}

/*
 * ReView
 */
void CNCGenFrm::ReView(wxCommandEvent& event)
{
	double CuterRadius;
	
    if ( Review ) Review = false;
	else          
    {
        Review = true;
        cncTemplateArray1->Item(1)->Get( "R", CuterRadius );
        MainFrame->CalcContour( CuterRadius );
        MainFrame->PrepareDrillSim();
        MainFrame->PrepareMillSim();
        
        // V2x
        MainFrame->V2xPrepareContourArray();
    }
    
    MainFrame->SetSimView(Review);
}

/*
 * CNCGenFrmSize
 */
void CNCGenFrm::CNCGenFrmSize(wxSizeEvent& event)
{
	Resize();
}


void CNCGenFrm::TemplateButtonClick(wxCommandEvent& event)
{
    //wxMessageBox( wxString::Format( "Nacisnales guzik nr %d karty nr %d", event.GetId(), WxNotebook1->GetSelection() ) );
    
    cncTemplateArray1->Item( WxNotebook1->GetSelection() )->ButtonClick( event.GetId() );
}

/*
 * WxTimer1Timer
 */
void CNCGenFrm::WxTimer1Timer(wxTimerEvent& event)
{
	//MainFrame->MakeCommonPointsArray();
}
