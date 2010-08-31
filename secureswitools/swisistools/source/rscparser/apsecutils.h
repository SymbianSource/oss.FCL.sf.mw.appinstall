// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// apsecutils.h
//

/** 
* @file apsecutils.h
*
* @internalComponent
* @released
*/

#ifndef __APSECUTILS_H__
#define __APSECUTILS_H__

#include "commontypes.h"
/** AppArc security utils.

This is a set of security related methods for apparc internal usage.

*/
class CApaSecurityUtils
    {
public:
    static TInt CheckAppSecurity( const Ptr16& aAppFilename, 
                                           TBool& aHasWriteDeviceDataCap, 
                                           TBool& aIsSidProtected,
                                           const std::string& aDerivedPath);

	static TInt GetInfo(std::string& aFilename);

private:
    inline static TBool IsSidProtected( const TUint32 aSid );
	inline static TBool HasWriteDeviceDataCap( const TUint32 aCapability );

private:
	static TUint32 iCapabilities;
	static TUint32 iSecureID;
    };
    
#endif // __APSECUTILS_H__
