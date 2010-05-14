/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "siscertificatechain.h"
#include "osinterface.h"
#include <algorithm>
using namespace std;

SISCertificateChain::SISCertificateChain()
	:iDevCertCapabilities(0) ,
	iCertChain (0)
	{
	}


int SISCertificateChain::Run(const Options& aOptions , const SWICertStore& aStore, const string& aDirectory)
	{
	const StringVector& chainDirectory = OSInterface::ExtractFilesFromDirectory(aDirectory , "pem");
	int size = chainDirectory.size();
	iCertChain = new CertificateChain[size];
	int i = 0 ;
	int valid = 0;
	//implies sis file is signed.
	if(size > 0)
		{
		for(vector<string>::const_iterator iterFile = chainDirectory.begin(); iterFile != chainDirectory.end(); iterFile++)
			{
			if(i < chainDirectory.size())
				{
				valid = iCertChain[i].ValidateChain(aOptions , *iterFile , aStore);
				if(valid)
					{
					if(iCertChain[i].IsDevCertificate())
						{
						iDevCertCapabilities = iDevCertCapabilities |(iCertChain[i].GetDevCaps());
						//only those rootcerts which validates the devcerts are pushed into this vector
						//so that when unifying caps is called,even though root cert may validate the chain,if the chain is b broken or expired
						//they wouldn't be considered.
						if(iCertChain[i].GetValidationStatus() == 0)
							{
							
							iDevCertValidatedRootSignature.push_back(iCertChain[i].GetValidatedRootSignatures());
							}
						}
					else
						{
						//even though the cert chain may be validated by any certs in the certstore,but the chain is broken or any certificate in the chain is expired.
						//they wouldn't be considered for unifying caps .
						if(iCertChain[i].GetValidationStatus() == 0)
							{
							iValidatedRootSignature.push_back(iCertChain[i].GetValidatedRootSignatures());
							}
						}
					}
			else
				{
#ifdef DUMPCHAINVALIDITYTOOL
				cout<<"Is Not Validated By Certstore:" << endl << endl;
#endif
				}
			}
		i++;
		}
	}

	else
		{
		cout<<"SIS File is Unsigned." << endl << endl;
		return 0;
		}

	return 1;
	}


void SISCertificateChain::VerifyMandatory(const SWICertStore& aStore)
	{
	string missingMandatoryCerts;
	const SWICertStore::MandatoryCertInfo* mandatoryList = aStore.GetMandatoryCertInfoList();
	while(mandatoryList)
		{
		if(iValidatedRootSignature.size())
			{
			StringVector::iterator start = iValidatedRootSignature.begin();
			StringVector::iterator last = iValidatedRootSignature.end();
			string signature((const char*)(mandatoryList->iSignature) , (mandatoryList->iSignatureLength));
			StringVector::iterator mandatory = find(start , last , signature);
			if(mandatory == last)
				{
				string missingMandatoryLabel = mandatoryList->iMandatoryCertName;
				missingMandatoryCerts.append(missingMandatoryLabel);
				missingMandatoryCerts.append("\n");
				}
			}
		mandatoryList = mandatoryList->iNext;
		}

	if(missingMandatoryCerts.length())
		{
#ifdef DUMPCHAINVALIDITYTOOL
		cout << "SIS File is not signed with the following Mandatory Certificate " << endl << "in the CertStore :" << endl << endl;
		cout << missingMandatoryCerts << endl;
#endif
		}
	}

SISCertificateChain::~SISCertificateChain()
	{
	delete [] iCertChain;
	}