/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   
*
*/


#include "catalogshttputils.h"

#ifndef __SERIES60_31__
	#include <cmmanager.h>
	#include <cmdestination.h>
	#include <cmconnectionmethod.h>
#else
	#include <commdb.h>
	#include <aputils.h>
#endif

#include <uriutils.h>
#include <escapeutils.h>

#include "catalogsdebug.h"

namespace CatalogsHttpUtils
    {

    
    _LIT(KHexDigit, "0123456789ABCDEF");
    const TInt KEscapeIndicator = '%';
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

#ifndef __SERIES60_31__
    
TBool ConnectionMethodExistsL( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN((""));
    
    RCmManager manager;
    manager.OpenLC();

    TInt err = KErrNone;
    TCatalogsConnectionMethodType type = aMethod.iType;
    
    // If APN id is set we don't care about the original id
    if ( aMethod.iApnId ) 
        {
        DLTRACE(("ApnId set, handling like accesspoint"));
        type = ECatalogsConnectionMethodTypeAccessPoint;
        }
    
    TBool apExists = EFalse;
    switch( type ) 
        {
        case ECatalogsConnectionMethodTypeDestination: // snap
            {
            DLTRACE(("Snap"));
            RCmDestination destination;
            TRAP( err, destination = manager.DestinationL( aMethod.iId ) );
            // If there's at least one connection method in the destination
            // then we consider it a great success
            apExists = ( err == KErrNone &&
                         destination.ConnectionMethodCount() );
            destination.Close();
            break;
            }

        case ECatalogsConnectionMethodTypeAccessPoint: // iap
            {
            DLTRACE(("Accesspoint"));
            RCmConnectionMethod connMethod;
                
            TRAP( err, connMethod = manager.ConnectionMethodL( 
                aMethod.CurrentApnId() ) );
            apExists = ( err == KErrNone );
            connMethod.Close();             
            break;
            }

        case ECatalogsConnectionMethodTypeDeviceDefault: // default connection
            {
            DLTRACE(("Device default"));
            apExists = ETrue;
            break;
            }

        case ECatalogsConnectionMethodTypeAlwaysAsk: // ask always
        default:
            {
            DLTRACE(("Always ask, default"));
            // nothing to do actually, return EFalse            
            break;
            }
        }

    CleanupStack::PopAndDestroy( &manager );
    DLTRACEOUT(("apExists: %d", apExists));
    return apExists;
    }

#else // __SERIES60_31__

TBool ConnectionMethodExistsL( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN((""));
    TCatalogsConnectionMethodType type = aMethod.iType;
    
    // If APN id is set we don't care about the original id
    if ( aMethod.iApnId ) 
        {
        DLTRACE(("ApnId set, handling like accesspoint"));
        type = ECatalogsConnectionMethodTypeAccessPoint;
        }

    TBool apExists = EFalse;
    switch( type ) 
        {
    	case ECatalogsConnectionMethodTypeAccessPoint: // iap
    		{
    		DLTRACEIN(("Accesspoint"));
            CCommsDatabase* commDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
            CleanupStack::PushL( commDb );
            CApUtils* utils = CApUtils::NewLC( *commDb );
            apExists = utils->IAPExistsL( aMethod.CurrentApnId() );
            CleanupStack::PopAndDestroy( 2, commDb ); // utils, commDb
    		break;
    		}
    		
    	case ECatalogsConnectionMethodTypeDeviceDefault: // default connection
    	    {
    	    DLTRACE(("Device default"));
    	    apExists = ETrue;
    	    break;
    	    }
        
    	case ECatalogsConnectionMethodTypeAlwaysAsk: // ask always
        case ECatalogsConnectionMethodTypeDestination: // snap        
        default:
        	{
            DLTRACE(("Always ask"));
            // return EFalse
            break;
        	}        	
        }
    return apExists;
    }

#endif // __SERIES60_31__
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt ApnFromConnection( 
    RConnection& aConnection,
    TUint32& aApn ) 
    {
    DLTRACEIN((""));
    aApn = 0;

    TUint count;
    aConnection.EnumerateConnections( count );
    
    DLTRACE(("connection count: %d",count));
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
    for (TInt i = 1 ; i <= count ; i++ )
        {
        TPckgBuf<TConnectionInfo> pkg;
        aConnection.GetConnectionInfo( i, pkg );
        DLINFO(("connection: %d, ap: %d",i, pkg().iIapId ));
        }
    #endif
    TInt err = aConnection.GetIntSetting( _L("IAP\\Id"), aApn );
    DLTRACE(("error from GetIntSetting: %d",err));
    return err;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool IsConnectionMethodOpen( 
    RConnection& aConnection, 
    TUint32 aConnectionCount, 
    const TCatalogsConnectionMethod& aMethod,
    TConnectionInfoBuf& aConnectedMethod )
    {
    DLTRACEIN((""));
    
    TUint32 realId = aMethod.iApnId;
    if ( !realId &&
         aMethod.iType == ECatalogsConnectionMethodTypeAccessPoint ) 
        {
        realId = aMethod.iId;
        }
    
    DLTRACE(("Checking if APN %u is open", realId ));
    // Check for a connection matching the one selected before
    for ( TInt i = 1; i <= aConnectionCount; i++ )
        {
        TConnectionInfoBuf connectionInfo;
        
        if ( aConnection.GetConnectionInfo( i, connectionInfo ) == KErrNone )
            {
            if ( connectionInfo().iIapId == realId )
                {
                DLTRACEOUT(("AP open"));
                aConnectedMethod = connectionInfo;
                return ETrue;
                }
            }
        }
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Encodes URI. Already encoded parts are not re-encoded
// ---------------------------------------------------------------------------
//
HBufC8* EncodeUriL( const TDesC8& aUri )
    {
    // Descriptor to hex digits and excluded chars
    const TDesC& KHexChars = KHexDigit;

    const TInt length = aUri.Length();
    
    // find out how many characters need escape encoding
    TInt count = 0;
    for( TInt i = 0; i < length; ++i )
        {
        TChar current( aUri[ i ] );
        if( EscapeUtils::IsExcludedChar( current ) && current != KFragmentDelimiter && 
            !( current == KEscapeIndicator && i + 2 < length && 
               TChar( aUri[ i + 1 ] ).IsHexDigit() && TChar( aUri[ i + 2 ] ).IsHexDigit() ) )
            {
            count++;
            }
        }
    
    if( count == 0 ) // no encoding needed, just allocate and return the whole string
        {
        return aUri.AllocL();
        }
    
    // pre-allocate space for the descriptor
    HBufC8* buf = HBufC8::NewLC( aUri.Length() + count * 2 ); // two extra chars for each escaped
    TPtr8 escaped = buf->Des();

    for( TInt i = 0; i < length; ++i )
        {
        // Check if current character must be escaped
        TChar current ( aUri[ i ] );
        // Check if current character is excluded, but not  if it appears to be escape encoded
        TBool excluded = EscapeUtils::IsExcludedChar( current ) && 
            current != KFragmentDelimiter &&
            !( current == KEscapeIndicator && i + 2 < length && 
                TChar( aUri[ i + 1 ] ).IsHexDigit() && TChar( aUri[ i + 2 ] ).IsHexDigit() );

        if( excluded )
            {
            // Excluded char - escape encode
            escaped.Append( KEscapeIndicator );
            const TInt mostSignificantNibble = ( current & 0xf0 ) >> 4;   // Get msNibble by masking against 11110000 and dividing by 16 (>>4)
            escaped.Append( KHexChars[ mostSignificantNibble ] );
            const TInt leastSignificantNibble = ( current & 0x0f );       // Get lsNibble by masking against 00001111
            escaped.Append( KHexChars[ leastSignificantNibble ] );
            }
        else
            {
            // Not an excluded char - just append             
            escaped.Append( current );
            }
        }
    CleanupStack::Pop( buf );
    return buf;
    }

    
    } // namespace




// ---------------------------------------------------------------------------
// TCatalogsContentDispositionParser
// ---------------------------------------------------------------------------
//    
TCatalogsContentDispositionParser::TCatalogsContentDispositionParser( 
    const TDesC8& aContentDisposition )
    : iContentDisposition( aContentDisposition )
    {
    }
    
    
    
HBufC* TCatalogsContentDispositionParser::FilenameLC() const
    {
    _LIT8( KFilename, "filename=" );
    TInt offset = iContentDisposition.FindF( KFilename );
    User::LeaveIfError( offset );

    // Grab the data after 'filename=' and before eos or field delimiter ';'
    TLex8 lex( iContentDisposition.Mid( offset + KFilename().Length() ) );
    lex.Mark();
    while ( !( lex.Eos() || lex.Peek() == ';' ) )
        {
        // Find end of the filename.
        lex.Inc();
        }
    
    // Strip possible quotations from the filename
    TPtrC8 marked( lex.MarkedToken() );
    
    if ( marked.Length() && marked[0] == '"' )
        {
        marked.Set( marked.Mid( 1 ) );
        }
    if ( marked.Length() && marked[marked.Length() - 1] == '"' )
        {
        marked.Set( marked.Left( marked.Length() - 1 ) );
        }

    // See if there is a path delimiter / or \ and strip any path info.
    TInt pathDelimiterOffset = marked.LocateReverse( '\\' );
    if ( pathDelimiterOffset == KErrNotFound )
        {
        pathDelimiterOffset = marked.LocateReverse( '/' );
        }
    if ( pathDelimiterOffset != KErrNotFound )
        {
        // Strip any path information if found.
        marked.Set( marked.Mid( pathDelimiterOffset + 1 ) );
        }

    // Encode the remaining filename for safety.
    HBufC* name16notencoded = HBufC::NewLC( marked.Length() );
    name16notencoded->Des().Copy( marked );    

    return name16notencoded;
    }

