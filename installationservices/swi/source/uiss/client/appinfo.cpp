/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "appinfo.h"


namespace Swi
{

EXPORT_C TAppInfo::TAppInfo(const TDesC& aAppName, const TDesC& aAppVendor,
			 const TVersion& aAppVersion)
	: iAppName(&aAppName), iAppVendor(&aAppVendor), iAppVersion(aAppVersion)
	{
	}

EXPORT_C TAppInfo::TAppInfo(const TAppInfo& aAppInfo)
	:
	iAppName(aAppInfo.iAppName),
	iAppVendor(aAppInfo.iAppVendor),
	iAppVersion(aAppInfo.iAppVersion)
	{
	}

EXPORT_C TAppInfo::TAppInfo()
	: iAppName(0),
	  iAppVendor(0),
	  iAppVersion(TVersion())
	{
	}

EXPORT_C void TAppInfo::ExternalizeL(RWriteStream& aStream) const
	{
	_LIT(KEmptyString, "");
	
	aStream << (iAppName ? *iAppName : KEmptyString());
	aStream << (iAppVendor ? *iAppVendor : KEmptyString());
	aStream.WriteL(TPckgC<TVersion>(iAppVersion));
	}
	
} // namespace Swi

