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
* Description:   Utility functions for DRM file handling
*
*/


#include "appmngr2drmutils.h"           // TAppMngr2DRMUtils
#include "appmngr2debugutils.h"         // FLOG macros
#include <caf/content.h>                // ContentAccess::CContent
#include <DRMHelper.h>                  // CDRMHelper


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TAppMngr2DRMUtils::IsDRMProtected()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAppMngr2DRMUtils::IsDRMProtected( const TDesC& aFileName )
    {
    FLOG( "TAppMngr2DRMUtils::IsDRMProtected( %S )", &aFileName );
    TInt isProtected = EFalse;

    ContentAccess::CContent* content = NULL;
    TRAPD( err, content = ContentAccess::CContent::NewL( aFileName ) );
    FLOG( "TAppMngr2DRMUtils::IsDRMProtected(), CAF err %d", err );
    if( err == KErrNone )
        {
        content->GetAttribute( ContentAccess::EIsProtected, isProtected );
        delete content;
        }
    
    FLOG( "TAppMngr2DRMUtils::IsDRMProtected() returns %d", isProtected );
    return isProtected;
	}

// ---------------------------------------------------------------------------
// TAppMngr2DRMUtils::IsDRMForwardable()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAppMngr2DRMUtils::IsDRMForwardable( const TDesC& aFileName )
    {
    FLOG( "TAppMngr2DRMUtils::IsDRMForwardable( %S )", &aFileName );
	TInt isForwardable = ETrue;

    ContentAccess::CContent* content = NULL;
    TRAPD( err, content = ContentAccess::CContent::NewL( aFileName ) );
    FLOG( "TAppMngr2DRMUtils::IsDRMForwardable(), CAF err %d", err );
    if( err == KErrNone )
        {
        content->GetAttribute( ContentAccess::EIsForwardable, isForwardable );
        delete content;
        }

    FLOG( "TAppMngr2DRMUtils::IsDRMForwardable() returns %d", isForwardable );
	return isForwardable;
	}

// ---------------------------------------------------------------------------
// TAppMngr2DRMUtils::IsDRMRightsObjectExpiredOrMissingL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAppMngr2DRMUtils::IsDRMRightsObjectExpiredOrMissingL( const TDesC& aFileName )
    {
    FLOG( "TAppMngr2DRMUtils::IsDRMRightsObjectExpiredOrMissingL( %S )", &aFileName );
	TInt err = KErrNone;
	
    ContentAccess::CContent* content = ContentAccess::CContent::NewLC( aFileName );
  	content->GetAttribute( ContentAccess::ECanExecute, err );
    CleanupStack::PopAndDestroy( content );

    TBool isRightsObjectExpiredOrMissing = EFalse;
    if( err == KErrCANoRights || err == KErrCANoPermission )
        {
        isRightsObjectExpiredOrMissing = ETrue;
        }
    FLOG( "TAppMngr2DRMUtils::IsDRMRightsObjectExpiredOrMissingL() returns %d", 
            isRightsObjectExpiredOrMissing );
    return isRightsObjectExpiredOrMissing;
	}

// ---------------------------------------------------------------------------
// TAppMngr2DRMUtils::IsDRMRightsObjectMissingL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAppMngr2DRMUtils::IsDRMRightsObjectMissingL( const TDesC& aFileName )
    {
    FLOG( "TAppMngr2DRMUtils::IsDRMRightsObjectMissingL( %S )", &aFileName );
    TUint32 intent = 0;
    TBool isExpired = EFalse;
    TBool isSendingAllowed = EFalse;

    CDRMHelperRightsConstraints* rightsPlay = NULL;
    CDRMHelperRightsConstraints* rightsDisplay = NULL; 
    CDRMHelperRightsConstraints* rightsExecute = NULL; 
    CDRMHelperRightsConstraints* rightsPrint = NULL;

    CDRMHelper* drm = CDRMHelper::NewLC( *CCoeEnv::Static() );
    TRAPD( err, drm->GetRightsDetailsL( aFileName, intent, isExpired, isSendingAllowed,
            rightsPlay, rightsDisplay, rightsExecute, rightsPrint ));
    FLOG( "TAppMngr2DRMUtils::IsDRMRightsObjectMissingL(), GetRightsDetailsL err %d", err );
    CleanupStack::PopAndDestroy( drm );
    
    delete rightsPlay;
    delete rightsDisplay;
    delete rightsExecute;
    delete rightsPrint;
    
    TBool isRightsObjectMissing = EFalse;
    if( err == KErrCANoRights )
        {
        isRightsObjectMissing = ETrue;
        }
    FLOG( "TAppMngr2DRMUtils::IsDRMRightsObjectMissingL() returns %d", isRightsObjectMissing );
    return isRightsObjectMissing;
    }

