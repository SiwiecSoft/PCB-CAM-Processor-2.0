///-----------------------------------------------------------------
///
/// @file      CAM.cpp
/// @author    rado4ever
/// Created:   2016-05-01 21:43:24
/// @section   DESCRIPTION
///            PCB_CAM_ProcessorFrm class CAM modul extension
///
///------------------------------------------------------------------

#include "PCB CAM ProcessorFrm.h"

void PCB_CAM_ProcessorFrm::PrepareMillSim()
{ 
    Cpoint PrimBegin, PrimEnd, SimBegin( 0, 0 ), SimEnd;
    double Norm, Normb, Norme, MinNorm;
    int CurrentPrim = 0;
    bool ReverseRequired = false;
    
    WX_CLEAR_ARRAY( *PrimitivesToSim );
    
    // sotrowanie prymitywow w kolejnosci od punktu startowego kolejno w lancuch
    while ( PrimitivesToMake->GetCount() )
    {
        MinNorm = 1000000000000.0;
        CurrentPrim = -1;
        for ( int i = 0; i < PrimitivesToMake->GetCount(); i++ )
        {
            PrimBegin = PrimitivesToMake->Item(i)->GetBeginPoint();
            PrimEnd   = PrimitivesToMake->Item(i)->GetEndPoint();
            
            Normb = norm( PrimBegin - SimBegin ); // kwadraty odleglosci punktow koncowych od poczatku symilacji
            Norme = norm( PrimEnd   - SimBegin );
            Norm = ( Normb < Norme ) ? Normb : Norme;

            if ( LT_DOUBLE( Norm, MinNorm, MICRO ) || /* jesli jest naprawde blizej od innych*/
                 EQ_DOUBLE( Norm, MinNorm, MICRO ) &&  /*lub jesli jaest tak samo blisko*/
                 ( EQ_DOUBLE( PrimBegin.imag(), SimBegin.imag(), MICRO ) || EQ_DOUBLE( PrimEnd.imag(), SimBegin.imag(), MICRO ) ) ) /* i ma ta samo na y'ku dla ktoregos konca*/
            {
                MinNorm = Norm;
                CurrentPrim = i;
                
                if ( Normb < Norme ) 
                { 
                    SimEnd   = PrimEnd; 
                    ReverseRequired = false;
                }  
                else 
                { 
                    SimEnd   = PrimBegin; 
                    ReverseRequired = true;
                }
            }
        }
        
        if ( CurrentPrim == -1 ) return; // bardzo dziwna i niebezpieczna sytuacja

        PrimitivesToMake->Item( CurrentPrim )->Reverse( ReverseRequired );
        
        PrimitivesToSim->Add( PrimitivesToMake->Item( CurrentPrim ) );
        PrimitivesToMake->RemoveAt( CurrentPrim );
        
        SimBegin = SimEnd; // koniec staje sie poczatkiem dla kolejnej iteracji
    }
    
}

void PCB_CAM_ProcessorFrm::MakeMillFile()
{
    wxString PieceOfCode, Line, FileName, Name, ProgName, ProgExtension;
    double BeginX, BeginY, _R, _CR, _P;
    Cpoint BP,EP,MP,IJ;
    Primitive *Prim;
    cncTemplate *cncTemp = CNCDialog->cncTemplateArray1->Item(1);
    
    cncTemp->Get( "X", BeginX ); 
    cncTemp->Get( "Y", BeginY ); 
    
    ProgName = "Mill";
    cncTemp->Get( "Ext", ProgExtension ); 
    if ( !ProgExtension.IsEmpty() && ProgExtension[0] != '.' ) ProgExtension = "." + ProgExtension;
    
    Name = MainFileName + "_" + ProgName;
    Name.Replace( " ", "_" );
    Name.Replace( ".", "_" );
    FileName = Name + ProgExtension;
    
    MyConfigFile MillFile;
	wxString Enter = "  ";
	Enter[0] = 13; Enter[1] = 10;
	
	MillFile.MyOpen( FileName );
	MillFile.Clear();
	
	cncTemp->Get( "Start", PieceOfCode ); 
	PieceOfCode.Replace( "@NAME@", Name );
	PieceOfCode.Replace( "\n", Enter );
	MillFile.AddLine( PieceOfCode );
	
	int i = 0;
	while ( i < V2xPrimitivesToSim->GetCount() )
	{
	    Prim = V2xPrimitivesToSim->Item(i);
        BP   = Prim->GetBeginPoint();
       
        cncTemp->Get( "In", PieceOfCode ); 
        PieceOfCode.Replace( "@X@", wxString::Format( "%f", BP.real() + BeginX ) );
        PieceOfCode.Replace( "@Y@", wxString::Format( "%f", BP.imag() + BeginY ) );
        PieceOfCode.Replace( "\n", Enter );
        MillFile.AddLine( PieceOfCode );
        
        for ( ; i < V2xPrimitivesToSim->GetCount(); i++ )
        {
            Prim = V2xPrimitivesToSim->Item(i);
            BP   = Prim->GetBeginPoint();
            EP   = Prim->GetEndPoint();
            MP   = Prim->GetMiddlePoint();
            
            if ( Prim->GetType() == line ) cncTemp->Get( "Lin", PieceOfCode ); 
            else
            {
                IJ  = Prim->GetCenterPoint() - BP;
                _R  = Prim->GetR();
                _P  = Prim->AngleLen() * 180.0 / M_PI;
                _CR = _P < 180.0 ? _R : -_R;
                
                if ( Prim->IsReverse() ) cncTemp->Get( "G2", PieceOfCode ); 
                else cncTemp->Get( "G3", PieceOfCode ); 

                PieceOfCode.Replace( "@AX@", wxString::Format( "%f", MP.real() + BeginX ) );
                PieceOfCode.Replace( "@AY@", wxString::Format( "%f", MP.imag() + BeginY ) );
                PieceOfCode.Replace( "@I@", wxString::Format( "%f", IJ.real() ) );
                PieceOfCode.Replace( "@J@", wxString::Format( "%f", IJ.imag() ) );
                PieceOfCode.Replace( "@R@", wxString::Format( "%f", _R ) );
                PieceOfCode.Replace( "@CR@", wxString::Format( "%f", _CR ) );
                PieceOfCode.Replace( "@P@", wxString::Format( "%f", _P ) );
            }
            
            PieceOfCode.Replace( "@X@", wxString::Format( "%f",  EP.real() + BeginX ) );
            PieceOfCode.Replace( "@Y@", wxString::Format( "%f",  EP.imag() + BeginY ) );
            
            PieceOfCode.Replace( "\n", Enter );
            MillFile.AddLine( PieceOfCode );
            
            if ( i+1 < V2xPrimitivesToSim->GetCount() )
            {
                if ( !EQ_DOUBLE( V2xPrimitivesToSim->Item(i+1)->GetMinNorm(BP), 0.0, MICRO ) &&
                     !EQ_DOUBLE( V2xPrimitivesToSim->Item(i+1)->GetMinNorm(EP), 0.0, MICRO ) )     break;
            }
        }
        
        cncTemp->Get( "Out", PieceOfCode ); 
        PieceOfCode.Replace( "@X@", wxString::Format( "%f", EP.real() + BeginX ) );
        PieceOfCode.Replace( "@Y@", wxString::Format( "%f", EP.imag() + BeginY ) );
        PieceOfCode.Replace( "\n", Enter );
        MillFile.AddLine( PieceOfCode );
        
        i++;
    }
    
    cncTemp->Get( "End", PieceOfCode ); 
    PieceOfCode.Replace( "\n", Enter );
	MillFile.AddLine( PieceOfCode );
	
	MillFile.MyClose();
	
	ShellExecute(NULL, "open", "Notepad", FileName, NULL, SW_SHOWNORMAL );
}

void PCB_CAM_ProcessorFrm::PrepareDrillSim()
{
    Cpoint LastPoint = Cpoint( 0.0, 0.0 );
    double Dist2, NextDist2, Diam;
    double StartPointX, StartPointY;
    PCBHole2 *CurrentHole, *NextHole;
    bool ChangeDiam;
    int i;
    unsigned int HolesListsCount;
    
    ArrayPCBHole2Array HolesLists;
    PCBHole2Array *CurrentHoleList;
    
    GroupMeneger->GetObjects( PCBHoles );
    
    //V2x
    CurrentHoleList = new PCBHole2Array();
    Diam = PCBHoles->Item(0)->GetDimension();
    
    for ( int i = 0; i < PCBHoles->GetCount(); i++ )
    {
        if ( !EQ_DOUBLE( PCBHoles->Item(i)->GetDimension(), Diam, NANO ) ) // ChangeDiam
        {
            Diam = PCBHoles->Item(i)->GetDimension();
            
            HolesLists.Add( CurrentHoleList );
            CurrentHoleList = new PCBHole2Array();
            CurrentHoleList->Add( PCBHoles->Item(i) );
        }
        else
        {
            CurrentHoleList->Add( PCBHoles->Item(i) );
        }  
    }
    
    if ( CurrentHoleList->GetCount() > 0 ) HolesLists.Add( CurrentHoleList );
    
    HolesListsCount = HolesLists.GetCount();
    
    V2xSolTSP *V2xSolTSP_Pure_RND_T[HolesListsCount];
    V2xSolTSP *V2xSolTSP_Pure_NN_T [HolesListsCount];
    V2xSolTSP *V2xSolTSP_Pure_FI_T [HolesListsCount];
    
    V2xSolTSP *V2xSolTSP_RND_2opt_T[HolesListsCount];
    V2xSolTSP *V2xSolTSP_NN_2opt_T [HolesListsCount];
    V2xSolTSP *V2xSolTSP_FI_2opt_T [HolesListsCount];
    
    V2xSolTSP *V2xSolTSP_RND_3opt_T[HolesListsCount];
    V2xSolTSP *V2xSolTSP_NN_3opt_T [HolesListsCount];
    V2xSolTSP *V2xSolTSP_FI_3opt_T [HolesListsCount];
    
    V2xSolTSP *V2xSolTSP_RND_GA_T[HolesListsCount];
    V2xSolTSP *V2xSolTSP_NN_GA_T [HolesListsCount];
    V2xSolTSP *V2xSolTSP_FI_GA_T [HolesListsCount];
    
    V2xSolTSP *V2xSolTSP_RND_GA2_T[HolesListsCount];
    V2xSolTSP *V2xSolTSP_NN_GA2_T [HolesListsCount];
    V2xSolTSP *V2xSolTSP_FI_GA2_T [HolesListsCount];
    ////////////////
    
    // Create Solutions
    WX_CLEAR_ARRAY( *SolTSPArray );
    
    CNCDialog->cncTemplateArray1->Item(0)->Get( "StartPointX", StartPointX ); 
    CNCDialog->cncTemplateArray1->Item(0)->Get( "StartPointY", StartPointY );
    
    Cpoint StartPoint( StartPointX, StartPointY );
    
    // Rozwi¹zania porównawcze
    SolTSPArray->Add( new V2xSolTSP_INIT(      HolesLists.Item(0), StartPoint, "Rozwi¹zanie programu v1.0", false ) );
    SolTSPArray->Add( new V2xSolTSP_LittleBAB( HolesLists.Item(0), StartPoint, "Little TSP", false ) );
    
    for ( int i = 0; i < HolesListsCount; i++ )
    {
        //Rozwi¹zania poczatkowe
        V2xSolTSP_Pure_RND_T[i] = new V2xSolTSP_RND( HolesLists.Item(i), StartPoint, "Rozwi¹zanie losowe", false );
        V2xSolTSP_Pure_NN_T[i]  = new V2xSolTSP_NN(  HolesLists.Item(i), StartPoint, "NN TSP", false );
        V2xSolTSP_Pure_FI_T[i]  = new V2xSolTSP_FI(  HolesLists.Item(i), StartPoint, "FI TSP", false );
        
        //Rozwi¹zania 2-Opt
        V2xSolTSP_RND_2opt_T[i] = new V2xSolTSP_2opt( HolesLists.Item(i), StartPoint, "2-Opt (RND)", false );
        V2xSolTSP_RND_2opt_T[i]->SetPreSolver( V2xSolTSP_Pure_RND_T[i] );
        
        V2xSolTSP_NN_2opt_T[i] =  new V2xSolTSP_2opt( HolesLists.Item(i), StartPoint, "2-Opt (NN)", false );
        V2xSolTSP_NN_2opt_T[i]->SetPreSolver( V2xSolTSP_Pure_NN_T[i] );
        
        V2xSolTSP_FI_2opt_T[i] =  new V2xSolTSP_2opt( HolesLists.Item(i), StartPoint, "2-Opt (FI)", false );
        V2xSolTSP_FI_2opt_T[i]->SetPreSolver( V2xSolTSP_Pure_FI_T[i] );
        
        //Rozwi¹zania 3-Opt
        V2xSolTSP_RND_3opt_T[i] = new V2xSolTSP_3opt( HolesLists.Item(i), StartPoint, "3-Opt (RND)", false );
        V2xSolTSP_RND_3opt_T[i]->SetPreSolver( V2xSolTSP_Pure_RND_T[i] );
        
        V2xSolTSP_NN_3opt_T[i] =  new V2xSolTSP_3opt( HolesLists.Item(i), StartPoint, "3-Opt (NN)", false );
        V2xSolTSP_NN_3opt_T[i]->SetPreSolver( V2xSolTSP_Pure_NN_T[i] );
        
        V2xSolTSP_FI_3opt_T[i] =  new V2xSolTSP_3opt( HolesLists.Item(i), StartPoint, "3-Opt (FI)", false );
        V2xSolTSP_FI_3opt_T[i]->SetPreSolver( V2xSolTSP_Pure_FI_T[i] );
        
        //R¹zwi¹zania GA
        V2xSolTSP_RND_GA_T[i] = new V2xSolTSP_GA( HolesLists.Item(i), StartPoint, "GA (RND)", false );
        V2xSolTSP_RND_GA_T[i]->SetPreSolver( V2xSolTSP_Pure_RND_T[i] );
        
        V2xSolTSP_NN_GA_T[i] =  new V2xSolTSP_GA( HolesLists.Item(i), StartPoint, "GA (NN)", false );
        V2xSolTSP_NN_GA_T[i]->SetPreSolver( V2xSolTSP_Pure_NN_T[i] );
        
        V2xSolTSP_FI_GA_T[i] =  new V2xSolTSP_GA( HolesLists.Item(i), StartPoint, "GA (FI)", false );
        V2xSolTSP_FI_GA_T[i]->SetPreSolver( V2xSolTSP_Pure_FI_T[i] );
        
        //R¹zwi¹zania GA + 2-OPT
        V2xSolTSP_RND_GA2_T[i] = new V2xSolTSP_GA( HolesLists.Item(i), StartPoint, "GA + 2-Opt (RND)", false, true );
        V2xSolTSP_RND_GA2_T[i]->SetPreSolver( V2xSolTSP_Pure_RND_T[i] );
        
        V2xSolTSP_NN_GA2_T[i] =  new V2xSolTSP_GA( HolesLists.Item(i), StartPoint, "GA + 2-Opt (NN)", false, true );
        V2xSolTSP_NN_GA2_T[i]->SetPreSolver( V2xSolTSP_Pure_NN_T[i] );
        
        V2xSolTSP_FI_GA2_T[i] =  new V2xSolTSP_GA( HolesLists.Item(i), StartPoint, "GA + 2-Opt (FI)", false, true );
        V2xSolTSP_FI_GA2_T[i]->SetPreSolver( V2xSolTSP_Pure_FI_T[i] );
        
        if ( i > 0 )
        {           
            V2xSolTSP_Pure_RND_T[0]->AddClone( V2xSolTSP_Pure_RND_T[i] );
            V2xSolTSP_Pure_NN_T [0]->AddClone( V2xSolTSP_Pure_NN_T[i] );
            V2xSolTSP_Pure_FI_T [0]->AddClone( V2xSolTSP_Pure_FI_T[i] );
            
            V2xSolTSP_RND_2opt_T[0]->AddClone( V2xSolTSP_RND_2opt_T[i] );
            V2xSolTSP_NN_2opt_T [0]->AddClone( V2xSolTSP_NN_2opt_T[i] );
            V2xSolTSP_FI_2opt_T [0]->AddClone( V2xSolTSP_FI_2opt_T[i] );
            
            V2xSolTSP_RND_3opt_T[0]->AddClone( V2xSolTSP_RND_3opt_T[i] );
            V2xSolTSP_NN_3opt_T [0]->AddClone( V2xSolTSP_NN_3opt_T[i] );
            V2xSolTSP_FI_3opt_T [0]->AddClone( V2xSolTSP_FI_3opt_T[i] );
            
            V2xSolTSP_RND_GA_T[0]->AddClone( V2xSolTSP_RND_GA_T[i] );
            V2xSolTSP_NN_GA_T [0]->AddClone( V2xSolTSP_NN_GA_T[i] );
            V2xSolTSP_FI_GA_T [0]->AddClone( V2xSolTSP_FI_GA_T[i] );
            
            V2xSolTSP_RND_GA2_T[0]->AddClone( V2xSolTSP_RND_GA2_T[i] );
            V2xSolTSP_NN_GA2_T [0]->AddClone( V2xSolTSP_NN_GA2_T[i] );
            V2xSolTSP_FI_GA2_T [0]->AddClone( V2xSolTSP_FI_GA2_T[i] );
        }
    }
    
    //Dodanie do listy
    SolTSPArray->Add( V2xSolTSP_Pure_RND_T[0] );
    SolTSPArray->Add( V2xSolTSP_Pure_NN_T[0] );
    SolTSPArray->Add( V2xSolTSP_Pure_FI_T[0] );
    
    SolTSPArray->Add( V2xSolTSP_RND_2opt_T[0] );
    SolTSPArray->Add( V2xSolTSP_NN_2opt_T[0] );
    SolTSPArray->Add( V2xSolTSP_FI_2opt_T[0] );
    
    SolTSPArray->Add( V2xSolTSP_RND_3opt_T[0] );
    SolTSPArray->Add( V2xSolTSP_NN_3opt_T[0] );
    SolTSPArray->Add( V2xSolTSP_FI_3opt_T[0] );
    
    SolTSPArray->Add( V2xSolTSP_RND_GA_T[0] );
    SolTSPArray->Add( V2xSolTSP_NN_GA_T[0] );
    SolTSPArray->Add( V2xSolTSP_FI_GA_T[0] );
    
    SolTSPArray->Add( V2xSolTSP_RND_GA2_T[0] );
    SolTSPArray->Add( V2xSolTSP_NN_GA2_T[0] );
    SolTSPArray->Add( V2xSolTSP_FI_GA2_T[0] );
    
    //Zewnetrzny solver
    SolTSPArray->Add( new V2xSolTSP_Ext( HolesLists.Item(0), StartPoint, "Concorde", false ) ); 
    
    //Przygotowanie wstepnej wizualizacji
    V2xPrepareDrillSim( V2xSolTSP_Pure_RND_T[0] );
}

void PCB_CAM_ProcessorFrm::V2xPrepareDrillSim( V2xSolTSP *Solution )
{
    CpointArray PointsToMake;
    GroupMeneger->GetObjects( PCBHoles );
    HolesToSim->Clear();
    
    Solution->GetPoints( &PointsToMake );
    
    for ( int i = 0; i < PointsToMake.GetCount(); i++ )
    {
        for ( int j = 0; j < PCBHoles->GetCount(); j++ )
        {
            if ( abs( PCBHoles->Item(j)->GetPosition() - *PointsToMake.Item(i) ) < NANO )
            {
                HolesToSim->Add( PCBHoles->Item(j) );
                break;
            }
        }
    }
    
    WX_CLEAR_ARRAY( PointsToMake );
}

void PCB_CAM_ProcessorFrm::MakeDrillFile()
{
    wxString PieceOfCode, Line, FileName, Name, ProgName, ProgExtension;
    double BeginX, BeginY;
    
    CNCDialog->cncTemplateArray1->Item(0)->Get( "X", BeginX ); 
    CNCDialog->cncTemplateArray1->Item(0)->Get( "Y", BeginY ); 
    
    ProgName = "Drill";
    CNCDialog->cncTemplateArray1->Item(0)->Get( "Ext", ProgExtension ); 
    if ( !ProgExtension.IsEmpty() && ProgExtension[0] != '.' ) ProgExtension = "." + ProgExtension;
    
    Name = MainFileName + "_" + ProgName;
    Name.Replace( " ", "_" );
    Name.Replace( ".", "_" );
    FileName = Name + ProgExtension;
    
    MyConfigFile DrillFile;
	wxString Enter = "  ";
	Enter[0] = 13; Enter[1] = 10;
	
	DrillFile.MyOpen( FileName );
	DrillFile.Clear();
	
	CNCDialog->cncTemplateArray1->Item(0)->Get( "Start", PieceOfCode ); 
	PieceOfCode.Replace( "@NAME@", Name );
	PieceOfCode.Replace( "\n", Enter );
	DrillFile.AddLine( PieceOfCode );
	
	CNCDialog->cncTemplateArray1->Item(0)->Get( "Drill", PieceOfCode ); 
	PieceOfCode.Replace( "\n", Enter );
    for ( int i = 0; i < HolesToSim->GetCount(); i++ ) // wiercenie
    {
        Line = PieceOfCode;
        Line.Replace( "@X@", wxString::Format( "%f", HolesToSim->Item(i)->GetPosition().real() + BeginX ) );
        Line.Replace( "@Y@", wxString::Format( "%f", HolesToSim->Item(i)->GetPosition().imag() + BeginY ) );
        
        DrillFile.AddLine( Line );
    }
    
    CNCDialog->cncTemplateArray1->Item(0)->Get( "End", PieceOfCode ); 
    PieceOfCode.Replace( "\n", Enter );
	DrillFile.AddLine( PieceOfCode );
	
	DrillFile.MyClose();
	
	ShellExecute(NULL, "open", "Notepad", FileName, NULL, SW_SHOWNORMAL );
}
