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


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSWICERTSTORETOOL_H__
#define __CSWICERTSTORETOOL_H__

#include "CSWICertStoreToolCert.h"
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <securitydefs.h>

class CTestConfig;
class CTestConfigSection;
class CPermanentFileStore;

class CSWICertStoreTool : public CBase
	{
 public:
	
	static CSWICertStoreTool* NewLC(const TDesC& aInputFile,
									const TDesC& aOutputFile,
									const TDesC& aLogFile);
	virtual ~CSWICertStoreTool();

	void RunToolL();

 private:
	CSWICertStoreTool(const TDesC& aInputFile,
					  const TDesC& aOutputFile);
	void ConstructL(const TDesC& aLogFile);
	void Cleanup();

	void DoRunToolL();

	void LogL(TRefByValue<const TDesC8> aFmt, ...);
	void TrapAndLogL(const TDesC8& aName, TInt err);
	void LogArgL(const TDesC8& aName, const TDesC& aValue);
	void LogParameterL(const TDesC8& aName, const TDesC8& aValue);
		
	void ParseScriptSectionL(const CTestConfigSection& aSection);
	void CheckCertLabelL(const TDesC& aLabel);
	void OpenInputFileL(const TDesC& aInputFile);
	void OpenStoreL(const TDesC& aOutputFile);
	HBufC8* LoadCertDataL(const TDesC8& aPath);
	TStreamId WriteCertDataL(const TDesC8& aData);
	TCapabilitySet ParseCapabilitiesL(const CTestConfigSection& aSection);
	void ParseApplicationsL(const CTestConfigSection& aSection,
							RArray<TUid>& aApplications);
	TStreamId WriteInfoStreamL();
	void WriteRootStreamL(TStreamId aInfoStreamId);
	TUid ParseApplicationNameL(const TDesC8& aName);
	/**
	 * Parse a string containg a capability name.
	 * @leave KErrArgument If the string is not a recognised capability name.
	 */
	TCapability ParseCapabilityNameL(const TDesC8& aName);
	
 private:
	const TDesC& iInputFile;
	const TDesC& iOutputFile;
	
	RFs iFs;
	RFile iLogFile;
	CTestConfig* iScript;
	CPermanentFileStore* iStore;
	TInt iCertificateId;
	RPointerArray<CSWICertStoreToolCert> iCerts;
	};

#endif
