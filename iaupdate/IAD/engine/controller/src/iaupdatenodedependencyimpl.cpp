/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "iaupdatenodedependencyimpl.h"
#include "iaupdatenodeimpl.h"


CIAUpdateNodeDependency* CIAUpdateNodeDependency::NewLC()
    {
    CIAUpdateNodeDependency *self = 
        new( ELeave ) CIAUpdateNodeDependency();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
CIAUpdateNodeDependency* CIAUpdateNodeDependency::NewL()
    {
    CIAUpdateNodeDependency *self = 
        CIAUpdateNodeDependency::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
    
CIAUpdateNodeDependency::CIAUpdateNodeDependency() 
: CBase()
    {
    Reset();
    }    
    

void CIAUpdateNodeDependency::ConstructL()
    {
    }


CIAUpdateNodeDependency::~CIAUpdateNodeDependency()
    {
    }    
     
    
const TIAUpdateVersion& CIAUpdateNodeDependency::VersionFloor() const
    {
    return iVersionFloor;
    }


void CIAUpdateNodeDependency::SetVersionFloor( 
    const TIAUpdateVersion& aVersion )
    {
    iVersionFloor = aVersion;
    }

    
const TIAUpdateVersion& CIAUpdateNodeDependency::VersionRoof() const
    {
    return iVersionRoof;
    }


void CIAUpdateNodeDependency::SetVersionRoof( 
    const TIAUpdateVersion& aVersion )
    {
    iVersionRoof = aVersion;
    }


TBool CIAUpdateNodeDependency::IsEmbedded() const
    {
    return iEmbedded;
    }


void CIAUpdateNodeDependency::SetEmbedded( 
    TBool aEmbedded )
    {
    iEmbedded = aEmbedded;
    }

    
CIAUpdateNode* CIAUpdateNodeDependency::BestMatch() const
    {
    return iBestMatch;
    }


void CIAUpdateNodeDependency::SetBestMatch( 
    CIAUpdateNode* aBestMatch )
    {
    iBestMatch = aBestMatch;
    }


const TUid& CIAUpdateNodeDependency::Uid() const
    {
    return iUid;
    }


void CIAUpdateNodeDependency::SetUid( 
    const TUid& aUid )
    {
    iUid = aUid;
    }


void CIAUpdateNodeDependency::Reset()
    {
    iUid = TUid::Null();
    iVersionFloor.SetToFloor();
    iVersionRoof.SetToRoof();    
    iEmbedded = EFalse;
    iBestMatch = NULL;
    }
