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
* Description:   Contains CNcdBaseProxy class implementation
*
*/


#include "ncdbaseproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"
#include "catalogsdebug.h"


// ======== PROTECTED MEMBER FUNCTIONS ========

CNcdBaseProxy::CNcdBaseProxy( MCatalogsClientServer& aSession,
                              TInt aHandle ) 
: CBase(),
  iSession( aSession ),
  iHandle( aHandle )
    {
    DLTRACEIN((""));
    }


CNcdBaseProxy::~CNcdBaseProxy()
    {
    DLTRACEIN((""));
    TInt tmpNum( 0 );
    ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdRelease,
                  KNullDesC,
                  tmpNum,
                  Handle() ); 
    DLTRACEOUT((""));
    }

  
MCatalogsClientServer& CNcdBaseProxy::ClientServerSession() const
    {
    DLTRACEIN((""));
    return iSession;
    }    
    
TInt CNcdBaseProxy::Handle() const
    {
    DLTRACEIN(("Handle: %i", iHandle));
    return iHandle;
    }

