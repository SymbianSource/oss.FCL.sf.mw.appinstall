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
 @released
 @internalTechnology
*/

#ifndef __CERTIFICATERETRIEVER_H__
#define __CERTIFICATERETRIEVER_H__

#include <e32base.h>
#include <ct/rmpointerarray.h>

class CCTCertInfo;
class CCertAttributeFilter;
class CSWICertStore;

namespace Swi
{

/**
 * This is an helper class for the Security Manager. Given a back end
 * certstore it retrieves the mandatory certificates it contains.
 *
 */
class CCertificateRetriever : public CActive
{
 public:

	/**
	 * Creates a CCertificateRetriever with access to the specified
	 * certstore. 
	 *
	 * @param aCerstore The certstore from where this class will retrieve
	 *                  certificates
	 * @return a new certificate retriever
	 */
 	static CCertificateRetriever* NewL(CSWICertStore& aCerstore);

	/**
	 * Creates a CCertificateRetriever with access to the specified
	 * certstore and leaves it on the cleanup stack.	
	 *
	 * @param aCerstore The certstore from where this class will retrieve
	 *                  certificates
	 * @return A new certificate retriever
	 */
  	static CCertificateRetriever* NewLC(CSWICertStore& aCerstore);

  	~CCertificateRetriever();

 public:

	/**
	 * This method will retrieve all self-signed (CA) certificates present
	 * in the back-end certstore it represents
	 *
	 * @param aCertificateListOut An output parameter which contains the mandatory 
	 *                            certificates retrieved
	 * @param aClientStatus       The client request status.
	 */ 

	void RetrieveCACertificates(RMPointerArray<CCTCertInfo>& aCertificateListOut, TRequestStatus& aClientStatus);
	

public: // from CActive 
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	
		
private:
	CCertificateRetriever(CSWICertStore& aCerstore);
	void ConstructL();
	void DoInitialize(RMPointerArray<CCTCertInfo>& aCertificateListOut, TRequestStatus& aClientStatus);

private:

	/**
	 * The client request status. 
	 */
	TRequestStatus* iClientStatus;

	/** 
	 * The DN of the certificates retrieved. It can be empty
	 * if there are not mandatory ones. The class does <b>not</b> own this.
	 */
	RMPointerArray<CCTCertInfo>* iCertificateList; 

	/**
	 * The underlying certstore backend. The certificates come from here.
	 */
	CSWICertStore& iCertStore; 
	
	/**
	 * A filter used to get relevant certificates from the certstore.
	 * In our case we are interested only in CA certificate.
	 */
	CCertAttributeFilter* iFilter; 
	
    enum TCertRetrieverState
	   {
	   EBuildCAList
       } iState;	
};

} // namespace SWI

#endif // __CERTIFICATERETRIEVER_H__
