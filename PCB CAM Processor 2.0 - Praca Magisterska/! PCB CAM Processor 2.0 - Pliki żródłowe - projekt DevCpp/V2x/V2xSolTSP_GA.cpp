///-----------------------------------------------------------------
///
/// @file      V2xSolTSP_GA.cpp
/// @author    rado4ever
/// Created:   2019-04-26 13:11:45
/// @section   DESCRIPTION
///            Implementation of V2xSolTSP_GA class
///------------------------------------------------------------------

#include "V2xSol.h"

/*******************************************************************************
                    Struct gaIndividual
*******************************************************************************/

void gaIndividual::Permute( int i, int j )
{
    uint16_t c = DNA[i];
                 DNA[i] = DNA[j];
                 DNA[j] = c;    
}

void gaIndividual::MutateRSM() // Reverse Seq Mut
{
    int a = RandomValue2( 1,   N-3, 1 );
    int b = RandomValue2( a+2, N-1, 1 ); 
    
    while ( a < b ) Permute( a++, b-- );
}

void gaIndividual::Mutate2opt( int i, int j )
{
    uint16_t c;
    
    while ( i < j )
    {
        c = DNA[i];
            DNA[i] = DNA[j];
            DNA[j] = c;
            
        i++; 
        j--;
    }
}

void gaIndividual::OrderCrossover( gaIndividual *Parrent2, gaIndividualArray *Out, int MatedCount )
{
    int i, j1, j2, a, b;
    
    for ( ;MatedCount > 0; MatedCount-- )
    {
        a = RandomValue2( 1,   N-3, 1 ); // losowe p. krzyzowania
        b = RandomValue2( a+2, N-1, 1 );
        
        j1 = j2 = b;
        
        gaIndividual *Child1 = new gaIndividual(N); // nowe dzieci
        gaIndividual *Child2 = new gaIndividual(N);
        
        for ( i = a; i < b; i++ ) // srodkowy fragment od pierwszego z rodzicow
        {
            Child1->DNA[i] = this    ->DNA[i];
            Child2->DNA[i] = Parrent2->DNA[i];
        }
        
        for ( i = b; i < N; i++ ) // ustalamy pola Child[i] od b do N
        {
            j1 = OXSetAllel( Parrent2, Child1, a, b, i, j1 );
            j2 = OXSetAllel( this    , Child2, a, b, i, j2 );
        }
        
        for ( i = 0; i < a; i++ ) // ustalamy pola Child[i] od 0 do a
        {
            j1 = OXSetAllel( Parrent2, Child1, a, b, i, j1 );
            j2 = OXSetAllel( this    , Child2, a, b, i, j2 );
        }
        
        Out->Add( Child1 ); // wyplucie dzieci
        Out->Add( Child2 );
    }
}

int gaIndividual::OXSetAllel( gaIndividual *Parrent, gaIndividual *Child, int a, int b, int i, int j )
{
    int k;
    uint16_t Cand;
    bool CandOK = false;

    while( 1 ) //jesli jeszcze go nie bylo w dziecku ---axxxb--- to wklejam inaczej j++
    {
        Cand = Parrent->DNA[j]; 
        CandOK = true;
        
        for ( k = a; k < b; k++ ) // czy dziecko ma juz ten allel miedzy a i b
            if ( Cand == Child->DNA[k] ) { CandOK = false; break; }
            
        if ( ++j == N ) j = 0;
        
        if ( CandOK ) 
        {
            Child->DNA[i] = Cand;
            return j;
        }
    }
}

wxString gaIndividual::ShowChroStr()
{
    wxString Ret = "[ ";
    for ( int i = 0; i < N; i++ ) Ret += wxString::Format( "%i ", DNA[i] );
    return Ret += "]";
}

/*******************************************************************************
                    Class V2xSolTSP_GA
*******************************************************************************/

V2xSolTSP_GA::V2xSolTSP_GA( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive, bool Hybrid2opt )  
{ 
    Init( In, StartPoint, Name, DefaultActive ); 
    hybrid2opt = Hybrid2opt;
    
    popArray     = new gaIndividualSortedArray( FitCompare );
    parentsArray = new gaIndividualArray;
    childArray   = new gaIndividualArray;
    
    if ( Hybrid2opt )
    {
        addParam( PM_GA2OPT_INIT, 0.0 );
        addParam( PM_GA2OPT_MUTX, 0.01 );
        
        addParam( PM_GA_ELIT, 1.0 );
        addParam( PM_GA_ITER, 50.0 );
        addParam( PM_GA_PARR, 40.0 );
        addParam( PM_GA_MATE, 10.0 );
        addParam( PM_GA_MUTX, 0.01 );
    }
    else
    {
        addParam( PM_GA_ELIT, 1.0 );
        addParam( PM_GA_ITER, 100.0 );
        addParam( PM_GA_PARR, 1000.0 );
        addParam( PM_GA_MATE, 10.0 );
        addParam( PM_GA_MUTX, 0.01 );
    } 
}

V2xSolTSP_GA::~V2xSolTSP_GA()                                                                                   
{ 
    destroy(); 
    delete popArray;
    delete parentsArray;
    delete childArray;
}

void V2xSolTSP_GA::calcFitness( gaIndividual *In )
{
    In->Fit = 0.0;
    for ( int i = 1; i < In->N; i++ ) In->Fit += abs( *points.Item( In->DNA[i] ) - *points.Item( In->DNA[i-1] ) );
    // Add len from last one to first one
    In->Fit += abs( *points.Item( In->DNA[0] ) - *points.Item( In->DNA[ In->N - 1 ] ) );
}

void V2xSolTSP_GA::Solve()
{
    wxStopWatch sw;
    solveClones();
    
    outData = "\nNo\t[ min\tmax\tœr\todch. st. ]\t\t[ rozk³ad dop. <5%\t<10% ... <100% ]\n";
                
    elit            = (bool)GetParam( PM_GA_ELIT ).GetValue();
    iter            = (int)GetParam( PM_GA_ITER ).GetValue(); //100;
    parrentPoolSize = (int)GetParam( PM_GA_PARR ).GetValue(); //1000
    mateX           = (int)GetParam( PM_GA_MATE ).GetValue(); //10 ilosc krzyzowan pary   
    popSize         = parrentPoolSize * mateX + 1;
    mumateRSMx      = (int)( popSize * GetParam( PM_GA_MUTX     ).GetValue() ); // robi mumateRSMx  mutacji na pokolenie
    mumate2OPTx     = (int)( popSize * GetParam( PM_GA2OPT_MUTX ).GetValue() ); // robi mumate2OPTx mutacji na pokolenie
    
    if ( preSolver != NULL ) 
    {
        preSolver->Solve();
        InheritSolution( preSolver );
    }
    
    setCpointArray(); 
    
    initPop();
    evocount = -1;
    calcStats();
    
    //Parzenie
    for ( evocount = 0; evocount < iter; evocount++ ) mate();
    
    setHamiltonSol( &bestSol );
    WX_CLEAR_ARRAY( *popArray );
    
    result = bestSol.GetLen();
    rtime = sw.Time();
    
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d) )", bestSol.GetLen(), sw.Time(), GetN() );
}   
    
void V2xSolTSP_GA::initPop()
{
    int CopyOfInitSol = 1;
    gaIndividual *NewIndividual;
    
    for ( int i = 0; i < popSize; i++ )
    {
        NewIndividual = new gaIndividual( this->GetN() );
        NewIndividual->SimpleInit();
        
        if ( i >= CopyOfInitSol ) 
        {
            NewIndividual->MutateRSM();
        }

        calcFitness( NewIndividual );
        if ( (int)GetParam( PM_GA2OPT_INIT ).GetValue() == 1 ) improve2opt( NewIndividual );
        
        popArray->Add( NewIndividual );
    }
}

void V2xSolTSP_GA::showPop()
{
    wxString Comment = "";
    for ( int i = 0; i < popArray->GetCount(); i++ )
    {
        Comment += wxString::Format( "%1.1f, ", popArray->Item(i)->Fit );
    }
    wxMessageBox( Comment );
}

void V2xSolTSP_GA::mate()
{
    int P1, P2;
    gaIndividual *Parrent1, *Parrent2;
    
    // Zapamietanie najlepszego - zrzut wskaznika
    gaIndividual *BestIndividual = popArray->Item(0); 
    // Budowa puli rodzicielskiej - zrzut wskaznikow lacznie z best
    for ( int i = 0; i < parrentPoolSize; i++ ) parentsArray->Add( popArray->Item(i) );
    // Œmieræ s³abych
    for ( int i = parrentPoolSize; i < popArray->GetCount(); i++ ) delete popArray->Item(i);
    //Czyszczenie tablicy(listy) populacji
    popArray->Clear();
    
    //Parzenie
    while ( parentsArray->GetCount() > 0 )
    {
        //Selekcja rodzica 1
        P1 = RandomValue2( 0, parentsArray->GetCount() -1, 1 );
        Parrent1 = parentsArray->Item(P1);
        parentsArray->RemoveAt(P1);
        //Selekcja rodzica 2
        P2 = RandomValue2( 0, parentsArray->GetCount() -1, 1 );
        Parrent2 = parentsArray->Item(P2);
        parentsArray->RemoveAt(P2);
        //Generowanie potomków do childArray
        Parrent1->OrderCrossover( Parrent2, childArray, mateX );
        //Œmieræ rodziców chyba ¿e któryœ jest najlepszy w populacji
        if ( elit )
        {   
            if ( Parrent1 != BestIndividual ) delete Parrent1;
            if ( Parrent2 != BestIndividual ) delete Parrent2;
        }
        else
        {
            delete Parrent1;
            delete Parrent2;
        }
    }
    
    //Liczenie przystosowania nowego pokolena, powtórne dodanie najlepszego rodzica oraz dzieci do populacji
    mutateChildren();
    if ( elit ) popArray->Add( BestIndividual );
    for ( int i = 0; i < childArray->GetCount(); i++ ) popArray->Add( childArray->Item(i) );
    //Czyszczenie listy dzieci
    childArray->Clear();
    
    //Obliczenia statystyk popupacji
    calcStats();
}

void V2xSolTSP_GA::mutateChildren() // robi 
{
    // Mutacja RSM
    for ( int i = 0; i < mumateRSMx; i++ )
    {
        childArray->Item( RandomValue2( 0, childArray->GetCount() -1, 1 ) )->MutateRSM();
    }
    
    // Obliczenie fitnes
    calcFitness( childArray );
    
    // Mutacja 2-OPT
    for ( int i = 0; i < mumate2OPTx; i++ )
    {
        improve2opt( childArray->Item( RandomValue2( 0, childArray->GetCount() -1, 1 ) ) );
    }
}

void V2xSolTSP_GA::improve2opt( gaIndividual *In, int IterLimit )
{
    int i, j, n = GetN();
    bool DoBreak = false;
    int i_1;
    
    while (1)
    {
        DoBreak = false;
        for ( i = 0; i < n - 2 && !DoBreak; i++ )
        {
            i_1 = ( i == 0 ) ? n-1 : i-1;
            for ( j = i + 2; j < n && !DoBreak; j++ )
            {
                if ( newTour2opt( In, i, i_1, j ) ) { DoBreak = true; break; }
            } 
        }
        if ( !DoBreak ) return; 
    }    
}

bool V2xSolTSP_GA::newTour2opt( gaIndividual *In, int i, int i_1, int j )
{   
    uint16_t c;
    double CandLen = In->Fit;
    // Usuniete luki
    CandLen -= abs( *points.Item( In->DNA[i_1] ) - *points.Item( In->DNA[i] ) ); // jeœli i == 0 to punkt i-1 zapetlamy do n
    CandLen -= abs( *points.Item( In->DNA[j-1] ) - *points.Item( In->DNA[j] ) ); 
    // Luki dodane
    CandLen += abs( *points.Item( In->DNA[i_1] ) - *points.Item( In->DNA[j-1] ) ); 
    CandLen += abs( *points.Item( In->DNA[i] )   - *points.Item( In->DNA[j]   ) );
    // Ocena rozwiazania
    if ( CandLen < In->Fit )
    {
        In->Mutate2opt( i, --j );
        In->Fit = CandLen;
        return true;
    }
    return false;
}

void V2xSolTSP_GA::calcStats()
{
    wxString RepRec = "";
    double min, max, avg = 0.0, delta, dev = 0.0, phist = 0.05, level;
    int count = 0;
    int size = popArray->GetCount();
    
    max = popArray->Item( size -1 )->Fit;
    min = popArray->Item( 0 )      ->Fit;
    
    for ( int i = 0; i < size; i++ ) avg += popArray->Item(i)->Fit;
    avg /= size;
    
    for ( int i = 0; i < size; i++ ) 
    {
        delta = popArray->Item(i)->Fit - avg;
        dev += delta*delta;
    }
    dev /= size;
    
    level = phist * ( max - min ) + min;
    for ( int i = 0; i < size; i++ )
    {  
        if ( popArray->Item(i)->Fit < level || phist == 1.0 ) count++;
        else
        {
            phist += 0.05;
            level = phist * ( max - min ) + min;
            
            RepRec += wxString::Format( "%d\t", count );
            
            i--;
            count = 0;
        }
    }
    RepRec += "\n";
    
    RepRec = wxString::Format( "%d\t%1.1f\t%1.1f\t%1.1f\t%1.1f\t\t", evocount+1, min, max, avg, dev ) + RepRec;
    outData += RepRec;
}
