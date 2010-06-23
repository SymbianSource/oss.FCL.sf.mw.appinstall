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
* Description:  
*
*/


#ifndef CATALOGS_SERVER_DEFINES
#define CATALOGS_SERVER_DEFINES

#include <e32cmn.h>

// Replaced with USE_BUILD_SCRIPT when using build script
#define DUMMY_DEFINE


#ifdef USE_BUILD_SCRIPT
// Server name
_LIT( KCatalogsServerName, "NCD_APP_NAME_POSTFIX" );
//Semaphore that is used when creating a new server
_LIT( KCatalogsServerSemaphoreName, "NCDSemaphore_APP_NAME_POSTFIX" );
// File that is passed to the new server-process for execution
_LIT( KCatalogsServerFilename, "ncdserver_APP_NAME_POSTFIX" );
#else
// Server name
_LIT( KCatalogsServerName, "NCD_20019119" );
//Semaphore that is used when creating a new server
_LIT( KCatalogsServerSemaphoreName, "NCDSemaphore_20019119" );
// File that is passed to the new server-process for execution
_LIT( KCatalogsServerFilename, "ncdserver_20019119" );
#endif // USE_BUILD_SCRIPT



// Server version number, which is used when creating a new session
const TInt KCatalogsServerMajorVersionNumber( 0 );
const TInt KCatalogsServerMinorVersionNumber( 0 );
const TInt KCatalogsServerBuildVersionNumber( 1 );

// Default message slot amount in a session
const TInt KCatalogsDefaultMessageSlots( 255 );

// Minimum return message size when using return message allocation
const TInt KCatalogsMinimumAllocSize( 65 );
// And the length of it
const TInt KCatalogsMinimumAllocLength( 65 );
// Character that divides elements of a message received that
// informs that given return descriptor is too small.
// (When using return message allocation)
const TText KCatalogsTooSmallDescMsgElementDivider( '|' );
// Error code to inform when using return message allocation
// that the allocated descriptor was too small
const TInt KCatalogsErrorTooSmallDescriptor( 50 );


// Enumeration to identify different types of messages. Used
// internally in ClientServer
enum TCatalogsServerFunction
    {
    // Types of internal messages in ClientServer
    ECatalogsCreateContext,
    ECatalogsCompleteMessage,
    ECatalogsCompleteMessageWide,
    ECatalogsRemoveIncompleteMessage,
    ECatalogsCreateProvider,
    ECatalogsClientSideDown,
    // Types of external messages in ClientServer (For example a message
    // that is conveyed between a nodeproxy and a corresponding node)
    ECatalogsExternalMessage,
    ECatalogsExternalAllocMessage
    };

#endif // CATALOGS_SERVER_DEFINES

