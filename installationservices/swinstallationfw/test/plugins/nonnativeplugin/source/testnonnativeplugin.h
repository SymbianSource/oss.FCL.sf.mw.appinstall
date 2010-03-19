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
* This class implements a non-native test SIF plug-in
*
*/


/**
 @file
 //@internalComponent
*/

#ifndef TESTNONNATIVEPLUGIN_H
#define TESTNONNATIVEPLUGIN_H

#include <usif/sif/sifplugin.h>
#include <usif/scr/screntries.h>
#include <e32base.h>
#include <e32std.h>

namespace Usif
	{

	NONSHARABLE_CLASS(CTestNonNativePlugin) : public CSifPlugin
		{
	public:
		static CTestNonNativePlugin* NewL();
		~CTestNonNativePlugin();
	
		// MSIFPlugin interface
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
		CTestNonNativePlugin() {}
		void ConstructL();
		CTestNonNativePlugin(const CTestNonNativePlugin &);
		CTestNonNativePlugin & operator =(const CTestNonNativePlugin &);
		};
	} // end namespace Usif

#endif // TESTNONNATIVEPLUGIN_H
