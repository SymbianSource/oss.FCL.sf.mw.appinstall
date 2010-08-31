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
* This file defines a package recognizer for the Reference Installer.
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef TSIFPKGREC_H
#define TSIFPKGREC_H

/**
A test package recognizer. This class implements the CApaDataRecognizerType interface.
Please see the description of @see CApaDataRecognizerType for details. 

The SIF Server uses this class through the AppArc APIs to recognize reference packages.
*/
class CTestSifPkgRecognizer : public CApaDataRecognizerType
	{
public:
	/**
	Gets the size of buffer preferred for the purpose of recognizing the data type.
	*/
	TUint PreferredBufSize();

	/**
	Gets one of the data (MIME) types that the recognizer can recognize.
	
	@param aIndex An index that identifies the data type.
	@return The data (MIME) type.
	*/
	TDataType SupportedDataTypeL(TInt aIndex) const;

	/**
	Creates an instance of the recognizer.
	*/
	static CApaDataRecognizerType* CreateRecognizerL();

private:
	CTestSifPkgRecognizer();

	/**
	Implements the recognition algorithm for the Reference Installer.
	*/
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

#endif // TSIFPKGREC_H
