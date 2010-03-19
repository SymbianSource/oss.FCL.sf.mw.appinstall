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
* This file defines a SIF plug-in for the Reference Installer.
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef SIFREFPLUGIN_H
#define SIFREFPLUGIN_H

#include <usif/sif/sifplugin.h>
#include <e32base.h>
#include <e32std.h>
#include "sifrefinstallerclient.h"

namespace Usif
	{
	/**
	An active object used by the CSifRefPlugin class to submit asynchronous requests.
	
	ECOM objects and CActive do not interact well - especially since SIFPlugin inherits
	from CBase and double C-inheritance is impossible. So, a separate class is used to drive
	asynchronous software management requests.
	*/
	NONSHARABLE_CLASS(CSifRefPluginActiveImpl) : public CActive
	{
	public:
		static CSifRefPluginActiveImpl* NewL();
		~CSifRefPluginActiveImpl();

		/* CActive interface */
		void RunL();
		void DoCancel();

		/* CSifPlugin interface */
		void GetComponentInfo(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
							CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		void GetComponentInfo(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
							CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		void Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
					const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
					TRequestStatus& aStatus);

		void Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
					const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
					TRequestStatus& aStatus);

		void Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
						const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, 
						TRequestStatus& aStatus);

		void Activate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
						TRequestStatus& aStatus);

		void Deactivate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
						TRequestStatus& aStatus);

	private:
		CSifRefPluginActiveImpl() : CActive(EPriorityStandard) {}
		CSifRefPluginActiveImpl(const CSifRefPluginActiveImpl&);
		CSifRefPluginActiveImpl& operator =(const CSifRefPluginActiveImpl&);
		void ConstructL();
		TBool CommonRequestPreamble(const TSecurityContext& aSecurityContext, const COpaqueNamedParams& aInputParams, 
									COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus);
	private:
		RSifRefInstallerClient iTransporter;
		TRequestStatus* iClientStatus;
		const COpaqueNamedParams* iInputParams;
		COpaqueNamedParams* iOutputParams;
		};

	/*
	An implementation of the CSifPlugin interface for the Reference Installer.
	
	The SIF Server loads and uses this plug-in when the MIME type of a package is 'binary/sif-refpkg'.
	*/
	NONSHARABLE_CLASS(CSifRefPlugin) : public CSifPlugin
		{
	public:
		static CSifRefPlugin* NewL();
		~CSifRefPlugin();

		/* CSifPlugin interface */
		void GetComponentInfo(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
							CComponentInfo& aComponentInfo, TRequestStatus& aStatus);

		void GetComponentInfo(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
							CComponentInfo& aComponentInfo, TRequestStatus& aStatus);
		
		void Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
							TRequestStatus& aStatus);

		void Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
							TRequestStatus& aStatus);

		virtual void Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
								TRequestStatus& aStatus);

		virtual void Activate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								TRequestStatus& aStatus);

		virtual void Deactivate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								TRequestStatus& aStatus);

		void CancelOperation();

	private:
		CSifRefPlugin() {}
		void ConstructL();
		CSifRefPlugin(const CSifRefPlugin &);
		CSifRefPlugin& operator =(const CSifRefPlugin &);

		CSifRefPluginActiveImpl *iImpl;
		};
	} // end namespace Usif

#endif // SIFREFPLUGIN_H
