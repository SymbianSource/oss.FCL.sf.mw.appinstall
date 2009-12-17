/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Declares TAppInfo class which holds information about an application
*
*/


/**
 @file
 @internalComponent
 library uiss.lib
*/
 
#ifndef __APPINFO_H__
#define __APPINFO_H__

#include <e32std.h>
#include <s32strm.h>

namespace Swi
{
	
/** 
 * Utility class to encapsulate information about an application's
 * name, vendor and version.
 *
 * @internalComponent
 * @released
 */
class TAppInfo
{
public:
	/** 
	 * Constructor, ownership is not taken of aAppName, aAppVendor
	 */
	IMPORT_C TAppInfo(const TDesC& aAppName, const TDesC& aAppVendor,
			 const TVersion& aAppVersion);

	IMPORT_C TAppInfo(const TAppInfo& aAppInfo);
	
	IMPORT_C TAppInfo();
	
	/**
	 * Getter method for the application name.
	 *
	 * @return The application name.
	 */
	inline const TDesC& AppName() const;

	/**
	 * Getter method for the application vendor.
	 * @return The application vendor.
	 */
	inline const TDesC& AppVendor() const;

	/**
	 * Getter method for the application version.
	 * @return The application version.
	 */
	inline const TVersion& AppVersion() const;

	/** 
	 * Externalizes this object to the specified write stream.
	 * 
	 * @param aStream    Stream to which the contents should be externalized.
 	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
	const TDesC* iAppName;			///< Application name
	const TDesC* iAppVendor;		///< Application vendor
	const TVersion iAppVersion;	///< Application version
};

} // namespace Swi

#endif // __APPINFO_H__
