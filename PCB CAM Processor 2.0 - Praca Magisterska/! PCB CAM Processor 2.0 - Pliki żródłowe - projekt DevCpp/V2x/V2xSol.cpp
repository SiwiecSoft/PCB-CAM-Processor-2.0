///-----------------------------------------------------------------
///
/// @file      V2xSol.cpp
/// @author    rado4ever
/// Created:   2019-03-23 18:06:38
/// @section   DESCRIPTION
///            V2xSol class implementation
///
///------------------------------------------------------------------

#include "V2xSol.h"

wxString V2xSol::GetName() { return name + comment; }

int V2xSol::GetParamCount() { return params.GetCount(); }

V2xParam V2xSol::GetParam( unsigned int I ) 
{
    V2xParam RetParam;
    if ( I < params.GetCount() ) RetParam = *params.Item(I);
    
    return RetParam;
}

V2xParam V2xSol::GetParam( wxString Name ) 
{
    V2xParam RetParam;
    int i = findParam( Name );
    if ( i > -1 ) RetParam = *params.Item(i);
    
    return RetParam;
}

bool V2xSol::SetParamValue( wxString ParamName, double Val )
{
    int i = findParam( ParamName );
    if ( i > -1 )
    {
        params.Item(i)->SetValue( Val );
        return true;
    }
    return false;
}
bool V2xSol::SetParamValue( wxString ParamName, wxString sVal ) 
{ 
    int i = findParam( ParamName );
    if ( i > -1 ) return params.Item(i)->SetValue( sVal );
    return false; 
}

void V2xSol::CopyParams( V2xSol *From )
{
    for ( int i = 0; ( i < params.GetCount() && i < From->params.GetCount() ); i++ )
    {
        if ( params.Item(i)->GetName() == From->params.Item(i)->GetName() )
        {
            params.Item(i)->SetValue( From->params.Item(i)->GetValue() );
        }
    }
}

void V2xSol::GetParamsNames( wxArrayString *Strings ) 
{
    Strings->Clear(); 
    for ( int i = 0; i < params.GetCount(); i++ ) Strings->Add( params.Item(i)->GetName() ); 
}

void V2xSol::addParam( wxString Name, double Val ) 
{ 
    params.Add( new V2xParam( Name, Val ) ); 
}

int V2xSol::findParam( wxString Name )
{
    for ( int i = 0; i < params.GetCount(); i++ ) 
        if ( params.Item(i)->GetName() == Name ) return i; 
            
    return -1;
}

/*******************************************************************************
                    Class V2xHamilton
*******************************************************************************/
void V2xHamilton::calcLen()
{
    len = 0.0;
    for ( int i = 1; i < points.GetCount(); i++ ) len += abs( *points.Item(i) - *points.Item(i-1) );
    // Add len from last one to first one
    len += abs( *points.Item(0) - *points.Item( points.GetCount() -1 ) );
    
    changed = false;
}

bool V2xHamilton::SetPoints( V2xHamilton *In, int From, int To  )
{
    if ( To == 0 ) To = points.GetCount();
    
    if ( In->GetCount() == points.GetCount() )
    {
        for ( int i = From; i < To; i++ ) *points.Item(i) = *In->points.Item(i);
        changed = true;
        return true;
    }
    else return false;
}

void V2xHamilton::GetPoints( Cpoint StartPoint, CpointArray *Out ) 
{ 
    unsigned int BestI = 0, NextI;
    
    for ( BestI = 0; BestI < points.GetCount(); BestI++ )
    {
        if ( abs( StartPoint - *points.Item( BestI ) ) < NANO ) break;
    }
    
    NextI = BestI + 1;
    if ( NextI >= points.GetCount() ) NextI = 0;
    
    while ( NextI != BestI )
    {
        Out->Add( new Cpoint( *points.Item( NextI ) ) ); 
        if ( ++NextI >= points.GetCount() ) NextI = 0;
    }

    //for ( int i = 0; i < points.GetCount(); i++ ) Out->Add( new Cpoint( *points.Item(i) ) ); 
}

Cpoint* V2xHamilton::MovePoint( unsigned int Index )
{
    Cpoint *Ptr = points.Item( Index );
    points.RemoveAt( Index );
    return Ptr;
}

void V2xHamilton::BestInsert( Cpoint *Point )
{
    double InsertCost, MinCost = 100000000000000000.0;
    int BestI = 0, ip1, n = points.GetCount();
    for ( int i = 0; i < n; i++ )
    {
        ip1 = ( i == n-1 ) ? 0 : i+1;
        
        InsertCost = abs( *points.Item(i) - *Point            ) +
                     abs( *Point          - *points.Item(ip1) ) -
                     abs( *points.Item(i) - *points.Item(ip1) );
                     
        if ( InsertCost < MinCost ) 
        {
            MinCost = InsertCost;
            BestI = i + 1;
        }
    }
    // W³¹czenie przed i+1 (po i)
    if ( BestI == 0 || BestI == n ) points.Add( Point );
    else                            points.Insert( Point, BestI );
}

double V2xHamilton::MaxDist( Cpoint Point )
{
    double D, MaxD = 0.0;
    for ( int i = 0; i < points.GetCount(); i++ )
    {
        D = abs( *points.Item(i) - Point );
        if ( D > MaxD ) MaxD = D;
    }
    return MaxD;
}

/*******************************************************************************
                    Class V2xSolTSP
*******************************************************************************/
void V2xSolTSP::Init( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive )     
{ 
    in            = In;
    name          = Name;
    defaultActive = DefaultActive;
    preSolver     = NULL;
    startPoint    = StartPoint;
    
    bestSol.AddPoint( new Cpoint( StartPoint ) );
    for ( int i = 0; i < in->GetCount(); i++ ) bestSol.AddPoint( new Cpoint( in->Item(i)->GetPosition() ) ); 
    
    wtfSol.Init( in->GetCount() +1 );
    
    clones = new V2xSolTSPArray;
}

void V2xSolTSP::Draw( wxString Col )
{
    for ( int i = 0; i < bestSol.GetCount(); i++ )
    {
        DrawCircle( bestSol.GetPoint(i), 0.1, rxColour("RED") );
        //Draw arrows
        if ( i > 0 ) DrawArrow( bestSol.GetPoint(i-1), bestSol.GetPoint(i), rxColour( Col ) );
    }
    DrawArrow( bestSol.GetPoint( bestSol.GetCount() -1 ), bestSol.GetPoint(0), rxColour( "RED" ) );
    
    //Narysuj Klony
    for ( int i = 0; i < clones->GetCount(); i++ ) clones->Item(i)->Draw( Col );
}

void V2xSolTSP::solveClones()
{
    for ( int i = 0; i < clones->GetCount(); i++ )
    {
        clones->Item(i)->CopyParams( this );
        clones->Item(i)->Solve();
    }
}

void V2xSolTSP::GetPoints( CpointArray *Out )
{
    WX_CLEAR_ARRAY( *Out );
    bestSol.GetPoints( startPoint, Out );
    // Dla kolow to samo
    for ( int i = 0; i < clones->GetCount(); i++ ) clones->Item(i)->bestSol.GetPoints( startPoint, Out );
}






