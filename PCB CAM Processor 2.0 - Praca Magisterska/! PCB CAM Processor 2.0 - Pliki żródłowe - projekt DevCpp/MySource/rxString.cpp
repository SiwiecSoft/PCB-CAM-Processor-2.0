///-----------------------------------------------------------------
///
/// @file      rxString.cpp
/// @author    Rado4ever
/// Created:   2015-07-08 14:17:53
///
///------------------------------------------------------------------

#include "inc/rx.h"

wxString ConvertToLatin( wxString Str )
{
    int i;
                
    for ( int i = 0; i < Str.Len(); i++ )
    {
        switch ( Str[i] )
        {
            case '�':    Str[i] = 'a';      break;
            case '�':    Str[i] = 'c';      break;
            case '�':    Str[i] = 'e';      break;
            case '�':    Str[i] = 'l';      break;
            case '�':    Str[i] = 'n';      break;
            case '�':    Str[i] = 'o';      break;
            case '�':    Str[i] = 's';      break;
            case '�':    Str[i] = 'z';      break;
            case '�':    Str[i] = 'z';      break;
            
            case '�':    Str[i] = 'A';      break;
            case '�':    Str[i] = 'C';      break;
            case '�':    Str[i] = 'E';      break;
            case '�':    Str[i] = 'L';      break;
            case '�':    Str[i] = 'N';      break;
            case '�':    Str[i] = 'O';      break;
            case '�':    Str[i] = 'S';      break;
            case '�':    Str[i] = 'Z';      break;
            case '�':    Str[i] = 'Z';      break;
        }
    }
    
    return Str;
}
            
wxString MakeLowerCase( wxString Str )
{
    int i;
    Str.MakeLower();
    
    for ( int i = 0; i < Str.Len(); i++ )
    {
        switch ( Str[i] )
        {
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
			case	'�':	Str[i]	=	'�';	break;
        }
    }
    
    return Str;
}

wxString MakeUpperCase( wxString Str )
{
    int i;
    Str.MakeUpper();
    
    for ( int i = 0; i < Str.Len(); i++ )
    {
        switch ( Str[i] )
        {
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
            case	'�':	Str[i]	=	'�';	break;
        }
    }
    
    return Str;
}

wxString ElementOfString( int ElementNumber, wxString SeparatedString, wxChar Separator ) // funkcja pobiera ze stringa fragment oddzielony sepatarorami ( grupuje separatory )
{  
    int SeparatorPos = 0;
    unsigned int i = 1;

    if ( SeparatedString.Find(Separator) == -1 ) return "";

    while ( i < ElementNumber || SeparatedString.Find(Separator) == 0) // wycinamy el z poczatku stringa jesli nie przekroczymy liczby wyciec NoOfElement -1 lub wycinamy jesli sep jest na poczatku
    {
        SeparatorPos = SeparatedString.Find(Separator);
        SeparatedString.Remove(0,SeparatorPos+1);
    
        if ( SeparatorPos > 0 ) i++; // jesli sep nie byl na poczatku to zaliczamy jako wyciacie elementu
        else if ( SeparatorPos == -1 ) return "";
    }

    SeparatorPos = SeparatedString.Find(Separator);

    return SeparatedString.Left(SeparatorPos);
}

wxString FixedLenString( wxString Str, unsigned int Len )                      // zwraca stringa o dl Len ze stringa Str reszte uzupelnia zpacjami
{
    wxString Ret;
    Ret = Str.Left( Len );
    for ( unsigned int i = Ret.Len(); i < Len; i++ )
    {
        Ret += " ";
    }
    return Ret;
}
        
wxArrayString StringToArray( wxString InStr, char Sep )                         // Pobiera separowany Sep'em tekst InStr i wpisuje skladniki do zwracanego wxArrayString, ignoruje separator na koncu
{
    wxArrayString RetArray;
    
    while ( !InStr.IsEmpty() )  // wyninanie kolejnych parametrow az do konca
    {
        RetArray.Add( InStr.BeforeFirst( Sep ) ); // dodanie kolejnego elementu do tablicy
        InStr = InStr.AfterFirst( Sep );          // odciecie poczatku
    }
    
    return RetArray;
}


