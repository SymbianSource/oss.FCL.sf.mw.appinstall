/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Information shared between the Software Component Registry client and server implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SCRCOMMON_H
#define SCRCOMMON_H
 
#include <e32ver.h>
#include <e32uid.h>
#include <f32file.h>
#include <e32std.h>
#include <s32file.h>

namespace Usif
	{
	/** Identifies the Software Component Registry Symbian OS Server. */
	_LIT(KSoftwareComponentRegistryName, "!SoftwareComponentRegistry");	

	const TInt KScrVerMajor = 1;		///< SCR Server version major component.
	const TInt KScrVerMinor = 0;		///< SCR Server version minor component.
	const TInt KScrVerBuild = 0;		///< SCR Server version build component.

	inline TVersion ScrServerVersion();
		
	/**
		Executable which hosts SCR Server.  Required to launch the process.

		@see KUidScrServer
	*/
	_LIT(KScrServerImageName, "scrserver.exe");
		
	/**
		Hosting the executable's secure ID.  Required to launch the process.

		@see KScrServerImageName
	 */
	const TUid KUidScrServer = {0x10285BC0};
		
	inline TUidType ScrServerImageFullUid();
	 	
	enum TScrSessionFunction
	/**
		Functions supported by a SCR server session.
	 */
		{
		ECreateTransaction        	 	 = 0x00,	///< Create a new transaction
		ERollbackTransaction       	 	 = 0x01,	///< Roll back the transaction in progress
		ECommitTransaction           	 = 0x02,	///< Commit the transaction in progress
		EAddComponent	         	 	 = 0x03,	///< Add a new component record into the SCR database 
		EAddComponentDependency		 	 = 0x04,	///< Add a new dependency between two software components.
		ESetComponentLocalizableProperty = 0x05, 	///< Set a string property of the component
		ESetComponentNumericProperty 	 = 0x06, 	///< Set a numeric property of the component
		ESetComponentBinaryProperty  	 = 0x07, 	///< Set a binary property of the component
		ERegisterComponentFile		 	 = 0x08,	///< Register a file to an existing component
		ESetFileStringProperty		 	 = 0x09,	///< Set a string property of the component's file
		ESetFileNumericProperty		 	 = 0x0A,	///< Set a string property of the component's file
		ESetComponentName		 	 	 = 0x0B,	///< Update the name of the component
		ESetComponentVendor		     	 = 0x0C,	///< Update the vendor of the component
		ESetComponentVersion		 	 = 0x0D,	///< Set the version of the component
		ESetIsComponentRemovable     	 = 0x0E,    ///< Set the removable attribute of the component
		ESetIsComponentDrmProtected		 = 0x0F,    ///< Set the DRM protected attribute of the component.
		ESetIsComponentHidden			 = 0x10,    ///< Set the hidden attribute of the component.
		ESetIsComponentKnownRevoked		 = 0x11,    ///< Set the known-revoked attribute of the component.
		ESetIsComponentOriginVerified	 = 0x12,    ///< Set the origin-verified attribute of the component.
		ESetComponentSize			 	 = 0x13, 	///< Set the install-time size of the component
		EDeleteComponentProperty	 	 = 0x14,	///< Delete a property of the component
		EDeleteFileProperty	 			 = 0x15,	///< Delete a property of the component's file
		EUnregisterComponentFile		 = 0x16,	///< Deregister a component's file
		EDeleteComponent				 = 0x17,	///< Delete a component from the SCR database
		EDeleteComponentDependency       = 0x18,	///< Delete an existing dependency between two software components.
		EGetSingleComponentSize		 	 = 0x19,	///< Retrieve the entry size of a given component id
		EGetSingleComponentData		 	 = 0x1A,	///< Retrieve the entry data of a given component id
		EGetFilePropertiesSize		 	 = 0x1B,	///< Retrieve the buffer size needed to write the file properties.
		EGetFilePropertiesData		 	 = 0x1C,	///< Retrieve the data of the file properties obtained from the SCR server.
		EGetSingleFilePropertySize   	 = 0x1D,	///< Retrieve the buffer size needed to write the value of the single file property given.
		EGetSingleFilePropertyData   	 = 0x1E,	///< Retrieve the data of the single file property.
		EGetComponentFilesCount		 	 = 0x1F,    ///< Retrieve the number of files registered by a component
		EGetFileComponentsSize		 	 = 0x20,	///< Retrieve the buffer size needed to write the component IDs possessing the given file.
		EGetFileComponentsData		 	 = 0x21,	///< Retrieve the data of the component IDs.
		EGetComponentPropertiesSize	 	 = 0x22,	///< Retrieve the buffer size needed to write the given component's properties.
		EGetComponentPropertiesData	 	 = 0x23,	///< Retrieve the data of the component's properties obtained from the SCR server.
		EGetComponentSinglePropertySize	 = 0x24,	///< Retrieve the buffer size needed to write the value of the single component property given.
		EGetComponentSinglePropertyData	 = 0x25,	///< Retrieve the data of the single component property.
		EGetLocalComponentId         	 = 0x26,	///< Retrieve the local component id of a given global component id.
		EGetComponentWithGlobalIdSize	 = 0x27,	///< Retrieve the component entry size of a given global component id
		EGetComponentWithGlobalIdData	 = 0x28, 	///< Retrieve the component entry data of a given global component id
		EGetSupplierComponentsSize	 	 = 0x29,	///< Retrieve the buffer size needed to write the supplier global IDs of a given dependant component.		
		EGetSupplierComponentsData   	 = 0x2A,	///< Retrieve the data of the supplier global IDs of a given dependant component.
		EGetDependantComponentsSize	 	 = 0x2B,	///< Retrieve the buffer size needed to write the dependant global IDs of a given supplier component.		
		EGetDependantComponentsData  	 = 0x2C,	///< Retrieve the data of the dependant global IDs of a given supplier component.
		EGetComponentIdListSize			 = 0x2D,   	///< Retrieve the buffer size needed to write the component IDs enumerated with a given filter.
		EGetComponentIdListData		     = 0x2E,	///< Retrieve the data of the component IDs enumerated with a given filter.
		EGetLogFileHandle		 		 = 0x2F,    ///< Transfer the handle of the SCR log file to the client process
		EGetComponentSupportedLocalesListSize = 0x30,    ///< Transfer the buffer size needed to write the list of matching supported languages 
		EGetComponentSupportedLocalesListData = 0x31,    ///< Retrieve the data of the list of matching supported languages
		EGetLocalizedComponentSize            = 0x32,    ///< Retrieve the localized information entry size of a given component id
		EGetLocalizedComponentData            = 0x33,    ///< Retrieve the localized information entry data of a given component id
		EGenerateNonNativeAppUid              = 0x34,    ///< Generate a new Application Uid which can be used to register non native applications.
		
        EGetApplicationLaunchersSize     = 0x3A,    ///< Gets the size of application launcher array
        EGetApplicationLaunchersData     = 0x3B,    ///< Gets the array application launchers
        
		EGetIsMediaPresent				 = 0x40,	///< Retrieve the presence info of medias where the component is installed.
		EGetIsComponentOrphaned			 = 0x41,	///< Retrieve whether the given component is orphaned.
		EGetIsComponentOnReadOnlyDrive	 = 0x42,	///< Retrieve if the component is present on a read-only drive.
		EGetIsComponentPresent			 = 0x43,	///< Retrieve if the software component is fully present.
        EGetComponentIdForApp            = 0x44,    ///< Retrieve the component Id for a given App.
        EGetAppUidsForComponentSize      = 0x45,    ///< Retrieve the size of the list of AppUids associated with a component.
        EGetAppUidsForComponentData      = 0x46,    ///< Retrieve the list of AppUids associated with a component.
						
		EAddApplicationEntry             = 0x47,    ///< Add a new application entry into SCR.
		EDeleteApplicationEntries        = 0x48,    ///< Delete the applications associated with a component from SCR.
		EDeleteApplicationEntry          = 0x49,    ///< Delete an application from SCR.
		ESetScomoState				 	 = 0x50,	///< Set the scomo state of a given component.
		ESetIsComponentPresent			 = 0x51,	///< Set the attribute if the component is present fully.
		EGetPluginUidWithMimeType    	 = 0x52,	///< Retrieve the SIF plugin UID for a given MIME type.
		EGetPluginUidWithComponentId 	 = 0x53,	///< Retrieves the SIF plugin UID for a given component Id.
		
		ESubSessCreateComponentsView 	 = 0x60,	///< Creates a sub-session to handle a components view. 
		ESubSessCreateFileList 	     	 = 0x61,	///< Creates a sub-session to handle a file list.

		ESubSessCreateRegInfoForApp      = 0x62,	///< Creates a sub-session to handle a App ID related query.
		ESubSessCreateAppRegInfo         = 0x63,   ///< Creates a sub-session to handle a query.
		
		EAddSoftwareType			 	 = 0x65,    ///< Add a new software type to the SCR database with MIME types, unique and localized names.
		EDeleteSoftwareType			 	 = 0x66,    ///< Delete an existing software type from the SCR database.
		EGetDeletedMimeTypes			 = 0x67,    ///< Retrieve the list of MIME types deleted in last software type deletion operation.
		
		ESubSessCreateAppInfoView        = 0x68,    ///< Creates a sub-session to handle a appinfo view.
		ESubSessCreateAppRegistryView    = 0x69     ///< Creates a sub-session to handle a application registration view.
		// NOTE: The range after this is used up. Add new function ids carefully.
		};
		
	enum TScrSubSessionFunction
	/**
		Functions supported by a SCR server subsession.
	 */
		{
		EOpenComponentsView       = 0x70,		///< A components view isopened on the server side by using a given filter
		EGetNextComponentSize     = 0x71,		///< Get the size of the next component entry from the components view.
		EGetNextComponentData     = 0x72,		///< Get the next component entry from the components view.
		EGetNextComponentSetSize  = 0x73,		///< Get the size of the next component entry set from the components view.
		EGetNextComponentSetData  = 0x74,		///< Get the next component entry set from the components view.
		EOpenFileList		  	  = 0x77,		///< A file list is opened on the server side by using a given filter
		EGetNextFileSize      	  = 0x78,		///< Get the size of the next file from the present file list.
		EGetNextFileData      	  = 0x79,		///< Get the next file from the present file list.
		EGetNextFileSetSize   	  = 0x7A,		///< Get teh size of the next file set from the present file list.
		EGetNextFileSetData       = 0x7B,		///< Get the next file set from the present file list.
		EOpenAppInfoView          = 0x7C,       ///< An AppInfo view is opened on the server side by using a given filter
		EGetNextAppInfoSize       = 0x7D,       ///< Next AppInfo size is calculated and sent to the client
		EGetNextAppInfoData       = 0x7E,       ///< Next AppInfo data is returned to the client  
		
		ESetAppUid         		      = 0x80,       ///< AppId is set on the server side to be used for the specific subsession.
		EGetServiceUidSize		      = 0x88,       ///< Get the size of the service Id list. 
		EGetServiceUidList		      = 0x89,       ///< Get the list of ServiceIds.
		EGetApplicationLanguage       = 0x8A,		///< Get the Application Language of the App and is nearest to the device language.
		EGetDefaultScreenNumber       = 0x8B,       ///< Get the Default screen number specified for an App.
		EGetNumberOfOwnDefinedIcons   = 0x8C,       ///< Get the number of defined icons of a specified locale of the App.
		EGetAppForDataTypeAndService  = 0x8D,       ///< Get the AppUid with the specified type and ServiceId		
		EGetAppForDataType	   	      = 0x8E,	    ///< Get the AppUid with the specified type and a highest priority.
		EGetViewSize			      = 0x8F,		///< Get the size of the view details of a given application
		EGetViewData			      = 0x90,		///< Get the array of view details of a given application
		EGetAppOwnedFilesSize         = 0x91,       ///< Get the size of files owned by given application
		EGetAppOwnedFilesData         = 0x92,       ///< Get the array of files owned by given application
		EGetAppCharacteristics        = 0x93,       ///< Get the characteristics of given application
		EGetAppIconForFileName    	  = 0x94,       ///< Get the application icon file name for given application
		EGetAppViewIconFileName       = 0x95,       ///< Get the application view  icon file name for given application
		EGetAppServiceInfoSize		  = 0x96,		///< Get the size of the required service details associated with the parameters set in the filter.
		EGetAppServiceInfoData        = 0x97,       ///< Get the required service details associated with the parameters set in the filter.
		EGetApplicationInfo           = 0x98,       ///< Get the required app's information details contained in TAppRegInfo.
				
		EOpenApplicationRegistrationInfoView           = 0xB0,  ///< An Application registration view is opened on the server side
		EGetApplicationRegistrationSize                = 0xB1,  ///< Get the size of the next application registration data
		EGetApplicationRegistrationData                = 0xB2,  ///< Get the next application registration data
		EOpenApplicationRegistrationInfoForAppUidsView = 0xB3,  ///< An Application registration AppUids view is opened on the server side
		};
	
	/**
		Delay in microseconds before the SCR Server is shut down, after
		the last remaining session has been closed.
	 */
	static const TInt KScrServerShutdownPeriod = 2 * 1000 * 1000;
	
	/** Maximum length of a UID string. */
	static const TInt KUidStringLen = 8;	
	} // End of namespace Usif
 
 #include <usif/scr/scrcommon.inl>
 
#endif // SCRCOMMON_H
