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


#include "CSWICertStoreToolCert.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
const TUint KReadOnlyFlagMask = 128;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

CSWICertStoreToolCert* CSWICertStoreToolCert::NewLC(TInt aSize,
													const TDesC& aLabel,
													TInt aCertId,
													const TDesC8& aSubjectKeyId,
													const RArray<TUid>& aApplications,
													TStreamId aDataStreamId,
													const TCertMetaInfo& aCertMetaInfo)
	{
	CSWICertStoreToolCert* self = new (ELeave) CSWICertStoreToolCert(aSize,
																	 aLabel,
																	 aCertId,
																	 aSubjectKeyId,
																	 aDataStreamId,
																	 aCertMetaInfo);
	CleanupStack::PushL(self);
	self->ConstructL(aApplications);
	return self;
	}

CSWICertStoreToolCert::CSWICertStoreToolCert(TInt aSize,
											 const TDesC& aLabel,
											 TInt aCertId,
											 const TDesC8& aSubjectKeyId,
											 TStreamId aDataStreamId,
											 const TCertMetaInfo& aCertMetaInfo) :
	iSize(aSize),
	iLabel(aLabel),
	iCertId(aCertId),
	iSubjectKeyId(aSubjectKeyId),
	iDataStreamId(aDataStreamId),
	iCertMetaInfo(aCertMetaInfo)
	{
	ASSERT(iSize > 0);
	ASSERT(iDataStreamId != KNullStreamId);
	}

CSWICertStoreToolCert::~CSWICertStoreToolCert()
	{
	iApplications.Close();
	}

void CSWICertStoreToolCert::ConstructL(const RArray<TUid>& aApplications)
	{
	for (TInt i = 0 ; i < aApplications.Count() ; ++i)
		{
		User::LeaveIfError(iApplications.Append(aApplications[i]));
		}
	}

const TDesC& CSWICertStoreToolCert::Label() const
	{
	return iLabel;
	}

void CSWICertStoreToolCert::ExternalizeL(RWriteStream& aStream) const
	{
	// Hardcoded format must be the same as expected by swicertstore. There is
	// some redunant information here in case we need to extend the format later.
	
	// Only supports X509 certs, SwiCertStore is always read-only (in ROM)
	aStream.WriteUint8L(EX509Certificate | KReadOnlyFlagMask ); 
	aStream.WriteInt32L(iSize);
	aStream << iLabel;
	aStream.WriteInt32L(iCertId); // Assign handle
	aStream.WriteUint8L(ECACertificate); // Only supports CA certs
	aStream << iSubjectKeyId;
	
	TKeyIdentifier issuerKeyId;
	issuerKeyId.Zero();
	aStream << issuerKeyId;

	TInt count = iApplications.Count();
	aStream.WriteInt32L(count);
	for (TInt i = 0 ; i < count ; ++i)
		{
		aStream << iApplications[i];
		}
	
	aStream.WriteUint8L(ETrue); // Certs are always trusted
	aStream << iDataStreamId;
	TPckg<TCapabilitySet> capsPckg(iCertMetaInfo.iCapabilities);
	aStream << capsPckg;
	// To keep the backward compatabitliy we have to write both the value in a single int
	TUint8 value = 0;
	if (iCertMetaInfo.iIsMandatory)
		{
		value = value | (1 << KMandatory);
		}
	if (iCertMetaInfo.iIsSystemUpgrade)
		{
		value = value | (1 << KSystemUpgrade);
		} 
	
	aStream.WriteUint8L(value);
	}
