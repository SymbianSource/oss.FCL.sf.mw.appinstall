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
 @internalComponent 
 @released
*/

#ifndef __CHECKEDVERSION__
#define __CHECKEDVERSION__

#include <e32base.h>
#include "sisversion.h"

namespace Swi 
	{
	/**
	 * A derivative of TVersion that allows for relational operator comparisons and validity checking
	 * 
	 * IMPORTANT NOTE: This class makes use of version comparisons using Wildcards. Any comparison made using
	 * the relational operator functions provided by this class that contain wildcards are deemed to result
	 * in ETrue if the overall condition has not already decided.
	 * 
	 * e.g.
	 * 
	 * 3.5.0 <= 2.*.*	->	EFalse	(iMajor component decides the overall comparison)
	 * 2.5.0 <= 2.*.*	->	ETrue
	 * 2.5.0 <= 2.5.*	->	ETrue
	 */
	class TCheckedVersion : public TVersion
		{
		public:
		/**
		 * Constructs the object with an instance of TVersion.
		 * 
		 * @param aVersion	Object containing version information
		 */
		TCheckedVersion (const TVersion aVersion);
		
		/**
		 * Constructs the object with an instance of SIS::CVersion.
		 * 
		 * @param aVersion	Object containing version information
		 */
		TCheckedVersion (const Sis::CVersion& aVersion);
		
		/**
		 * Determines whether the stored version is "Equal To" the version stored within the specified
		 * TCheckedVersion object.
		 * 
		 * Important Note: This == operator is NOT the inverse of != due to wildcard comparisons
		 * resulting in an ETrue result.
		 * 
		 * @param aOther	TCheckedVersion object with which to compare each component
		 * 					value.
		 * 
		 * @return			ETrue is returned if the component version values being compared are:
		 * 						- equal to each other
		 * 						- either side contains a Wildcard
		 * 
		 *					Otherwise, EFalse is returned.
		 */
		TBool operator==(const TCheckedVersion& aOther);
		
		/**
		 * Determines whether the stored version is "Less Than Or Equal To" the version stored within
		 * the specified TCheckedVersion object.
		 * 
		 * Important Note: This <= operator is NOT the inverse of > due to wildcard comparisons
		 * resulting in an ETrue result. 
		 * 
		 * @param aOther	TCheckedVersion object with which to compare each component 
		 * 					value.
		 * 
		 * @return			ETrue is returned if: 
		 * 						- the stored version is less than or equal to the value contained within aOther 
		 * 						- wildcards have been used within the comparison and the other components don't
		 * 						  satisfy the condition.  
		 * 
		 * 					Otherwise, EFalse is returned.	
		 */
		TBool operator<=(const TCheckedVersion& aOther);
		
		/**
		 * Determines whether the stored version is "Greater Than Or Equal To" the version stored within
		 * the specified TCheckedVersion object.
		 * 
		 * Important Note: This >= operator is NOT the inverse of < due to wildcard comparisons
		 * resulting in an ETrue result.
		 * 
		 * @param aOther	TCheckedVersion object with which to compare each component 
		 * 					value.
		 * 
		 * @return			ETrue is returned if: 
		 * 						- the stored version is greater than or equal to the value contained within aOther 
		 * 						- wildcards have been used within the comparison and the other components don't
		 * 						  satisfy the condition.  
		 * 
		 * 					Otherwise, EFalse is returned.	
		 */
		TBool operator>=(const TCheckedVersion& aOther);
		
		/**
		 * Determines whether the stored version is "Less Than" the version stored within the specified
		 * TCheckedVersion object.
		 * 
		 * Important Note: This < operator is NOT the inverse of >= due to wildcard comparisons
		 * resulting in an ETrue result.
		 * 
		 * @param aOther	TCheckedVersion object with which to compare each component 
		 * 					value.
		 * 
		 * @return			ETrue is returned if: 
		 * 						- the stored version is less than the value contained within aOther 
		 * 						- wildcards have been used within the comparison and the other components don't
		 * 						  satisfy the condition.
		 * 
		 * 					Otherwise, EFalse is returned.
		 */
		TBool operator<(const TCheckedVersion& aOther);
		
		/**
		 * Determines whether the stored version is "Greater Than" the version stored within the specified
		 * TCheckedVersion object.
		 * 
		 * Important Note: This > operator is NOT the inverse of <= due to wildcard comparisons
		 * resulting in an ETrue result.
		 * 
		 * @param aOther	TCheckedVersion object with which to compare each component 
		 * 					value.
		 * 
		 * @return			ETrue is returned if: 
		 * 						- the stored version is greater than the value contained within aOther 
		 * 						- wildcards have been used within the comparison and the other components don't
		 * 						  satisfy the condition.
		 * 
		 * 					Otherwise, EFalse is returned.
		 */
		TBool operator>(const TCheckedVersion& aOther);
		
		/**
		 * Determines whether the stored version is "Not Equal To" the version stored within the specified
		 * TCheckedVersion object.
		 * 
		 * Important Note: This != operator is NOT the inverse of == due to wildcard comparisons
		 * resulting in an ETrue result.
		 * 
		 * @param aOther	TCheckedVersion object with which to compare each component
		 * 					value.
		 * 
		 * @return			ETrue is returned if the component version values being compared are:
		 * 						- not equal to each other
		 * 						- either side contains a Wildcard
		 * 
		 *					Otherwise, EFalse is returned.
		 */
		TBool operator!=(const TCheckedVersion& aOther);

		/**
		 * Determines whether the iMajor, iMinor and iBuild components of the stored version are valid and
		 * within range.
		 * 
		 * @return			ETrue is returned if ALL of the following conditions result to ETrue:
		 * 						- iMajor ranges between 0 and 127 inclusive, or is a wildcard
		 * 						- iMinor ranges between 0 and 99 inclusive, or is a wildcard
		 * 						- iBuild ranges between 0 and 32767 inclusive, or is a wildcard
		 * 
		 * 					Otherwise, EFalse is returned. 
		 */
		TBool IsValid();
		
		static const TInt KWildCard=-1;
		};

	} // Swi::

#endif /* __CHECKEDVERSION__ */
