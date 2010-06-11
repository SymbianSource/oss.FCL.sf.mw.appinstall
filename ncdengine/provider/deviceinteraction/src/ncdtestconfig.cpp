/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of CNcdTestConfig
*
*/


#include "ncdtestconfig.h"
#include "ncdpanics.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"


_LIT( KCurrentMnc, "currentmnc" );
_LIT( KCurrentMcc, "currentmcc" );

_LIT( KHomeMnc, "homemnc" );
_LIT( KHomeMcc, "homemcc" );

_LIT( KImsi, "imsi" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CNcdTestConfig* CNcdTestConfig::NewL( 
    RFs& aFs, const TDesC& aConfigFile )
    {
    CNcdTestConfig* self = new (ELeave) CNcdTestConfig;
    CleanupStack::PushL( self );
    self->ConstructL( aFs, aConfigFile );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdTestConfig::~CNcdTestConfig()
    {
    DLTRACEIN((""));
    
    iConfigStrings.ResetAndDestroy();
    
    }


// ---------------------------------------------------------------------------
// IsSet
// ---------------------------------------------------------------------------
//
TBool CNcdTestConfig::IsSet( TConfigValue aValue ) const
    {
    DLTRACEIN(("aValue: %d", aValue));

    return GetString( aValue ) != NULL;
    }


// ---------------------------------------------------------------------------
// Value getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdTestConfig::Value( TConfigValue aValue ) const
    {
    DLTRACEIN(("aValue: %d", aValue));
    HBufC* value = GetString( aValue );
    NCD_ASSERT_ALWAYS( value, ENcdPanicNoData );
    DLTRACEOUT(( _L("Value: %S"), value ));
    return *value;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdTestConfig::CNcdTestConfig()
    {
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdTestConfig::ConstructL( RFs& aFs, const TDesC& aConfigFile )
    {
    DLTRACEIN(( _L("file: %S"), &aConfigFile ));
    
    // Fill the array with null pointers
    for ( TInt i = 0; i < EConfigInternal; ++i ) 
        {
        iConfigStrings.AppendL( NULL );
        }
        
    // Read & parse the configuration file
    HBufC8* data8 = ReadFileL( aFs, aConfigFile );
    
    CleanupStack::PushL( data8 );
    HBufC16* data16 = ConvertUtf8ToUnicodeL( *data8 );
    CleanupStack::PopAndDestroy( data8 );
    
    CleanupStack::PushL( data16 );   
    ParseL( *data16 );
    CleanupStack::PopAndDestroy( data16 );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdTestConfig::ParseL( const TDesC& aData )
    {
    DLTRACEIN((""));
        
    TLex lex( aData );
    lex.Mark();
    for ( ;; )
        {
        if ( lex.Eos() ||              
             lex.Peek() == 0x0d || 
             lex.Peek() == 0x0a )
            {
            TPtrC data( lex.MarkedToken() );
            
            // basically if-else-if
            TBool doStuff = ( 
                 ParseEntityL( data, KCurrentMnc, EConfigCurrentMnc ) || 
                 ParseEntityL( data, KCurrentMcc, EConfigCurrentMcc ) ||
                 ParseEntityL( data, KHomeMnc, EConfigHomeMnc ) ||
                 ParseEntityL( data, KHomeMcc, EConfigHomeMcc ) ||
                 ParseEntityL( data, KImsi, EConfigImsi) );

            if ( lex.Eos() )
                {
                break;
                }
            else
                {
                lex.SkipAndMark( 1 );
                }
            }
        else
            {
            lex.Inc();
            }
        }

/*    
    // Check that at least one of the attributes was set
    if ( !( iCurrentMnc ||
            iCurrentMcc ||
            iHomeMnc ||
            iHomeMcc ||
            iImsi ) )
        {
        DLERROR(("No attribute had been set, leaving with KErrArgument"));
        User::Leave( KErrArgument );
        }
*/        
    }
    


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CNcdTestConfig::ParseEntityL( 
    const TDesC& aData, const TDesC& aEntityName, TConfigValue aTarget )
    {
    DLTRACEIN((""));
    if ( aData.FindF( aEntityName ) == 0 )
        {
        // current mcd
        TInt offset = aData.LocateF( '=' );
        if ( offset != KErrNotFound )
            {
            delete iConfigStrings[aTarget];
            
            iConfigStrings[aTarget] = NULL;
            
            // Decode all possible encoded characters
            iConfigStrings[aTarget] = aData.Mid( offset + 1 ).AllocL();
            
            // Remove trailing white space
            iConfigStrings[aTarget]->Des().TrimRight();
            return ETrue;
            }
        else
            {
            User::Leave( KErrCorrupt );
            }
        }
    return EFalse;    
    }


// ---------------------------------------------------------------------------
// GetValue
// ---------------------------------------------------------------------------
//
HBufC* CNcdTestConfig::GetString( TConfigValue aValue ) const
    {
    DLTRACEIN(("aValue: %d", aValue));
    NCD_ASSERT_ALWAYS( aValue < EConfigInternal, ENcdPanicIndexOutOfRange ); 
    return iConfigStrings[aValue];
    }

