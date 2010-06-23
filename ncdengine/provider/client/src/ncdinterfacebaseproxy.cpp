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
* Description:   Contains CNcdInterfaceBaseProxy class implementation
*
*/


#include "ncdinterfacebaseproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"
#include "catalogsdebug.h"


// ======== PROTECTED MEMBER FUNCTIONS ========

CNcdInterfaceBaseProxy::CNcdInterfaceBaseProxy( MCatalogsClientServer& aSession,
                                                TInt aHandle,
                                                CCatalogsInterfaceBase* aParent ) 
: CCatalogsInterfaceBase( aParent ),
  iSession( aSession ),
  iHandle( aHandle ),
  iReleaseId( NcdNodeFunctionIds::ENcdRelease )
    {
    DLTRACEIN(("this-ptr: %x", this));    
    }


CNcdInterfaceBaseProxy::~CNcdInterfaceBaseProxy()
    {
    DLTRACEIN(("this-ptr: %x", this));
    TInt tmpNum( 0 );
    ClientServerSession().
        SendSync( iReleaseId,
                  KNullDesC,
                  tmpNum,
                  Handle() ); 
    DLTRACEOUT(("this-ptr: %x", this));
    }

  
MCatalogsClientServer& CNcdInterfaceBaseProxy::ClientServerSession() const
    {
    DLTRACEIN(("this-ptr: %x", this));
    return iSession;
    }    
    
TInt CNcdInterfaceBaseProxy::Handle() const
    {
    DLTRACEIN(("this-ptr: %x, Handle: %i", this, iHandle));
    return iHandle;
    }


void CNcdInterfaceBaseProxy::SetReleaseId( TInt aId )
    {
    iReleaseId = aId;
    }
