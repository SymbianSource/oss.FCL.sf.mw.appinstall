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
* SisRegistry - class CSisRegistryDependency implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include <s32strm.h>
#include "sisregistrydependency.h"
#include "sisdependency.h"
#include "sisuid.h"
#include "sisversionrange.h"
#include "sisversion.h"
#include "versionutil.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "scrhelperutil.h"
#endif

using namespace Swi;


EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewL(const Sis::CDependency& aDependency)
	{
	CSisRegistryDependency* self = NewLC(aDependency);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewLC(const Sis::CDependency& aDependency)
	{
	CSisRegistryDependency* self = new(ELeave) CSisRegistryDependency();
	CleanupStack::PushL(self);
	self->ConstructL(aDependency);
	return self;
	}

EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewL(const CSisRegistryDependency& aDependency)
	{
	CSisRegistryDependency* self = NewLC(aDependency);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewLC(const CSisRegistryDependency& aDependency)
	{
	CSisRegistryDependency* self = new(ELeave) CSisRegistryDependency();
	CleanupStack::PushL(self);
	self->ConstructL(aDependency);
	return self;
	}

EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewL(RReadStream& aStream)
	{
	CSisRegistryDependency* self = NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewLC(RReadStream& aStream)
	{
	CSisRegistryDependency* self = new(ELeave) CSisRegistryDependency();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewL(const TUid aUid, const TVersion aFromVersion, const TVersion aToVersion)
	{
	CSisRegistryDependency* self = NewLC(aUid, aFromVersion, aToVersion);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CSisRegistryDependency* CSisRegistryDependency::NewLC(const TUid aUid, const TVersion aFromVersion, const TVersion aToVersion)
	{
	CSisRegistryDependency* self = new(ELeave) CSisRegistryDependency();
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aFromVersion, aToVersion);
	return self;
	}
#endif

CSisRegistryDependency::CSisRegistryDependency():
	iFromVersion(KIrrelevant, KIrrelevant, KIrrelevant), 
	iToVersion(KIrrelevant, KIrrelevant, KIrrelevant)
	{
	}

void CSisRegistryDependency::ConstructL(const Sis::CDependency& aDependency)
	{
	iUid = aDependency.Uid().Uid();
	
	// from version
 	Set(iFromVersion, aDependency.VersionRange()->From());
	// to version
	// if there is to version; then get it; otherwise specify it as KIrrelevant 
	if (aDependency.VersionRange()->To()!=NULL)
		{
 		Set(iToVersion, *(aDependency.VersionRange()->To()));
		}
	}

void CSisRegistryDependency::ConstructL(const CSisRegistryDependency& aDependency)
	{
	iUid = aDependency.Uid();
	iFromVersion = aDependency.iFromVersion;
	iToVersion   = aDependency.iToVersion;
	}
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CSisRegistryDependency::ConstructL(const TUid aUid, const TVersion aFromVersion, const TVersion aToVersion)
	{
	iUid = aUid;
	iFromVersion = aFromVersion;
	iToVersion   = aToVersion;
	}
#endif
	
void CSisRegistryDependency::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}
	
EXPORT_C void CSisRegistryDependency::InternalizeL(RReadStream& aStream)
	{
	iUid.iUid = aStream.ReadInt32L();
	
	aStream >> iFromVersion;
	aStream >> iToVersion;
	}
	
EXPORT_C void CSisRegistryDependency::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iUid.iUid);
	
	aStream << iFromVersion;
	aStream << iToVersion;
	}

EXPORT_C TBool CSisRegistryDependency::IsCompatible(const TVersion& aVersion) const
	{
    return ((aVersion >= iFromVersion)&&(aVersion <= iToVersion));
	} 
