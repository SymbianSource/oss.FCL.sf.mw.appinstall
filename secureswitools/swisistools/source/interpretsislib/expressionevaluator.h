/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef	EXPRESSIONEVALUATOR_H
#define	EXPRESSIONEVALUATOR_H

#include "sisfile.h"
#include "sisregistry.h"
#include "errors.h"
#include "sisexpression.h"

// Classes referenced
class RomManager;
class ConfigManager;

// Max recursion depth
const static int KMaxExpressionDepth=100;

/** 
* @file EXPRESSIONEVALUATOR.H
*
* @internalComponent
* @released
*/
class ExpressionEnvironment
	{
public:
	ExpressionEnvironment( const SisFile& aSisFile, 
                           const SisRegistry& aRegistry,
                           RomManager& aRomManager,
                           ConfigManager& aConfigManager,
                           const std::wstring& aCDrive );

	/**
	 * Check whether a file exists, corresponds to EXISTS() function in package
	 * format.
	 */
	bool FindFile(const std::wstring& aFileName, bool aLogInfo = true);

	/**
	 * Queries an application property, which is a key,value pair associated 
	 * with an isntalled package. This corresponds to APPPROP() function in
	 * package format.
	 */
	TUint32 ApplicationProperty(TUint32 aPackageUid, TUint32 aKey);

	/**
	 * Queries the registry to check if a package exists or not.
	 */
	bool Package(TUint32 aKey);

	int Variable( int aVariableId, bool aLogInfo = true );

	const std::wstring GetPackageName();

	/**
	 * Queries the registry to determine whether the version of a specific
	 * installed package satisfies the given relational condition 
	 *
	 * @param aArgsString	The argument string should contain the following
	 *						comma seperated parameters:
	 *
	 *						-packageUid 			- 	UID of the package to check
	 *						-relational Operator -	Comparision operator
	 *						-vMajor				-	Major Version component
	 *						-vMinor				-	Minor Version component
	 *						-vBuild				-	Build Version component					
	 *
	 * @return				"TRUE" if the queried package version satisfies the
	 *						condition, otherwise "FALSE".
	 */
	bool PackageVersion(const std::wstring& aArgsString);

	/**
	 * Check whether a languageID exists within the list device supported languages.
	 *
	 * @param aLanguageIdString	The Language Id to check for.
	 * @return 					True if the language exists, False otherwise.
	 */
	bool DeviceLanguage(const std::wstring& aLanguageIdString);
	
private:
	const SisFile& iSisFile;
	const SisRegistry& iSisRegistry;
    RomManager& iRomManager;
    ConfigManager& iConfigManager;
	const std::wstring& iCDrive;
	};











/**
Implements the ExpressionResult.
@internalComponent
*/
class ExpressionResult
	{
public:
	enum TType { EString, EInt };
	ExpressionResult(const std::wstring& aString);
	ExpressionResult(TUint32 aInt);
	TUint32 IntegerValue() const;
	bool BoolValue() const;
	const std::wstring& StringValue() const;
	TType Type() const { return iType; }

private:
	TType iType;
	union
		{
		const std::wstring* iString;
		TUint32 iInt;
		};
	};













inline bool operator==( const ExpressionResult& aLhs, const ExpressionResult& aRhs )
    {
    bool result( false );
    //
	if  ( aLhs.Type() != aRhs.Type() )
		{
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
		throw InvalidSis("",error,INVALID_SIS);
		}
    else if ( aLhs.Type() == ExpressionResult::EInt )
		{
        result = ( aLhs.IntegerValue() == aRhs.IntegerValue() );
		}
	else
		{
        result = ( aLhs.StringValue() == aRhs.StringValue() );
		}
    //
    return result;
    }


inline ExpressionResult operator!=( const ExpressionResult& aLhs, const ExpressionResult& aRhs )
    {
    bool equal = ( aLhs == aRhs );
    const ExpressionResult result( !equal );
    return result;
    }


inline ExpressionResult operator!( const ExpressionResult& aExpression )
    {
	if  ( aExpression.Type() != ExpressionResult::EInt )
		{
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
        throw InvalidSis("",error,INVALID_SIS);
		}
    //
    return ExpressionResult(!aExpression.IntegerValue()); // Will Never Leave
    }


inline ExpressionResult operator<( const ExpressionResult& aLhs, const ExpressionResult& aRhs )
    {
	if  ( aLhs.Type() != aRhs.Type() || aLhs.Type() != ExpressionResult::EInt )
		{
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
		throw InvalidSis("",error,INVALID_SIS);
		}
    //
    const ExpressionResult result( aLhs.IntegerValue() < aRhs.IntegerValue() );
    return result;
    }


inline ExpressionResult operator>( const ExpressionResult& aLhs, const ExpressionResult& aRhs )
    {
	if  ( aLhs.Type() != aRhs.Type() || aLhs.Type() != ExpressionResult::EInt )
		{
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
		throw InvalidSis("",error,INVALID_SIS);
		}
    //
    const ExpressionResult result( aLhs.IntegerValue() > aRhs.IntegerValue() );
    return result;
    }


inline ExpressionResult operator<=( const ExpressionResult& aLhs, const ExpressionResult& aRhs )
    {
	if  ( aLhs.Type() != aRhs.Type() || aLhs.Type() != ExpressionResult::EInt )
		{
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
		throw InvalidSis("",error,INVALID_SIS);
		}
    //
    const ExpressionResult result( aLhs.IntegerValue() <= aRhs.IntegerValue() );
    return result;
    }


inline ExpressionResult operator>=( const ExpressionResult& aLhs, const ExpressionResult& aRhs )
    {
	if  ( aLhs.Type() != aRhs.Type() || aLhs.Type() != ExpressionResult::EInt )
		{
		std::string error = "corrupt SIS file: expression corrupt in condition statement";
		throw InvalidSis("",error,INVALID_SIS);
		}
    //
    const ExpressionResult result( aLhs.IntegerValue() >= aRhs.IntegerValue() );
    return result;
    }







/**
Implements the ExpressionEvaluator.
@internalComponent
*/
class ExpressionEvaluator
	{
public:
	ExpressionEvaluator(ExpressionEnvironment& aEnvironment);
	void SetFile(const SisFile& aSisFile);
	void SetRegistry(const SisRegistry& aSisRegistry);
	
	ExpressionResult Evaluate(const CSISExpression& aExpression, bool aLogInfo = true);
	ExpressionResult Evaluate(const CSISExpression* aExpression, bool aLogInfo = true);

private:
	void Require(const void *aPointer) const;

private:
	ExpressionEnvironment& iExpEnv;
	int iExpressionDepth;
	ExpressionResult iTempResult;
	};

#endif	/* EXPRESSIONEVALUATOR_H */