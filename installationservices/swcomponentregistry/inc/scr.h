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
* RSoftwareComponentRegistry provides interfaces allowing install, uninstall, and update 
* of software components .
* Software Components Registry interface is designed to be used by installers, application managers, device managers and the software install framework (SIF).
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SOFTWARECOMPONENTREGISTRY_H
#define SOFTWARECOMPONENTREGISTRY_H

#include <e32base.h>
#include <usif/usifcommon.h>
#include <usif/scr/screntries.h>
#include <usif/scr/appregentries.h>
#include <scs/scsclient.h>
#include <usif/scr/appreginfo.h>
#include <usif/scr/screntries_platform.h>

namespace Usif
	{	 
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
	class CScrLogEntry;
	class CLocalizedSoftwareTypeName;
#endif SYMBIAN_ENABLE_SPLIT_HEADERS

	NONSHARABLE_CLASS(RSoftwareComponentRegistry) : public RScsClientBase
	/*
		The clients of Software Component Registry (SCR) use this class to communicate 
		with the SCR server. 
		
		This class provides several sets of functionality:
		(i) Components' management, such as adding and removing components, registering files and other operations
		(ii) Transaction management, which allows rolling back or committing a set of mutating operations together.
		(iii) Read-only queries, such as getting component properties, or component information. 
		
		Operations which update or create new components in the SCR are restricted to the registered installers or layered execution
		environments (LEE). For example, only a Java installer or the Java Virtual Machine (JVM) can create new Java components. Additionally,
		only the same entities can change properties of or delete already existing Java components.
		
		Please note that mutating functions of this class can be used if any of the following conditions applies:
		- There is no active transaction or reading operation with component or file views.
		- A transaction exists and has been started by this connection.
		
		Additionally, please note that SCR limits the size of strings it receives in input to 512 characters, e.g. supplying component names, property names longer than that
		will result in KErrArgument.
	 */
		{
	
	public:
		/**
			Default constructor of SCR session.
		 */
		IMPORT_C RSoftwareComponentRegistry();
		
		/**
			Opens a connection to the SCR server.
			@return KErrNone, if the connection is successful. Otherwise, a system-wide error code.  
		 */
		IMPORT_C TInt Connect();
		
		/**
			Closes the connection with the SCR server.
		 */
		IMPORT_C void Close();
		
		/**
			The SCR allows its clients to perform all mutable operations related to a software component management operation
			(e.g. install, uninstall, upgrade, and replace) within a single transaction. If a software component management operation 
			fails or is aborted then the SCR will revert to a consistent state.  If mutating operations are not done under a transaction, 
			it will be impossible to roll them back automatically in conjunction with other operations.

			A transaction is started and persists throughout the connection until the next commit/rollback function call.
			The transaction automatically rolls back if the server connection is closed without commit/rollback functions 
			being invoked.  If the device unexpectedly shuts down while a transaction is in progress, the SCR server will 
			revert to consistent state on next usage. 
			 
			Please note that nested/concurrent transactions are NOT supported. 
			 
			N.B. Transaction operations are permitted only to registered installers or execution environments.
			
			@leave KErrScrWriteOperationInProgress If a transaction is already in progress on the SCR server.
			@leave KErrPermissionDenied An unauthorised process attempted a transaction operation. 
			@leave Or system-wide error codes.
		 */
		IMPORT_C void CreateTransactionL();
		
		/**
			Rolls back the transaction started by this session by reverting all the changes made during the transaction. 
			
			N.B. Transaction operations are permitted only to registered installers or execution environments.			
			
			@leave KErrScrNoActiveTransaction There is no active transaction on the SCR Server started by this connection.
			@leave KErrPermissionDenied An unauthorised process attempted a transaction operation.
			@leave Or system-wide error codes.
		 */ 
		IMPORT_C void RollbackTransactionL();
		
		/**
			Commits the transaction started by this session.
			
			N.B. Transaction operations are permitted only to registered installers or execution environments.			
			
			@leave KErrScrNoActiveTransaction There is no active transaction on the SCR Server started by this connection.
			@leave KErrPermissionDenied An unauthorised process attempted a transaction operation.
			@leave Or system-wide error codes.
		 */
		IMPORT_C void CommitTransactionL();
		
		/**
			Adds a new software component entry to the SCR database.
			
			If the component and vendor names of the software component being installed are NOT localizable, 
			then this API should be used to add a new component.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
				
			@param aName The non-localizable name of the component being added.
			@param aVendor The non-localizable vendor name of the component being added.
			@param aUniqueSwTypeName The non-localized, unique software type name, such as @see KSoftwareTypeNative and @see KSoftwareTypeJava.
			@param aGlobalId A named, nullable, case-sensitive string used to specify a deterministic identifier for the component 
						that is unique within the context of that software type. The global component id might be 
						the package UID for native applications or the MIDlet name/vendor pair for Java MIDLets, etc. 
						Note that if KNullDesC is provided, the global identifier will be empty.
			@param aCompOpType The type of the component operation. Typical operations are install or upgrade. @see TScrComponentOperationType
			@return  The component ID of the newly added entry.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		 */
		IMPORT_C TComponentId AddComponentL(const TDesC& aName, const TDesC& aVendor, const TDesC& aUniqueSwTypeName, const TDesC* aGlobalId=NULL, TScrComponentOperationType aCompOpType=EScrCompInstall);
		
		/**
			Adds a new software component entry to the SCR database.
			If the component and vendor names of the software component being installed are localizable,
			then this API should be used to add a new component.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
					
			@param aComponentInfo The list of localizable component information (such as vendor and name) with one entry per each supported locale.
			@param aUniqueSwTypeName The non-localized, unique software type name, e.g. "Native".
			@param aGlobalId A named, nullable, case-sensitive string used to specify a deterministic identifier for the component 
							that is unique within the context of that software type. The global component id would be 
							the package UID for native applications or the midlet-names for Java, etc. 
							Note that if KNullDesC is provided, the global identifier will be empty.
			@param aCompOpType The type of the component operation. Typical operations are install or upgrade. @see TScrComponentOperationType
			@return  The component ID of the newly added entry. 
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		 */
		IMPORT_C TComponentId AddComponentL(const RPointerArray<CLocalizableComponentInfo>& aComponentInfo, const TDesC& aUniqueSwTypeName, const TDesC* aGlobalId=NULL, TScrComponentOperationType aCompOpType=EScrCompInstall);
		
		/** 
			Adds a new dependency between two software components (supplier and dependant) of any type.
			For example, if VNC Viewer is ported to the Symbian OS using the POSIX library, VNC Viewer
			depends on the POSIX library. Here, VNC Viewer is dependant, whereas the POSIX library is supplier. 
			
			A dependency exists on global component ids, since a dependency is a global relation between 
			two components, regardless to their status on a particular system. While local component ids 
			(@see TComponentId) are private to a particular device and can change for the same global id 
			in the case of upgrades or reinstalls, global component ids are unique and equal for the same 
			component on any device.
			
			Since a dependency is defined using global ids, it can be added even if the participant
			components don't exist in the system. 
			N.B. A duplicate dependency is not allowed. 
					
			@param aSupplierVerCompId The versioned software component identifier of the supplier component. 
			@param aDependantGlobalCompId The global software component identifier of the dependant component.
			@leave KErrAlreadyExists If the dependency is already defined in the SCR.
			@leave Or other system-wide error codes.
		 */
		IMPORT_C void AddComponentDependencyL(const CVersionedComponentId& aSupplierVerCompId, const CGlobalComponentId& aDependantGlobalCompId);

		/**
			Adds a new named 8-bit descriptor property to the registry for a given software component and locale. 
			If the property already exits, then its value is updated.
						
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			 	
			@param aComponentId The unique ID of the given software component.
			@param aName The name of the software component property.
			@param aValue The value of the software component property.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes. 
		 */
		
		IMPORT_C void SetComponentPropertyL(TComponentId aComponentId, const TDesC& aName, const TDesC8& aValue);		
		
		/**
			Adds a new named localizable 16-bit string property to the registry for a given software component and locale. 
			If the property already exists, then its value is updated.
			
			N.B. If the property exists with the same value but for a different locale, it is NOT replaced, but instead added for the new locale.
			N.B. This API should not be used for raw binary data. For binary data, the TDesC8 variant of SetComponentProperty should be used instead.
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			 	
			@param aComponentId The unique ID of the given software component.
			@param aName The name of the software component property.
			@param aValue The value of the software component property.
			@param aLocale The language code of the property. The default value is non-localized property.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes. 
		 */
		
		IMPORT_C void SetComponentPropertyL(TComponentId aComponentId, const TDesC& aName, const TDesC& aValue, TLanguage aLocale=KNonLocalized);
		
		/**
			Adds a new named numeric property to the registry for a given software component. If the property already
			exits, then its value is updated. Signed values from -2^63+1 to +2^63-1 are supported. 
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
					 	
			@param aComponentId The unique ID of the given software component.
			@param aName The name of the software component property.
			@param aValue The value of the software component property. Can be TInt; if the value cannot exceed 32-bit range, as TInt64 provides automatic casting from TInt.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
	     */
		IMPORT_C void SetComponentPropertyL(TComponentId aComponentId, const TDesC& aName, TInt64 aValue);
						
		/**
			Registers a file to a given software component.
			
			Before registration, tests whether the provided filename is syntactically correct. 
			For more information about the syntax, check @see RFs::IsValidName.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			    
			@param aComponentId The unique ID of the given software component.
			@param aFileName The fully qualified name of the file being registered.
			@param aConsiderForInstalledDrives This parameter specifies whether the list of installed drives ( @see CComponentEntry::InstalledDrives) should be maintained as a result of this operation. 
			In most cases, this should be left at the default setting, unless a file is registered for uninstall purposes only and is not actually present on the system, for example a possible temporary file which might be created later by the component's application(s)
			Please note that if UnregisterComponentFileL is later used on the component, this parameter will not apply, and the file registration may still affect the list of installed drives.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave KErrArgument If the file name is invalid or is not fully qualified
			@leave Or system-wide error codes.
		 */	
		IMPORT_C void RegisterComponentFileL(TComponentId aComponentId, const TDesC& aFileName, TBool aConsiderForInstalledDrives = ETrue); 
		
		/**
			Adds a new named 8-bit descriptor property to the registry for a given file and software component. If the property already
			exits, then its value is updated.
			
			A file property allows adding custom information about a component’s file ownership, for example, a native installer 
			may associate custom operations to be done on uninstall for a particular file.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			
			@param aComponentId The unique ID of the given software component.
			@param aFileName  The fully qualified name of the file.
			@param aPropName  The name of the file property being added.
			@param aPropValue The value of the file property being set.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		*/
		IMPORT_C void SetFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropName, const TDesC8& aPropValue);
		
		/**
			Adds a new named numeric property to the registry for a given file and software component. 
			If the property already exists, it is just updated.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
				
			@param aComponentId The unique ID of the given software component.	 	
			@param aFileName  The fully qualified name of the file.
			@param aPropName  The name of the file property being added.
			@param aPropValue The value of the file property being added.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		 */
		IMPORT_C void SetFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropName, TInt aPropValue);
		
		/**
			Adds a new localized component name for a given software component. 
			If the localized name already exists for the specified locale, or one of the locales in its downgrade path (@see BaflUtils::GetDowngradePathL),
			then it is updated. 
			If no locale is specified, then the default component name is updated (or added if no default name exists).
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			
			@param aComponentId The unique ID of the given software component.
			@param aName The new value of the component name for a given locale.
			@param aLocale The language code of the requested locale. The default value is non-localized.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		*/
		IMPORT_C void SetComponentNameL(TComponentId aComponentId, const TDesC& aName, TLanguage aLocale=KNonLocalized);
		
		/**
			Adds a new localized component vendor for a given software component. 
			If the localized vendor name already exists for the specified locale, or one of the locales in its downgrade path (@see BaflUtils::GetDowngradePathL),
			then it is updated. 
			If no locale is specified, then the default vendor name is updated (or added if no default vendor name exists).
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
						
			@param aComponentId The unique ID of the given software component.
			@param aVendor The new value of the vendor name for a given locale.
			@param aLocale The language code of the requested locale. The default value is non-localized.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
	    */
		IMPORT_C void SetVendorNameL(TComponentId aComponentId, const TDesC& aVendor, TLanguage aLocale=KNonLocalized);
		
		/**
			Updates the version of a given software component.
			
			N.B. The default value of component version is NULL.
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
						
			@param aComponentId The unique ID of the given software component.
			@param aVersion The new value of the version attribute.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		 */
		IMPORT_C void SetComponentVersionL(TComponentId aComponentId, const TDesC& aVersion);
		
		/**
			Sets whether the component is removable.
			When a component is added, this is set to True by default (i.e. all non-ROM components are removable unless stated otherwise).
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			 
			@param aComponentId The unique ID of the given software component.
			@param aValue ETrue to set component as removable. EFalse to set component as unremovable.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
			
			@capability AllFiles Creating non-removable components requires AllFiles.
		 */
		IMPORT_C void SetIsComponentRemovableL(TComponentId aComponentId, TBool aValue);
		
		/**
			Sets whether the component is DRM protected.
			When a component is added, this is set to False by default (i.e. all components are non-DRM protected unless stated otherwise).
					
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
					 
			@param aComponentId The unique ID of the given software component.
			@param aValue ETrue to set component as DRM protected. EFalse to set component as non-DRM protected.
					
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
					
			@capability WriteDeviceData Creating DRM protected components requires WriteDeviceData.
		 */
		IMPORT_C void SetIsComponentDrmProtectedL(TComponentId aComponentId, TBool aValue);
		
		/**
			Sets the hidden attribute of a component indicating that the component shouldn't be displayed in the list of installed components.
			When a component is added, this is set to False by default (i.e. all components are visible unless stated otherwise).
			
			N.B. SCR does not enforce usage of this flag, and application managers may ignore it, e.g. in an admin view.
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
							 
			@param aComponentId The unique ID of the given software component.
			@param aValue ETrue to set component as hidden. EFalse to set component as non-hidden.
							
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
							
			@capability WriteDeviceData Creating hidden components requires WriteDeviceData.
		 */
		IMPORT_C void SetIsComponentHiddenL(TComponentId aComponentId, TBool aValue);
		
		/**
			Sets the Known-Revoked attribute of a component. If this attribute is true, then it indicates 
			a revocation check was performed during or after the installation and the component
			was found to be revoked. When a component is added, this is set to False by default.
							
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
							 
			@param aComponentId The unique ID of the given software component.
			@param aValue ETrue to set component as known-revoked. EFalse to set component as known-unrevoked.
							
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
							
			@capability WriteDeviceData Creating known-revoked components requires WriteDeviceData.
		 */
		IMPORT_C void SetIsComponentKnownRevokedL(TComponentId aComponentId, TBool aValue);
		
		/**
			Sets the Origin-Verified attribute of a component. If this attribute is true, then it indicates that 
			the installer has verified the originator of the software component by using appropriate checking mechanisms (e.g. certificates). 
			If the attribute is false, it means that such a verification has not been performed, or that the installer does not support origin verification. 
			When a component is added, this is set to False by default.
							
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
							 
			@param aComponentId The unique ID of the given software component.
			@param aValue ETrue to set component as origin-verified EFalse to set component as origin-non-verified.
							
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
							
			@capability WriteDeviceData Creating origin verified components requires WriteDeviceData.
		 */
		IMPORT_C void SetIsComponentOriginVerifiedL(TComponentId aComponentId, TBool aValue);

		/**
			Sets the total size in bytes of the files owned by a given component. Note that SCR doesn't maintain this value.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
									 	
			@param aComponentId The unique ID of the given software component.
			@param aComponentSizeInBytes The size of the component's files in bytes.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		 */
		IMPORT_C void SetComponentSizeL(TComponentId aComponentId, TInt64 aComponentSizeInBytes);
		
		/**
			Deletes a specific property entry of a given software component.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			N.B. If the component or property doesn't exist, the function completes successfully.
			N.B. If the property is specified for multiple locales, it is deleted for all of them.
			 	
			@param aComponentId The unique ID of the given software component.
			@param aPropName The name of the component property being deleted.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
	     */
		IMPORT_C void DeleteComponentPropertyL(TComponentId aComponentId, const TDesC& aPropName);
		
		/** 
			Deletes a specific file property entry for a given file and software component.
						
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			
			@param aComponentId The unique ID of the given software component.
			@param aFileName The fully qualified name of the file.
			@param aPropName The name of the file property being deleted.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		 */
		IMPORT_C void DeleteFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropName);
		
		/**
			Unregisters a specific file entry from a given software component.
			Note that all associated file properties for the same component are removed as well.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			N.B. This operation also deletes all the corresponding file properties.
			  	
			@param aComponentId The unique ID of the given software component.
			@param aFileName The fully qualified name of the file being deregistered.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
	     */
		IMPORT_C void UnregisterComponentFileL(TComponentId aComponentId, const TDesC& aFileName);
		
		/**
			Deletes a sofware component entry and all its data from the registry, including properties and file registration records.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			N.B. If the component doesn't exist, the function completes successfully.
				
			@param aComponentId The unique ID of the given software component.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or system-wide error codes.
		*/
		IMPORT_C void DeleteComponentL(TComponentId aComponentId);
		
		/** 
			Deletes an existing dependency between two software components (supplier and dependant) of any type.
			(@see RSoftwareComponentRegistry::AddComponentDependencyL for more information about component dependency.)
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			N.B. If the dependency doesn't exist, the function completes successfully.
							
			@param aSupplierGlobalCompId The global software component identifier of the supplier component. 
			@param aDependantGlobalCompId The global software component identifier of the dependant component.
			@leave System-wide error codes.
		 */
		IMPORT_C void DeleteComponentDependencyL(const CGlobalComponentId& aSupplierGlobalCompId, const CGlobalComponentId& aDependantGlobalCompId);
		
		/**
			Retrieves a specific software component entry with the component name and vendor for a particular locale. 
							
			@param aComponentId The unique ID of the given software component.
			@param aEntry An output parameter for returning information for the component matching the given id. 
				          If no such component is present, this parameter remains unchanged.
			@param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
						   the names. If it fails, then the non-localized names are returned. If non-localized versions
						   couldn't be found too, the first available language for the component is retrieved.
						   If KNonLocalized is provided, then the non-localized component and vendor names are returned,
						   and if non-localized versions don't exist, the function will leave with KErrScrUnsupportedLocale.
						   If a particular language is provided, then the names are searched for this particular language
						   and its downgraded languages. If there is no name and vendor with the provided language, then the function leaves with KErrScrUnsupportedLocale.
		    @return If the requested component is found, then ETrue is returned. Otherwise, EFalse.
		    @leave KErrScrUnsupportedLocale The requested component name/vendor couldn't be found for the specified locale.
		    @leave System-wide error codes.
	     */
		IMPORT_C TBool GetComponentL(TComponentId aComponentId, CComponentEntry& aEntry, TLanguage aLocale=KUnspecifiedLocale) const;							
		
		/**
		    Retrieves the localized information from ComponentLocalizable table for a given component id.
							
			@param aComponentId The unique ID of the given software component.
			@param aCompLocalizedInfoArray An output parameter for returning localized information for a given component id. 
			@leave System-wide error codes.
			@capability  ReadUserData Accessing a component's localized information requires ReadUserData
		*/
		IMPORT_C void GetComponentLocalizedInfoL(TComponentId aComponentId, RPointerArray<CLocalizableComponentInfo>& aCompLocalizedInfoArray) const;
		
		/**
			Retrieves the array of all software component IDs in the SCR, optionally matching a particular filter.
				
			@param aComponentIdList The component Id list of all sofwtare components.
			@param aFilter Pointer to the filter object which will be used by the SCR to enumerate the software components.
						   If no filter is provided, all software components in the SCR are listed.
		
			@leave System-wide error codes.

			@capability  ReadUserData Accessing list of components installed by the user requires ReadUserData
		 */
		IMPORT_C void GetComponentIdsL(RArray<TComponentId>& aComponentIdList, CComponentFilter* aFilter=NULL) const;
		
		/**
			Retrieves the local component id of a given global component id.
			
			@param aGlobalIdName A deterministic identifier for the component that is unique within the context of that software type.
			@param aUniqueSwTypeName The non-localized, unique software type name, such as @see KSoftwareTypeNative and @see KSoftwareTypeJava.
			@return The local unique component id.
			
			@leave KErrNotFound Software component with this global id is not present in the system.
			@leave System-wide error codes.
		 */
		IMPORT_C TComponentId GetComponentIdL(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName) const;
					
		/**
			Retrieves a specific software component entry with the specified global id and software type name for a given locale. 
							
			@param aGlobalIdName A deterministic identifier for the component that is unique within the context of that software type.
			@param aUniqueSwTypeName The non-localized, unique software type name, such as @see KSoftwareTypeNative and @see KSoftwareTypeJava.
			@param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
						   the names. If it fails, then the non-localized names are returned. If non-localized versions
						   couldn't be found too, the first available language for the component is retrieved.
						   If KNonLocalized is provided, then the non-localized component and vendor names are returned,
						   and if non-localized versions don't exist, the function will leave with KErrScrUnsupportedLocale.
						   If a particular language is provided, then the names are searched for this particular language
						   and its downgraded languages. If there is no name and vendor with the provided language, then the function leaves with KErrScrUnsupportedLocale.
		    @return aEntry A pointer to the component entry object for the given global component id. 
		    
		    @leave KErrScrUnsupportedLocale The component requested with the global id couldn't be found for the specified locale.
		    @leave System-wide error codes.
		 */
		IMPORT_C CComponentEntry* GetComponentL(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName, TLanguage aLocale=KUnspecifiedLocale) const;
		
		/**
			Retrieves the list of supplier components with the range of versions on which a given 
			component depends. The range of versions here indicates specific versions of the returned 
			components (suppliers) which the given component (dependant) depend on.

			@param aDependantGlobalId The global component id of the given software component.
			@param aSupplierList The versioned component id list of supplier components for the given dependant component.
			@leave System-wide error codes.
		 */
		IMPORT_C void GetSupplierComponentsL(const CGlobalComponentId& aDependantGlobalId, RPointerArray<CVersionedComponentId>& aSupplierList) const;
				
		/**
			Retrieves the list of components (dependants) with the range of versions that depend on 
			a given component (supplier). The range of versions here indicates specific versions of 
			the given component which the returned components depend on.
					
			@param aSupplierGlobalId The global component id of the given software component.
			@param aDependantList The versioned component id list of dependant components for the given supplier component.
			@leave System-wide error codes.
		 */
		IMPORT_C void GetDependantComponentsL(const CGlobalComponentId& aSupplierGlobalId, RPointerArray<CVersionedComponentId>& aDependantList) const;

		/**
			Retrieves the array of properties owned by a specific file registered to a given component.
			
			@param aComponentId The unique ID of the given software component.	
			@param aFileName The fully qualified name of the file.
			@param aProperties An output parameter for returning the list of properties owned by the given file.
			@leave System-wide error codes.
		 */
		IMPORT_C void GetFilePropertiesL(TComponentId aComponentId, const TDesC& aFileName, RPointerArray<CPropertyEntry>& aProperties) const;
		
		/**
			Retrieves a specified property entry for a given file and software component.
					
			@param aComponentId The unique ID of the given software component.
			@param aFileName The fully qualified name of the file.
			@param aPropertyName The name of the requested property.
			@return Returns a pointer to the property object containing the requested file property.
				    The ownership is transferred. If the property couldn't be found, then NULL is returned.
			@leave System-wide error codes.	    
		 */
		IMPORT_C CPropertyEntry* GetFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropertyName) const;
		
		/**
			Retrieves the number of files registered by a component.
					
			@param aComponentId The unique ID of the given software component.
			@return The number of registered files.
			@leave System-wide error codes.	    
		 */
		IMPORT_C TUint GetComponentFilesCountL(TComponentId aComponentId) const;		
		
		/**
			Retrieves the set of components which register a given file.
				
			@param aFileName The fully qualified name of the file.
			@param aComponents An output parameter for returning for the list of components which own the given file.
			@leave System-wide error codes.	
		 */ 
		IMPORT_C void GetComponentsL(const TDesC& aFileName, RArray<TComponentId>& aComponents) const;
		
		/**
			Retrieves a specified property for a given component, property name and locale. 
			
			When searching for localizable properties, it is possible that the requested language 
			variant is not available, however, there may be other language variants that are acceptable.
			Therefore, a language downgrade path that starts with the language associated with the requested 
			locale and ends with the least best match is first created. Then the downgrade path is used to
			search for a language specific property and the first match is returned to the user.
	
			@param aComponentId The unique ID of the given software component.
			@param aPropertyName The name of the component property.
			@param aLocale The optional language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its downgraded path (@see BaflUtils::GetDowngradePathL) is used 
						   as explained above to find the property.  If it fails, then the non-localized version is looked up.
						   If non-localized version couldn't be found too, the function leaves with KErrNotFound.
						   If KNonLocalized is provided, then the non-localized version of the property is returned,
						   If non-localized version doesn't exist, then the function will leave with KErrNotFound.
						   If a particular language is provided, then the property is searched for this particular language
						   and its downgraded languages. If there is no property with the provided language, then the function 
						   leaves with KErrNotFound.
			@return Returns a pointer to the property object containing the requested component property entry.
					The ownership is transferred. If the property couldn't be found, then NULL is returned.
            @leave A system-wide error code.
		 */
		IMPORT_C CPropertyEntry* GetComponentPropertyL(TComponentId aComponentId, const TDesC& aPropertyName, TLanguage aLocale=KUnspecifiedLocale) const;
		
		/**
			Retrieves the set of properties for a particular locale belonging to the given component.
			if no locale is specified, then the current locale is used as default.
			
			For more information about searching for localized properties look at @see RSoftwareComponentRegistry::GetComponentPropertyL.
				
			@param aComponentId The unique ID of the given software component.
			@param aProperties An output parameter for returning the list of properties which owned by the given component.
			@param aLocale The optional language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its downgraded path (@see BaflUtils::GetDowngradePathL) is used 
						   as explained above to find properties.  If it fails, then the non-localized version is looked up.
						   If non-localized versions couldn't be found too, the function leaves with KErrNotFound.
						   If KNonLocalized is provided, then the non-localized versions of properties are returned,
						   If non-localized versions don't exist, then the function will leave with KErrNotFound.
						   If a particular language is provided, then the properties are searched for this particular language
						   and its downgraded languages. Otherwise, the function leaves with KErrNotFound.
            @leave A system-wide error code.
		*/
		IMPORT_C void GetComponentPropertiesL(TComponentId aComponentId, RPointerArray<CPropertyEntry>& aProperties, TLanguage aLocale=KUnspecifiedLocale) const;
				
		/**
			Returns whether all drives registered for the given component are present.
			The list of drives is derived using the set of files registered by the component.
			
			N.B. For removable media, this method does NOT check that the actual media matches the one which contains the component's files.
			It just verifies that any media is present in the drive.
			
			@param aComponentId The unique ID of the software component.
			@return ETrue, if all drives registered for the component are present; otherwise EFalse.
			@publishedPartner
		 */
		IMPORT_C TBool IsMediaPresentL(TComponentId aComponentId) const;
		
		/** 
			Sets the SCOMO state of a given component.
			
			@param aComponentId The unique ID of the given software component. 	
			@param aScomoState The new SCOMO state of the component.
            @leave A system-wide error code.
		 */
		IMPORT_C void SetScomoStateL(TComponentId aComponentId, TScomoState aScomoState) const;
		
		/** 
			Retrieves the UID of the plug-in which is responsible for a given MIME type.
					
			@param aMimeType The MIME type. 	
			@return The UID of the plug-in.
			@leave KErrSifUnsupportedSoftwareType There is no corresponding plug-in for this MIME type.
            @leave A system-wide error code.			
			@internalTechnology
		 */
		IMPORT_C TUid GetPluginUidL(const TDesC& aMimeType) const;
		
		/** 
			Retrieves the UID of the plug-in that installed a given component.
							
			@param aComponentId Component's local identifier. 	
			@return The UID of the plug-in.
            @leave A system-wide error code.
			@internalTechnology
		 */
		IMPORT_C TUid GetPluginUidL(TComponentId aComponentId) const;
		
		/**
			Add a new software type name to the SCR database.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			This can be called only by sisregistry.

			@param aUniqueSwTypeName Non-localized unique software type name.
			@param aSifPluginUid The SIF plug-in UID of the installer which is responsible for installing components with this software type
			@param aInstallerSecureId The secure id of the installer which is responsible for installing components with this software type
			@param aExecutionLayerSecureId The secure id of the installer's execution layer which refers to an executable that interprets or  
			                               executes byte code for programs written in a high level language Java, Python, Widgets, etc.
			                               This paramater is optional. If it doesn't exist, it should be equal to the installer's secure id.                                
			@param aMimeTypes The list of MIME types associated with this software type.
			@param aLocalizedSwTypeNames A pointer to the list of optional localized software type names.
            @leave A system-wide error code.
			@internalTechnology
		 */
		IMPORT_C void AddSoftwareTypeL(const Usif::CSoftwareTypeRegInfo& aSwTypeRegInfo);
		
		/**
			Deletes a sofware type name entry and all its mime types from the registry.
			
			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
			N.B. If the software type doesn't exist, the function completes successfully.
			
			@param aUniqueSwTypeName Non-localized unique software type name.
			@param aDeletedMimeTypes An output parameter, where the MIME types deleted from SCR as a result of this operation will be stored.
            @leave A system-wide error code.
			@internalTechnology
		 */
		IMPORT_C void DeleteSoftwareTypeL(const TDesC& aUniqueSwTypeName, RPointerArray<HBufC>& aDeletedMimeTypes);
	
		/**
			Returns whether the software installer of a given component is present.
					
			@param aComponentId The unique ID of the given software component. 
			@return ETrue, if the software installer of the given component is present; otherwise EFalse.
            @leave A system-wide error code.
		 */
		IMPORT_C TBool IsComponentOrphanedL(TComponentId aComponentId);
		
		 /**
			Retrieve the log entries recorded by SCR. A log entry (@see Usif::CScrLogEntry) contains information 
			about the component operation (installation, uninstallation or upgrade) made by SCR.
		
			@param aLogEntries The caller supplies an empty array of CScrLogEntry which is loaded
					with the log entries from the SCR. If the SCR Log is empty then the array count 
					will be zero. The caller is responsible for deleting all memory.
			@param aUniqueSwTypeName Optional parameter for non-localized unique software type name.
									 If provided, only the log entries of the given software type are returned.
            @leave A system-wide error code.
			@capability ReadUserData
		 */
		IMPORT_C void RetrieveLogEntriesL(RPointerArray<CScrLogEntry>& aLogEntries, const TDesC* aUniqueSwTypeName=NULL) const;
		
		/**
			Compares two generic, arbitrary length, dotted decimal version numbers. e.g. "1.2.32.4" < "1.3"
			
			@param aVersionLeft The version on the left side of the equality.
			@param aVersionRight The version on the right side of the equality.
			@return A positive value indicates that the left version is greater than the right version. 
				    A negative value indicates that the left version is less than the right version. 
				    Zero indicates that the left and right versions are equal. 
            @leave A system-wide error code.
		 */
		IMPORT_C static TInt CompareVersionsL(const TDesC& aVersionLeft, const TDesC& aVersionRight);

		/**
			Returns whether the component is on a read-only drive.

			@param aComponentId The unique ID of the software component. 
			@return ETrue, if the component is present on a read only drive; otherwise EFalse.
            @leave A system-wide error code.
		 */
		IMPORT_C TBool IsComponentOnReadOnlyDriveL(TComponentId aComponentId) const;
		
		/**
			Returns whether the component is present on the device. As a component might be 
			installed to a media card which was later removed, not all components registered are 
			necessarily present. Please note that IsMediaPresentL does not check whether the media 
			card containing the component is present. 
			
			N.B. This function relies on values reported by installers, and thus, component presence 
			might not be updated immediately when a media card is replaced.
			
			@param aComponentId The unique ID of the component.
			@return ETrue, if the component is present; otherwise EFalse.
			
            @leave A system-wide error code.
		 */
		IMPORT_C TBool IsComponentPresentL(TComponentId aComponentId) const;
		
		/**
			Sets whether the software component is present fully after being installed on to a 
			removable drive.

			N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
					 
			@param aComponentId The unique ID of the given software component.
			@param aValue ETrue to set component as present. EFalse to set component as not present.
			
			@leave KErrScrWriteOperationInProgress If a transaction started by another session is in progress.
			@leave Or a system-wide error code.
		 */
		IMPORT_C void SetIsComponentPresentL(TComponentId aComponentId, TBool aValue);
		
		/**
			Gets the installed supported language ID's array.

			@param aComponentId The unique ID of the given software component.
			@param aMatchingSupportedLanguages RArray containing the list of installed supported languages
			
			@leave A system-wide error code.
            @capability  ReadUserData Accessing the installed supported language ID's array requires ReadUserData
		 */
		IMPORT_C void GetComponentSupportedLocalesListL(TComponentId aComponentId, RArray<TLanguage>& aMatchingSupportedLanguages) const;
		
		/**
		    Adds a new application entry to the SCR database.
		    
		    N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
		    
		    @param aComponentId The componentId associated to the application   
		    @param aApplicationRegistrationData The information associated with the application.         
		    @leave KErrArgument If the AppUid or AppFile in aApplicationRegistrationData is NULL
                   Or a system-wide error code.
		*/
	
		IMPORT_C void AddApplicationEntryL(const TComponentId aComponentId, const CApplicationRegistrationData& aApplicationRegistrationData);
		
		/**
		    Deletes the information of all applications associated to an existing component entry from the SCR database.
		    
		    N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
		    
		    @param aComponentId The componentId, associated to which, all applications are deleted.
		    @leave A system-wide error code.
		*/
		IMPORT_C void DeleteApplicationEntriesL(TComponentId aComponentId);
		
		/**
            Deletes an application entry from the SCR database.
            
            N.B. This is a mutating function. Please see @see Usif::RSoftwareComponentRegistry.
            
            @param aApplicationUid Uid of the application to be deleted.
            @leave A system-wide error code.            
        */
        IMPORT_C void DeleteApplicationEntryL(TUid aApplicationUid);

        /**
            Gets the Component Id associated to a given Application Uid

            @param aAppUid The AppUid of an application.
            @return The componentId associated with the given application.  
            @leave A system-wide error code.                      
         */
        IMPORT_C TComponentId GetComponentIdForAppL(TUid aAppUid) const;

        /**
            Gets the array of Application Uids associated to a given Component.

            @param aCompId ComponentId of a component whose associated applications need to be fetched.
            @param aAppUids Returns the array of application Uids associated with the component.
            @leave A system-wide error code.            
         */
        IMPORT_C void GetAppUidsForComponentL(TComponentId aCompId, RArray<TUid>& aAppUids) const;
        
        /**
            Returns the array of application launchers.
                N.B. This function can only be used by AppArc to determine the launcher associated with 
                an application.
    
            @param aLaunchers The current list of launchers present on the device.
            @leave A system-wide error code.            
         */
        IMPORT_C void GetApplicationLaunchersL(RPointerArray<CLauncherExecutable>& aLaunchers) const;
        		
	private:

		TInt SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const;
		template <class C> void SendDependencyArgsL(TInt aFunction, const C& aSupplierId, const CGlobalComponentId& aDependantGlobalCompId);
		template <class C> friend HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize);
		template <class T, class C> friend T* GetObjectL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
		template <class T, class C> friend TBool GetObjectL(const C& aConnection, T& aObject, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RArray<T>& aArray);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray);
		};
	
	
	NONSHARABLE_CLASS(RSoftwareComponentRegistryView) : public RScsClientSubsessionBase
	/**
		This class opens a sub-session to the SCR server and sends a request to create a component view
		by using the supplied filter on the server side.
	 */
		{
	public:
		/**
			Default constructor of registry view sub-session.
		 */
		IMPORT_C RSoftwareComponentRegistryView();
		
		/**
			Closes the registry view on the SCR server.
		 */
		IMPORT_C void Close();
		
		/**
	    	Opens a view of the installed software components in the SCR.
			The SCR initializes the view by enumerating the components with the given filter.
		
			If the call to this function completes successfully, then the interface client will access 
			the enumerated records by using @see RSoftwareComponentRegistryView::NextComponentL.
			

			N.B. After this function completes successfully, NextComponentLor NextComponentSetL functions 
			can be used together. However, GetComponentIdsL function cannot be called before or after
			these functions. 
			
			@param aCompReg An active SCR connection.
			@param aFilter Pointer to the filter object which will be used by the SCR to enumerate the software components.
						   If no filter is provided, all software components in the SCR are enumerated.
			@leave A system-wide error code.

			@capability  ReadUserData Accessing list of components installed by the user requires ReadUserData
		*/
		IMPORT_C void OpenViewL(const RSoftwareComponentRegistry& aCompReg, CComponentFilter* aFilter=NULL);		
		
		/**
			Retrieves the next software component entry from the view which has already been 
			created in the SCR with @see RSoftwareComponentRegistryView::OpenViewL.
			
			The locale for the component name and vendor of the retrieved component can be specified.
				
			@param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
						   the names. If it fails, then the non-localized names are returned. If non-localized versions
						   couldn't be found too, the first available language for the component is retrieved.
						   If KNonLocalized is provided, then the non-localized component and vendor names are returned,
						   and if non-localized versions don't exist, the function will leave with KErrScrUnsupportedLocale.
						   If a particular language is provided, then the names are searched for this particular language
						   and its downgraded languages. If there is no name and vendor with the provided language, 
						   then the function leaves with KErrScrUnsupportedLocale.
			@return Returns the next component matching the given filter. If no more components can be found, returns NULL.
				     The ownership is transferred to the calling client.	     
			@leave KErrScrUnsupportedLocale The requested component name/vendor couldn't be found for the specified locale.
			@leave Or a system-wide error code.

			@capability  ReadUserData Accessing list of components installed by the user requires ReadUserData
		 */
		IMPORT_C CComponentEntry* NextComponentL(TLanguage aLocale=KUnspecifiedLocale) const;
		
		/**
			Retrieves the next software component entry from the view which has already been 
			created in the SCR with @see RSoftwareComponentRegistryView::OpenViewL.
			
			The locale for the component name and vendor of the retrieved component can be specified.
			
			@param aEntry An output parameter for returning the current component registry entry of the view.	
			@param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
						   the names. If it fails, then the non-localized names are returned. If non-localized versions
						   couldn't be found too, the first available language for the component is retrieved.
						   If KNonLocalized is provided, then the non-localized component and vendor names are returned,
						   and if non-localized versions don't exist, the function will leave with KErrScrUnsupportedLocale.
						   If a particular language is provided, then the names are searched for this particular language
						   and its downgraded languages. If there is no name and vendor with the provided language, 
						   then the function leaves with KErrScrUnsupportedLocale.		
			@return EFalse, if the end of the view has been reached. Otherwise, ETrue.  		
			@leave KErrScrUnsupportedLocale The requested component name/vendor couldn't be found for the specified locale.
			@leave Or a system-wide error code.

			@capability  ReadUserData Accessing list of components installed by the user requires ReadUserData
	     */
		IMPORT_C TBool NextComponentL(CComponentEntry& aEntry, TLanguage aLocale=KUnspecifiedLocale) const;				
		
		/**
			Retrieves the set of software component records from a given view which has already been 
			created in the SCR. The number of returned records is limited by the size of the component
			list.  
			
			The locale for the component names and vendors of the retrieved components can be specified.
						
			@param aMaxCount The maximum size of the returned components array.
			@param aComponentList The list of component entry objects retrieved from the given view. 
			@param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
						   the names. If it fails, then the non-localized names are returned. If non-localized versions
						   couldn't be found too, the first available language for the component is retrieved.
						   If KNonLocalized is provided, then the non-localized component and vendor names are returned,
						   and if non-localized versions don't exist, the function will leave with KErrScrUnsupportedLocale.
						   If a particular language is provided, then the names are searched for this particular language
						   and its downgraded languages. If there is no name and vendor with the provided language, 
						   then the function leaves with KErrScrUnsupportedLocale.		
			@leave KErrScrUnsupportedLocale The requested component name/vendor couldn't be found for the specified locale.
			@leave Or a system-wide error code.

			@capability  ReadUserData Accessing list of components installed by the user requires ReadUserData
		 */
		IMPORT_C void NextComponentSetL(TInt aMaxCount, RPointerArray<CComponentEntry>& aComponentList, TLanguage aLocale=KUnspecifiedLocale) const;
	
	private:
		void CheckSubSessionHandleL() const;     //Checks for Valid SubSessionHandle
		TInt SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const;
		template <class C> friend HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize);
		template <class T, class C> friend TBool GetObjectL(const C& aConnection, T& aObject, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RArray<T>& aArray);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray);
		};
	
	NONSHARABLE_CLASS(RSoftwareComponentRegistryFilesList) : public RScsClientSubsessionBase
	/**
		This class supports listing of files registered to a particular component.
	 */
		{
	public:
		/**
			Default constructor of registry file list sub-session.
			*/
		IMPORT_C RSoftwareComponentRegistryFilesList();
		
		/**
			Closes the registry view on the SCR server.
		 */
		IMPORT_C void Close();
		
		/**
			Opens the list of files in the SCR owned by a given software component.
			The SCR initializes the list by enumerating the files of the given component Id.
				
			If the call to this function completes successfully, then the interface client will access 
			to the enumerated files by using NextFileL/NextFileSetL interface functions.
			
			@param aCompReg An active SCR connection.	
			@param aComponentId The unique ID of the given software component.
			 
			@leave A system-wide error code.
		 */
		IMPORT_C void OpenListL(const RSoftwareComponentRegistry& aCompReg, TComponentId aComponentId);		
		
		/**
			Retrieves the next file name from the list which has already been created in the SCR with OpenListL.
										
			@return Returns the next file name. If no more files can be found, returns NULL
			@leave A system-wide error code.	
	     */
		IMPORT_C HBufC* NextFileL() const;	
		
		/**
			Retrieves the set of records from a given file list which has already been created in the SCR. 
								
			@param aMaxCount The maximum size of the files array returned.
			@param aFileList The list of file names retrieved from the given list. 
			@leave A system-wide error code.
		 */
		IMPORT_C void NextFileSetL(TInt aMaxCount, RPointerArray<HBufC>& aFileList) const;
	
	private:
		void CheckSubSessionHandleL() const;     //Checks for Valid SubSessionHandle
		TInt SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const;
		template <class C> friend HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize);
		template <class T, class C> friend T* GetObjectL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray);
	};


	NONSHARABLE_CLASS(RApplicationRegistryView) : public RScsClientSubsessionBase
	/**
        This class opens a sub-session to the SCR server and sends a request to create a complete Application Registration view.
        This class can only be used by Apparc. However RApplicationInfoView is accessible by all.
	*/
	{
	public:
	    /**
	      Default constructor of registry view sub-session.
	     */
	      IMPORT_C RApplicationRegistryView();
	            
	      /**
	           Closes the registry view on the SCR server.
	      */
	      IMPORT_C void Close();


	      /**
	           Opens a complete view of Application Registration in the SCR.
	            
	           If the call to this function completes successfully, then the interface client will access 
	           the enumerated records by using @see RApplicationRegistryView::GetNextApplicationRegistrationInfoL.
	                
	           N.B. After this function completes successfully, GetNextApplicationRegistrationInfoL function
	           can be used  
	                
	           @param aCompReg An active SCR connection.
	           @param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
	           if aLocale is provided, then Application Registration Info for that locale is returned(if present in SCR).
	           If aLocale is provided but the localizable info for that locale is not present in the SCR then 
	           its(aLocale) downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
	           the reg details. If it fails, then application registration info for that application is not returned.
	           If aLocale is not provided i.e. KUnspecifiedLocale then it takes the current locale or its downgradables.             
	           @leave A system-wide error code.
	     */
	     IMPORT_C void OpenViewL(const RSoftwareComponentRegistry& aCompReg, TLanguage aLocale=KUnspecifiedLocale);                 
	     
		 /**
	           Opens a view of the Application Registration information for a list of AppUids.
	            
	           If the call to this function completes successfully, then the interface client will access 
	           the enumerated records by using @see RApplicationRegistryView::GetNextApplicationRegistrationInfoL.
	                
	           N.B. After this function completes successfully, GetNextApplicationRegistrationInfoL function
	           can be used  
	                
	           @param aCompReg An active SCR connection.
			   @param aAppRegAppUids Array of AppUids to be fetched.
	           @param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
	           if aLocale is provided, then Application Registration Info for that locale is returned(if present in SCR).
	           If aLocale is provided but the localizable info for that locale is not present in the SCR then 
	           its(aLocale) downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
	           the reg details. If it fails, then application registration info for that application is not returned.
	           If aLocale is not provided i.e. KUnspecifiedLocale then it takes the current locale or its downgradables.             
	           @leave A system-wide error code.
	     */
		 
	     IMPORT_C void OpenViewL(const RSoftwareComponentRegistry& aCompReg, const RArray<TUid>& aAppRegAppUids, TLanguage aLocale=KUnspecifiedLocale);
	                
	     /**
	           Retrieves the next Application Registration entry from the view which has already been 
	           created in the SCR with @see RApplicationRegistryView::OpenViewL.                                
	                                                  
            @param aNoOfEntries Number of AppInfo Entries to be fetched in a single call.
            @param aApplicationRegistration An output parameter for returning the array of CApplicationRegistrationData.                 
			@return Once all the entries have been retrieved, this call returns an empty array in aApplicationRegistration.
            @leave A system-wide error code
         */
	     IMPORT_C void GetNextApplicationRegistrationInfoL(TInt aNoOfEntries, RPointerArray<CApplicationRegistrationData>& aApplicationRegistration) const; 
	            
        private:
	            TInt iNoOfEntries;
	            TInt SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const;
	            template <class C> friend HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize);
	            template <class T, class C> friend TBool GetObjectL(const C& aConnection, T& aObject, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
	            template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RArray<T>& aArray);
	            template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray);
	            
	};  //End of RApplicationRegistryView
	
	
	
NONSHARABLE_CLASS(RApplicationInfoView) : public RScsClientSubsessionBase
/**
	This class opens a sub-session to the SCR server and sends a request to create a AppInfo view
	by using the supplied filter on the server side. 
	It returns information associated to a list of applications, information includes the full pathname to the application and the application's caption and short caption information @see TAppRegInfo, 
	however localizable specific information is retrieved only if the app has localizable information 
	matching the locale with which the subsession was opened or matching the current device language.
	It is different from class RApplicationRegistryView in the following ways:
	1. This subsession has the ability to filter the applications based on certain parameters @see CAppInfoFilter.
	2. This returns specific information contained in TAppRegInfo, however class RApplicationRegistryView returns all the information associated with an app in CApplicationRegistrationData.
	3. Functionality provided by this class can be used by all, however class RApplicationRegistryView can be used only by AppArc.
	 
*/
{
	public:
		/**
			Default constructor of AppInfo sub-session.
		 */
		IMPORT_C RApplicationInfoView();
		
		/**
			Closes the registry view on the SCR server.
		*/
		IMPORT_C void Close();

		/**
	    	Opens a view of Application Info in the SCR.
		
			If the call to this function completes successfully, then the interface client can access 
			the enumerated records by using @see RApplicationInfoView::GetNextAppInfoL.
			
			N.B. After this function completes successfully, GetNextAppInfoL function
			can be used  
			
			@param aCompReg An active SCR connection.
			@param aAppInfoFilter Pointer to the filter object which will be used by the SCR to enumerate the Application Info. Also @see CAppInfoFilter.
	        @param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
		    if aLocale is provided, then application info for that locale is returned(if present in SCR).
	        If aLocale is provided but the localizable info for that locale is not present in the SCR then 
	        its(aLocale) downgraded path (@see BaflUtils::GetDowngradePathL) is used to find
	        the reg details. If it fails, then the non-localized application info is returned.
	        If aLocale is not provided i.e. KUnspecifiedLocale then it takes the current locale or its downgradables.             
			@leave A system-wide error code.
			@capability  ReadUserData, Accessing list of App Info requires ReadUserData
		*/
		IMPORT_C void OpenViewL(const RSoftwareComponentRegistry& aCompReg, CAppInfoFilter* aAppInfoFilter = NULL,TLanguage aLocale = KUnspecifiedLocale);

		/**
			Retrieves the next AppInfo entry from the view which has already been 
			created in the SCR with @see RApplicationInfoView::OpenViewL.
			
			@param aNoOfEntries Number of AppInfo Entries to be fetched in a single call.
			@param aAppInfo An output parameter for returning the array of AppInfo.		
			@return Once all the entries have been retrieved, this call returns an empty array in aAppInfo.
			@leave A system-wide error code.
	     */
		IMPORT_C void GetNextAppInfoL(TInt aNoOfEntries, RPointerArray<TAppRegInfo>& aAppInfo) const;

	private:
		TInt SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const;
		template <class C> friend HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize);
		template <class T, class C> friend TBool GetObjectL(const C& aConnection, T& aObject, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RArray<T>& aArray);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray);

};//End of RApplicationInfoView

NONSHARABLE_CLASS(RRegistrationInfoForApplication) : public RScsClientSubsessionBase
/**
	This class opens a sub-session to the SCR server with a particular application uid 
	and fetches the respective details for this particular application only.
*/
    {
    public:
		/**
			Default constructor of registry view sub-session.
		*/
		IMPORT_C RRegistrationInfoForApplication();

		/**
			Closes the registry view on the SCR server.
		 */
		IMPORT_C void Close();

		/**
            Creates a new subsession for the given App Uid. 
		    @param aCompReg An active SCR connection.                            
		    @param aAppUid The Application ID to be used for further queries.
			@param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
						   If KUnspecifiedLocale is provided, the current locale with its nearest language
						   (@see  BaflUtils::GetEquivalentLanguageList) will be used .
		    @leave KErrNotFound If aAppUid is not found.
                   Or a system-wide error code.
		*/
		IMPORT_C void OpenL(const RSoftwareComponentRegistry& aCompReg,const TUid aAppUid,TLanguage aLocale=KUnspecifiedLocale);

		/**
			Returns the list of Service Uids for the application. 
                                                    
            @param aServiceUids The list of Service Uids retrieved for the application. 
            @leave A system-wide error code.
        */		
		IMPORT_C void GetAppServicesUidsL(RArray<TUid>& aServiceUids) const;

        /**
            Returns the Application Language(Locale) for the application. 
			This is identified after mapping the Application languages to the nearest device language.                                       
            @return TLanguage The Application Language(Locale) retrieved for the application. 
            @leave A system-wide error code.
        */		
        IMPORT_C TLanguage ApplicationLanguageL() const;

        /**
            Returns the default Screen Number for the application. 
                                        
            @return  The default Screen Number retrieved for the application. 
            @leave A system-wide error code.
        */        
        IMPORT_C TInt DefaultScreenNumberL() const;

        /**
            Returns the number of defined icons for the application and locale. 
            
            @return  The number of defined icons retrieved for the given locale of the application. 
            @leave A system-wide error code.
        */         
		IMPORT_C TInt NumberOfOwnDefinedIconsL() const;
		
		/**
            Returns the array of files owned by the application .
		                                                
            @param aAppOwnedFiles returns array of Files owned by the application. 
            @leave A system-wide error code.                     		           
		*/     
		IMPORT_C void GetAppOwnedFilesL(RPointerArray<HBufC>& aAppOwnedFiles) const;
		
		/**
            Returns the characteristics of the application. 
		                                                        
            @param aApplicationCharacteristics returns capabilty of the application.  
            @leave A system-wide error code.                       		                    
		*/        
		IMPORT_C void GetAppCharacteristicsL(TApplicationCharacteristics& aApplicationCharacteristics) const;
		/**
            Returns the file name of the icon associated with the application. 
		                                                                
            @param aFullFileName  returns file name of the icon associated with the application. 
            @leave A system-wide error code.                        		                            
		*/                
		IMPORT_C void GetAppIconL(HBufC*&  aFullFileName) const;
		        
		/**
            Returns the file name of the icon associated with the application, given a View Uid. 

            @param aViewUid view Uid of the app                                                            
            @param aFullFileName  returns file name of the icon of the application.   
            @leave A system-wide error code.                 		                                   
		*/ 
		IMPORT_C void GetAppViewIconL(TUid aViewUid, HBufC*& aFullFileName) const;
		
		/** Returns the view information published by the application for the current locale.

		    @param aAppViewInfoArray On return, the array contains information on all of the views 
		    published by the specified application for the current locale.
		    @see CAppViewData 
            @leave A system-wide error code.		    
		**/

		 IMPORT_C void GetAppViewsL(RPointerArray<CAppViewData>& aAppViewInfoArray) const;

	private:
		void CheckSubSessionHandleL() const;     //Checks for Valid SubSessionHandle
		TInt SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const;
		template <class C> void SendDependencyArgsL(TInt aFunction, const C& aSupplierId, const CGlobalComponentId& aDependantGlobalCompId);
		template <class C> friend HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize);
		template <class T, class C> friend T* GetObjectL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
		template <class T, class C> friend TBool GetObjectL(const C& aConnection, T& aObject, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RArray<T>& aArray);
		template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray);
		
	}; //End of RRegistrationInfoForApplication

NONSHARABLE_CLASS(RApplicationRegistrationInfo) : public RScsClientSubsessionBase
/**
    This class opens a sub-session to the SCR server and is used to retrieve application information.

 */   
    {
    public:
        /**
            Default constructor of registry view sub-session.
         */
        IMPORT_C RApplicationRegistrationInfo();
        
        /**
            Closes the registry view sub-session on the SCR server.
         */
        IMPORT_C void Close();

        /**
            Creates a new subsession with SCR. 
            
            Once the call to this function succeeds, the client will be able to query SCR for various information 
            such as data type and service information. 
            
            If a transaction is owned by another session is in progress on the SCR server, then this request 
            will be rejected.
            
            @param aCompReg An active SCR connection.                          
            @leave A system-wide error code.
        */
        IMPORT_C void OpenL(const RSoftwareComponentRegistry& aCompReg);

        /**
            Returns the App Uid for a given Service Uid that handles the specified datatype with the highest priority. 
                                        
            @param aName The Data Type Text.
            @param aServiceUid  The Service Uid.
            @return  The App Uid for the given Data type and Service Uid. 
            @leave A system-wide error code.
        */      
        IMPORT_C TUid GetAppForDataTypeAndServiceL(const TDesC &aName, const TUid aServiceUid) const;
        
        /**
            Return the App Uid for the given Data type with the highest priority. 
                                        
            @param aName The Data Type Text.
            @return  The App Uid for the given Data type. 
            @leave KErrNotFound If the AppUid is not found
                   Or a system-wide error code.
        */
        IMPORT_C TUid GetAppForDataTypeL(const TDesC &aName) const;

        /**
            Return the Service Info details for the set parameters of the filter. 
                                        
            @param aAppServiceInfoFilter A pointer to AppServiceInfoFilter enumerating the parameters on the basis of which serviceinfo details are to be fetched.
            @param aAppServiceInfoArray  On return contains the service info details assoicated with the parameters set in the filter.
            @param aLocale               The language code of the requested locale. The default value is KUnspecifiedLocale.
                                         If KUnspecifiedLocale is provided, the current locale with its nearest language (@see  BaflUtils::GetEquivalentLanguageList) is used to find
                                         the names.             
            @leave KErrNotFound If the Service Info details are not found
                   Or a system-wide error code.
        */
        IMPORT_C void GetServiceInfoL(CAppServiceInfoFilter* aAppServiceInfoFilter, RPointerArray<CServiceInfo>& aAppServiceInfoArray, TLanguage aLocale=KUnspecifiedLocale) const;

        /**
            Return the basic application information contained in  TAppRegInfo for a given application present in the SCR DB.
                    
            @param aAppUid  An application's Uid.                  
            @param aAppInfo An output parameter for returning the array of AppInfo. 
            @param aLocale The language code of the requested locale. The default value is KUnspecifiedLocale.
                           If KUnspecifiedLocale is provided, the current locale with its nearest language (@see  BaflUtils::GetEquivalentLanguageList) is used to find
                           the names.  
            @return ETrue if found, EFalse if application is not found
                                    
            @leave A system-wide error code.
        */
        IMPORT_C TBool GetAppInfoL(TUid aAppUid, TAppRegInfo& aAppInfo, TLanguage aLocale=KUnspecifiedLocale);
        
        
    private:
        void CheckSubSessionHandleL() const;     //Checks for Valid SubSessionHandle
        TInt SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const;
        template <class C> void SendDependencyArgsL(TInt aFunction, const C& aSupplierId, const CGlobalComponentId& aDependantGlobalCompId);
        template <class C> friend HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize);
        template <class T, class C> friend T* GetObjectL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
        template <class T, class C> friend TBool GetObjectL(const C& aConnection, T& aObject, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs);
        template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RArray<T>& aArray);
        template <class C, class T> friend void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray);
        
    }; //End of RApplicationRegistrationInfo       

}// End of namespace Usif
#endif // SOFTWARECOMPONENTREGISTRY_H
