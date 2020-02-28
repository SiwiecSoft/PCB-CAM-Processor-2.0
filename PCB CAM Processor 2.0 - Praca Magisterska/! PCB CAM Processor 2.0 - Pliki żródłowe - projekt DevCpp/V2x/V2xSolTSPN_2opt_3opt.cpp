///-----------------------------------------------------------------
///
/// @file      V2xSolTSPN_2opt_3opt.cpp
/// @author    rado4ever
/// Created:   2019-05-08 18:06:38
/// @section   DESCRIPTION
///            
///
///------------------------------------------------------------------

#include "V2xSolTSPN.h"

/*******************************************************************************
                    Class V2xSolTSPN_2opt
*******************************************************************************/

V2xSolTSPN_2opt::V2xSolTSPN_2opt( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive, bool RndInit )   
{
    Init( Source, StartPoint, Name, DefaultActive ); 
    rndInit = RndInit;
    addParam( PM_OPT_RBA_TYPE, 1.0 );
    addParam( PM_OPT_IMPROVEX, 0.1 );
    
    if ( rndInit ) randSweep( bestSol, 1000*n, true );

    bestSol->CalcFit();
    srcSol = new gaIndividual2(bestSol);
}

V2xSolTSPN_2opt::~V2xSolTSPN_2opt() 
{ 
    destroy(); 
    delete srcSol;
};

void V2xSolTSPN_2opt::loadParam2()
{
    loadParam();
    rbaType = (int)GetParam( PM_OPT_RBA_TYPE ).GetValue();
    impX    =      GetParam( PM_OPT_IMPROVEX ).GetValue();
}

void V2xSolTSPN_2opt::Solve()
{ 
    wxStopWatch sw;
    loadParam2(); 
    
    if ( reInit == 1 )
    {    
        initInd( bestSol ); // ponowna inicjalizacja bestsol
        if ( rndInit ) randSweep( bestSol, 1000*n, true );
        bestSol->CalcFit();
        srcSol ->SetFrom( bestSol );
    }
    
    bool DoBreak = false;
    int Scount = 0, Zcount = 0;
    
    while (1)
    {
        DoBreak = false;
        for ( int i = 0; i < n - 2 && !DoBreak; i++ )
        {
            for ( int j = i + 2; j < n && !DoBreak; j++ )
            {
                Scount++;
                if ( newTour( i, j ) ) { Zcount++; DoBreak = true; break; }
            } 
        }
        if ( !DoBreak ) 
        {
            result = bestSol->CalcFit();
            rtime = sw.Time();
            
            comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d, S: %d, Z: %d) )", bestSol->Fit, sw.Time(), n, Scount, Zcount );
            return; 
        }
    }
}

bool V2xSolTSPN_2opt::newTour( int i, int j )
{   
    // 0--i---j--n
    // 0-i_1 j_1-i j-n
    
    int h, c = 0;
    candSol->Fit = 0.0;

    // Kompozycja rozwiazania
    for ( h = 0;   h < i;   h++ ) { if ( !setAndRBA( h, c++ ) ) return false; }
    for ( h = j-1; h > i-1; h-- ) { if ( !setAndRBA( h, c++ ) ) return false; }
    for ( h = j;   h < n;   h++ ) { if ( !setAndRBA( h, c++ ) ) return false; }
    
    nRBA( candSol, epsRBA );
    if ( rbaType == 1 )srcSol->SetCdnaFrom( candSol );
    
    if ( candSol->Fit < bestSol->Fit - impX )
    {
        bestSol->SetFrom( candSol );
        if ( rbaType == 1 )srcSol ->SetFrom( candSol );
        return true;
    }
    else return false;
}

bool V2xSolTSPN_2opt::setAndRBA( int From, int To )
{
    static int From_1 = 0;
    
    if      ( rbaType == 0 ) candSol->SetFrom( bestSol, From, To ); // stale   punkty poczatkowe
    else if ( rbaType == 1 ) candSol->SetFrom( srcSol, From, To );  // ruchome punkty poczatkowe
    else                                                            // losowe  punkty poczatkowe
    {
        candSol->DNA[To]  = bestSol->DNA[From];
        candSol->cDNA[To] = contours.Item( candSol->DNA[To] )->GetRandomStartPoint();
    }
    
    if ( To > 1 ) 
    {
        candSol->cDNA[To-1]   = contours.Item( candSol->DNA[To-1] )->GetRBA( candSol->cDNA[To-2], candSol->cDNA[To] );
        srcSol ->cDNA[From_1] = candSol->cDNA[To-1];
    }
    if ( To > 2 ) 
    {
        candSol->Fit += abs( candSol->cDNA[To-2] - candSol->cDNA[To-1] );
    } 
    
    From_1 = From;
    
    return ( candSol->Fit < bestSol->Fit );
}

/*******************************************************************************
                    Class V2xSolTSPN_3opt
*******************************************************************************/

V2xSolTSPN_3opt::V2xSolTSPN_3opt( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive, bool RndInit )   
{
    Init( Source, StartPoint, Name, DefaultActive ); 
    rndInit = RndInit;
    addParam( PM_OPT_RBA_TYPE, 1.0 );
    addParam( PM_OPT_IMPROVEX, 0.1 );
    
    if ( rndInit ) randSweep( bestSol, 1000*n, true );
    
    bestSol->CalcFit();
    srcSol = new gaIndividual2(bestSol);
}

V2xSolTSPN_3opt::~V2xSolTSPN_3opt() 
{ 
    destroy(); 
    delete srcSol;
};

void V2xSolTSPN_3opt::loadParam2()
{
    loadParam();
    rbaType = (int)GetParam( PM_OPT_RBA_TYPE ).GetValue();
    impX    =      GetParam( PM_OPT_IMPROVEX ).GetValue();
}

void V2xSolTSPN_3opt::Solve()
{ 
    wxStopWatch sw;
    loadParam2(); 
    
    if ( reInit == 1 )
    {    
        initInd( bestSol ); // ponowna inicjalizacja bestsol
        if ( rndInit ) randSweep( bestSol, 1000*n, true );
        bestSol->CalcFit();
        srcSol ->SetFrom( bestSol );
    }
    
    bool DoBreak = false;
    int Scount = 0, Zcount = 0;
    
    while (1)
    {
        DoBreak = false;
        for ( int i = 0; i < n - 2 && !DoBreak; i++ )
        {
            for ( int j = i + 1; j < n - 1 && !DoBreak; j++ )
            {
                for ( int k = j + 1; k < n && !DoBreak; k++ )
                {
                    Scount++;
                    if ( newTour( i, j, k, false ) ) { Zcount++; DoBreak = true; break; }
                    
                    Scount++;
                    if ( newTour( i, j, k, true  ) ) { Zcount++; DoBreak = true; break; }
                } 
            } 
        }
        if ( !DoBreak ) 
        {
            result = bestSol->CalcFit();
            rtime = sw.Time();
            
            comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d, S: %d, Z: %d) )", bestSol->Fit, sw.Time(), n, Scount, Zcount );
            return; 
        }
    }
}

bool V2xSolTSPN_3opt::newTour( int i, int j, int k, bool Rev )
{   
    // 0--i---j---k--n
    // 0-i j-k i-j k-n (non rev)
    // 0-i j-k j-i k-n (rev)
    
    int h, c = 0;
    candSol->Fit = 0.0;
    
    // Kompozycja rozwiazania
    for ( h = k; h < n; h++ ) { if ( !setAndRBA( h, c++ ) ) return false; }
    
    for ( h = 0; h < i; h++ ) { if ( !setAndRBA( h, c++ ) ) return false; }
    for ( h = j; h < k; h++ ) { if ( !setAndRBA( h, c++ ) ) return false; }
    
    if ( Rev ) for ( h = j-1; h > i-1; h-- ) { if ( !setAndRBA( h, c++ ) ) return false; }
    else       for ( h = i;   h < j;   h++ ) { if ( !setAndRBA( h, c++ ) ) return false; }                                      //setAndRBA( h, c++ );
    
    nRBA( candSol, epsRBA );
    if ( rbaType == 1 ) srcSol->SetCdnaFrom( candSol );
    
    if ( candSol->Fit < bestSol->Fit - impX ) // zrobic jako parametr
    {
        bestSol->SetFrom( candSol );
        if ( rbaType == 1 )srcSol ->SetFrom( candSol );
        return true;
    }
    else return false;
}

bool V2xSolTSPN_3opt::setAndRBA( int From, int To )
{
    static int From_1 = 0;
    
    if      ( rbaType == 0 ) candSol->SetFrom( bestSol, From, To ); // stale   punkty poczatkowe - 154,2 mm 2,7s 
    else if ( rbaType == 1 ) candSol->SetFrom( srcSol, From, To );  // ruchome punkty poczatkowe - 154.0 mm 2.4s
    else                                                            // losowe  punkty poczatkowe
    {
        candSol->DNA[To]  = bestSol->DNA[From];
        candSol->cDNA[To] = contours.Item( candSol->DNA[To] )->GetRandomStartPoint();
    }
    
    if ( To > 1 ) 
    {
        candSol->cDNA[To-1]   = contours.Item( candSol->DNA[To-1] )->GetRBA( candSol->cDNA[To-2], candSol->cDNA[To] );
        srcSol ->cDNA[From_1] = candSol->cDNA[To-1];
    }
    if ( To > 2 ) 
    {
        candSol->Fit += abs( candSol->cDNA[To-2] - candSol->cDNA[To-1] );
    } 
    
    From_1 = From;
    
    return ( candSol->Fit < bestSol->Fit );
}
