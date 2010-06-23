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
* Definition of the CUserSelections
*
*/




/**
 @file
 @internalTechnology
*/

#ifndef __USERSELECTIONS_H__
#define __USERSELECTIONS_H__

#include <e32base.h>

namespace Swi
{
/**
 * Class which contains details of the user's selections.
 */
class CUserSelections : public CBase
	{
public:
	static CUserSelections* NewL();

	static CUserSelections* NewLC();

	/**
	 * Returns selected language
	 *
	 * @return Selected Language 
	 */
	TLanguage Language() const;

	/**
	 * Returns selected drive.
	 *
	 * @return User selected drive to install applications on
	 */
	TChar Drive() const;

	/**
	 * Returns selected options.
	 *
	 * @return User selected drive to install applications on
	 */
	const RArray<TBool>& Options() const;

	/**
	 * Sets selected drive.
	 *
	 * aDrive The drive the user selected to install the applications on
	 */
	void SetDrive(TChar aSelectedDrive);

	/**
	 * Sete selected language.
	 *
	 * aLanguage The language the user selected
	 */
	void SetLanguage(TLanguage aLanguage);

	/**
	 * Sets selected options.
	 *
	 * aDrive The drive the user selected to install the applications on
	 */
	IMPORT_C void SetOptionsL(const RArray<TBool>& aOptions);

	~CUserSelections();
private:
	// constructor
	CUserSelections();
	
private:
	
	/// Drive user wished to install application(s) onto
	TChar iDrive;

	/// List of languages user has selected to install
	TLanguage iLanguage;

	/// List of options, ETrue if the option was selected by the user, EFalse otherwise
	RArray<TBool> iOptions;
	}; 

} // end namespace Swi

#include "userselections.inl"

#endif
