/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* CSisControllerVerifier class definition
* @released
* @internalComponent
*
*/



#ifndef __SISCONTROLLERVERIFIER_H__
#define __SISCONTROLLERVERIFIER_H__

#include <e32base.h>
#include <swi/msisuihandlers.h>

// Forward Declarations
class CX509Certificate;

namespace Swi
{

// Swi: Forward Declarations
class CDesDataProvider;
class CSecurityManager;

namespace Sis
	{
	// Swi::Sis: Forward Declarations
	class CController;
	}


/*
 * This class is used to handle asyncronous function call to CSecurityManager
 * to verify the controller.
 * 
 * @internalComponent
 * @released
 */
class CSisControllerVerifier: public CActive
	{
public:
	static CSisControllerVerifier* NewL(const RMessage2& aMessage);
	
	virtual ~CSisControllerVerifier();

public: // from CActive 
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	

public:
	void VerifyControllerL(RPointerArray<HBufC8>& aControllers);
	
private:
	void ConstructL();

	void VerifyControllerL( HBufC8* aRawController);
	CSisControllerVerifier(const RMessage2& aMessage);
	void CompleteRequestL();

private:
	const RMessage2&							iMessage;
	TBool 										isVerified;
	CSecurityManager*							iSecurityManager;
	Sis::CController*							iController;
	CDesDataProvider*							iDesProvider;

	TSignatureValidationResult 					iResultOut;
	RPointerArray<CPKIXValidationResultBase> 	iPkixResultsOut;
	RPointerArray<CX509Certificate> 			iCertsOut;
	RPointerArray<CX509Certificate> 			iX509RootCertArray;
	TCapabilitySet 								iCapabilitySetOut;
	TBool	 									iAllowUnsigned;
	TBool 										iIsEmbedded;
	TInt										iCurrentController;
	RPointerArray<HBufC8>* 						iControllers;
	};
}

#endif // __SISCONTROLLERVERIFIER_H__
