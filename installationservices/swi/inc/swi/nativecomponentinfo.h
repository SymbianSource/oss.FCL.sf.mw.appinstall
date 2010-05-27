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
* This file defines the component info structure of the Symbian native components.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef NATIVECOMPONENTINFO_H
#define NATIVECOMPONENTINFO_H

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <e32base.h>
#include <s32strm.h>
#include <usif/scr/scr.h>
#include <usif/sif/sifcommon.h>

namespace Swi
	{

	class CNativeComponentInfo : public CBase
		{
	public:
	    
	    class CNativeApplicationInfo : public CBase
	         {
	     public:
	         IMPORT_C static CNativeApplicationInfo* NewLC(const TUid& aAppUid, const TDesC& aName, const TDesC& aGroupName, const TDesC& aIconFileName);
	         static CNativeApplicationInfo* NewL(RReadStream& aStream);
	         void ExternalizeL(RWriteStream& aStream) const;
	                             
	         IMPORT_C const TUid& AppUid() const;
	         IMPORT_C const TDesC& Name() const;
	         IMPORT_C const TDesC& GroupName() const;
	         IMPORT_C const TDesC& IconFileName() const;            	         
	         IMPORT_C virtual ~CNativeApplicationInfo();	            
	     private:                  
	         CNativeApplicationInfo();
	                 
	         TUid iAppUid; // Application UID
	         HBufC* iName; // Name of the application
	         HBufC* iGroupName; // Group folder name where the application will be present
	         HBufC* iIconFileName; // Full path of the icon file
	         };
	    
		/**
			Creates a new instance of the CNativeComponentInfo class.
			@leave System wide error code
		 */
		IMPORT_C static CNativeComponentInfo* NewL();

		/**
			Creates a new instance of the CNativeComponentInfo class.
			@leave System wide error code
		 */
		IMPORT_C static CNativeComponentInfo* NewLC();
		
		/**
			Destructor.
		*/
		IMPORT_C virtual ~CNativeComponentInfo();
		
		/**
		* Internalizes the object from the given read stream.
		*/
		IMPORT_C void InternalizeL(RReadStream& aStream);
		
		/**
		* Externalizes the object to the given write stream.
		*/		
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	private:
		// Constructor
		CNativeComponentInfo();

	public:		
		HBufC* iComponentName;							///< The name of the software component
		HBufC* iVersion;								///< The Version of the software component 
		HBufC* iVendor;									///< The of the vendor of the software component
		HBufC* iGlobalComponentId;						///< The global id of the component if already installed
		Usif::TScomoState iScomoState;					///< The SCOMO status of the component
		Usif::TInstallStatus iInstallStatus;			///< Informs whether the component is already installed or upgradeable
		Usif::TComponentId iComponentId;				///< The unique id of the component if already installed
		Usif::TAuthenticity iAuthenticity;				///< The authenticity of the component
		TCapabilitySet iUserGrantableCaps;				///< User grantable capabilities required be the component
		TInt iMaxInstalledSize;							///< The maximum size of the component after installation
		TBool iHasExe;									///<Indicates Whether the component has an exe or not
		TBool iIsDriveSelectionRequired;                ///< Indicates Whether drive selection is required or not.
		RCPointerArray<CNativeApplicationInfo> iApplications; ///< The array of application info of the component being queried
		RCPointerArray<CNativeComponentInfo> iChildren;	///< The children component info, incase of embedded packages.	
	
		enum
		    {
		    // An arbitrary limit for the length of a single descriptor, for example aComponentName, aVersion or aVendor.
		    KMaxDescriptorLength = 256,
		    };
		
		};	
	} // nameSpace Swi
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#endif // NATIVECOMPONENTINFO_H
