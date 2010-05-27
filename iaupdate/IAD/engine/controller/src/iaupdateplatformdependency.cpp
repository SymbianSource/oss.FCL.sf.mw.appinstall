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



// For the system version info
#include <versioninfo.h>

#include "iaupdateplatformdependency.h"
#include "iaupdateversion.h"
#include "iaupdateprotocolconsts.h"

#include "iaupdatedebug.h"


CIAUpdatePlatformDependency* CIAUpdatePlatformDependency::NewL()
    {
    CIAUpdatePlatformDependency* self =
        CIAUpdatePlatformDependency::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
    
CIAUpdatePlatformDependency* CIAUpdatePlatformDependency::NewLC()
    {
    CIAUpdatePlatformDependency* self =
        new( ELeave ) CIAUpdatePlatformDependency();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdatePlatformDependency::CIAUpdatePlatformDependency()
: CBase()
    {
    Reset();
    }

void CIAUpdatePlatformDependency::ConstructL()
    {
    // Nothing to do here.
    }


CIAUpdatePlatformDependency::~CIAUpdatePlatformDependency()
    {
    delete iPlatform;
    }


TBool CIAUpdatePlatformDependency::AcceptablePlatformL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePlatformDependency::AcceptablePlatformL() begin");
    
    // Get the platform version
    VersionInfo::TPlatformVersion platformVersion;
    User::LeaveIfError( VersionInfo::GetVersion( platformVersion ) );

    IAUPDATE_TRACE_2("Platform version major: %d, minor: %d",
                     platformVersion.iMajorVersion,
                     platformVersion.iMinorVersion);
    IAUPDATE_TRACE_2("Node version floor major: %d, minor: %d",
                     iVersionFloor.iMajor,
                     iVersionFloor.iMinor);
    IAUPDATE_TRACE_2("Node version roof major: %d, minor: %d",
                     iVersionRoof.iMajor,
                     iVersionRoof.iMinor);
                                          
    // Create TIAUpdateVersion for comparing purposes. 
    // Notice, that platform version does not have build value.
    TVersion tmpVersion( platformVersion.iMajorVersion, platformVersion.iMinorVersion, 0 );
    TIAUpdateVersion iaPlatformVersion( tmpVersion );

    // Create temporary version infos.
    // Notice, here we ignore build values, because platform version
    // does not support them. So, set build to extreame values.
    TIAUpdateVersion floor;
    floor.SetToFloor();
    floor.iMajor = iVersionFloor.iMajor;
    floor.iMinor = iVersionFloor.iMinor;
    TIAUpdateVersion roof;
    roof.SetToRoof();
    roof.iMajor = iVersionRoof.iMajor;
    roof.iMinor = iVersionRoof.iMinor;
    
    // Now compare the version ranges.
    TBool correctVersion(  
        floor <= iaPlatformVersion && iaPlatformVersion <= roof );
   
    // Check if the platform description and versions are acceptable.
    if ( ( !iPlatform 
           || iPlatform->CompareF( IAUpdateProtocolConsts::KPlatformDescriptionValue() ) == 0 )
         && correctVersion )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePlatformDependency::AcceptablePlatformL() end: Acceptable");
        return ETrue;
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePlatformDependency::AcceptablePlatformL() end: Not Acceptable");
        return EFalse; 
        }
    }
    
    
void CIAUpdatePlatformDependency::SetPlatformL( const TDesC8& aPlatform )
    {
    HBufC8* tmp( aPlatform.AllocL() );
    delete iPlatform;
    iPlatform = tmp;
    }
    

const TIAUpdateVersion& CIAUpdatePlatformDependency::VersionFloor() const
    {
    return iVersionFloor;
    }
    
    
void CIAUpdatePlatformDependency::SetVersionFloor( const TVersion& aVersion )
    {
    iVersionFloor = aVersion;
    }
    
    
const TIAUpdateVersion& CIAUpdatePlatformDependency::VersionRoof() const
    {
    return iVersionRoof;
    }
    
    
void CIAUpdatePlatformDependency::SetVersionRoof( const TVersion& aVersion )
    {
    iVersionRoof = aVersion;    
    }
    
    
void CIAUpdatePlatformDependency::Reset()
    {
    iVersionFloor.SetToFloor();
    iVersionRoof.SetToRoof();

    delete iPlatform;
    iPlatform = NULL;    
    }
