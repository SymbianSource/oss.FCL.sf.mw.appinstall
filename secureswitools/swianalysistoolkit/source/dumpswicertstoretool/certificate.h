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

#ifndef	__CERTIFICATE_H__
#define	__CERTIFICATE_H__

#include "options.h"
#include "pfsdump.h"
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <assert.h>
#include <openssl/bio.h>

int const KLength = 256 ;
class Stream;
class StoreFile;
class Options;
/**
 * The Certificate class provides functions which reads the data streams containing the certificates
 * present in the swicertstore.
 * @internalComponent 
 */	
class Certificate
{
public:

	Certificate(const Stream& aStream, StoreFile& aStore);

	~Certificate();

	/**
	  Reads the data stream containing the certificates .
	  This stream is then written to a file with certificate label name passed as input.
	  The file created is of .PEM form 
	  @param	aCertLabel	Name of the certificate which needs to be extracted.
	  @return	true if the certificate information is to be displayed(in case both writable
				and ROM certstore is specified ,and certificate is present in both certstores
				only those which are in writabel certstore will be considered and those which are unique 
				to the certstores).
	 */
	bool Read(const char*  aCertLabel , const Options& aOptions);

	const string& GetIssuerName() const {  return iIssuerName ; }
	
private:

	const Stream& iStream;
	StoreFile& iStore;
	string iIssuerName;
	static StringVector iCertSignature;

};
#endif