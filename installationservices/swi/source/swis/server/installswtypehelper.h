/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains a set of functions used by the installation and restore processors.
* These functions simplify registration of a Layered Execution Environment.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef INSTALLSWTYPEHELPER_H
#define INSTALLSWTYPEHELPER_H

#include <e32std.h>
#include <usif/scr/screntries_platform.h>

namespace Swi 
	{
	class CSoftwareTypeRegInfo;
	
	namespace InstallSoftwareTypeHelper
		{
		/**
		Indicates if a file is an XML registration file of a software type.
		@param aFileName The file to be examined.
		@param The uid of a component being installed.
		@return ETrue if the files has been recognized as a registration file, EFalse otherwise.
		*/
		TBool IsValidSwRegFileL(const TDesC& aFileName, TInt32 aAppUid);

		/**
		Parses an XML registration file of a software type.
		@param aFs The handle to the file server session
		@param aFileName The file name of an XML file to be parsed.
		@param aInfoArray The output array of registration info objects read from the aFileName.
		*/
		void ParseRegFileL(RFs& aFs, const TDesC& aFileName, RPointerArray<Usif::CSoftwareTypeRegInfo>& aInfoArray);

		/**
		Registers software type MIME types to the AppArc. Each Layered Execution Environment must register its
		MIME types so the SifLauncher may handle installation requests.
		@param aInfoArray The array of registration info objects describing new software types to be registered.
		*/
		void RegisterMimeTypesL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aInfoArray);
		}
	}

#endif // INSTALLSWTYPEHELPER_H
