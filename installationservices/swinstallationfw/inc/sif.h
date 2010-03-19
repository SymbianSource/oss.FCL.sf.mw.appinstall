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
* This file defines the SIF API.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SIF_H
#define SIF_H

#include <usif/sif/siftransportclient.h>

namespace Usif
	{
	/**
		The RSoftwareInstall class provides an asynchronous interface to the Software
		Install Framework. This class wraps all the details of client-server communication
		with the SIF server residing in a separate process. This API is asynchronous and
		therefore clients may issue concurrect requests. However, the SIF constrains
		concurrent operations by default. A client issuing an Install or Uninstall request
		may set the aExclusiveOperation flag to EFalse in order to allow for concurrent
		execution of his request. However, even if this flag is set to EFalse, a request
		may fail with KErrServerBusy if a corresponding underlying installer doesn't
		support concurrent operations.

		This class is intended to be used by SIF client applications, for example, application
		or device managers.

		Software @see TComponentId needed for the Uninstall/Activate/Deactivate APIs is a unique
		identifier of an installed component regardless of its type across the system.  When installing
		a package containing other embedded packages each embedded component gets its own @see
		TComponentId in the descending order. For example, we have a package P1 that contains an
		embedded package P2 that contains yet another embedded package P3.
		After installation the components may be identified by the following Ids:
		- Id of Component 1 (Package 1): 6 (example value)
		- Id of Component 2 (Package 2): 7
		- Id of Component 3 (Package 3): 8
		Component 3 with Id = 8 is the last installed/most embedded component and therefore its id
		is the highest.

		All the SIF operations may require special capabilities in order to complete successfully.
		This check may be implemented in a SIF plug-in. For example, a silent installation may require
		the TrustedUI capability. However, this is expected behavour that standard operations
		don't require any capabilities. Please see @see Usif::CSifPlugin description for further details
		if there are any deviations from this rule.
	 */

	class RSoftwareInstall
		{
	public:
		IMPORT_C RSoftwareInstall();

		/**
		   Connects a client to the SIF server.
		 
		@return	Symbian OS error code where KErrNone indicates
				success and any other value indicates failure.
		*/
		IMPORT_C TInt Connect();

		/**
		   Disconnects a client from the SIF server.
		*/
		IMPORT_C void Close();

		/**
		Returns the details of a component to be installed by file name
		 
		@param aFileName The file name of a component to be queried
		@param aComponentInfo On return, contains the details of a component
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		IMPORT_C void GetComponentInfo(const TDesC& aFileName, CComponentInfo& aComponentInfo,
										TRequestStatus& aStatus);

		/**
		 * Returns the details of a component to be installed by file handle
		 
		@param aFileHandle The file handle of a component to be queried
		@param aComponentInfo On return, contains the details of a component
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		IMPORT_C void GetComponentInfo(RFile& aFileHandle, CComponentInfo& aComponentInfo,
										TRequestStatus& aStatus);

		/**
		Installs a component by file name
		
		@param aFileName The file name of a component to be installed
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation If this flag is set to ETrue, the Install
		operation is executed in the Exclusive mode. In this mode the SIF
		Server executes only one operation at a time. If there is another
		operation in progress the SIF Server returns KErrServerBusy.
		If this flag is set to EFalse, the SIF Server executes the operation
		even if there is a concurrent operation in progress. The default value
		is ETrue.
		*/
		IMPORT_C void Install(const TDesC& aFileName, TRequestStatus& aStatus,
							TBool aExclusiveOperation = ETrue);

		/**
		Installs a component by file name using opaque arguments/results
		
		@param aFileName The file name of a component to be installed
		@param aArguments The array of opaque params for a SIF plug-in. An empty
		array may be passed.  The following param is defined for the "SCOMO Install
		Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		@param aResults The array of opaque params returned from a SIF plug-in.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation If this flag is set to ETrue, the Install
		operation is executed in the Exclusive mode. In this mode the SIF
		Server executes only one operation at a time. If there is another
		operation in progress the SIF Server returns KErrServerBusy.
		If this flag is set to EFalse, the SIF Server executes the operation
		even if there is a concurrent operation in progress. The default value
		is ETrue.
		*/
		IMPORT_C void Install(const TDesC& aFileName, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus,
								TBool aExclusiveOperation = ETrue);

		/**
		Installs a component by file handle
		
		@param aFileHandle The file handle of a component to be installed
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation If this flag is set to ETrue, the Install
		operation is executed in the Exclusive mode. In this mode the SIF
		Server executes only one operation at a time. If there is another
		operation in progress the SIF Server returns KErrServerBusy.
		If this flag is set to EFalse, the SIF Server executes the operation
		even if there is a concurrent operation in progress. The default value
		is ETrue.
		*/
		IMPORT_C void Install(RFile& aFileHandle, TRequestStatus& aStatus,
								TBool aExclusiveOperation = ETrue);

		/**
		Installs a component by file handle using opaque arguments/results
		
		@param aFileHandle The file handle of a component to be installed
		@param aArguments The array of opaque params for a SIF plug-in. An empty
		array may be passed.  The following param is defined for the "SCOMO Install
		Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		@param aResults The array of opaque params returned from a SIF plug-in.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation If this flag is set to ETrue, the Install
		operation is executed in the Exclusive mode. In this mode the SIF
		Server executes only one operation at a time. If there is another
		operation in progress the SIF Server returns KErrServerBusy.
		If this flag is set to EFalse, the SIF Server executes the operation
		even if there is a concurrent operation in progress. The default value
		is ETrue.
		*/
		IMPORT_C void Install(RFile& aFileHandle, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus,
								TBool aExclusiveOperation = ETrue);

		/**
		Uninstalls a component
		
		@param aComponentId The id of a component to be uninstalled
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation If this flag is set to ETrue, the Uninstall
		operation is executed in the Exclusive mode. In this mode the SIF
		Server executes only one operation at a time. If there is another
		operation in progress the SIF Server returns KErrServerBusy.
		If this flag is set to EFalse, the SIF Server executes the operation
		even if there is a concurrent operation in progress. The default value
		is ETrue.
		*/
		IMPORT_C void Uninstall(TComponentId aComponentId, TRequestStatus& aStatus,
								TBool aExclusiveOperation = ETrue);

		/**
		Uninstalls a component
		
		@param aComponentId The id of a component to be uninstalled
		@param aArguments The array of opaque params for a SIF plug-in. An empty
		array may be passed.
		@param aResults The array of opaque results returned from a SIF plug-in.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation If this flag is set to ETrue, the Uninstall
		operation is executed in the Exclusive mode. In this mode the SIF
		Server executes only one operation at a time. If there is another
		operation in progress the SIF Server returns KErrServerBusy.
		If this flag is set to EFalse, the SIF Server executes the operation
		even if there is a concurrent operation in progress. The default value
		is ETrue.
		*/
		IMPORT_C void Uninstall(TComponentId aComponentId, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus,
								TBool aExclusiveOperation = ETrue);

		/**
		Activates a component.

  		The SCOMO state of a component can be changed at any time through the SIF API. However, capability
		checking may apply depending on the particular installer. For example, a Python installer may require the 
		ECapabilityWriteUserData capability to activate or deactivate a Python component.
		
		@param aComponentId The id of a component to be activated
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		IMPORT_C void Activate(TComponentId aComponentId, TRequestStatus& aStatus);

		/**
		Deactivates a component.

  		The SCOMO state of a component can be changed at any time through the SIF API. However, capability
		checking may apply depending on the particular installer. For example, a Python installer may require the 
		ECapabilityWriteUserData capability to activate or deactivate a Python component.
		
		@param aComponentId The id of a component to be deactivated
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		IMPORT_C void Deactivate(TComponentId aComponentId, TRequestStatus& aStatus);

		/**
		Cancels an ongoing asynchronous request
		
		This is a synchronous call. When it returns the original asynchronous call is completed.
		*/
		IMPORT_C void CancelOperation();
	
#ifdef _DEBUG
		friend class CSifOperationStep;
#endif //_DEBUG

	private:
		RSifTransportClient iTransport;
		};

	} // namespace Usif

#endif // SIF_H
