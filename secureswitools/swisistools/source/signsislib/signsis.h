/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef __SIGNING_H__
#define __SIGNING_H__

#include <iostream>

#include "sisblob.h"
#include "siscontents.h"
#include "sissignaturealgorithm.h"

// Forward declarations
class CSISDataUnit;
class CSISInstallBlock;
class CSisSignatureCertificateChain;

class CSignSis
	{
public:
	/**
	 * Parameterized constructor
	 * @param aSisFileName sis file name
	 */
	explicit CSignSis(const std::wstring aSisFileName);
	/**
	 * Destructor
	 */
	~CSignSis();
public:
	/**
	 * Sign the sis with given certificate and key
	 * @param aOutputFileName - Signed sis file name
	 * @param aCertificate - certificate path
	 * @param aPrivateKey - private key path
	 * @param aPassPhrase - password/passphrase by which the private key is encrypted.
	 * @param aAlgorithm - algorithm used for signing.
	 */
	void SignSis(	const std::wstring& aOutputFileName, 
					const std::wstring& aCertificate, 
					const std::wstring& aPrivateKey, 
					const std::wstring& aPassPhrase, 
					const CSISSignatureAlgorithm::TAlgorithm aAlgorithm);
	/**
	 * Remove the most recent signature from the sis file.
	 * @param aOutputFileName - Sis file with last signature removed
	 */
	void RemoveSignature(const std::wstring& aOutputFileName);
	/**
	 * Remove all signatures from the sis file.
	 * @param aOutputFileName - Sis file name of the unsigned sis.
	 */
	void RemoveAllSignatures(const std::wstring& aOutputFileName);

	/**
	 * This function will verify CSISContents. In case the controller
	 * is not a valid one it will throw an exception (CSISException).
	 */
	void Verify();
	/**
	 * Iterate through each of the files present in the install block. The hash value
	 * associated with each file will be compared with a value re-computed from the file 
	 * data. This is to ensure that a malicious file cannot be substituted after the 
	 * sis file has been signed. In case of any abnormality the funtion will throw an 
	 * exception (CSISException).
	 * 
	 * @param aInstallBlock Install block from which the files will be tested.
	 * @param aDataUnit To retrieve file data.
	 */
	void VerifyHashAgainstFileData (const CSISInstallBlock& aInstallBlock, const CSISDataUnit& aDataUnit);

private:
	CSignSis(const CSignSis&){}

private:
	CSISContents	iSisContents;
	};

#endif // __SIGNING_H__
	
