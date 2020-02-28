///-----------------------------------------------------------------
///
/// @file      V2xSolTSPN.cpp
/// @author    rado4ever
/// Created:   2019-05-08 18:06:38
/// @section   DESCRIPTION
///            
///
///------------------------------------------------------------------

#include "V2xSolTSPN.h"

/*******************************************************************************
                    Class V2xSolTSPN
*******************************************************************************/

void V2xSolTSPN::Init( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive )
{
    //len           = 0.0;
    name          = Name;
    comment       = "";
    defaultActive = DefaultActive;
    startPoint    = StartPoint;
    
    n = Source->GetCount(); // tu jest jeszcze kontur zerowy
    
    copyContours( Source, &contours );
    
    addParam( PM_RBA_EPS, 0.1 );
    addParam( PM_REINIT,  1.0 );
    
    bestSol = new gaIndividual2(n);
    initInd( bestSol );// bestSol->SimpleInit();
    bestSol->CalcFit();
    
    candSol = new gaIndividual2(bestSol);
    wtfSol  = new gaIndividual2(bestSol);
}

void V2xSolTSPN::destroy() 
{ 
    WX_CLEAR_ARRAY(params); 
    delete bestSol; 
    delete candSol; 
}

void V2xSolTSPN::Draw( wxString Col )
{
    for ( int i = 0; i < n; i++ )
    {
        contours.Item(i)->Draw( Col ); // wyrzucis start point z konturu i tu narysowac punkt w cDNA 
        DrawCircle( bestSol->cDNA[i], 0.1, rxColour("BLUE") );
        //Draw arrows
        if ( i > 0 ) DrawArrow( bestSol->cDNA[i-1],  bestSol->cDNA[i], rxColour("BLUE") ); // strza³ki skrócic 
        else         DrawArrow( bestSol->cDNA[n-1],  bestSol->cDNA[0], rxColour("RED") );
    }
}

void V2xSolTSPN::GetPrimitives( PrimitiveArray *OutPrimitives )
{
    unsigned int BestI = 0, NextI;
    WX_CLEAR_ARRAY(*OutPrimitives);    
        
    for ( BestI = 0; BestI < n; BestI++ )
    {
        if ( abs( startPoint - bestSol->cDNA[BestI] ) < NANO ) break;
    }
    
    NextI = BestI + 1;
    if ( NextI >= n ) NextI = 0;
    
    while ( NextI != BestI )
    {
        contours.Item( bestSol->DNA[NextI] )->GetPrimitives( bestSol->cDNA[NextI], OutPrimitives );
        if ( ++NextI >= n ) NextI = 0;
    }
}

void V2xSolTSPN::copyContours( V2xContourArray *In, V2xContourArray *Out )
{
    WX_CLEAR_ARRAY( *Out );
    for ( int i = 0; i < In->GetCount(); i++ ) Out->Add( new V2xContour( In->Item(i) ) );
}

void V2xSolTSPN::loadParam()
{
    epsRBA = GetParam( PM_RBA_EPS ).GetValue();
    reInit = (int)GetParam( PM_REINIT ).GetValue();
}

void V2xSolTSPN::initInd( gaIndividual2 *Ind )
{
    for ( int i = 0; i < n; i++ ) 
    {
        Ind-> DNA[i] = i;
        Ind->cDNA[i] = contours.Item(i)->GetBeginEndPoint();
    }
}

void V2xSolTSPN::randSweep( gaIndividual2 *Ind, int Iter, bool ContRND )
{
    int i, j;
    uint16_t di, dj;
    Cpoint c;
    
    if ( Iter == 0 ) Iter = RandomValue2( 1, n, 1 );
    
    for ( ; Iter > 0; Iter-- )
    {
        i = RandomValue2( 0,   n-2, 1 );
        j = RandomValue2( i+1, n-1, 1 );
        
        di = Ind->DNA[i];
        dj = Ind->DNA[j];
        
        Ind->DNA[i] = dj;
        Ind->DNA[j] = di;
        
        if ( ContRND )
        {
            Ind->cDNA[i] = contours.Item(dj)->GetRandomStartPoint();
            Ind->cDNA[j] = contours.Item(di)->GetRandomStartPoint();
        }
        else
        {
            c = Ind->cDNA[i];
                Ind->cDNA[i] = Ind->cDNA[j];
                Ind->cDNA[j] = c;
        }
    }
            
}

void V2xSolTSPN::oRBA( gaIndividual2 *Ind ) // tylko raz przeliczone do oko³a
{
    int LastI = n-1;
    Ind->cDNA[0] = contours.Item( Ind->DNA[0] )->GetRBA( Ind->cDNA[LastI], Ind->cDNA[1] );
    
    for ( int i = 1; i < LastI; i++ )
    {
        Ind->cDNA[i] = contours.Item( Ind->DNA[i] )->GetRBA( Ind->cDNA[i-1], Ind->cDNA[i+1] );
    }
    Ind->cDNA[LastI] = contours.Item( Ind->DNA[LastI] )->GetRBA( Ind->cDNA[0], Ind->cDNA[LastI -1] );
}

int V2xSolTSPN::nRBA( gaIndividual2 *Ind, double Eps ) // wiele razy az rozw nie lepsze Eps
{
    int Count = 0;
    double SupportLen, NewSupportLen;
    
    SupportLen = Ind->CalcFit();
    oRBA( Ind );
    NewSupportLen = Ind->CalcFit();
    
    while ( SupportLen - NewSupportLen > Eps )
    {
        SupportLen = NewSupportLen;
        oRBA( Ind );
        NewSupportLen = Ind->CalcFit();
        Count++;
    }
    
    return Count;
}

/*******************************************************************************
                    Class V2xSolTSPN_INIT
*******************************************************************************/

V2xSolTSPN_INIT::V2xSolTSPN_INIT( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive )   
{ 
    Init( Source, StartPoint, Name, true ); 
}

void V2xSolTSPN_INIT::Solve()
{ 
    wxStopWatch sw;
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d) )", bestSol->CalcFit(), sw.Time(), n );
}

/*******************************************************************************
                    Class V2xSolTSPN_oRBA
*******************************************************************************/

V2xSolTSPN_oRBA::V2xSolTSPN_oRBA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive )   
{ 
    Init( Source, StartPoint, Name, DefaultActive ); 
}

void V2xSolTSPN_oRBA::Solve()
{ 
    wxStopWatch sw;
    oRBA( bestSol );
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d) )", bestSol->CalcFit(), sw.Time(), n );
}

/*******************************************************************************
                    Class V2xSolTSPN_nRBA
*******************************************************************************/

V2xSolTSPN_nRBA::V2xSolTSPN_nRBA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive )   
{ 
    Init( Source, StartPoint, Name, DefaultActive ); 
}

void V2xSolTSPN_nRBA::Solve()
{ 
    wxStopWatch sw;
    loadParam(); 
    nRBA( bestSol, epsRBA );
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d) )", bestSol->CalcFit(), sw.Time(), n );
}

/*******************************************************************************
                    Class V2xSolTSPN_RND
*******************************************************************************/

V2xSolTSPN_RND::V2xSolTSPN_RND( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive )   
{ 
    Init( Source, StartPoint, Name, DefaultActive ); 
    addParam( PM_GENERAL_ITER, 100000.0 ); 
}

void V2xSolTSPN_RND::loadParam2()
{
    loadParam();
    iter = (int)GetParam( PM_GENERAL_ITER ).GetValue();
}

void V2xSolTSPN_RND::Solve()
{ 
    wxStopWatch sw;
    loadParam2(); 
    
    randSweep( bestSol, n, true );
    bestSol->CalcFit();
    
    gaIndividual2 *CandSol = new gaIndividual2( bestSol );
    
    for ( ; iter > 0; iter-- )
    {
        randSweep( CandSol, 0, true );
        CandSol->CalcFit();
        
        if ( CandSol->Fit < bestSol->Fit ) bestSol->SetFrom( CandSol );
    }
    delete CandSol;
    
    result = bestSol->CalcFit();
    rtime = sw.Time();
    
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d) )", bestSol->Fit, sw.Time(), n );
}

/*******************************************************************************
                    Class V2xSolTSPN_RND_RBA
*******************************************************************************/

V2xSolTSPN_RND_RBA::V2xSolTSPN_RND_RBA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive )   
{
    Init( Source, StartPoint, Name, DefaultActive ); 
    addParam( PM_GENERAL_ITER, 10000.0 ); 
}

void V2xSolTSPN_RND_RBA::loadParam2()
{
    loadParam();
    iter = (int)GetParam( PM_GENERAL_ITER ).GetValue();
}

void V2xSolTSPN_RND_RBA::Solve()
{ 
    wxStopWatch sw;
    loadParam2(); 
    
    randSweep( bestSol, n, true );
    bestSol->CalcFit();
    
    gaIndividual2 *CandSol = new gaIndividual2( bestSol );
    
    for ( ; iter > 0; iter-- )
    {
        randSweep( CandSol, 0, false );
        nRBA( CandSol, epsRBA );
        
        if ( CandSol->Fit < bestSol->Fit ) bestSol->SetFrom( CandSol );
    }
    delete CandSol;
    
    result = bestSol->CalcFit();
            rtime = sw.Time();
    
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d) )", bestSol->Fit, sw.Time(), n );
}
