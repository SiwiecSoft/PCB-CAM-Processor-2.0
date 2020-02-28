///-----------------------------------------------------------------
///
/// @file      V2xSol.h
/// @author    rado4ever
/// Created:   2019-03-23 16:59:12
/// @section   DESCRIPTION
///            V2xSol class declaration
///
///------------------------------------------------------------------

#ifndef V2XSOL
#define V2XSOL

#include "..\Geometry\Geometry.h"
#include "..\MySource\inc\rx.h"
#include "..\MySource\inc\rx.h"
#include "..\MySource\inc\myconfigfile.h"

#include <wx/filedlg.h>

class V2xParam
{
    public:
        V2xParam() { name = "Brak nazwy"; val = 0.0; }; // taki dupochron
        V2xParam( wxString Name, double Val ) { name = Name; val = Val; };
        double GetValue() { return val; };
        wxString GetName() { return name; };
        void SetValue( double Val ) { val = Val; };
        bool SetValue( wxString sVal ) { return sVal.ToDouble( &val ); };
    
    private:
        wxString name;
        double val;
};
WX_DEFINE_ARRAY_PTR( V2xParam *,  V2xParamArray );

class V2xSol
{
    public:
        virtual ~V2xSol() {};
        wxString GetName();
        wxString GetOutData() { return outData; };
        
        int      GetParamCount();
        V2xParam GetParam( unsigned int I );
        V2xParam GetParam( wxString Name );
        bool SetParamValue( wxString ParamName, double Val );
        bool SetParamValue( wxString ParamName, wxString sVal );
        void CopyParams( V2xSol *From );
        bool IsDefaultActive() { return defaultActive; };
        
        void GetParamsNames( wxArrayString *Strings );
        
        void GetResults( double *R, double *T ) { *R = result; *T = rtime; };

        virtual void Solve() = 0; 
    
    protected:
        double result, rtime;
        wxString name;
        wxString comment;
        wxString outData;
        
        bool   defaultActive;
        
        V2xParamArray    params;
        
        void addParam( wxString Name, double Val );
        int findParam( wxString Name );
};
WX_DEFINE_ARRAY_PTR( V2xSol *,  V2xSolArray );

WX_DEFINE_ARRAY_PTR( Cpoint *,  CpointArray );

class V2xHamilton
{
    public:
        V2xHamilton()                                                 { changed = true; };
        V2xHamilton( V2xHamilton *OldHamilton )                       { for ( int i = 0; i < OldHamilton->points.GetCount(); i++ ) this->points.Add( new Cpoint( *OldHamilton->points.Item(i) ) ); };
        ~V2xHamilton()                                                { WX_CLEAR_ARRAY( points ); }; // jakby byly bledy usunac desttuktor
        
        void         AddPoint( Cpoint *NewPoint )                     { points.Add( NewPoint ); changed = true; };
        void         Clear()                                          { points.Clear(); };
        void         DeletePoints()                                   { WX_CLEAR_ARRAY( points ); };
        unsigned int GetCount()                                       { return points.GetCount(); };
        Cpoint       GetPoint( unsigned int Index )                   { return *points.Item( Index ); };
        Cpoint       GetLastPoint()                                   { return *points.Last(); };
        Cpoint*      GetPointPtr( unsigned int Index )                { return points.Item( Index ); };
        void         SetPoint( unsigned int Index, Cpoint PointVal )  { *points.Item( Index ) = PointVal; changed = true; };
        double       GetLen()                                         { if ( changed ) calcLen(); return len; };
        double       Dist( int i, int j )                             { return abs( *points.Item(i) - *points.Item(j) ); };
        double       Dist( int i, Cpoint *Point )                     { return abs( *points.Item(i) - *Point ); };
        
        Cpoint*      MovePoint( unsigned int Index );
        
        void         BestInsert( Cpoint *Point );
        double       MaxDist( Cpoint Point );
        
        void Init( int N )                                            { for ( int i = 0; i < N; i++ ) points.Add( new Cpoint( 0.0, 0.0 ) ); changed = true; };
        
        bool SetPoints( V2xHamilton *In, int From = 0, int To = 0 );
        
        void GetPoints( Cpoint StartPoint, CpointArray *Out );
    
    private:
        CpointArray points;
        bool changed;
        double len;
        void calcLen();
};
WX_DEFINE_ARRAY_PTR( V2xHamilton *,  V2xHamiltonArray );

class V2xSolTSP;
WX_DEFINE_ARRAY_PTR( V2xSolTSP *,  V2xSolTSPArray );

class V2xSolTSP : public V2xSol
{
    public: 
        virtual ~V2xSolTSP() {}; 
        void Init( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        
        void Init( V2xHamilton *In )                   { bestSol.SetPoints( In ); };
        void SetPreSolver( V2xSolTSP *PreSolver )      { preSolver = PreSolver; };
        void InheritSolution( V2xSolTSP *PreSolver )   { bestSol.SetPoints( &preSolver->bestSol ); };
        
        unsigned int GetN()                { return bestSol.GetCount(); };
        double       GetLen()              { return bestSol.GetLen(); }; 
        
        void         Draw( wxString Col = "BLUE" );
        
        void wtfBackUpSol()  { wtfSol.SetPoints( &bestSol ); };
        void wtfRestoreSol() { bestSol.SetPoints( &wtfSol ); };
        
        void AddClone( V2xSolTSP *Clone ) { clones->Add( Clone ); };
        void GetPoints( CpointArray *Out );
        
        virtual void Solve() = 0; 
        
    protected:
        Cpoint         startPoint;
        V2xHamilton    bestSol;
        V2xHamilton    wtfSol;
        PCBHole2Array *in;
        
        V2xSolTSP      *preSolver; 
        V2xSolTSPArray *clones;
        
        
        
        void solveClones();
        
        void destroy() { WX_CLEAR_ARRAY(params); }; // destroy clonow
}; 

class V2xSolTSP_LittleBAB : public V2xSolTSP // troche dziwacznie ale definicje sa w LittleTSP
{
    public:
        V2xSolTSP_LittleBAB() {};
        V2xSolTSP_LittleBAB( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true )    { Init( In, StartPoint, Name, DefaultActive ); };
        ~V2xSolTSP_LittleBAB()                                                                                   { destroy(); };
        
        virtual void Solve();      
};

class V2xSolTSP_INIT : public V2xSolTSP // definicje sa w V2xSolTSP_2opt_3opt 
{
    public:
        V2xSolTSP_INIT( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true )    { Init( In, StartPoint, Name, DefaultActive ); Solve(); };
        ~V2xSolTSP_INIT()                                                                                   { destroy(); };
        
        virtual void Solve() { comment = wxString::Format( " - ( D³.: %1.1f mm )", bestSol.GetLen()); };         
};

class V2xSolTSP_RND : public V2xSolTSP // definicje sa w V2xSolTSP_2opt_3opt 
{
    public:
        V2xSolTSP_RND( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true )    { Init( In, StartPoint, Name, DefaultActive ); };
        ~V2xSolTSP_RND()                                                                                   { destroy(); };
        
        virtual void Solve()
        {
            wxStopWatch sw;
            solveClones();
            
            while ( bestSol.GetCount() > 0 ) candSol.AddPoint( bestSol.MovePoint( RandomValue2( 0, bestSol.GetCount() -1, 1 ) ) );
            while ( candSol.GetCount() > 0 ) bestSol.AddPoint( candSol.MovePoint(0));
            
            result = bestSol.GetLen();
            rtime = sw.Time();

            comment = wxString::Format( " - ( D³.: %1.1f mm, T: %ld ms )", bestSol.GetLen(), sw.Time() );
        };
        
    protected:    
        V2xHamilton candSol;       
};

#define PM_NN_TYPE "Metoda: (0) - start w p. 0 (1 iter.); (1) - N iteracji (start w ka¿dym p.)"
#define PM_NN_XYPR "Wspomaganie siatki: 0/1/2 (OFF/X/Y) - pr. kier. dla równych odl."

class V2xSolTSP_NN : public V2xSolTSP // definicje sa w V2xSolTSP_2opt_3opt
{
    public:
        V2xSolTSP_NN( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        ~V2xSolTSP_NN() { destroy(); };
        
        enum NNtype
        {
            nnZERO, // Poczatek w punkcie 0 (pojedyncze wykonianie)
            nnN     // poczatek w kazdym mozliwym punkcie - N wywolan
        };
        
        enum NNxypr
        {
            nnPR0, // brak priorytetu
            nnPRX, // pr X
            nnPRY  // pr Y
        };
        
        virtual void Solve(); 
        
    protected:  
        void addNNPoint();
        
        int xyPR;
        V2xHamilton iSol, ciSol, candSol;
};

class V2xSolTSP_2opt : public V2xSolTSP // definicje sa w V2xSolTSP_2opt_3opt 
{
    public:
        V2xSolTSP_2opt() {};
        V2xSolTSP_2opt( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true )    { Init( In, StartPoint, Name, DefaultActive ); candSol.Init( GetN() ); };
        ~V2xSolTSP_2opt()                                                                                   { destroy(); };
        
        virtual void Solve(); 
        
    protected:    
        bool NewTour( int i, int i_1, int j );
          
        V2xHamilton candSol;       
};

class V2xSolTSP_3opt : public V2xSolTSP // definicje sa w V2xSolTSP_2opt_3opt
{
    public:
        V2xSolTSP_3opt() {};
        V2xSolTSP_3opt( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true )    { Init( In, StartPoint, Name, DefaultActive ); candSol.Init( GetN() ); };
        ~V2xSolTSP_3opt()                                                                                   { destroy(); };
        
        virtual void Solve(); 
        
    protected:  
        bool NewTour3( int i, int i_1, int j, int k );
          
        V2xHamilton candSol;
};

#define PM_FI_TYPE "Metoda: (0) - start w p. 0 (1 iter.); (1) - 1 iter. (2 najdal. punkty); (2) - N iteracji (start w ka¿dym p.)"

class V2xSolTSP_FI : public V2xSolTSP // definicje sa w V2xSolTSP_2opt_3opt
{
    public:
        V2xSolTSP_FI() {};
        V2xSolTSP_FI( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true );//    { Init( In, StartPoint, Name, DefaultActive ); addParam( PM_FI_TYPE, 1.0 ); };
        ~V2xSolTSP_FI()                                                                                   { destroy(); };
        
        enum FItype
        {
            fiZERO, // Poczatek w punkcie 0, jako drugi dodany dobierany jest najdalszy od niego (pojedyncze wykonianie)
            fiONE,  // pojedyncze wykonanie poczynajac od pary janodleglejszych punktow
            fiN     // poczatek w kazdym mozliwym punkcie - N wywolan ( za kazdym razem jako drugi dodany dobierany jest najdalszy )
        };
        
        virtual void Solve(); 
        
    protected:  
        void addFarthestPoint();
        void addFarthestPair();
        
        V2xHamilton iSol, ciSol, candSol;
};

class gaIndividualArray;
class gaIndividualSortedArray;

class gaIndividual
{
    public:
    gaIndividual() {};
    gaIndividual( int N ) { this->N = N; DNA = new uint16_t[N]; };
    ~gaIndividual()       { delete[] DNA; };
    
    void SimpleInit()     { for ( uint16_t i = 0; i < N; i++ ) DNA[i] = i; }; // Ustawia numerki od 0 do N
    
    void Permute( int i, int j );
    int OXSetAllel( gaIndividual *Parrent, gaIndividual *Child, int a, int b, int i, int j ); // ustawia i-ty allen w dziecku szukajac w rodzicu od punktu j czegos czego nie ma w dziecku miedzy a i b
    
    void MutateRSM(); // Reverse Seq Mut 
    void Mutate2opt( int i, int j ); //Identyczne lecz nie losowe
    void OrderCrossover( gaIndividual *Parrent2, gaIndividualArray *Out, int MatedCount = 1 );
    
    wxString ShowChroStr();
    
    int N;
    double Fit;
    uint16_t *DNA;
};

static int FitCompare( gaIndividual *first, gaIndividual *second )
{
    if ( first->Fit > second->Fit ) return  1;
    else                            return -1;
}

static int FitCompare2( gaIndividual **first, gaIndividual **second )
{
    gaIndividual *s1 = *first;
    gaIndividual *s2 = *second;

    return s1->Fit > s2->Fit;
}

WX_DEFINE_ARRAY_PTR( gaIndividual *,  gaIndividualArray );
WX_DEFINE_SORTED_ARRAY( gaIndividual *,  gaIndividualSortedArray );

#define PM_GA_ELIT "Elitaryzm: 0/1 (W³/Wy³) - najlepszy nie ginie"
#define PM_GA_ITER "Iloœæ iteracji algorytmu"
#define PM_GA_PARR "Wielkoœæ puli rodzicielskiej"
#define PM_GA_MATE "Iloœæ krzy¿owañ pary - ka¿de daje dwóch potomków"
#define PM_GA_MUTX "Prawdopod. mutacji RSM"

#define PM_GA2OPT_INIT "Inicjalizacja pop. z 2-OPT: 0/1 (W³/Wy³)"
#define PM_GA2OPT_MUTX "Prawdopod. mutacji 2-OPT"

class V2xSolTSP_GA : public V2xSolTSP // definicje sa w V2xSolTSP_GA
{
    public:
        V2xSolTSP_GA() {};
        V2xSolTSP_GA( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true, bool Hybrid2opt = false );
        ~V2xSolTSP_GA();                                                                                   
        
        virtual void Solve(); 
        
    protected:
        bool hybrid2opt;
        bool elit;
        int  evocount;
        
        int iter;
        int parrentPoolSize;
        int mateX; // ilosc krzyzowan pary   
        int popSize; // popSize = (1 + ) parrentPoolSize * mateX
        int mumateRSMx;  // Iloœæ mutacji RSM wœrod dzieci
        int mumate2OPTx; // Iloœæ mutacji 2OPT wœrod dzieci
        
        CpointArray points; // niskopoziomowa interpretacja punktow
        gaIndividualSortedArray *popArray;
        gaIndividualArray       *parentsArray;
        gaIndividualArray       *childArray;
        
        void setCpointArray()                       { for ( int i = 0; i < bestSol.GetCount(); i++ ) points.Add( new Cpoint( bestSol.GetPoint(i) ) ); };
        
        void initPop();
        void showPop();
        void mate();
        void mutateChildren(); // mutuje czasami tablice childArray
        
        void calcFitness( gaIndividual *In );
        void calcFitness( gaIndividualArray *In )         { for ( int i = 0; i < In->GetCount(); i++ ) calcFitness( In->Item(i) ); };
        void calcFitness( gaIndividualSortedArray *In )   { for ( int i = 0; i < In->GetCount(); i++ ) calcFitness( In->Item(i) ); };
        
        void setHamiltonSol( V2xHamilton *Out )           { for ( int i = 0; i < Out->GetCount(); i++ ) Out->SetPoint( i, *points.Item( popArray->Item(0)->DNA[i] ) ); };
        
        void improve2opt( gaIndividual *In, int IterLimit = 0 );
        bool newTour2opt( gaIndividual *In, int i, int i_1, int j );
        void calcStats();
};

class V2xSolTSP_Ext : public V2xSolTSP // definicje sa w V2xSolTSP_2opt_3opt
{
    public:
        V2xSolTSP_Ext() {};
        V2xSolTSP_Ext( PCBHole2Array *In, Cpoint StartPoint, wxString Name, bool DefaultActive = true )    { Init( In, StartPoint, Name, DefaultActive ); candSol.Init( GetN() ); };
        ~V2xSolTSP_Ext()                                                                                   { destroy(); };
        
        virtual void Solve(); 
        
    protected:  
        void genFile( wxString FileName );
        void loadResult( wxString FileName );
          
        V2xHamilton candSol;
};

#endif
