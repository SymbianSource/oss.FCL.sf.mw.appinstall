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
* CSWICertStoreToolCert class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSWICERTSTORETOOLCERT_H__
#define __CSWICERTSTORETOOLCERT_H__

#include <e32base.h>
#include <s32std.h>
#include <cctcertinfo.h>
#include <swicertstore.h>


/**
 * Temporary info about a cert read from the ini file.
 */
class CSWICertStoreToolCert : public CBase
	{
 public:
	static CSWICertStoreToolCert* NewLC(TInt aSize,
										const TDesC& aLabel,
										TInt aCertId,
										const TDesC8& aSubjectKeyId,
										const RArray<TUid>& aApplications,
										TStreamId aDataStreamId,
										const TCertMetaInfo& aCertMetaInfo);
	virtual ~CSWICertStoreToolCert();

	const TDesC& Label() const;
	void ExternalizeL(RWriteStream& aStream) const;

 private:
	CSWICertStoreToolCert(TInt aSize,
						  const TDesC& aLabel,
						  TInt aCertId,
						  const TDesC8& aSubjectKeyId,
						  TStreamId aDataStreamId,
						  const TCertMetaInfo& aCertMetaInfo);
	void ConstructL(const RArray<TUid>& aApplications);
	
 private:
	TInt iSize;
	TCertLabel iLabel;
	TInt iCertId;
	TKeyIdentifier iSubjectKeyId;
	RArray<TUid> iApplications;
	TStreamId iDataStreamId;
	TCertMetaInfo iCertMetaInfo;
	};

#endif
