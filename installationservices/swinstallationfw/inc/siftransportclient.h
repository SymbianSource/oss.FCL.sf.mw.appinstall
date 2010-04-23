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
* This file defines the client side of the SIF Transport.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SIFTRANSPORTCLIENT_H
#define SIFTRANSPORTCLIENT_H

#include <usif/sif/sifcommon.h>
#include <scs/scsclient.h>
#include <e32base.h>
#include <f32file.h>

namespace Usif
	{

	/**
	A client part of the SIF Transport. This class sends software management requests
	across the process boundary to a specified Transport Server.
	*/
	class RSifTransportClient : public RScsClientBase
		{
	public:
		IMPORT_C RSifTransportClient();

		/**
		 Connects a client to the SIF server.
		 
		@return	Symbian OS error code where KErrNone indicates
				success and any other value indicates failure.
		*/
		IMPORT_C TInt Connect(const TDesC& aServerName, const TVersion& aRequiredVersion, const TDesC& aServerImageName, TUid aServerUid);

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
		operation is executed in the Exclusive mode. In this mode the SIF Transport
		Server executes only one operation at a time. If there is another
		operation in progress the SIF Server returns KErrServerBusy.
		If this flag is set to EFalse, the SIF Server executes the operation
		even if there is a concurrent operation in progress.
		*/
		IMPORT_C void Install(const TDesC& aFileName, TRequestStatus& aStatus,
							TBool aExclusiveOperation);

		/**
		Installs a component by file name using opaque arguments/results
		
		@param aFileName The file name of a component to be installed
		@param aArguments The array of opaque params for the plug-in. An empty
		array may be passed.  The following param is defined for the "SCOMO Install
		Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		@param aResults The array of opaque params returned from the plug-in.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation Enables the ExclusiveOperation mode. Please
		see @see RSifTransportClient::Install by file name for details.
		*/
		IMPORT_C void Install(const TDesC& aFileName, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus,
								TBool aExclusiveOperation);

		/**
		Installs a component by file handle
		
		@param aFileHandle The file handle of a component to be installed
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation Enables the ExclusiveOperation mode. Please
		see @see RSifTransportClient::Install by file name for details.
		*/
		IMPORT_C void Install(RFile& aFileHandle, TRequestStatus& aStatus,
							TBool aExclusiveOperation);

		/**
		Installs a component by file handle using opaque arguments/results
		
		@param aFileHandle The file handle of a component to be installed
		@param aArguments The array of opaque params for the plug-in. An empty
		array may be passed.  The following param is defined for the "SCOMO Install
		Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		@param aResults The array of opaque params returned from the plug-in.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation Enables the ExclusiveOperation mode. Please
		see @see RSifTransportClient::Install by file name for details.
		*/
		IMPORT_C void Install(RFile& aFileHandle, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus,
								TBool aExclusiveOperation);

		/**
		Uninstalls a component
		
		@param aComponentId The id of a component to be uninstalled
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation Enables the ExclusiveOperation mode. Please
		see @see RSifTransportClient::Install by file name for details.
		*/
		IMPORT_C void Uninstall(TComponentId aComponentId, TRequestStatus& aStatus,
								TBool aExclusiveOperation);

		/**
		Uninstalls a component
		
		@param aComponentId The id of a component to be uninstalled
		@param aArguments The array of opaque params for the plug-in. An empty
		array may be passed.
		@param aResults The array of opaque results returned from the plug-in.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		@param aExclusiveOperation Enables the ExclusiveOperation mode. Please
		see @see RSifTransportClient::Install by file name for details.
		*/
		IMPORT_C void Uninstall(TComponentId aComponentId, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus,
								TBool aExclusiveOperation);

		/**
		Activates a component
		
		@param aComponentId The id of a component to be activated
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		IMPORT_C void Activate(TComponentId aComponentId, TRequestStatus& aStatus);

		/**
		Deactivates a component
		
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

	private:
		void CallSifFunction(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus);
		void PrepareOpaqueParamsL(TIpcArgs& aIpcArgs, const COpaqueNamedParams& aArguments,
									COpaqueNamedParams& aResults);

		HBufC8* iArgBuf;
		HBufC8* iResBuf;
		TInt iOngoingFunction;
		};

	} // namespace Usif

#endif // SIFTRANSPORTCLIENT_H
