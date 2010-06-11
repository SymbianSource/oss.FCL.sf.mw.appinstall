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


#include "swicertstore.h"
#include "osinterface.h"
#include <sstream>
using namespace std;

SWICertStore::SWICertStore()
	:iUnifiedCapabilities(0) ,
	iMandatoryCertInfo (NULL),
	iCertstoreInfo (NULL)
	{
	}

void SWICertStore::Run(const Options& aOptions)
	{
	char buf[KLength];
	char mandatory[2];
	const char* fileName = CCIFile.c_str();
	string CertstoreFile = ".\\";
	CertstoreFile.append(fileName);
	
	MandatoryCertInfo* iNext = NULL;
	MandatoryCertInfo* node = NULL ;
	MandatoryCertInfo* nextNode = NULL; 

	CertstoreInfo* iNextNode = NULL;
	CertstoreInfo* certNode = NULL ;
	CertstoreInfo* certNextNode = NULL; 

	FILE *fp = fopen(fileName , "r" );
	if(fp == NULL)
		{
#ifdef DUMPCHAINVALIDITYTOOL
		cout << "File containing Certificate and Capability Information from DumpSWICertstore :" << endl;
#endif
		throw EUnableToOpenFile;
		}

	while(!feof(fp))
		{
		X509* rootArray = PEM_read_X509 (fp, NULL, NULL, NULL);
		if(rootArray)
			{
			iRootCertVector.push_back(rootArray);
			X509_NAME_oneline (X509_get_issuer_name (rootArray) , buf , KLength);
			char* issued = strstr (buf, "/CN=");
			string certLabel;
			if (issued)
				{
				issued += 4;
				char* end = strchr (issued, '/');
				if (end)
					{
					*end = 0;
					}
				certLabel = issued;
				}

			string iniSectionName = "MANDATORY" ;
			OSInterface::ReadPrivateProfile(iniSectionName , certLabel , CertstoreFile , mandatory);
			if(!strcmp(mandatory , "1" ))
				{
				node = new MandatoryCertInfo;
				node ->iMandatoryCertName = certLabel;
				node ->iSignature = rootArray->signature->data ; 
				node ->iSignatureLength = rootArray->signature->length;
				node ->iNext = NULL;
				//when it is the first node in the list
				if(iMandatoryCertInfo == NULL)
					{
					iMandatoryCertInfo = node;
					}
				else
					{
					nextNode = iMandatoryCertInfo;
					while(nextNode -> iNext != NULL)
						{
						nextNode = nextNode->iNext;
						}

					nextNode -> iNext = node;
					}
				}
			//linked list holding the capabilities and signature of the root certs in certstore.
			//This is required for unifying capabilities of the validated chains.
			int rootCaps = 	GetRootCertCapabilities(certLabel);
			certNode = new CertstoreInfo;
			certNode -> iRootCertSignature = rootArray->signature->data ; 
			certNode -> iSignatureLen = rootArray->signature->length;
			certNode -> iRootCapabilities =  rootCaps;
			certNode -> iNextNode = NULL;
			//when it is the first node in the list
			if(iCertstoreInfo == NULL)
				{
				iCertstoreInfo = certNode;
				}
			else
				{
				certNextNode = iCertstoreInfo;
				while(certNextNode -> iNextNode != NULL)
					{
					certNextNode = certNextNode -> iNextNode;
					}

				certNextNode -> iNextNode = certNode;
				}	
			}
		}
	
	fclose(fp);
	}

int SWICertStore::GetRootCertCapabilities(const std::string& aRootLabel)	
	{
	const char* fileName = CCIFile.c_str();
	string CertstoreFile = ".\\";
	CertstoreFile.append(fileName);
	int CertCapabilities = 0;
	char* capabilities = new char[KLength];
	string iniSectionName = "CAPABILITIES";
	OSInterface::ReadPrivateProfile(iniSectionName , aRootLabel , CertstoreFile , capabilities);
	std::istringstream stream(capabilities);
	stream >> CertCapabilities;
	delete [] capabilities;
	return CertCapabilities;
	}

void SWICertStore::UnifyCapabilitiesList(const SISCertificateChain& aChain)
	{
	int devCertCaps = aChain.GetDevCertCapabilities();
	StringVector validatingRoot = aChain.GetValidatedRootSignature();
	StringVector devCertValidatingRoot = aChain.GetDevCertValidatedRootSignature();
	StringVector::iterator start = validatingRoot.begin();
	StringVector::iterator last = validatingRoot.end();
	StringVector::iterator devCertIterStart = devCertValidatingRoot.begin();
	StringVector::iterator devCertIterEnd = devCertValidatingRoot.end();
	while(iCertstoreInfo)
		{
		if(validatingRoot.size())
			{
			string signature((const char*)(iCertstoreInfo->iRootCertSignature) , (iCertstoreInfo->iSignatureLen));
			StringVector::iterator iter = std::find(start , last , signature);
			if(iter != last)
				{
				iUnifiedCapabilities = (iUnifiedCapabilities | (iCertstoreInfo -> iRootCapabilities)) ;
				}
			}

		if(devCertValidatingRoot.size())
			{
			string signature((const char*)(iCertstoreInfo->iRootCertSignature) , (iCertstoreInfo->iSignatureLen));
			StringVector::iterator iter = std::find(devCertIterStart , devCertIterEnd , signature);
			if(iter != devCertIterEnd)
				{
				//if devcert has capability constraint
				if(devCertCaps > 0)
					{
					int capability = (iCertstoreInfo -> iRootCapabilities) & devCertCaps;
					iUnifiedCapabilities = (iUnifiedCapabilities | capability);
					}
				else
					{
					//in case devcert has no capabilitiy constraint.
					iUnifiedCapabilities = (iUnifiedCapabilities | (iCertstoreInfo -> iRootCapabilities));
					}
				}
			}
		iCertstoreInfo = iCertstoreInfo ->iNextNode;
		}
	}

void SWICertStore::DisplayUnifiedCapabilities(const SISCertificateChain& aChain)
	{
	int uniCaps = iUnifiedCapabilities;
	int i = 0;
	if(uniCaps == 0 )
		{
		 if(aChain.GetValidatedRootSignature().size() > 0)
			{
#ifdef DUMPCHAINVALIDITYTOOL
			cout<<"Root Certificate used for validation doesn't grant any capabilities"<<endl<<endl;
#endif
			}

		if(!aChain.GetDevCertValidatedRootSignature().size() && !aChain.GetValidatedRootSignature().size())
			{
#ifdef DUMPCHAINVALIDITYTOOL
			cout << "Either Certificate Chain/Chains are not validated by the CertStore\n or Certificate Chain is broken " << endl << endl;
#endif
			}
		}
	else
		{
		while(uniCaps)
			{
			if(uniCaps & 0x01)
				{
#ifdef DUMPCHAINVALIDITYTOOL
				cout << CapabilityList[i] << endl;
#endif
				}
			i++;
			uniCaps = uniCaps >> 1;
			}
		}
	}


SWICertStore::~SWICertStore()
	{
	for(vector <X509*>::iterator iter = iRootCertVector.begin() ; iter != iRootCertVector.end() ; ++iter)
		{
		X509_free(*iter);
		}

	for(MandatoryCertInfo* list = iMandatoryCertInfo ; list ; )
		{
		MandatoryCertInfo* next = list -> iNext;
		delete list;
		list = next;
		}

	for(CertstoreInfo* certList = iCertstoreInfo ; certList ; )
		{
		CertstoreInfo* nextNode = certList -> iNextNode;
		delete certList;
		certList = nextNode;
		}

	}