/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* swipubsubdefs.h
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __SWIPUBSUBDEFS_H__
#define __SWIPUBSUBDEFS_H__

#include <e32property.h>
#include <sacls.h>
#include <saclscommon.h>

namespace Swi
{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
	/**
	 * @publishedPartner
	 * @released
	 * 
	 * The unique identifier number which identifies the property category of Native Software Install Server. 
	 */
	const TUid KUidInstallServerCategory = {0x101F7295};
	
	/**
	 * @publishedPartner
	 * @released
	 *      
	 * This key defines a property that provides the percentage value of the progress of the native software installer.
	 * The category for that property is @see Swi::KUidInstallServerCategory
	 */
	const TUint KUidSwiProgressBarValueKey = 0x102866F4;

	/**
	 * @publishedPartner
	 * @released	
	 */
	const TUint KSwisSafeModeUninstallEnabled = 1;
	
	
	/**
	 * @publishedPartner
	 * @released
	 *      
	 * This key defines a property that tells the installer not to run Run-On-Uninstall executables. 
	 * If the property is defined with a value @see KSwisSafeModeUninstallEnabled, Run-On-Uninstall executables won't be executed during uninstall.
	 * The category for that property is @see Swi::KUidInstallServerCategory.
	 */
	const TUint KUidSafeModeUninstallKey = 0x102866F5;	
#endif	
	/**
     * @publishedPartner
     * @released
     *
     * This key and values that follow are now aliased to those defined in the 
     * header base header SaCls.h. This is necessary so Core OS components
     * can still be dependent on software install.
     * DO NOT change the assigned values here, update sacls.h for aliased
     * definitions.        
     */
	const TUint KUidSoftwareInstallKey = KSAUidSoftwareInstallKeyValue; 
	
	/**
     * @publishedPartner
     * @released
     *      
 	 * Software install operation mask - 0xFF.
     */
	const TUint KSwisOperationMask = KSASwisOperationMask;

	/**
     * @publishedPartner
     * @released
     * 
     * Software install operations states.          
     */
	enum TSwisOperation
		{
		
		ESwisNone		= ESASwisNone,      ///< 0x00 No operation
		ESwisInstall	= ESASwisInstall,   ///< 0x01 Swis install operation is in progress
		ESwisUninstall	= ESASwisUninstall, ///< 0x02 Swis uninstall operation is in progress
		ESwisRestore	= ESASwisRestore    ///< 0x04 Swis restore operation is in progress
		
		};
		
	/**
     * @publishedPartner
     * @released
     * 
     * Software install operation status mask - 0xFF00.          
     */
	const TUint KSwisOperationStatusMask = KSASwisOperationStatusMask;
	
	/**
     * @publishedPartner
     * @released
     * 
     * Software Install operation status value.          
     */
	enum TSwisOperationStatus
		{
		
		ESwisStatusNone		= ESASwisStatusNone,    ///< 0x0000 The current operation is in progress
		ESwisStatusSuccess	= ESASwisStatusSuccess, ///< 0x0100 The current/last operation succeeded
		ESwisStatusAborted	= ESASwisStatusAborted  ///< 0x0200 The current/last operation failed
		
		};	

}

// Java Install definitions

/**
 * @publishedPartner
 * @released
 *
 * This key is now aliased to KSAUidJavaInstallKeyValue defined in
 * SaCls.h.
 *
 */

const TUint KUidJavaInstallKey = KSAUidJavaInstallKeyValue;

/**
 * @publishedPartner
 * @released
 */

const TUint KJavaOperationMask = 0x00FF0000;

/**
 * @publishedPartner
 * @released
 */

const TUint KJavaStatusMask = 0x0000FF00;

/**
 * @publishedPartner
 * @released
 */

enum TJavaOperation
	{
	EJavaNone			= 0x00000000, /* No Java operation */
	EJavaInstall		= 0x00010000, /* Java install operation is in progress */
	EJavaUninstall		= 0x00020000  /* Java uninstall operation is in progress */
	};

/**
 * @publishedPartner
 * @released
 */

enum TJavaOperationStatus
	{
	EJavaStatusNone			= 0x00000000, /* No operation type set */
	EJavaStatusSuccess		= 0x00000100, /* An install operation is in progress */
	EJavaStatusAborted		= 0x00000200  /* An uninstall operation is in progress */
	};


#endif /*__SWIPUBSUBDEFS_H__*/

