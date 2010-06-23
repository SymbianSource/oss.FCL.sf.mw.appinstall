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


/**
 @file 
 @internalComponent 
*/

#ifndef __SWICERTSTORE_H__
#define __SWICERTSTORE_H__
#pragma warning(disable: 4786)

#include "options.h"
#include "siscertificatechain.h"
#include "dumpchainvaliditytool.h"
#include "osinterface.h"
#include <sstream>

typedef std::vector <std::string>  StringVector ; 

class SWICertStore
{
public:
	//contains the swicertstore's mandatory certificate signature and label information.
	struct MandatoryCertInfo
		{
		MandatoryCertInfo* iNext;
		unsigned char* iSignature;
		int iSignatureLength;
		std::string iMandatoryCertName;
		};

	//contains the swicertstore root certs signature and capabilities information.
	struct CertstoreInfo
		{
		CertstoreInfo* iNextNode;
		unsigned char* iRootCertSignature;
		int iSignatureLen;
		int iRootCapabilities;
		};

public:

	SWICertStore();

	~SWICertStore();

	/**Reads the swicertstore.cci file (containing the root certificates and their corressponding capabilities)
	   and stores the certificates(PEM format) in a vector.
	  */
	void Run(const Options& aOptions);

	/**
	   Unifies the capabilities that are granted by the root certificates which
	   have validated all the certificate chains.The labels of the root certificates obtained from SISCertificateChain
	   class are used to extract their corressponding capabilities.
	 */
	void UnifyCapabilitiesList(const SISCertificateChain& aChain);

	/** 
	   Returns a vector containing the Root Certificates .
	 */
	const std::vector <X509*>& GetRootCertVector() const { return iRootCertVector;}

	/** 
	   Returns the unified list of capabilities that are grabted by the root certificates 
	   which validates the chains.
	 */
	int GetUnifiedCapabilities() const { return iUnifiedCapabilities ;}

	void DisplayUnifiedCapabilities(const SISCertificateChain& aChain);

	const MandatoryCertInfo* GetMandatoryCertInfoList() const  { return iMandatoryCertInfo ; }

private:
	
	/**
	   Extracts the capability value of the corressponding root certificate label passed 
	   as a parameter from the swicertstore.cci file .
	   @param	aRootLabel	Root Certificate label whose capability values needs to be extracted 
						    from the swicertstore.cci file.
	 */
	int GetRootCertCapabilities(const std::string& aRootLabel);

private:

	int iUnifiedCapabilities;
	MandatoryCertInfo* iMandatoryCertInfo ; 
	CertstoreInfo* iCertstoreInfo;
	std::string iCertstoreFile;
	std::vector <X509*> iRootCertVector;
};

#endif
