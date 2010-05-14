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
* Description:   CNcdUnknownParser implementation
*
*/


#include <e32debug.h>
#include "ncdunknownparser.h"
#include "catalogsdebug.h"

CNcdUnknownParser* CNcdUnknownParser::NewL( MNcdParserObserverBundle& aObservers,
                                            MNcdSubParserObserver& aSubParserObserver,
                                            TInt aDepth,
                                            const Xml::RTagInfo& aElement, 
                                            const Xml::RAttributeArray& aAttributes )
    {
    DLTRACE(("depth=%d, tag=%S",aDepth,&aElement.LocalName().DesC()));
    CNcdUnknownParser* self = new(ELeave) CNcdUnknownParser( aObservers,
                                                             aSubParserObserver,
                                                             aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdUnknownParser::CNcdUnknownParser( MNcdParserObserverBundle& aObservers,
                                      MNcdSubParserObserver& aSubParserObserver,
                                      TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth, EParserUnknown ),
      iInitialDepth( aDepth )
    {
    DLTRACEIN((""));
    DLINFO(("initial depth=%d",iInitialDepth));
    }

CNcdUnknownParser::~CNcdUnknownParser()
    {
    DLTRACEIN((""));
    }

void CNcdUnknownParser::ConstructL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );
    }


void CNcdUnknownParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                         const Xml::RAttributeArray& aAttributes, 
                                         TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    DLTRACEIN(("unknown start tag=%S depth=%d error=%d",&aElement.LocalName().DesC(),iDepth,aErrorCode));
    
    iDepth++;
    // Ignore everything
    }

void CNcdUnknownParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN(("unknown end tag=%S depth=%d error=%d",&aElement.LocalName().DesC(),iDepth,aErrorCode));
    // Ignore everything
    if( iDepth == iInitialDepth && aElement.LocalName().DesC() == Tag() )
        {
        DLINFO(("unknown section ended, tag=%S",iTag));
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
        }
    else
        {
        iDepth--;
        }
    }

void CNcdUnknownParser::OnContentL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/) 
    {
    // Ignore.
    DLTRACEIN((""));
    }

