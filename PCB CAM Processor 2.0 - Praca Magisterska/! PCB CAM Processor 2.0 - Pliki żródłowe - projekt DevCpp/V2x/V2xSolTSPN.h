///-----------------------------------------------------------------
///
/// @file      V2xSolTSPN.h
/// @author    rado4ever
/// Created:   2019-05-08 16:59:12
/// @section   DESCRIPTION
///            
///
///------------------------------------------------------------------

#ifndef V2XSOLTSPN
#define V2XSOLTSPN

#include "..\Geometry\Geometry.h"
#include "..\MySource\inc\rx.h"
#include "V2xSol.h"

class V2xContour
{
    public:
        V2xContour();
        V2xContour( V2xContour *contour );
        ~V2xContour();
        
        void AddPrim( Primitive *Prim );
        Cpoint GetPercentPoint( double ContourPercentPosition );
        void SetRandomStartPrim( unsigned int RandFncNo = 1 );
        void SetRandomStartPoint( unsigned int RandFncNo = 1 );
        Cpoint GetRandomStartPoint( unsigned int RandFncNo = 1 );
        Cpoint GetRBA( Cpoint P1, Cpoint P2 );
        double GetLen();
        void Draw( wxString Col = "BLACK" );
        
        Cpoint GetBeginEndPoint();
        void InitQuickCords();
        
        void GetPrimitives( Cpoint StartPoint, PrimitiveArray *OutPrimitives );
    
    private:
        PrimitiveArray  mainContainer;
        Primitive      *startPrim;
        bool            changed;
        bool            discrete;
        double          len;
        double          startPosition;
        Cpoint          startPoint;
        
        static const int quickCordsSize = 1024 ;// * 16 * 4;
        Cpoint *quickCords;
        bool hasOwnQuickCords;
};

WX_DEFINE_ARRAY_PTR( V2xContour *,  V2xContourArray );

class gaIndividual2Array;
class gaIndividual2SortedArray;

class gaIndividual2 : public gaIndividual
{
    public:
        gaIndividual2( int N )               { this->N = N;      DNA = new uint16_t[N]; cDNA = new Cpoint[N]; };
        gaIndividual2( gaIndividual2 *Inv )  { this->N = Inv->N; DNA = new uint16_t[N]; cDNA = new Cpoint[N]; SetFrom(Inv); };
        ~gaIndividual2()                     { delete[] cDNA; };
        
        void   SetFrom( gaIndividual2 *Inv )                   { Fit = Inv->Fit; for ( int i = 0; i < N; i++ ) { DNA[i] = Inv->DNA[i]; cDNA[i] = Inv->cDNA[i]; } }
        void   SetFrom( gaIndividual2 *Inv, int From, int To ) { DNA[To] = Inv->DNA[From]; cDNA[To] = Inv->cDNA[From]; }
        
        void SetCdnaFrom( gaIndividual2 *Inv )
        {
            for ( int i = 0; i < N; i++ )
                for ( int j = 0; j < N; j++ )
                    if ( DNA[i] == Inv->DNA[j] ) 
                    {
                        cDNA[i] = Inv->cDNA[j];
                        break;
                    }
        }
        
        double CalcFit();
        void   Randomize();
        
        // override
        void MutateRSM_2(); // Reverse Seq Mut 
        void Mutate2opt_2( int a, int b ); //Identyczne lecz nie losowe
        void OrderCrossover_2( gaIndividual2 *Parrent2, gaIndividual2Array *Out, int MatedCount = 1 );
        int  OXSetAllel_2( gaIndividual2 *Parrent, gaIndividual2 *Child, int a, int b, int i, int j );
        
        Cpoint *cDNA;
};

static int FitCompare_2( gaIndividual2 *first, gaIndividual2 *second )
{
    if ( first->Fit > second->Fit ) return  1;
    else                            return -1;
}

static int FitCompare2_2( gaIndividual2 **first, gaIndividual2 **second )
{
    gaIndividual2 *s1 = *first;
    gaIndividual2 *s2 = *second;

    return s1->Fit > s2->Fit;
}

WX_DEFINE_ARRAY_PTR( gaIndividual2 *,  gaIndividual2Array );
WX_DEFINE_SORTED_ARRAY( gaIndividual2 *,  gaIndividual2SortedArray );

#define PM_RBA_EPS "Epsilon met. RBA"
#define PM_GENERAL_ITER "Iloœæ iteracji"
#define PM_REINIT "Reinicjalizacja przy starcie: 0/1 (W³/Wy³)"

class V2xSolTSPN : public V2xSol
{
    public:
        virtual ~V2xSolTSPN() {};
        void Init( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        
        double GetLen() { return bestSol->Fit; };
        void   Draw( wxString Col = "BLACK" );
        
        void wtfBackUpSol() { wtfSol->SetFrom( bestSol ); };
        void wtfRestoreSol() { bestSol->SetFrom( wtfSol ); };
        
        void GetPrimitives( PrimitiveArray *OutPrimitives );
        
        virtual void Solve() = 0; 
        
    protected:
        unsigned int n;
        double epsRBA;
        int    reInit;
        Cpoint startPoint;
        
        V2xContourArray  contours; 
        gaIndividual2    *bestSol;
        gaIndividual2    *candSol;
        gaIndividual2    *wtfSol;
        
        void loadParam();
        void initInd( gaIndividual2 *Ind );
        void randSweep( gaIndividual2 *Ind, int Iter = 0, bool ContRND = true ); 
        void oRBA( gaIndividual2 *Ind );                    // tylko raz przeliczone do oko³a
        int  nRBA( gaIndividual2 *Ind, double Eps = 0.01 ); // wiele razy az rozw nie lepsze Eps - zwraca liczbe itreracji
        
        void copyContours( V2xContourArray *In, V2xContourArray *Out );                              // True Copy
        void destroy(); 
}; 
WX_DEFINE_ARRAY_PTR( V2xSolTSPN *,  V2xSolTSPNArray );

class V2xSolTSPN_INIT : public V2xSolTSPN
{
    public:
        V2xSolTSPN_INIT( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        ~V2xSolTSPN_INIT() { destroy(); };
        
        virtual void Solve(); 
};

class V2xSolTSPN_oRBA : public V2xSolTSPN
{
    public:
        V2xSolTSPN_oRBA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        ~V2xSolTSPN_oRBA() { destroy(); };
        
        virtual void Solve(); 
};

class V2xSolTSPN_nRBA : public V2xSolTSPN
{
    public:
        V2xSolTSPN_nRBA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        ~V2xSolTSPN_nRBA() { destroy(); };
        
        virtual void Solve(); 
};

class V2xSolTSPN_RND : public V2xSolTSPN
{
    public:
        V2xSolTSPN_RND( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        ~V2xSolTSPN_RND() { destroy(); };
        
        virtual void Solve();
        
    private:
        int  iter;
        void loadParam2();
};

class V2xSolTSPN_RND_RBA : public V2xSolTSPN
{
    public:
        V2xSolTSPN_RND_RBA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true );
        ~V2xSolTSPN_RND_RBA() { destroy(); };
        
        virtual void Solve();
        
    private:
        int  iter;
        void loadParam2();  
};

#define PM_OPT_RBA_TYPE "Wersja RBA: (0) - sta³a; (1) - ruchoma; (2) - losowa"
#define PM_OPT_IMPROVEX "Lepszoœæ kolejnych rozwi¹zañ [mm]"

class V2xSolTSPN_2opt : public V2xSolTSPN
{
    public:
        V2xSolTSPN_2opt( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true, bool RndInit = false );
        ~V2xSolTSPN_2opt();
        
        virtual void Solve(); 
        
    protected:
        gaIndividual2 *srcSol;
        bool rndInit;
        int  rbaType;
        double impX;
        
        void loadParam2();
        bool newTour( int i, int j );
        bool setAndRBA( int From, int To );  
            
};

class V2xSolTSPN_3opt : public V2xSolTSPN
{
    public:
        V2xSolTSPN_3opt( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true, bool RndInit = false );
        ~V2xSolTSPN_3opt();
        
        virtual void Solve(); 
        
    protected:
        gaIndividual2 *srcSol;
        bool rndInit;
        int  rbaType;
        double impX;
        
        void loadParam2();
        bool newTour( int i, int j, int k, bool Rev );
        bool setAndRBA( int From, int To );     
};

class V2xSolTSPN_GA : public V2xSolTSPN
{
    public:
        V2xSolTSPN_GA( V2xContourArray *Source, Cpoint StartPoint, wxString Name, bool DefaultActive = true, bool Hybrid2opt = false, bool RndInit = false  );
        ~V2xSolTSPN_GA() { destroy(); };
        
        virtual void Solve(); 
        
    protected:
        bool rndInit;
        bool hybrid2opt;
        bool elit;
        int  evocount;
        
        int iter;
        int parrentPoolSize;
        int mateX; // ilosc krzyzowan pary   
        int popSize; // popSize = (1 + ) parrentPoolSize * mateX
        int mumateRSMx;  // Iloœæ mutacji RSM wœrod dzieci
        int mumate2OPTx; // Iloœæ mutacji 2OPT wœrod dzieci
             
        gaIndividual2SortedArray *popArray;
        gaIndividual2Array       *parentsArray;
        gaIndividual2Array       *childArray;
        
        void loadParam2();
        
        void initPop();
        void mate();
        void mutateChildren(); // mutuje czasami tablice childArray
        void calcStats();
        
        void calcFitness( gaIndividual2 *In );
        void calcFitness( gaIndividual2Array *In )         { for ( int i = 0; i < In->GetCount(); i++ ) calcFitness( In->Item(i) ); };
        void calcFitness( gaIndividual2SortedArray *In )   { for ( int i = 0; i < In->GetCount(); i++ ) calcFitness( In->Item(i) ); };
        
        void improve2opt( gaIndividual2 *In, int IterLimit = -1 );
        bool newTour2opt( gaIndividual2 *In, int i, int j );
            
};

#endif

