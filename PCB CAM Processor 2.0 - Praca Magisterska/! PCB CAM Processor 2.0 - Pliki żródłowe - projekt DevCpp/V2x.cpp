///-----------------------------------------------------------------
///
/// @file      V2x.cpp
/// @author    rado4ever
/// Created:   2019-02-25 17:21:43
/// @section   DESCRIPTION
///            PCB_CAM_ProcessorFrm class V2x modul extension
///
///------------------------------------------------------------------

#include "PCB CAM ProcessorFrm.h"

void PCB_CAM_ProcessorFrm::V2xPrepareContourArray()
{
    WX_CLEAR_ARRAY( *ContoursToMake );
    
    Cpoint BP, EP;
    Primitive  *Prim;
    V2xContour *Contour;
    double StartPointX, StartPointY;
 
    int Counter = 0;
	
	int i = 0;
	while ( i < PrimitivesToSim->GetCount() )
	{
        Contour = new V2xContour();
        ContoursToMake->Add( Contour ); 
        
        Counter++;
        
        for ( ; i < PrimitivesToSim->GetCount(); i++ )
        {
            Prim = PrimitivesToSim->Item(i);
            BP   = Prim->GetBeginPoint();
            EP   = Prim->GetEndPoint();
            
            Contour->AddPrim( Prim );
            
            if ( i+1 < PrimitivesToSim->GetCount() )
            {
                if ( !EQ_DOUBLE( PrimitivesToSim->Item(i+1)->GetMinNorm(BP), 0.0, MICRO ) &&
                     !EQ_DOUBLE( PrimitivesToSim->Item(i+1)->GetMinNorm(EP), 0.0, MICRO ) )     break;
            }
        }
        i++;
    }
    
    for ( int i = 0; i < ContoursToMake->GetCount(); i++ ) ContoursToMake->Item(i)->InitQuickCords();
    
    // Dodanie konturu "0"
    CNCDialog->cncTemplateArray1->Item(1)->Get( "StartPointX", StartPointX ); 
    CNCDialog->cncTemplateArray1->Item(1)->Get( "StartPointY", StartPointY ); 
    
    Cpoint StartPoint( StartPointX, StartPointY );
    V2xContour *ZeroContour = new V2xContour();
    ZeroContour->AddPrim( new Primitive( StartPoint, StartPoint ) );
    ContoursToMake->Add( ZeroContour ); 

    WX_CLEAR_ARRAY( *SolTSPNArray );
    SolTSPNArray->Add( new V2xSolTSPN_INIT(    ContoursToMake, StartPoint, "Rozwi¹zanie programu v1.0", false ) );
    SolTSPNArray->Add( new V2xSolTSPN_oRBA(    ContoursToMake, StartPoint, "Czyste RBA - test 1 iteracja", false ) );
    SolTSPNArray->Add( new V2xSolTSPN_nRBA(    ContoursToMake, StartPoint, "Czyste RBA", false ) );
    SolTSPNArray->Add( new V2xSolTSPN_RND(     ContoursToMake, StartPoint, "Poszukiwanie losowe", false ) );
    SolTSPNArray->Add( new V2xSolTSPN_RND_RBA( ContoursToMake, StartPoint, "Poszukiwanie losowe + RBA", false ) );
    SolTSPNArray->Add( new V2xSolTSPN_2opt(    ContoursToMake, StartPoint, "2-opt + RBA (RND)", false, true ) );
    SolTSPNArray->Add( new V2xSolTSPN_2opt(    ContoursToMake, StartPoint, "2-opt + RBA (NN)", false ) );
    SolTSPNArray->Add( new V2xSolTSPN_3opt(    ContoursToMake, StartPoint, "3-opt + RBA (RND)", false, true ) );
    SolTSPNArray->Add( new V2xSolTSPN_3opt(    ContoursToMake, StartPoint, "3-opt + RBA (NN)", false ) );
    
    SolTSPNArray->Add( new V2xSolTSPN_GA( ContoursToMake, StartPoint, "GA + RBA (RND)", false, false, true ) );
    SolTSPNArray->Add( new V2xSolTSPN_GA( ContoursToMake, StartPoint, "GA + RBA (NN)", false, false ) );
    SolTSPNArray->Add( new V2xSolTSPN_GA( ContoursToMake, StartPoint, "GA + 2-opt + RBA (RND)", false, true, true ) );
    SolTSPNArray->Add( new V2xSolTSPN_GA( ContoursToMake, StartPoint, "GA + 2-opt + RBA (NN)", false, true ) );
    
    // Launch Solution Manager Window
    OptDialog->RefreshWindow( MainFilePath );
    OptDialog->Show();
    OptDialog->Solve();
}
