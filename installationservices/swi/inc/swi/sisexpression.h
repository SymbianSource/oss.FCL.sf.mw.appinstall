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
* Definition of the Swi::Sis::CExpression
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __SISEXPRESSION_H__
#define __SISEXPRESSION_H__

#include <e32base.h>
#include "sisfield.h"

namespace Swi
{
const static TInt KVariableLanguage = 0x1000;
const static TInt KVariableRemoteInstall = 0x1001;

namespace Sis
 {

class CString;
class CExpression;
class TPtrProvider;

/**
 * This class represents a SisExpression. SisExpression is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CExpression : public CField
	{
public:

	/**
	 * This creates a new CExpression object.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CExpression representing the data read.	 	 	 	 	 	 	 	 
	 */
	IMPORT_C static CExpression* NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CExpression object and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CExpression representing the data read.	 
	 */
	IMPORT_C static CExpression* NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CExpression object in place.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CExpression representing the data read.	 
	 */

	IMPORT_C static CExpression* NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	/**
	 * This creates a new CExpression object in place and places it on the cleanup stack.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 *
	 * @return An instance of CExpression representing the data read.	 
	 */

	IMPORT_C static CExpression* NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	IMPORT_C virtual ~CExpression();

public: // Accessors

	TOperator Operator() const;
	
	CExpression* Lhs() const;
	
	CExpression* Rhs() const;
	
	TInt32 IntegerValue() const;
	
	CString* StringValue() const;

private:

	CExpression();

	/**
	 * The second-phase constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second-phase in-place constructor.
	 *
	 * @param aDataProvider		 An instance of a MSisDataProvider to read the entity from.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	
	void ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:

	TOperator iOperator;
	
	CExpression* iLeftExpression;
	
	CExpression* iRightExpression;

	TInt32 iIntegerValue;

	CString* iString;
	};

// inline functions from CExpression

inline TOperator CExpression::Operator() const
	{
	return iOperator;	
	}
	
inline CExpression* CExpression::Lhs() const
	{
	return iLeftExpression;
	}
	
inline CExpression* CExpression::Rhs() const
	{
	return iRightExpression;
	}
	
inline TInt32 CExpression::IntegerValue() const
	{
	return iIntegerValue;
	}
	
inline CString* CExpression::StringValue() const
	{
	return iString;
	}

 } // namespace Sis
} //namespace Swi


#endif // #ifndef __SISEXPRESSION_H__
