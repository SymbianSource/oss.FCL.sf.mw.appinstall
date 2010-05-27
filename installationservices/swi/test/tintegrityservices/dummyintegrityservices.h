/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of CDummyIntegrityServices test class
*
*/




/**
 @file
 @test
 @internalTechnology 
*/

#ifndef __DUMMYINTEGRITYSERVICES_H__
#define __DUMMYINTEGRITYSERVICES_H__

#include "integrityservices.h"

namespace Swi
{
namespace Test
{
const TInt KIntegrityServicesSimulatedBatteryFailure=-10205;	
/**
 * This class is used to test integrity Services by exposing a function to set
 * the simulated failure point.
 *
 * @test
 * @internalTechnology 
 */
class CDummyIntegrityServices : public CIntegrityServices
	{
	public:

		/**
		 * Constructs a new CIntegrityServices object specifying a journal path
		 */
		static CDummyIntegrityServices* NewL(TInt64 aTransactionID, const TDesC& aPath);
		
		/**
		 * Constructs a new CIntegrityServices object specifying a journal path
		 * and puts it on the cleanup stack
		 */
		static CDummyIntegrityServices* NewLC(TInt64 aTransactionID, const TDesC& aPath);

		/**
		 * Sets the test failure class, this tells the class when to simulate a battery failure
		 * by leaving in the middle of an operation
		 *
		 * @param aTestfailure - Test failure details
		 */
		void SetSimulatedFailure(TPtrC16 aFailType, TPtrC16 aFailPosition, TPtrC16 aFileName);
		
		void CreateNewTestFileL(TDesC& aFileName);
		
		void CreateTempTestFileL(TDesC& aFileName);
		
	private:
		CDummyIntegrityServices(TInt64 aTransactionID);
		
		/**
		 * Second phase constructor for CDummyIntegrityServices
		 */
		void ConstructL(const TDesC& aPath);
		
		virtual void SimulatePowerFailureL(TFailType aFailType, TFailPosition aPosition, const TDesC& aFailFileName);
		};

} //namespace
} //namespace
#endif
