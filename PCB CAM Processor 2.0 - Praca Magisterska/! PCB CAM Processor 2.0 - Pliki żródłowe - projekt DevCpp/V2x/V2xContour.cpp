///-----------------------------------------------------------------
///
/// @file      V2xContour.cpp
/// @author    rado4ever
/// Created:   2019-02-25 17:01:34
/// @section   DESCRIPTION
///            V2xContour class implementation
///
///------------------------------------------------------------------

#include "V2xSolTSPN.h"

V2xContour::V2xContour() 
{ 
    startPrim        = NULL;
    changed          = true;
    discrete         = true;
    len              = 0.0;
    startPosition    = 0.0;
    startPoint       = Cpoint( 0.0, 0.0 ); 
    quickCords       = NULL;
    hasOwnQuickCords = false;
}

V2xContour::V2xContour( V2xContour *contour )
{
    startPrim        = contour->startPrim;
    changed          = contour->changed;
    discrete         = contour->discrete;
    len              = contour->len;
    startPosition    = contour->startPosition;
    startPoint       = contour->startPoint;
    quickCords       = contour->quickCords;
    hasOwnQuickCords = false;
    
    for ( int i = 0; i < contour->mainContainer.GetCount(); i++ ) 
        mainContainer.Add( contour->mainContainer.Item(i) );
} 

V2xContour::~V2xContour()
{
    if ( hasOwnQuickCords ) delete[] quickCords;
}

void V2xContour::AddPrim( Primitive *Prim ) 
{ 
    mainContainer.Add( Prim );
    if ( !startPrim ) startPrim = Prim;
    changed = true;
}

Cpoint V2xContour::GetPercentPoint( double ContourPercentPosition )
{
    Primitive *Prim;
    double PrimLen;
    startPosition = this->GetLen() * ContourPercentPosition;
    
    for ( int i = 0; i < mainContainer.GetCount(); i++ )
    {
        Prim = mainContainer.Item(i);
        PrimLen = Prim->Len();
        
        if ( startPosition > PrimLen ) startPosition -= PrimLen;
        else                           return Prim->GetPercentPoint( startPosition / PrimLen );
    }
    return Prim->GetEndPoint();
}

void V2xContour::SetRandomStartPrim( unsigned int RandFncNo ) 
{ 
    startPrim = mainContainer.Item( RandomValue2( 0, mainContainer.GetCount() -1, RandFncNo ) );
    discrete = true;
}

void V2xContour::SetRandomStartPoint( unsigned int RandFncNo )
{

    startPoint = quickCords[ RandomValue2( 0, quickCordsSize -1, RandFncNo ) ];
    discrete = false;
}

Cpoint V2xContour::GetRandomStartPoint( unsigned int RandFncNo )
{
    if ( quickCords == NULL ) return mainContainer.Item(0)->GetBeginPoint();
    else                      return quickCords[ RandomValue2( 0, quickCordsSize -1, RandFncNo ) ];
}

Cpoint V2xContour::GetRBA( Cpoint P1, Cpoint P2 )
{
    if ( quickCords == NULL ) return mainContainer.Item(0)->GetBeginPoint();
    
    unsigned int n = mainContainer.GetCount();
    if ( n == 0 ) return Cpoint(0.0,0.0);
    
    double Norm1 = 0.0, Norm2 = 1000.0;
    int quickCordsIndex, quickCordsStep, Iplus, Iminus; 
            
    quickCordsIndex = quickCordsSize /2; 
    quickCordsStep  = quickCordsSize /4;
    
    while ( quickCordsStep > 0 )
    {
        Iplus  = quickCordsIndex + quickCordsStep;
        Iminus = quickCordsIndex - quickCordsStep;  
        
        Norm1 = abs( P1 - quickCords[ Iplus ] )  + abs( P2 - quickCords[ Iplus ] );
        Norm2 = abs( P1 - quickCords[ Iminus ] ) + abs( P2 - quickCords[ Iminus ] );
        
        if ( Norm1 < Norm2 ) quickCordsIndex += quickCordsStep;
        else                 quickCordsIndex -= quickCordsStep;
        
        quickCordsStep /= 2;
    }
    
    return quickCords[ quickCordsIndex ];
}

double V2xContour::GetLen()
{
    if ( changed )
    {
        len = 0.0;
        for ( int i = 0; i < mainContainer.GetCount(); i++ ) len += mainContainer.Item(i)->Len();
        changed = false;
    }
    return len;
}

void V2xContour::Draw( wxString Col )
{
    for ( int i = 0; i < mainContainer.GetCount(); i++ ) mainContainer.Item(i)->Draw( Col );
}

Cpoint V2xContour::GetBeginEndPoint()
{
    if ( discrete ) return startPrim ? startPrim->GetBeginPoint() : Cpoint( 0.0, 0.0 );
    else            return startPoint;
}

void V2xContour::InitQuickCords()
{
    quickCords = new Cpoint[ quickCordsSize ];
    hasOwnQuickCords = true;
    
    Primitive *Prim;
    double PrimLen;
    
    double Pos = 0.0, Step = this->GetLen() / quickCordsSize;
    int PrimCount = 0;
    
    Prim = mainContainer.Item(0);
    PrimLen = Prim->Len();
    
    for ( int i = 0; i < quickCordsSize; i++ )
    {
        if ( Pos > PrimLen ) 
        {
            Pos -= PrimLen;
            if ( ++PrimCount == mainContainer.GetCount() ) return;
            Prim = mainContainer.Item(PrimCount);
            PrimLen = Prim->Len();
        }  
        
        quickCords[i] = Prim->GetPercentPoint( Pos / PrimLen );
        Pos += Step;
    }
}

void V2xContour::GetPrimitives( Cpoint StartPoint, PrimitiveArray *OutPrimitives )
{
    unsigned int StartI = 0, i = 0;
    Primitive *Prim, *NewPrim;
    PrimitiveArray TempPrimArray;
    Cpoint mStartPoint;
    bool PrimIsRev;
    
    for ( i = 0; i < mainContainer.Count(); i++ )
    {
        Prim = mainContainer.Item(i);
        
        if ( Prim->GetType() == line )
        {
            mStartPoint = GetPointOnLine( Prim->GetBeginPoint(), Prim->GetEndPoint(), StartPoint ); // Rzut punktu StartPoint na prost¹ prim
            if ( abs( mStartPoint - StartPoint ) < NANO )  // Odlegloœæ punktu od jego rzutu rowna ZERO
            {
                StartI = i;
                break;
            }
        }
        else if ( abs( abs( Prim->GetCenterPoint() - StartPoint ) - Prim->GetR() ) < NANO && Prim->IsIn( StartPoint ) )
        {
            StartI = i;
            break;
        }
    }
    
    Prim = new Primitive( mainContainer.Item( StartI ) );
    Prim->Init();
    Prim->addpoint( StartPoint );
    Prim->GetFragments( &TempPrimArray );
    PrimIsRev = Prim->IsReverse();
    delete Prim;
    
    if ( PrimIsRev )
    {
        TempPrimArray.Item(0)->Reverse();
        TempPrimArray.Item(1)->Reverse();
    }
    
    OutPrimitives->Add( TempPrimArray.Item( PrimIsRev? 0:1 ) );
    
    i = StartI;
    if ( ++i >= mainContainer.Count() ) i = 0;
    
    while ( i != StartI )
    {

        OutPrimitives->Add( new Primitive( mainContainer.Item( i ) ) );
        if ( ++i >= mainContainer.Count() ) i = 0;
    }
    
    OutPrimitives->Add( TempPrimArray.Item( PrimIsRev? 1:0 ) );
}



