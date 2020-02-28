///-----------------------------------------------------------------
///
/// @file      V2xSolTSP_2opt_3opt.cpp
/// @author    rado4ever
/// Created:   2019-04-23 12:34:45
/// @section   DESCRIPTION
///            Implementation of V2xSolTSP_2opt_3opt class
///------------------------------------------------------------------

#include "V2xSol.h"

/*******************************************************************************
                    Class V2xSolTSP_NN
*******************************************************************************/

V2xSolTSP_NN::V2xSolTSP_NN( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive )    
{ 
    Init( In, StartPoint, Name, DefaultActive ); 
    addParam( PM_NN_TYPE, 0.0 );
    addParam( PM_NN_XYPR, 1.0 );
    
    // Rozw poczatkowe
	iSol.Init( bestSol.GetCount() );
    iSol.SetPoints( &bestSol );
}

void V2xSolTSP_NN::Solve() 
{ 
	wxStopWatch sw;
	solveClones();
	
    int Type = (int)GetParam( PM_NN_TYPE ).GetValue();
	xyPR     = (int)GetParam( PM_NN_XYPR ).GetValue();
    
	int n = GetN(), BestI = -1, Zcount = 0;
    
    if ( Type == V2xSolTSP_NN::nnZERO )
    {
        // Kopia rozw pocz
        ciSol.Init(n);
        ciSol.SetPoints( &iSol );
        
        candSol.AddPoint( ciSol.MovePoint(0) );
        while ( ciSol.GetCount() > 0 ) addNNPoint(); 
        
        bestSol.SetPoints( &candSol );
        candSol.DeletePoints();
    }
    else
    {
        for ( int i = 0; i < n; i++ )
        {
            // Kopia rozw pocz
            ciSol.Init(n);
            ciSol.SetPoints( &iSol );
            
            candSol.AddPoint( ciSol.MovePoint(i) );
            while ( ciSol.GetCount() > 0 ) addNNPoint();   
            
            if ( candSol.GetLen() < bestSol.GetLen() ) 
            {
                bestSol.SetPoints( &candSol );
                BestI = i;
                Zcount++;
            }
    
            candSol.DeletePoints();
        }
    }
    
    result = bestSol.GetLen();
    rtime = sw.Time();
    
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d, I: %d, Z: %d) )", bestSol.GetLen(), sw.Time(), n, BestI, Zcount);
} 

void V2xSolTSP_NN::addNNPoint()
{
    Cpoint LastPoint = candSol.GetLastPoint();
    Cpoint NextPoint;
    double Dist = norm ( ciSol.GetPoint(0) - LastPoint );
    double NextDist = 0.0;
    int BestI = 0;
    
    for ( int i = 1; i < ciSol.GetCount(); i++ )
    {
        NextPoint = ciSol.GetPoint(i);
        NextDist  = norm ( NextPoint - LastPoint );
        
        if ( LT_DOUBLE( NextDist, Dist, MICRO ) || /* jesli jest naprawde blizej od innych*/
             xyPR == nnPRX && EQ_DOUBLE( NextDist, Dist, MICRO ) && EQ_DOUBLE( NextPoint.imag(), LastPoint.imag(), MICRO ) || /*lub jesli jast tak samo blisko i ma to samo na y'ku*/
             xyPR == nnPRY && EQ_DOUBLE( NextDist, Dist, MICRO ) && EQ_DOUBLE( NextPoint.real(), LastPoint.real(), MICRO ) )  /*lub jesli jast tak samo blisko i ma to samo na x'sie*/
        {
            Dist = NextDist;
            BestI = i;
        }
    }
    candSol.AddPoint( ciSol.MovePoint( BestI ) );
}

/*******************************************************************************
                    Class V2xSolTSP_FI
*******************************************************************************/

V2xSolTSP_FI::V2xSolTSP_FI( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive )    
{ 
    Init( In, StartPoint, Name, DefaultActive ); 
    addParam( PM_FI_TYPE, 1.0 ); 
    
    // Rozw poczatkowe
	iSol.Init( bestSol.GetCount() );
    iSol.SetPoints( &bestSol );
}

void V2xSolTSP_FI::Solve() 
{ 
	int Type = (int)GetParam( PM_FI_TYPE ).GetValue();
    
    wxStopWatch sw;
    solveClones();
    
	int n = GetN(), BestI = -1, Zcount = 0;
    
    if ( Type == V2xSolTSP_FI::fiONE )
    {
        // Kopia rozw pocz
        ciSol.Init(n);
        ciSol.SetPoints( &iSol );
        
        addFarthestPair();
        while ( ciSol.GetCount() > 0 ) addFarthestPoint(); 
        
        bestSol.SetPoints( &candSol );
        candSol.DeletePoints();
    }
    else
    {
        for ( int i = 0; i < ( Type == V2xSolTSP_FI::fiN ? n : 1 ); i++ )
        {
            // Kopia rozw pocz
            ciSol.Init(n);
            ciSol.SetPoints( &iSol );
            
            candSol.AddPoint( ciSol.MovePoint(i) );
            while ( ciSol.GetCount() > 0 ) addFarthestPoint();   
            
            if ( candSol.GetLen() < bestSol.GetLen() ) 
            {
                bestSol.SetPoints( &candSol );
                BestI = i;
                Zcount++;
            }
    
            candSol.DeletePoints();
        }
    }
    
    result = bestSol.GetLen();
    rtime = sw.Time();
    
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d, I: %d, Z: %d) )", bestSol.GetLen(), sw.Time(), n, BestI, Zcount);
} 

void V2xSolTSP_FI::addFarthestPoint()
{
    double D, MaxD = 0.0;
    int BestI;
    
    for ( int i = 0; i < ciSol.GetCount(); i++ )
    {
        D = candSol.MaxDist( ciSol.GetPoint(i) );
        if ( D > MaxD ) 
        {
            MaxD = D;
            BestI = i;
        }
    }
    candSol.BestInsert( ciSol.MovePoint( BestI ) );
}

void V2xSolTSP_FI::addFarthestPair()
{
    double D, MaxD = 0.0;
    int BestI, BestJ;
    
    for ( int i = 0; i < ciSol.GetCount() -1; i++ )
    {
        for ( int j = i+1; j < ciSol.GetCount(); j++ )
        {
            D = ciSol.Dist( i, j );
            if ( D > MaxD ) 
            {
                MaxD = D;
                BestI = i;
                BestJ = j;
            }
        }
    }
    candSol.AddPoint( ciSol.MovePoint(BestJ) );
    candSol.AddPoint( ciSol.MovePoint(BestI) );
}

/*******************************************************************************
                    Class V2xSolTSP_2opt
*******************************************************************************/

void V2xSolTSP_2opt::Solve() 
{ 
	wxStopWatch sw;
    solveClones();
    
    int i, j, n = GetN();
    bool DoBreak = false;
    int Scount = 0, Zcount = 0;
    int i_1;
    
    if ( preSolver != NULL ) 
    {
        preSolver->Solve();
        InheritSolution( preSolver );
    }
    
    while (1)
    {
        DoBreak = false;
        for ( i = 0; i < n - 2 && !DoBreak; i++ )
        {
            i_1 = ( i == 0 ) ? n-1 : i-1;
            for ( j = i + 2; j < n && !DoBreak; j++ )
            {
                Scount++;
                if ( NewTour( i, i_1, j ) ) { Zcount++; DoBreak = true; break; }
            } 
        }
        if ( !DoBreak ) 
        {
            result = bestSol.GetLen();
            rtime = sw.Time();
            
            comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d, S: %d, Z: %d) )", bestSol.GetLen(), sw.Time(), n, Scount, Zcount );
            return; 
        }
    }
} 

bool V2xSolTSP_2opt::NewTour( int i, int i_1, int j )
{   
    // 0--i---j--n
    // 0-i_1 j_1-i j-n
    
    int h, n = GetN(), c = 0;
    
    double CandLen = bestSol.GetLen();

    // Usuniete luki
    CandLen -= bestSol.Dist( i_1, i ); // jeœli i == 0 to punkt i-1 zapetlamy do n
    CandLen -= bestSol.Dist( j-1, j );
    // Luki dodane
    CandLen += bestSol.Dist( i_1, j-1 );
    CandLen += bestSol.Dist( i, j );
    
    // Ocena rozwiazania
    if ( CandLen < bestSol.GetLen() -0.01 )
    {
        // Kompozycja rozwiazania
        for ( h = 0;   h < i;   h++ ) candSol.SetPoint( c++, bestSol.GetPoint(h) );
        for ( h = j-1; h > i-1; h-- ) candSol.SetPoint( c++, bestSol.GetPoint(h) );   
        for ( h = j;   h < n;   h++ ) candSol.SetPoint( c++, bestSol.GetPoint(h) );
            
        // Zwrocenie wyniku
        bestSol.SetPoints( &candSol );
        return true;
    }
    return false;
}

/*******************************************************************************
                    Class V2xSolTSP_3opt
*******************************************************************************/

void V2xSolTSP_3opt::Solve() 
{ 
	wxStopWatch sw; 
	solveClones();
    
    int n = GetN();
    bool DoBreak = false;
    int Scount = 0, Zcount = 0;
    int i_1;
    
    if ( preSolver != NULL ) 
    {
        preSolver->Solve();
        InheritSolution( preSolver );
    }
    
    while (1)
    {
        DoBreak = false;
        for ( int i = 0; i < n - 2 && !DoBreak; i++ )
        {
            i_1 = ( i == 0 ) ? n-1 : i-1;
            for ( int j = i + 1; j < n - 1 && !DoBreak; j++ )
            {
                for ( int k = j + 1; k < n && !DoBreak; k++ )
                {
                    Scount++;
                    if ( NewTour3( i, i_1, j, k ) ) { Zcount++; DoBreak = true; break; }
                } 
            } 
        }
        if ( !DoBreak ) 
        {
            result = bestSol.GetLen();
            rtime = sw.Time();
            
            comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d, S: %d, Z: %d) )", bestSol.GetLen(), sw.Time(), n, Scount, Zcount );
            return; 
        }
    }
} 

bool V2xSolTSP_3opt::NewTour3( int i, int i_1, int j, int k )
{   
    // 0--i---j---k--n
    // 0-i j-k i-j k-n (non rev)
    // 0-i j-k j-i k-n (rev)
    
    int h, n = GetN(), c = 0;
    
    double CandLenS = bestSol.GetLen();
    double CandLenR;
    
    bool BetterSol = false;
    bool Rev = false;
    
    // Usuniete luki
    CandLenS -= bestSol.Dist( i_1, i ); // jeœli i == 0 to punkt i-1 zapetlamy do n
    CandLenS -= bestSol.Dist( j-1, j );
    CandLenS -= bestSol.Dist( k-1, k );
    // Dodane luki - wspolne
    CandLenS += bestSol.Dist( i_1, j );
    CandLenR = CandLenS;
    // Luki dodane do 1*
    CandLenS += bestSol.Dist( j-1, k );
    CandLenS += bestSol.Dist( k-1, i );
    // Luki dodane do 2* - reverse sol
    CandLenR += bestSol.Dist( k-1, j-1 );
    CandLenR += bestSol.Dist( i, k );
    
    // Ocena rozwiazania
    if ( CandLenS < bestSol.GetLen() -0.01 )
    {
        BetterSol = true;
        if ( CandLenR < CandLenS ) Rev = true;
    }
    else if ( CandLenR < bestSol.GetLen() -0.01 )
    {
        BetterSol = true;
        Rev = true;
    }  
    
    if ( !BetterSol ) return false;
    
    // Kompozycja rozwiazania
    for ( h = k; h < n; h++ ) candSol.SetPoint( c++, bestSol.GetPoint(h) );
    
    for ( h = 0; h < i; h++ ) candSol.SetPoint( c++, bestSol.GetPoint(h) );
    for ( h = j; h < k; h++ ) candSol.SetPoint( c++, bestSol.GetPoint(h) );
    
    if ( Rev ) for ( h = j-1; h > i-1; h-- ) candSol.SetPoint( c++, bestSol.GetPoint(h) );
    else       for ( h = i;   h < j;   h++ ) candSol.SetPoint( c++, bestSol.GetPoint(h) );
        
    // Zwrocenie wyniku
    bestSol.SetPoints( &candSol );
    return true;
}

/*******************************************************************************
                    Class V2xSolTSP_Ext
*******************************************************************************/

void V2xSolTSP_Ext::Solve()
{
    wxFileDialog *FileDialog1 = new wxFileDialog( NULL, wxT("Choose a file"), wxT(""), wxT(""), wxT("*.cyc*"), wxOPEN );
    
    genFile( "concorde.tsp" );
    ShellExecute( NULL, NULL, "concorde.tsp", NULL, NULL, NULL );
    wxMessageBox( "Przelicz w Concorde, zapisz 'Save Tour' i wska¿ plik wyjœciowy ");
    
    if ( FileDialog1->ShowModal() == wxID_OK )
    {
        loadResult( FileDialog1->GetPath() );
        bestSol.SetPoints( &candSol );
    }
    
    comment = wxString::Format( " - ( D³.: %1.1f mm (N: %d) )", bestSol.GetLen(), bestSol.GetCount() );
    delete FileDialog1;
}

void V2xSolTSP_Ext::genFile( wxString FileName )
{
    Cpoint Point;
    wxString sFile;
    MyConfigFile File;
    
    sFile += wxString::Format( "DIMENSION: %d\n", bestSol.GetCount() );
    sFile += "EDGE_WEIGHT_TYPE: EUC_2D\n";
    sFile += "NODE_COORD_SECTION\n";
    
    for ( int i = 0; i < bestSol.GetCount(); i++ )
    {
        Point = bestSol.GetPoint(i);
        sFile += wxString::Format( "%d %f %f\n", i+1, Point.real()*1000.0, Point.imag()*1000.0 );
    }
    
    sFile += "EOF\n";
    
    File.MyOpen( FileName );
    File.Clear();
    File.AddLine( sFile );
    File.MyClose();
}

void V2xSolTSP_Ext::loadResult( wxString FileName )
{
    int Index;
    MyConfigFile File;
    File.Open( FileName );
    
    for ( int i = 0; i < candSol.GetCount() /*File.GetLineCount()*/; i++ )
    {
        Index = wxAtoi( File.GetLine(i) );
        candSol.SetPoint( i, bestSol.GetPoint( Index ) );
    }
    
    File.Close();
}
