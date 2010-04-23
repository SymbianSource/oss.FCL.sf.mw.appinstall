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
* Description:   ?Description
*
*/


#include "ncd_cp_detailimpl.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"

CNcdConfigurationProtocolContentImpl* CNcdConfigurationProtocolContentImpl::NewL()
    {
    CNcdConfigurationProtocolContentImpl* self = new(ELeave) 
        CNcdConfigurationProtocolContentImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolContentImpl* CNcdConfigurationProtocolContentImpl::NewLC()
    {
    CNcdConfigurationProtocolContentImpl* self = 
        new (ELeave) CNcdConfigurationProtocolContentImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

                                             
void CNcdConfigurationProtocolContentImpl::ConstructL()
    {
    DLTRACEIN((""));
    NcdProtocolUtils::AssignEmptyDesL( iKey );
    NcdProtocolUtils::AssignEmptyDesL( iValue );
    NcdProtocolUtils::AssignEmptyDesL( iContent );
    DLTRACEOUT((""));
    }

CNcdConfigurationProtocolContentImpl::~CNcdConfigurationProtocolContentImpl()
    {
    DLTRACEIN((""));
    delete iKey;
    delete iValue;
    delete iContent;
    DLTRACEOUT((""));
    }

const TDesC& CNcdConfigurationProtocolContentImpl::Key() const
    {
    return *iKey;
    }

const TDesC& CNcdConfigurationProtocolContentImpl::Value() const
    {
    return *iValue;
    }

const TDesC& CNcdConfigurationProtocolContentImpl::Content() const
    {
    return *iContent;
    }

CNcdConfigurationProtocolDetailImpl* CNcdConfigurationProtocolDetailImpl::NewL()
    {
    CNcdConfigurationProtocolDetailImpl* self = new(ELeave) 
        CNcdConfigurationProtocolDetailImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolDetailImpl* CNcdConfigurationProtocolDetailImpl::NewLC()
    {
    CNcdConfigurationProtocolDetailImpl* self = 
        new (ELeave) CNcdConfigurationProtocolDetailImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

                                             
void CNcdConfigurationProtocolDetailImpl::ConstructL()
    {
    DLTRACEIN((""));
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iValue );
    NcdProtocolUtils::AssignEmptyDesL( iGroupId );
    NcdProtocolUtils::AssignEmptyDesL( iLabel );
    DLTRACEOUT((""));
    }

CNcdConfigurationProtocolDetailImpl::~CNcdConfigurationProtocolDetailImpl()
    {
    DLTRACEIN((""));
    delete iId;
    delete iValue;
    delete iGroupId;
    delete iLabel;
    iDetails.ResetAndDestroy();
    iContents.ResetAndDestroy();
    DLTRACEOUT((""));
    }

const TDesC& CNcdConfigurationProtocolDetailImpl::Id() const
    {
    return *iId;
    }

const TDesC& CNcdConfigurationProtocolDetailImpl::Value() const
    {
    return *iValue;
    }

const TDesC& CNcdConfigurationProtocolDetailImpl::GroupId() const
    {
    return *iGroupId;
    }

const TDesC& CNcdConfigurationProtocolDetailImpl::Label() const
{
    return *iLabel;
}

const RPointerArray<MNcdConfigurationProtocolDetail>& CNcdConfigurationProtocolDetailImpl::Details() const
{
    return iDetails;
}

const RPointerArray<MNcdConfigurationProtocolContent>& CNcdConfigurationProtocolDetailImpl::Contents() const
{
    return iContents;
}
