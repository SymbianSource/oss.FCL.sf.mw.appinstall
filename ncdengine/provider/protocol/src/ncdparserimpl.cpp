/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdParserImpl implementation
*
*/


#include <xml/parser.h>
#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>

#include <s32file.h>

#include "ncdparserimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdtoplevelparser.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"
#include "ncdprotocoldefaultobserver.h"


// Input buffer grow size for CBufSeg when appending new xml data in async mode
const TInt KInputBufferGrowSize( 4096 );

// Size of the buffer for the data to be processed in one async step.
// This is an important figure as it determines the time used in the parser
// in each step. Too big -> too unresponsive a system, too small --> too slow
// a parsing operation.
const TInt KFeedBufferSize( 1024 );

CNcdParserImpl* CNcdParserImpl::NewL( 
    MNcdProtocolDefaultObserver* aDefaultObserver )
    {
    CNcdParserImpl* self = new(ELeave) CNcdParserImpl( aDefaultObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CNcdParserImpl::CNcdParserImpl( 
    MNcdProtocolDefaultObserver* aDefaultObserver  )
    : CActive( EPriorityStandard ), 
      iDefaultObserver( aDefaultObserver )
    {
    CActiveScheduler::Add( this );
    }

CNcdParserImpl::~CNcdParserImpl()
    {
    DLTRACEIN((""));
    CancelParsing();
    
    delete iXmlParser;
    
    DLINFO((""));
    delete iSubParser;
    
    DLINFO((""));
    delete iInputBuffer;
    DLINFO((""));
    delete iFeedBuffer;
    DLINFO((""));
    delete iObservers;
    DLINFO((""));
    delete iDefaultObserver;
    DLTRACEOUT((""));
    }

void CNcdParserImpl::ConstructL()
    {
    iObservers = CNcdParserObserverBundleImpl::NewL( iDefaultObserver );
    }



MNcdParserObserverBundle& CNcdParserImpl::Observers() const
    {
    return *iObservers;
    }

MNcdProtocolDefaultObserver& CNcdParserImpl::DefaultObserver() const
    {
    return *iDefaultObserver;
    }


void CNcdParserImpl::BeginSyncL()
    {
    DLTRACEIN((""));    
    BeginSyncL( NULL );
    }


void CNcdParserImpl::BeginSyncL( CNcdSubParser* aSubParser )
    {
    DLTRACEIN((""));
    iSynchronous = ETrue;
    BeginL( aSubParser );
    }


void CNcdParserImpl::BeginAsyncL()
    {
    DLTRACEIN((""));
    BeginAsyncL( NULL );
    }
    
    
void CNcdParserImpl::BeginAsyncL( CNcdSubParser* aSubParser )
    {
    DLTRACEIN((""));
    iSynchronous = EFalse;
    iPleaseFinish = EFalse;
    CleanupStack::PushL( aSubParser );
    delete iFeedBuffer;
    iFeedBuffer = 0;
    iFeedBuffer = HBufC8::NewL( KFeedBufferSize );
    CleanupStack::Pop( aSubParser );
    
    BeginL( aSubParser );
    }
    
    
void CNcdParserImpl::BeginL( CNcdSubParser* aSubParser )
    {
    DLTRACEIN(("begin"));

    DASSERT( iObservers->ParserObserver() );

    iCancelled = EFalse;

    delete iSubParser;
    iSubParser = 0;
    
    if ( aSubParser ) 
        {
        DLTRACE(("Using given subparser"));
        iSubParser = aSubParser;
        }
    else 
        {   
        DLTRACE(("Using toplevel parser"));     
        iSubParser = CNcdTopLevelParser::NewL( 
            *iObservers, *this, 0 /*initial depth=0*/ );
        }

    delete iInputBuffer;
    iInputBuffer = 0;
    iInputBuffer = CBufSeg::NewL( KInputBufferGrowSize );

    delete iXmlParser;
    iXmlParser = 0;

    _LIT8( KXmlType, "text/xml" );
    iXmlParser = Xml::CParser::NewL( KXmlType, *iSubParser );
    iXmlParser->ParseBeginL();
    }

void CNcdParserImpl::EndL()
    {
    DLTRACEIN(("data left=%d",iInputBuffer->Size()));

    if( iSynchronous || ( !IsActive() && iInputBuffer->Size() == 0 ) )
        {
        DLINFO(("sync end"));
        // After ParseEndL() OnError() call may come if the end of the data
        // is screwed.
        iXmlParser->ParseEndL();

        // Cancel callback removed according to users' wishes.
        if ( !iCancelled ) 
            iObservers->ParserObserver()->ParseCompleteL( KErrNone );
        }
    else
        {
        DLINFO(("please finish"));
        iPleaseFinish = ETrue;
        }
    DLTRACEOUT((""));
    }

void CNcdParserImpl::SetOriginL( const TDesC& aOrigin )
    {
    iObservers->SetSessionOriginL( aOrigin );
    }

void CNcdParserImpl::ParseL( const TDesC8& aData )
    {
    DLTRACEIN(("8-bit parse, length=%d",aData.Length()));
    DLINFO(("data=%S",&aData));

    if( iSynchronous )
        {
        iXmlParser->ParseL( aData );
        }
    else
        {
        DLINFO(("inserting, buffer size=%d",iInputBuffer->Size()));
        // Compress to free some memory if possible.
        iInputBuffer->Compress();
        iInputBuffer->InsertL( iInputBuffer->Size(), aData );
        if( ! IsActive() )
            {
            DLINFO(("activating"));
            iStatus = KRequestPending;
            SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            }
        }
    }

void CNcdParserImpl::ParseL( const TDesC16& aData )
    {
    DLTRACEIN(("16-bit parse, length=%d",aData.Length()));
    HBufC8* utf8 = NcdProtocolUtils::ConvertUnicodeToUtf8L( aData );
    CleanupStack::PushL( utf8 );
    ParseL( *utf8 );
    CleanupStack::PopAndDestroy( utf8 );
    }


void CNcdParserImpl::SubParserFinishedL( const TDesC8& /*aTag*/, TInt /*aErrorCode*/ )
    {
    DLTRACE((""));
    }


void CNcdParserImpl::CancelParsing()
    {
    DLTRACE(("active=%d", IsActive() ));
    if( !IsActive() )
        {
        iCancelled = ETrue;
        }
    
    // Prevent another Cancel-call while handling the first one
    if ( !iCancelled ) 
        {
        Cancel();
        }
    }

void CNcdParserImpl::DoCancel()
    {
    DLTRACE(("iCancelled=%d", iCancelled ));

    // iCancelled is never ETrue when coming here unless someone
    // errorneously calls CNcdParserImpl::Cancel() from outside this class
    iCancelled = ETrue;
    TRAP_IGNORE( 
        {
        // this usually causes a parse error callback to the observer
        iXmlParser->ParseEndL();
        } );
    }

TInt CNcdParserImpl::RunError( TInt aError )
    {
    DLERROR(("Fatal parser error %d",aError));
    iObservers->ParserObserver()->ParseError( aError );
    return KErrNone;
    }

void CNcdParserImpl::RunL()
    {
    TRequestStatus* status = &iStatus;
    DLTRACEIN(("data left=%d status=%d",iInputBuffer->Size(),status->Int()));

    if( iCancelled )
        {
        DLINFO(("user cancelled!"));
        return;
        }

    // @ Is it even possible to get != KErrNone here?
    if( status->Int() != KErrNone )
        {
        DLERROR(("RunL() error, canceling!"));
        CancelParsing();
        return;
        }

    if( iInputBuffer->Size() == 0 )
        {
        // All done.
        DLINFO(("all done"));
        if( iPleaseFinish )
            {
            EndL();
            }
        return;
        }

    TPtr8 buf = iFeedBuffer->Des();
    TInt length = buf.MaxSize();
    if( iInputBuffer->Size() < length )
        {
        length = iInputBuffer->Size();
        }
    iInputBuffer->Read( 0, buf, length );
    iInputBuffer->Delete( 0, buf.Length() );
    iXmlParser->ParseL( buf );

    // Next step
    
    // Check again if the parser is cancelled. It is possible since callbacks occur
    // as consequence of calling ParseL.
    if ( iCancelled ) 
        {
        return;
        }
        
    iStatus = KRequestPending;
    SetActive();
    User::RequestComplete( status, KErrNone );

    }



