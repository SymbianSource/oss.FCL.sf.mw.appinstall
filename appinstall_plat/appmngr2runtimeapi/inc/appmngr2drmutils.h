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
* Description:   Utility definitions for DRM file handling
*
*/


#ifndef C_APPMNGR2DRMUTILS_H
#define C_APPMNGR2DRMUTILS_H

#include <e32std.h>                     // basic definitions

/**
 * Utility functions to check the DRM status of a given file. 
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class TAppMngr2DRMUtils
	{
public:     // new functions
    IMPORT_C static TBool IsDRMProtected( const TDesC& aFileName );
    IMPORT_C static TBool IsDRMForwardable( const TDesC& aFileName );
    IMPORT_C static TBool IsDRMRightsObjectExpiredOrMissingL( const TDesC& aFileName );
    IMPORT_C static TBool IsDRMRightsObjectMissingL( const TDesC& aFileName );
	};

#endif  // C_APPMNGR2DRMUTILS_H

