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
* Definition of the CExpressionEvaluator
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef __EXPRESSIONEVALUATOR_H__
#define __EXPRESSIONEVALUATOR_H__

#include <e32base.h>
#include "sisinstallerrors.h"

namespace Swi
{
class CApplication;

namespace Sis
	{
	class CExpression;
	}

/// Maximum recursion depth
const static TInt KMaxExpressionDepth=100;

/**
 * Class which provides an environment in which to evaluate 
 * CExpression expressions from a SISX file.
 *
 * @released
 * @internalTechnology
 */
class MExpressionEnvironment
	{
public:
	/**
	 * Check whether a file exists, corresponds to EXISTS() function in package format.
	 *
	 * @param aFileName	The filename to check for.
	 * @return 			ETrue if the file exists, EFalse otherwise.
	 */
	virtual TBool FileExistsL(const TDesC& aFileName)=0;
	
	/**
	 * Check whether a languageID exists within the list device supported languages.
	 *
	 * @param aLangId	The Language Id to check for.
	 * @return 			ETrue if the language exists, EFalse otherwise.
	 */
	virtual TBool DeviceLanguageL(const TDesC& aLangId)=0;

	
	/**
	 * Queries an application property, which is a key,value pair associated with
	 * an installed package. This corresponds to APPPROP() function in package format.
	 *
	 * @param aPackageUid	The UID of the package to look for the property in.
	 * @param aKey			The key to search for.
	 * @return				The value corresponding to the property found, or 0 if no
	 *						property was found with the given key.
	 */
	virtual TInt ApplicationPropertyL(TInt aPackageUid, TInt aKey)=0;

	/**
	 * Queries the registry to check if a package exists or not.
	 *
	 * @param aKey			The UID of the package to check
	 * @return				True if the package exists, false otherwise
	 */
	virtual TInt PackageL(TInt aKey)=0;

	/**
	 * Checks to see if an option was selected by the user.
	 *
	 * @param aOptionNumber	The number of the option in the SISX file to check.
	 * @return				ETrue if the option was selected, EFalse otherwise.
	 */	
	virtual TBool OptionL(TInt aOptionNumber)=0;

	/**
	 * Gets the value of a variable, indexed by a number.
	 *
	 * @param aVariableNumber	The number of the variable to look up.
	 * @return					The value of the variable.
	 */
	virtual TInt VariableL(TInt aVariableNumber)=0;
	
	/**
	 * Queries the registry to determine whether the version of a specific
	 * installed package satisfies the given relational condition 
	 * 
	 * @param aArgsString	The argument string should contain the following
	 * 						comma seperated parameters:
	 * 
	 * 						packageUid 			- 	UID of the package to check
	 *						relational Operator -	Comparision operator
	 * 						vMajor				-	Major Version component
	 * 						vMinor				-	Minor Version component
	 * 						vBuild				-	Build Version component					
	 * 
	 * @return				ETrue if the queried package version satisfies the
	 * 						condition, otherwise EFalse.
	 */
	virtual TBool PackageVersionL(const TDesC& aArgsString)=0;
	};

/**
 * Class which provides an environment in which to evaluate 
 * CExpression expressions from a SISX file.
 *
 * @released
 * @internalTechnology
 */
class CSwisExpressionEnvironment : public CBase, public MExpressionEnvironment
	{
public:
	static CSwisExpressionEnvironment* NewL(const CApplication& aAplication);
	static CSwisExpressionEnvironment* NewLC(const CApplication& aAplication);
	
protected:
	CSwisExpressionEnvironment(const CApplication& aApplication);

	// From MExpressionEnvironment
public:
	/**
	 * Check whether a file exists, corresponds to EXISTS() function in package format.
	 *
	 * @param aFileName	The filename to check for.
	 * @return 			ETrue if the file exists, EFalse otherwise.
	 */
	virtual TBool FileExistsL(const TDesC& aFileName);
	
	/**
	 * Queries an application property, which is a key,value pair associated with
	 * an installed package. This corresponds to APPPROP() function in package format.
	 *
	 * @param aPackageUid	The UID of the package to look for the property in.
	 * @param aKey			The key to search for.
	 * @return				The value corresponding to the property found, or 0 if no
	 *						property was found with the given key.
	 */
	virtual TInt ApplicationPropertyL(TInt aPackageUid, TInt aKey);

	/**
	 * Queries the registry to check if a package exists or not.
	 * This corresponds to package() function in package format.
	 *
	 * @param aKey			The key to search for.
	 * @return				True if the package exists, false otherwise
	 */
	virtual TInt PackageL(TInt aKey);

	/**
	 * Checks to see if an option was selected by the user.
	 *
	 * @param aOptionNumber	The number of the option in the SISX file to check.
	 * @return				ETrue if the option was selected, EFalse otherwise.
	 */	
	virtual TBool OptionL(TInt aOptionNumber);
	
	/**
	 * Gets the value of a variable, indexed by a number.
	 *
	 * @param aVariableNumber	The number of the variable to look up.
	 * @return					The value of the variable.
	 */
	virtual TInt VariableL(TInt aVariableNumber);
	
	/**
	 * Check whether a languageID exists within the list device supported languages.
	 *
	 * @param aLangId	The Language Id to check for.
	 * @return 			ETrue if the language exists, EFalse otherwise.
	 */
	virtual TBool DeviceLanguageL(const TDesC& aLanguageIdString);

	
	/**
	 * Queries the registry to determine whether the version of a specific
	 * installed package satisfies the given relational condition 
	 * 
	 * @param aArgsString	The argument string should contain the following
	 * 						comma seperated parameters:
	 * 
	 * 						packageUid 			- 	UID of the package to check
	 *						relational Operator -	Comparision operator
	 * 						vMajor				-	Major Version component
	 * 						vMinor				-	Minor Version component
	 * 						vBuild				-	Build Version component					
	 * 
	 * @return				ETrue if the queried package version satisfies the
	 * 						condition, otherwise EFalse.
	 */
	virtual TBool PackageVersionL(const TDesC& aArgsString);
	
private:
	TInt ParseLanguageId(const TDesC& aComponentString);	
private:
	/**
	 * Parses the argument string by searching for the next instance of the
	 * separator, sets the next token and updates the parse string with 
	 * the remaining arguments yet to be extracted.
	 * 
	 * @param aParseString	The current state of the parse string which is used
	 * 						to extract the next token. Once the token has been 
	 * 						extracted, the string is updated so that the 
	 * 						new token and separator are removed from the
	 * 						front of the string.
	 * 
	 * @param aTokenString	Reference to the TPtrC object in which the new token
	 * 						is to be stored.		
	 * 
	 * @return				KErrNone is returned if the separator was located and
	 * 						the token and parse strings have been set, Otherwise
	 * 						KErrNotFound is returned.
	 */
	TInt ExtractNextToken(TPtrC& aParseString, TPtrC& aTokenString);

	/**
	 * Checks the passed Package UID string, setting the Package UID (TUid)
	 * if successful.
	 * 
	 * @param aUidString	Hexadecimal UID string to be converted
	 * 						into a TUid.
	 * 
	 * @param aUid			Reference to the TUid object to be set if the
	 * 						parse is successful.			
	 * 
	 * @return				KErrNone is returned if the parse was successful
	 * 						and the TUid object has been set, Otherwise 
	 * 						KErrNotFound is returned.
	 */
	TInt ParsePackageUid(const TDesC& aUidString, TUid& aUid);

	/**
	 * Checks the passed version component string returning the version
	 * component value as a TInt.
	 * 
	 * Note: This function does NOT support Wildcards.
	 * 
	 * @param aComponentString	Version component string to be converted
	 * 							into a TInt.
	 * 
	 * @return					KErrNotFound if the parse fails, otherwise the decimal
	 * 							representation as a TInt.
	 */
	TInt ParseVersionComponent(const TDesC& aComponentString);
	
private:
	const CApplication& iApplication;
	};

/**

 * Class which provides an environment in which to evaluate CExpression
 * expressions from a SISX file, before any options have been chosen.
 *
 * @released
 * @internalTechnology
 */
class CSwisCreateExpressionEnvironment : public CSwisExpressionEnvironment
	{
public:
	static CSwisCreateExpressionEnvironment* NewL(const CApplication& aAplication);
	static CSwisCreateExpressionEnvironment* NewLC(const CApplication& aAplication);
	
private:
	CSwisCreateExpressionEnvironment(const CApplication& aApplication);
	
public:
	/**
	 * Returns ETrue, regardless of the selected option.
	 *
	 * @param aOptionNumber	The number of the option in the SISX file to check.
	 * @return				ETrue.
	 */	
	virtual TBool OptionL(TInt aOptionNumber);	

private:
	const CApplication& iApplication;
	};

/**
 * Class to provide the result of an expression evaluation.
 */
class TExpressionResult
	{
public:
	enum TType { EString, EInt };

	TExpressionResult(const TDesC& aString);
	TExpressionResult(TInt aInt);
	
	TInt IntegerValueL() const;
	
	TBool BoolValueL() const;
	
	const TDesC& StringValueL() const;

	TType Type() const;	

//	friend TExpressionResult operator!(const TExpressionResult& aExpression);
private:
	TType iType;
	union
		{
		const TDesC* iString; // Needs to be Pointer due to initialisation
		TInt iInt;
		};
	};
 
TExpressionResult operator!=(const TExpressionResult& aLhs, const TExpressionResult& aRhs);
TExpressionResult operator==(const TExpressionResult& aLhs, const TExpressionResult& aRhs);
TExpressionResult operator<(const TExpressionResult& aLhs, const TExpressionResult& aRhs);
TExpressionResult operator>(const TExpressionResult& aLhs, const TExpressionResult& aRhs);
TExpressionResult operator<=(const TExpressionResult& aLhs, const TExpressionResult& aRhs);
TExpressionResult operator>=(const TExpressionResult& aLhs, const TExpressionResult& aRhs);
TExpressionResult operator!(const TExpressionResult& aExpression);

/**
 * Class which evaluates CExpression expressions from a SISX file.
 *
 * @released
 * @internalTechnology
 */
class CExpressionEvaluator : public CBase
	{
public:
	/**
	 * This creates a new CExpressionEvaluator object.
	 * @param aEnvironment	The environment to carry out the expression in.
	 *						This provides, lookup of options, etc.
	 */
	static CExpressionEvaluator* NewL(MExpressionEnvironment& aEnvironment);

	/**
	 * This creates a new CExpressionEvaluator object and leaves it in the cleanup stack.
	 * @param aEnvironment	The environment to carry out the expression in.
	 *						This provides, lookup of options, etc.
	 */
	static CExpressionEvaluator* NewLC(MExpressionEnvironment& aEnvironment);

	/**
	 * Evaluate the expression.
	 * 
	 * @param aExpression	The expression to be evaluated.
	 * @return					The result of the expression evaluation.
	 */
	TExpressionResult EvaluateL(const Sis::CExpression& aExpression);

	/**
	 * Evaluate the expression.
	 * 
	 * @param aExpression	The expression to be evaluated.
	 * @return					The result of the expression evaluation.
	 */
	TExpressionResult EvaluateL(const Sis::CExpression* aExpression);

private:
	
	/// Constructor
	CExpressionEvaluator(MExpressionEnvironment& aEnvironment);

	/// This function leaves with KErrSwiInvalidExpression if the pointers passed are NULL
	void RequireL(const void *aPointer) const;
	
private:
	/// The environment to carry out expression evaluation in
	MExpressionEnvironment& iEnvironment;
	
	/** 
	 * The depth of the expression currently being evaluated, used to prevent attempts to evaluate
	 * too complex expressions, which would cause stack overflows.
	 */
	TInt iExpressionDepth;
	
	/**
	 * Used to hold the result temporarily, between evaluating the recursive expression
	 * and returning the result, while the depth counter is decremented.
	 */
	TExpressionResult iTemporaryResult;
	};

#include "expressionevaluator.inl"

} // end namespace Swi
#endif
