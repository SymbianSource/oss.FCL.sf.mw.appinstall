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


#include "msisuihandlers.h"

namespace Swi
{
// CAppInfo

EXPORT_C CAppInfo* CAppInfo::NewL(RReadStream& aStream)
	{
	CAppInfo* self=CAppInfo::NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAppInfo* CAppInfo::NewLC(RReadStream& aStream)
	{
	CAppInfo* self=new(ELeave) CAppInfo();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}
		
EXPORT_C CAppInfo* CAppInfo::NewL(const TDesC& aAppName,
								  const TDesC& aAppVendor, const TVersion& aAppVersion)
	{
	CAppInfo* self=CAppInfo::NewLC(aAppName, aAppVendor, aAppVersion);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAppInfo* CAppInfo::NewLC(const TDesC& aAppName,
								   const TDesC& aAppVendor, const TVersion& aAppVersion)
	{
	CAppInfo* self=new(ELeave) CAppInfo(aAppVersion);
	CleanupStack::PushL(self);
	self->ConstructL(aAppName, aAppVendor);
	return self;
	}

CAppInfo::CAppInfo(const TVersion& aAppVersion) 
: iAppVersion(aAppVersion)
	{
	}
	
CAppInfo::~CAppInfo()
	{
	delete iAppName;
	delete iAppVendor;
	}

void CAppInfo::ConstructL(const TDesC& aAppName, const TDesC& aAppVendor)
	{
	iAppName=aAppName.AllocL();
	iAppVendor=aAppVendor.AllocL();
	}

void CAppInfo::ReadBufFromStreamL(HBufC*& aBuffer, RReadStream& aStream)
	{
	delete aBuffer;
	aBuffer=NULL;
	aBuffer=HBufC::NewL(aStream, KMaxTInt);
	}

EXPORT_C void CAppInfo::InternalizeL(RReadStream& aStream)
	{
	ReadBufFromStreamL(iAppName, aStream);
	ReadBufFromStreamL(iAppVendor, aStream);

	TPckg<TVersion> pckgAppVersion(iAppVersion);
	aStream.ReadL(pckgAppVersion);
	}

EXPORT_C void CAppInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iAppName;
	aStream << *iAppVendor;

	aStream.WriteL(TPckgC<TVersion>(iAppVersion));
	}


// CCertificateInfo

EXPORT_C CCertificateInfo* CCertificateInfo::NewL(const CX509Certificate& aCertificate)
	{
	CCertificateInfo* self=CCertificateInfo::NewLC(aCertificate);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCertificateInfo* CCertificateInfo::NewLC(const CX509Certificate& aCertificate)
	{
	CCertificateInfo* self=new(ELeave) CCertificateInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aCertificate);
	return self;
	}

EXPORT_C CCertificateInfo* CCertificateInfo::NewL(RReadStream& aStream)
	{
	CCertificateInfo* self=CCertificateInfo::NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCertificateInfo* CCertificateInfo::NewLC(RReadStream& aStream)
	{
	CCertificateInfo* self=new(ELeave) CCertificateInfo();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}

CCertificateInfo::~CCertificateInfo()
	{
	delete iSubjectName;
	delete iIssuerName;
	delete iFingerprint;
	delete iSerialNumber;
	}

CCertificateInfo::CCertificateInfo()
	{
	}

void CCertificateInfo::ConstructL(const CX509Certificate& aCertificate)
	{
	iSubjectName=aCertificate.SubjectName().DisplayNameL();
	iIssuerName=aCertificate.IssuerName().DisplayNameL();
	iFingerprint=aCertificate.Fingerprint().AllocL();
	iSerialNumber=aCertificate.SerialNumber().AllocL();
	iValidFrom=aCertificate.ValidityPeriod().Start().DateTime();
	iValidTo=aCertificate.ValidityPeriod().Finish().DateTime();
	}

void CCertificateInfo::ReadBufFromStreamL(HBufC*& aBuffer, RReadStream& aStream)
	{
	delete aBuffer;
	aBuffer=NULL;
	aBuffer=HBufC::NewL(aStream, KMaxTInt);
	}

void CCertificateInfo::ReadBufFromStreamL(HBufC8*& aBuffer, RReadStream& aStream)
	{
	delete aBuffer;
	aBuffer=NULL;
	aBuffer=HBufC8::NewL(aStream, KMaxTInt);
	}

void CCertificateInfo::InternalizeL(RReadStream& aStream)
	{
	ReadBufFromStreamL(iSubjectName, aStream);
	ReadBufFromStreamL(iIssuerName, aStream);
	ReadBufFromStreamL(iFingerprint, aStream);
	ReadBufFromStreamL(iSerialNumber, aStream);

	TPckg<TDateTime> pckgValidFrom(iValidFrom);
	aStream.ReadL(pckgValidFrom);

	TPckg<TDateTime> pckgValidTo(iValidTo);
	aStream.ReadL(pckgValidTo);
	}

void CCertificateInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iSubjectName;
	aStream << *iIssuerName;
	aStream << *iFingerprint;
	aStream << *iSerialNumber;

	aStream.WriteL(TPckgC<TDateTime>(iValidFrom));
	aStream.WriteL(TPckgC<TDateTime>(iValidTo));
	}

} // namespace Swi
