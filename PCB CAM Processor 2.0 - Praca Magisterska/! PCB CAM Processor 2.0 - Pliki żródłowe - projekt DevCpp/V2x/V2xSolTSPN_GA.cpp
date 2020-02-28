///-----------------------------------------------------------------
///
/// @file      V2xSolTSPN_GA.cpp
/// @author    rado4ever
/// Created:   2019-05-14 13:16:31
/// @section   DESCRIPTION
///            
///
///------------------------------------------------------------------

#include "V2xSolTSPN.h"

/*******************************************************************************
                    Class gaIndividual2
*******************************************************************************/

double gaIndividual2::CalcFit()
{
    Fit = 0.0;
    for ( unsigned int i = 1; i < N; i++ ) Fit += abs( cDNA[i] - cDNA[i-1] );
    Fit += abs( cDNA[0] - cDNA[N-1] );
    
    return Fit;
}

void gaIndividual2::MutateRSM_2() // Reverse Seq Mut 
{
    uint16_t c;
    Cpoint   d;
    
    int a = RandomValue2( 1,   N-3, 1 );
    int b = RandomValue2( a+2, N-1, 1 ); 
    
    while ( a < b ) 
    {
        c = DNA[a];
            DNA[a] = DNA[b];
            DNA[b] = c;
            
        d = cDNA[a];
            cDNA[a] = cDNA[b];
            cDNA[b] = d;
        
        a++; b--;
    }
}

void gaIndividual2::Mutate2opt_2( int a, int b )
{
    uint16_t c;
    Cpoint   d;

    while ( a < b ) 
    {
        c = DNA[a];
            DNA[a] = DNA[b];
            DNA[b] = c;
            
        d = cDNA[a];
            cDNA[a] = cDNA[b];
            cDNA[b] = d;
        
        a++; b--;
    }
}

void gaIndividual2::OrderCrossover_2( gaIndividual2 *Parrent2, gaIndividual2Array *Out, int MatedCount )
{
    int i, j1, j2, a, b;
    
    for ( ;MatedCount > 0; MatedCount-- )
    {
        a = RandomValue2( 1,   N-3, 1 ); // losowe p. krzyzowania
        b = RandomValue2( a+2, N-1, 1 );
        
        j1 = j2 = b;
        
        gaIndividual2 *Child1 = new gaIndividual2(N); // nowe dzieci
        gaIndividual2 *Child2 = new gaIndividual2(N);
        
        for ( i = a; i < b; i++ ) // srodkowy fragment od pierwszego z rodzicow
        {
            Child1->DNA[i] = this    ->DNA[i];
            Child2->DNA[i] = Parrent2->DNA[i];
            
            Child1->cDNA[i] = this    ->cDNA[i];
            Child2->cDNA[i] = Parrent2->cDNA[i];
        }
        
        for ( i = b; i < N; i++ ) // ustalamy pola Child[i] od b do N
        {
            j1 = OXSetAllel_2( Parrent2, Child1, a, b, i, j1 );
            j2 = OXSetAllel_2( this    , Child2, a, b, i, j2 );
        }
        
        for ( i = 0; i < a; i++ ) // ustalamy pola Child[i] od 0 do a
        {
            j1 = OXSetAllel_2( Parrent2, Child1, a, b, i, j1 );
            j2 = OXSetAllel_2( this    , Child2, a, b, i, j2 );
        }
        
        Out->Add( Child1 ); // wyplucie dzieci
        Out->Add( Child2 );
    }
}

int gaIndividual2::OXSetAllel_2( gaIndividual2 *Parrent, gaIndividual2 *Child, int a, int b, int i, int j )
{
    int k;
    uint16_t Cand;
    Cpoint  cCand;
    bool CandOK = false;

    while( 1 ) //jesli jeszcze go nie bylo w dziecku ---axxxb--- to wklejam inaczej j++
    {
        Cand  = Parrent-> DNA[j];
        cCand = Parrent->cDNA[j]; 
        
        CandOK = true;
        
        for ( k = a; k < b; k++ ) // czy dziecko ma juz ten allel miedzy a i b
            if ( Cand == Child->DNA[k] ) { CandOK = false; break; }
            
        if ( ++j == N ) j = 0;
        
        if ( CandOK ) 
        {
            Child-> DNA[i] =  Cand;
            Child->cDNA[i] = cCand;
            return j;
        }
    }
}

/*******************************************************************************
                    Class V2xSolTSPN_GA
*******************************************************************************/

V2xSolTSPN_GA::V2xSolTSPN_GA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive, bool Hybrid2opt, bool RndInit )   
{
    Init( Source, StartPoint, Name, DefaultActive );
    hybrid2opt = Hybrid2opt;
    rndInit    = RndInit;
    
    if ( rndInit ) randSweep( bestSol, 1000*n, true );
    
    popArray     = new gaIndividual2SortedArray( FitCompare_2 );
    parentsArray = new gaIndividual2Array;
    childArray   = new gaIndividual2Array;
    
    if ( hybrid2opt )
    {
        addParam( PM_GA2OPT_INIT, 0.0 );
        addParam( PM_GA2OPT_MUTX, 0.01 );
        
        addParam( PM_GA_ELIT, 1.0 );
        addParam( PM_GA_ITER, 50.0 ); //50
        addParam( PM_GA_PARR, 40.0 ); // 40
        addParam( PM_GA_MATE, 10.0 );
        addParam( PM_GA_MUTX, 0.01 );
    }
    else
    {
        addParam( PM_GA_ELIT, 1.0 );
        addParam( PM_GA_ITER, 100.0 );
        addParam( PM_GA_PARR, 100.0 ); // 1000
        addParam( PM_GA_MATE, 10.0 );
        addParam( PM_GA_MUTX, 0.01 );
    } 
}

void V2xSolTSPN_GA::loadParam2()
{
    loadParam();
    
    elit            = (bool)GetParam( PM_GA_ELIT ).GetValue();
    iter            = (int)GetParam( PM_GA_ITER ).GetValue(); //100;
    parrentPoolSize = (int)GetParam( PM_GA_PARR ).GetValue(); //1000
    mateX           = (int)GetParam( PM_GA_MATE ).GetValue(); //10 ilosc krzyzowan pary   
    popSize         = parrentPoolSize * mateX + 1;
    mumateRSMx      = (int)( popSize * GetParam( PM_GA_MUTX     ).GetValue() ); // robi mumateRSMx  mutacji na pokolenie
    mumate2OPTx     = (int)( popSize * GetParam( PM_GA2OPT_MUTX ).GetValue() ); // robi mumate2OPTx mutacji na pokolenie
}

void V2xSolTSPN_GA::Solve()
{ 
    wxStopWatch sw;
    loadParam2(); 
    outData = "\nNo\t[ min\tmax\tœr\todch. st. ]\t\t[ rozk³ad dop. <5%\t<10% ... <100% ]\n";
    
    if ( reInit == 1 )
    {    
        initInd( bestSol ); // ponowna inicjalizacja bestsol
        if ( rndInit ) randSweep( bestSol, 1000*n, true );
        bestSol->CalcFit();
    }
    
    initPop();
    evocount = -1;
    calcStats();

    //Parzenie
    for ( evocount = 0; evocount < iter; evocount++ ) mate();
    
    // Wypisanie wyniku
    bestSol->SetFrom( popArray->Item(0) );
    WX_CLEAR_ARRAY( *popArray );
    
    result = bestSol->CalcFit();
    rtime = sw.Time();
    
    comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms (N: %d) )", bestSol->Fit, sw.Time(), n );
}

void V2xSolTSPN_GA::calcFitness( gaIndividual2 *In )
{
    nRBA( In, epsRBA );
}

void V2xSolTSPN_GA::initPop()
{
    int CopyOfInitSol = 1;
    gaIndividual2 *NewIndividual;
    
    for ( int i = 0; i < popSize; i++ )
    {
        NewIndividual = new gaIndividual2(bestSol);

        if ( i >= CopyOfInitSol ) 
        {
            NewIndividual->MutateRSM_2();
            NewIndividual->CalcFit();
        }

        if ( hybrid2opt && (int)GetParam( PM_GA2OPT_INIT ).GetValue() == 1 ) improve2opt( NewIndividual );
        
        popArray->Add( NewIndividual );
    }
}

void V2xSolTSPN_GA::mate()
{
    int P1, P2;
    gaIndividual2 *Parrent1, *Parrent2;
    
    // Zapamietanie najlepszego - zrzut wskaznika
    gaIndividual2 *BestIndividual = popArray->Item(0); 
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
        Parrent1->OrderCrossover_2( Parrent2, childArray, mateX );
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

void V2xSolTSPN_GA::mutateChildren() // robi 
{
    int RndI;
    
    // Mutacja RSM
    for ( int i = 0; i < mumateRSMx; i++ )
    {
        RndI = RandomValue2( 0, childArray->GetCount() -1, 1 );
        childArray->Item( RndI )->MutateRSM_2();
    }
    // Obliczenie fitnes
    calcFitness( childArray );
    
    // Mutacja 2-OPT
    if ( !hybrid2opt ) return;
    for ( int i = 0; i < mumate2OPTx; i++ )
    {
        improve2opt( childArray->Item( RandomValue2( 0, childArray->GetCount() -1, 1 ) ), 1000 );
    }
}

void V2xSolTSPN_GA::calcStats() 
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

void V2xSolTSPN_GA::improve2opt( gaIndividual2 *In, int IterLimit )
{
    int i, j;
    bool DoBreak = false;
    
    while ( 1 )
    {
        DoBreak = false;
        for ( i = 0; i < n - 2 && !DoBreak; i++ )
        {
            for ( j = i + 2; j < n && !DoBreak; j++ )
            {
                if ( newTour2opt( In, i, j ) ) { DoBreak = true; break; }
                if ( IterLimit-- != 0 ) return;
            } 
        }
        if ( !DoBreak ) return; 
    } 
}

bool V2xSolTSPN_GA::newTour2opt( gaIndividual2 *In, int i, int j ) // przyspieszyc przec permute ??
{   
    int i_1 = ( i == 0 )   ? n : i-1;
    int jp1 = ( j == n-1 ) ? 0 : j+1;
    
    candSol->SetFrom(In);
    candSol->Mutate2opt_2( i, j );
    
    nRBA( candSol, epsRBA*10 );
    
    if ( candSol->Fit < In->Fit )
    {
        In->SetFrom(candSol);
        return true;
    }
    return false;
}
