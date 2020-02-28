///-----------------------------------------------------------------
///
/// @file      V2xDraw.cpp
/// @author    rado4ever
/// Created:   2019-02-26 18:32:53
/// @section   DESCRIPTION
///            PCB_CAM_ProcessorFrm class V2xDraw modul extension
///
///------------------------------------------------------------------

#include "PCB CAM ProcessorFrm.h"

void PCB_CAM_ProcessorFrm::V2xSimpleDraw()
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(GluPosition.x, GluPosition.x + GluSize.x,   GluPosition.y, GluPosition.y + GluSize.y );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // uklad wsp
    
    DrawRect( Cpoint(0.0,-0.15),Cpoint(5.0,0.15),rxColour("GREEN") );
    DrawRect( Cpoint(-0.15,0.0),Cpoint(0.15,5.0),rxColour("RED") );
    DrawCircle( Cpoint(0,0),0.5,rxColour("BLACK") );  
    
    if ( GridVisible ) DrawGrid();
}

void PCB_CAM_ProcessorFrm::V2xDrawMillPreView()
{
    V2xSimpleDraw();
    
    //Draw choosen solution
    SolTSPNArray->Item( OptDialog->GetSelCS() )->Draw( WxTimerSimulation->IsRunning() ? "LIGHT GREY" : "BLACK" );
    
    if ( WxTimerSimulation->IsRunning() ) 
    {
        for ( int i = 0; i < SimulationCounter; i++ ) V2xPrimitivesToSim->Item(i)->Draw("RED");
        SimPrim->Draw("RED");
    }
    
    glFlush();
    SwapBuffers( hDC );
}

void PCB_CAM_ProcessorFrm::V2xDrawDrillPreView()
{
    V2xSimpleDraw();
    
    for ( int i = 0; i < HolesToSim->GetCount(); i++ ) HolesToSim->Item(i)->Draw( Cpoint( 0.0, 0.0 ), WxTimerSimulation->IsRunning() ? "LIGHT GREY" : "BLACK" );
    
    if ( WxTimerSimulation->IsRunning() ) 
        for ( int i = 0; i < SimulationCounter; i++ ) HolesToSim->Item(i)->Draw( Cpoint( 0.0, 0.0 ), "RED" );
        
    //Draw choosen solution
    SolTSPArray->Item( OptDialog->GetSelTS() )->Draw( WxTimerSimulation->IsRunning() ? "LIGHT GREY" : "BLUE" );
    
    glFlush();
    SwapBuffers( hDC );
}
