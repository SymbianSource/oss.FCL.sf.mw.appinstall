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
* NOTE: This file is common to PC side (MakeSIS etc.) and device side (SW Install) code
*
*/

/**
 @file
 @internalTechnology
 */
 
#ifndef __INSTALLTYPES_H__
#define __INSTALLTYPES_H__

namespace Swi
{
	namespace Sis
	{

		/** 
		@publishedPartner
		@released
		
		These are the five possible install types for a SIS controller inside a SISx file
		
		The EInstPreInstalledApp and EInstPreInstalledPatch both just reference files already present on the device. They
		contain no files in their SisData field. It simply contains an empty (ie. zero elements) SisArray<SisDataUnit> object.
		SIS files with no data units are known as "stub" SIS files. During installation or uninstallation of these 
		types no files are added or removed. These types would be used for adding applications to a device before
		sale, ie. at the factory.
		
		There is also another type of SIS stub file generated dynamically during software install. When SWI installs 
		a SIS file containing only EInstInstallation or EInstAugmentation SIS controllers on a removeable
		media device it will also create a SIS stub file. This allows the media to be inserted into another
		device and the application will be automatically installed. This behavior can be turned off using the SWI policy 
		AllowPackagePropagate. 
		
		Removable media stub SIS files will not be generated for EInstPartialUpgrade, EInstPreInstalledApp or 
		EInstPreInstalledPatch types. They will also not be generated if any files including those in embedded packages 
		are installed on different drives to the main package. The removable media SIS stub is the same as the original
		SIS file except the data has been removed. 
		
		Removable media SIS stubs with type EInstInstallation or EInstAugmentation will be treated somewhat like 
		EInstPreInstalledApp and EInstPreInstalledPatch respectively during installation. The controllers and file hashes
		will be verified but files will not be moved. An uninstallation of the stub will result in all files and the 
		SIS stub itself being removed.
		*/
		enum TInstallType
		{
			EInstInstallation,			///< The application is a complete installation (may result in an upgrade)
			EInstAugmentation,			///< The application is an addition to an exisitng application (eg. extra game levels)
			EInstPartialUpgrade,		///< The application is a partial upgrade, replaces some but not all of the files in an exsiting application
			EInstPreInstalledApp,		///< The application files are already in place, this SIS controller just allows SWI verify and register the application. No files are moved and no files are present in the SIS data field
			EInstPreInstalledPatch,		///< Contains an addition to a preinstalled application. The extra files are already in place, this SIS controller just allows SWI verify and register the application. No files are moved and no files are present in the SIS data field
		};

		enum TInstallFlags
		{
			// Considered to be of 8 bit length, and bitwise OR-ed to give more than one option.
			// So the values must be of 2 power n, where n => 0 to 7.
			EInstFlagShutdownApps 	= 1<<0,
			EInstFlagNonRemovable 	= 1<<1,
			EInstFlagROMUpgrade   	= 1<<2,
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			EInstFlagHide			= 1<<3
			#endif
		};
	} 
}

#endif
