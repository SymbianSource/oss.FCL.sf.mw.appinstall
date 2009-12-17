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
 @internalComponent
 @released
*/

#include <iostream>
#include <string>
#include <fstream>
#include <openssl/err.h>

#include "parameter.h"
#include "siscontents.h"
#include "utility_interface.h"
#include "siscontroller.h"
#include "signsis.h"
#include "certificateinfo.h"
#include "sissignaturecertificatechain.h"

const char* KMonthString[]=
	{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
	};

void DeletePEMFiles(std::string& aDirectorypath)
	{
	int certNumber = 1;
	char intChain[2];
	while(true)
		{
		std::string dirPath = aDirectorypath;
		itoa(certNumber,intChain,10);
		dirPath.append("/cert");
		dirPath.append(intChain) ;
		dirPath.append(".pem");
		if(remove(dirPath.c_str())==-1)
			{
			if(RemoveDirectoryA(dirPath.c_str()))
				{
				std::cout << "Cannot delete directory" << std::endl;
				}
			break;
			}
		certNumber++;
		}
	}


inline void PrintWithIndent(std::ostream& aStream, const std::string& aString, int aIndent)
	{
	aStream.width(aIndent);
	aStream << "";
	aStream << aString;
	}

inline void PrintWithIndent(std::wostream& aStream, const std::wstring& aString, int aIndent)
	{
	aStream.width(aIndent);
	aStream << L"";
	aStream << aString;
	}

void PrintExtensions(const std::vector<TExtension>& aExtensions, int aIndent)
	{
	if(aExtensions.size() > 0)
		{
		PrintWithIndent(std::cout, "X509v3 extensions:", aIndent);
		}
	for(int i = 0; i < aExtensions.size(); ++i)
		{
		std::cout << std::endl;
		PrintWithIndent(std::cout, aExtensions[i].iExtensionName, aIndent+4); 
		std::cout << ": ";
		std::cout << (aExtensions[i].iIsCritical? "critical":"") << std::endl;
		if(aExtensions[i].iValue != "")
			{
			PrintWithIndent(std::cout, aExtensions[i].iValue, aIndent+4);
			std::cout << std::endl;  
			}
		else
			{
			for(int j = 0; j < aExtensions[i].iValueList.size(); ++j)
				{
				if(0 != j)
					{
					if(aExtensions[i].iIsMultiLine)
						{
						std::cout << std::endl;
						PrintWithIndent(std::cout, "", aIndent+4); 
						}
					else
						std::cout << ", ";
					}
				else
					{
					PrintWithIndent(std::cout, "", aIndent+4);
					}
				std::cout << aExtensions[i].iValueList[j].iName;
				if(aExtensions[i].iValueList[j].iName != "" && aExtensions[i].iValueList[j].iValue != "")
					{
					std::cout << ":";
					}
				std::cout << aExtensions[i].iValueList[j].iValue;
				}
			}
		std::cout << ((aExtensions[i].iIsMultiLine)? "\n": "") ;
		}
	}

void PrintDateTime(const CSISDateTime& aDateTime)
	{
	int year 	= aDateTime.Date().Year();
	int month 	= aDateTime.Date().Month();
	int day		= aDateTime.Date().Day();
	int hours 	= aDateTime.Time().Hours();
	int mins 	= aDateTime.Time().Minutes();
	int secs 	= aDateTime.Time().Seconds();
	printf("%s %d %02d:%02d:%02d %d GMT", KMonthString[month], day, hours, mins, secs, year);
	}

inline std::ostream& operator << (std::ostream& aStream, const CSISDate& aDate)
	{
	aStream << (int)aDate.Day() << "/" << (int)aDate.Month()+1 << "/" << (int)aDate.Year();
	return aStream; 
	}

void PrintCertificateInfo(CCertificateInfo* aCertInfo)
	{
	std::wcout << L"Issued by : " << aCertInfo->IssuerName(false) << "." << std::endl;
	std::wcout << L"Issued to : " << aCertInfo->SubjectName(false) << "." << std::endl;
	std::cout << "Valid from " << aCertInfo->ValidFrom().Date() << " to " <<  aCertInfo->ValidTo().Date() << std::endl;
	}


void PrintCertificateDetails(CCertificateInfo* aCertInfo)
	{
	std::cout << "Certificate: " << std::endl;
	PrintWithIndent(std::cout, "Data:", 4);
	std::cout << std::endl;
	int version = aCertInfo->Version();
	PrintWithIndent(std::cout, "Version: ", 8);
	std::cout << version+1 << "(0x" << version << ")" << std::endl;
	PrintWithIndent(std::cout, "Serial Number: ", 8);
	std::cout << aCertInfo->SerialNumber();
	std::cout << std::endl;
	PrintWithIndent(std::cout, "Signature Algorithm: ", 8);
	std::cout << aCertInfo->SignatureAlgo() << std::endl;
	PrintWithIndent(std::wcout, L"Issuer: ", 8);
	std::wcout << aCertInfo->IssuerName(true) << std::endl;
	PrintWithIndent(std::cout, "Validity", 8);
	std::cout << std::endl;
	PrintWithIndent(std::cout, "Not Before: ", 12);
	PrintDateTime(aCertInfo->ValidFrom());
	std::cout << std::endl;
	PrintWithIndent(std::cout, "Not After : ", 12);
	PrintDateTime(aCertInfo->ValidTo());
	std::cout << std::endl;
	PrintWithIndent(std::wcout, L"Subject: ", 8);
	std::wcout << aCertInfo->SubjectName(true) << std::endl;
	PrintWithIndent(std::cout, "Subject Public Key Info:", 8);
	std::cout << std::endl;
	PrintWithIndent(std::cout, "Public Key Algorithm: ", 12);
	std::cout << aCertInfo->PublicKeyAlgo() << std::endl;
	aCertInfo->PrintPublicKey(std::cout, 16);
	const std::vector<TExtension>& extList = aCertInfo->Extensions();
	PrintExtensions(extList, 8);
	aCertInfo->PrintSignature(std::cout, 12);
	}

void ReportSignatures(const CSISController& aController, bool aExtractCerts = false)
	{
	int signatureCount = aController.SignatureCount();
	if(0 == signatureCount)
		{
		std::cout << "No primary signatures." << std::endl;
		return;
		}

	std::string directoryPath = "Chain";
	if(aExtractCerts)
		{
		DeletePEMFiles(directoryPath);

		CreateDirectoryA(directoryPath.c_str(),NULL);
		}

	std::cout << std::endl << "Primary:" << std::endl;
	for(int i = 0; i < signatureCount; ++i)
		{
		CSignatureCertChainData& sigdata = const_cast<CSignatureCertChainData&>(aController.SignatureCertChain(i));
		CSisSignatureCertificateChain certChain(sigdata);
		const std::vector<CCertificateInfo*>& certList = certChain.CertChain();
		for(int j = 0; j < certList.size(); ++j)
			{
			if(aExtractCerts)
				{
				PrintCertificateDetails(certList[j]);
				}
			else
				{
				PrintCertificateInfo(certList[j]);
				std::cout << std::endl;
				}
			}
		if(aExtractCerts)
			{
			char intChain[2];
			itoa(i+1,intChain,10);
			std::string certFullPath = directoryPath + "/cert";
			certFullPath = certFullPath + intChain;
			certFullPath = certFullPath + ".pem";
			certChain.ExtractCertificateChain(certFullPath);
			}
		}
	}

int main(int argc, char *argv[])
	{
    wchar_t **argv1 = CommandLineArgs(argc,argv);

	CParameter parameter;
	
	if (! parameter.CommandLine (argc, argv1))
		{
		return 4;
		}
	if (parameter.Sis().empty())
		{
		cleanup(argc,argv1);
		return 0;
		}

	ERR_load_crypto_strings ();
	OpenSSL_add_all_algorithms ();
	OpenSSL_add_all_ciphers ();
	OpenSSL_add_all_digests ();
	int retValue = 0;
	try
		{
		bool isSisFile = CSISContents::IsSisFile(parameter.Sis ());

		if(isSisFile)
			{
			CSISContents content;
			content.Load (parameter.Sis ());
			CSignSis signsis(parameter.Sis ());
			if (parameter.Sign ())
				{
				if (parameter.Verbose ())
					{
					std::cout << "Signing" << std::endl;
					}
				signsis.SignSis(parameter.Output (), parameter.Certificate(), parameter.Key(), parameter.PassPhrase(), parameter.Algorithm());
				}
			if (parameter.Unsign ())
				{
				if (parameter.Verbose ())
					{
					std::cout << "Removing signature" << std::endl;
					}
				signsis.RemoveSignature (parameter.Output ());
				}
			if (parameter.Report() || parameter.ExtractCert())
				{
				ReportSignatures(content.Controller(), parameter.ExtractCert());
				}
			if (parameter.Dump ())
				{
				content.Dump (std::cout, 0);
				}
			}
		else
			{
			if (parameter.Report ()||parameter.ExtractCert())
				{
				std::cout<<"Reporting Stub Sis Controller"<<std::endl;
				CSISController ctrl;
				ctrl.Load (parameter.Sis ());
				ReportSignatures(ctrl, parameter.ExtractCert());
				}
			}
		}
	catch(const CSISException& oops)
		{
		std::wcout << oops.widewhat () << std::endl;
		retValue = 1;
		}
	catch (...)
		{
		std::wcout << L"Unexpected error." << std::endl;
		retValue = 1;
		}
	EVP_cleanup ();
	ERR_free_strings ();

	// cleanup the memory
	cleanup(argc,argv1);
	return retValue;
	}

