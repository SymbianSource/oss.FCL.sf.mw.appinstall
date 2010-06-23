/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* sisregistrylogging class implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
 
#include "sisregistryobject.h"
#include "sisregistrylog.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "sisregistrylogversion.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "log.h"
using namespace Swi;


CLogEntry* CLogEntry::NewL(const CSisRegistryObject& aObject,TSwiLogTypes InstallInfo)
	{
	CLogEntry* self = CLogEntry ::NewLC(aObject,InstallInfo);
	CleanupStack::Pop(self);
	return self;
	}

CLogEntry* CLogEntry::NewLC(const CSisRegistryObject& aObject,TSwiLogTypes InstallInfo)
	{
	CLogEntry* self = new(ELeave) CLogEntry;
	CleanupStack::PushL(self);
	self->ConstructL(aObject,InstallInfo);
	return self;
	}
	
 CLogEntry* CLogEntry::NewL(RReadStream& aStream)
	{
	CLogEntry* self = CLogEntry::NewLC(aStream);
	CleanupStack::Pop();
	return self;
	}

 CLogEntry* CLogEntry::NewLC(RReadStream& aStream)
	{
	CLogEntry* self = new(ELeave) CLogEntry();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}	

			
CLogEntry::~CLogEntry()
	{
	delete iPackageName;
	}
	
	
void CLogEntry::ConstructL(const CSisRegistryObject& aObject,TSwiLogTypes InstallInfo)
	{	
	RFs iFs;
	CleanupClosePushL(iFs);
	User::LeaveIfError(iFs.Connect());
	iPackageName= HBufC::NewL(aObject.Name().Length());
	TPtr pkgPtr = iPackageName->Des();
    pkgPtr.Copy(aObject.Name());
    
    TVersion verVal = aObject.Version();
    iMajorVersion = verVal.iMajor;
    iMinorVersion = verVal.iMinor;
    iBuildVersion = verVal.iBuild;
    
    iInstallType = InstallInfo;
  	iEvent.HomeTime();
  	
 	iUid = aObject.Uid();
 
    CleanupStack::PopAndDestroy(&iFs);
    }

void CLogEntry::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}
	
void CLogEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iPackageName;
	aStream.WriteInt8L(static_cast<TInt8>(iInstallType));
	TPckg <TTime> timePckg(iEvent);
 	aStream.WriteL(timePckg, timePckg.MaxLength());
	aStream.WriteInt32L(iMajorVersion);
	aStream.WriteInt32L(iMinorVersion);
	aStream.WriteInt32L(iBuildVersion);
	aStream.WriteInt32L(iUid.iUid);
	}

void CLogEntry::InternalizeL(RReadStream& aStream)
	{
	iPackageName = HBufC::NewL(aStream, KMaxTInt);
	iInstallType = static_cast<TSwiLogTypes>(aStream.ReadInt8L());
   	TPckg <TTime> timePckg(iEvent);
	aStream.ReadL(timePckg, timePckg.MaxLength());
	iMajorVersion = aStream.ReadInt32L();
	iMinorVersion = aStream.ReadInt32L();
	iBuildVersion = aStream.ReadInt32L();
	iUid.iUid= aStream.ReadInt32L();
	}

EXPORT_C const TTime CLogEntry::GetTime() const
	{
	return iEvent;
	}

EXPORT_C const TDesC& CLogEntry::GetPkgName() const
	{
	return *iPackageName;	
	}	

EXPORT_C TSwiLogTypes CLogEntry::GetInstallType() const
	{
	return iInstallType;
	}

EXPORT_C TInt32 CLogEntry::GetMajorVersion() const
	{
	return iMajorVersion;	
	}	
	
EXPORT_C TInt32 CLogEntry::GetMinorVersion() const 
	{
	return iMinorVersion;
	}

EXPORT_C TInt32 CLogEntry::GetBuildVersion() const
	{
	return iBuildVersion;	
	}	

EXPORT_C const TUid CLogEntry::GetUid() const
	{
	return iUid;	
	}


CLogFileVersion* CLogFileVersion::NewL(RReadStream& aStream)
	{
	CLogFileVersion* self = CLogFileVersion::NewLC(aStream);
	CleanupStack::Pop();
	return self;
	}

 CLogFileVersion* CLogFileVersion::NewLC(RReadStream& aStream)
	{
	CLogFileVersion* self = new(ELeave) CLogFileVersion();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}	

void CLogFileVersion::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

void CLogFileVersion::InternalizeL(RReadStream& aStream)
	{
	iLogFileMajorVersion = aStream.ReadUint8L();
	iLogFileMinorVersion = aStream.ReadUint8L();
	if (iLogFileMajorVersion > KLogFileMajorVersion)
	    {
	    User::Leave(KErrNotSupported);
	    }
	}
	
void CLogFileVersion::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(iLogFileMajorVersion);
	aStream.WriteUint8L(iLogFileMinorVersion);
	}
	
CLogFileVersion::CLogFileVersion(const CLogFileVersion& aObject1)
	{
	iLogFileMajorVersion = aObject1.iLogFileMajorVersion;
	iLogFileMinorVersion = aObject1.iLogFileMinorVersion;
	}	
	

