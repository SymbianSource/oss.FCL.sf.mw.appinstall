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
* siftransportclient.h
* This file defines the client side of the Reference Installer.
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef SIFREFINSTALLERCLIENT_H
#define SIFREFINSTALLERCLIENT_H

#include <usif/sif/siftransportclient.h>

namespace Usif
	{
	/**
	The client side of the Reference Installer. This class uses the SIF Transport to send
	software management requests to the Reference Installer Server that resides in a separate
	process. The Reference SIF Plug-in is a client of the Reference Installer Server so it
	uses this class to connect to the Server process and send SIF requests .
	 */
	class RSifRefInstallerClient
		{
	public:
		RSifRefInstallerClient();

		/**
		Connects a client to the Reference Installer Server.
		 
		@return	Symbian OS error code where KErrNone indicates
				success and any other value indicates failure.
		*/
		TInt Connect();

		/**
		Disconnects a client from the Reference Installer Server.
		*/
		void Close();

		/**
		Returns the details of a component to be installed by file name
		
		@param aFileName, The file name of a component to be queried
		@param aComponentInfo On return, contains the details of a component
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		void GetComponentInfo(const TDesC& aFileName, CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		/**
		Returns the details of a component to be installed by file handle
		
		@param aFileHandle, The file handle of a component to be queried
		@param aComponentInfo On return, contains the details of a component
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		void GetComponentInfo(RFile& aFileHandle, CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		/**
		Installs a component by file name
		
		@param aFileName The file name of a component to be installed
		@param aArguments The array of opaque params for the Reference Installer Server.
		An empty array may be passed. The following param is defined for the "SCOMO
		Install Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		@param aResults The array of opaque params returned from the Reference Installer
		Server.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		void Install(const TDesC& aFileName, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus);

		/**
		Installs a component by file handle
		
		@param aFileHandle The file handle of a component to be installed
		@param aArguments The array of opaque params for the Reference Installer Server.
		An empty array may be passed.  The following param is defined for the "SCOMO
		Install Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		@param aResults The array of opaque params returned from the Reference Installer
		Server.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		void Install(RFile& aFileHandle, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus);

		/**
		Uninstalls a component
		
		@param aComponentId The id of a component to be uninstalled
		@param aArguments The array of opaque params for the Reference Installer
		Server. An empty array may be passed.
		@param aResults The array of opaque results returned from a SIF plug-in.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		void Uninstall(TComponentId aComponentId, const COpaqueNamedParams& aArguments,
								COpaqueNamedParams& aResults, TRequestStatus& aStatus);

		/**
		Activates a component
		
		@param aComponentId The id of a component to be activated
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		void Activate(TComponentId aComponentId, TRequestStatus& aStatus);

		/**
		Deactivates a component
		
		@param aComponentId The id of a component to be deactivated
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		void Deactivate(TComponentId aComponentId, TRequestStatus& aStatus);

		/**
		Cancels an ongoing asynchronous request
		
		This is a synchronous call. When it returns the original asynchronous call is completed.
		*/
		void CancelOperation();

	private:
		RSifRefInstallerClient(const RSifRefInstallerClient&);
		RSifRefInstallerClient& operator=(const RSifRefInstallerClient&);

		RSifTransportClient iTransporter;
		};

	} // namespace Usif

#endif // SIFREFINSTALLERCLIENT_H
