/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This file contains the implementation of CUIUtils
*                class member functions.
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <coemain.h>
 
#include "CUIUtils.h"

using namespace SwiUI::CommonUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCUIUtils::LoadResourceFileL
// Load the given resource file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUIUtils::LoadResourceFileL( const TDesC& aFileName, CCoeEnv* aCoeEnv )
    {
    TFileName file;       
    // If user gives full path let's use it.     
    if ( TParsePtrC( aFileName ).DrivePresent() )    
        {        
        file.Zero();        
        file.Append( aFileName ); 
        }
    else
        { 
        // get dll path, this returns ComminUIs path. 
        Dll::FileName( file ); 
        TDriveName drive( TParsePtrC( file ).Drive( ) ); // solve drive
        file.Zero();
        file.Append( drive );
        file.Append( aFileName ); 
        }
    // Get the rsc file path. Note this function does return Z: drive 
    // if drive lietter is not give. It do not scan user drives: C,E,D etc.
    BaflUtils::NearestLanguageFile( aCoeEnv->FsSession(), file );
 
    return aCoeEnv->AddResourceFileL( file );   
    }

// -----------------------------------------------------------------------------
// CCUIUtils::ConstructVersionString
// Construct a string representation of given version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CUIUtils::ConstructVersionStringLC( TInt aMajor, TInt aMinor, TInt aBuild )
    {
    // Sanity check
    if ( aMajor < 0 )
        {
        aMajor = 0;        
        }  
    if ( aMinor < 0 )
        {
        aMinor = 0;        
        }    
    if ( aBuild < 0 )
        {
        aBuild = 0;        
        }    

    // First calculate the number of digits in version numbers
    TInt result( 0 );
    TInt majorDigits( 1 );    
    TInt minorDigits( 1 );    
    TInt buildDigits( 1 );    

    result = aMajor / 10;
    while ( result > 0 )
        {
        result = result / 10;
        majorDigits++;
        }
         
    result = aMinor / 10;
    while ( result > 0 )    
        {
        result = result / 10;
        minorDigits++;
        }    

    result = aBuild / 10;
    while ( result > 0 )    
        {
        result = result / 10;
        buildDigits++;
        }    
        
    // Now we can construct the actual version descriptor

    HBufC* tmp = HBufC::NewLC( majorDigits + minorDigits + buildDigits + 4 ); // . + possible 0 and brackets
    TPtr tmpBuf( tmp->Des() );
    
    //tmpBuf.Append( ' ' );
    tmpBuf.AppendNum( aMajor );
    tmpBuf.Append( '.' );
    if ( aMinor < 10 )
        {        
        tmpBuf.Append('0');
        }
    
    tmpBuf.AppendNum( aMinor );

    tmpBuf.Append( '(' );
    tmpBuf.AppendNum( aBuild );
    tmpBuf.Append( ')' );

    return tmp;    
    }

// End of File



