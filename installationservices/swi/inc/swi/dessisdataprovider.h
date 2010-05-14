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
* Declaration of the CDesSisDataProvider
*
*/


/**
 @file
 @released
 @internalTechnology 
*/

#ifndef __DESSISDATAPROVIDER_H
#define __DESSISDATAPROVIDER_H

#include <e32std.h>

#include "sisdataprovider.h"

namespace Swi
{

/**
 * Implementation of MSisDataProvider interface that takes a descriptor as data
 * source
 *
 * @released
 * @internalTechnology
 */
class CDesDataProvider : public CBase, public MSisDataProvider
	{
public:
	/**
	 * Constructs a new DES data provider object.
	 *
	 * @param  aDes Descriptor to use as data source
	 *
	 * @return Pointer to a newly created object
	 */
	IMPORT_C static CDesDataProvider* NewL(const TDesC8& aDes);
	
	/**
	 * Constructs a new DES data provider object and places it on the 
	 * cleanup stack.
	 *
	 * @param  aDes Descriptor to use as data source
	 *
	 * @return Pointer to a newly created object
	 */
	IMPORT_C static CDesDataProvider* NewLC(const TDesC8& aDes);

public: // From MSisDataProvider
	
	/**
	 * Reads data from data source into a descriptor
	 *
	 * @param  aDes Descriptor to read into (up to its max length)
	 * @return KErrNone or an error code
	 */
	IMPORT_C virtual TInt Read(TDes8& aDes);
	
	/**
	 * Reads the specified amount of data into a descriptor
	 *
	 * @param  aDes    Descriptor to read data into
	 * @param  aLength Max data length to be read
	 * @return KErrNone or an error code
	 */
	IMPORT_C virtual TInt Read(TDes8& aDes, TInt aLength);
	
	/**
	 * Seeks into data source
	 *
	 * @param  aMode Seek mode; the following modes are supported:
	 *		   ESeekStart   Seek from the beginning of data
	 *		   ESeekCurrent Seek from the current position
	 *		   ESeekEnd     Seek from the end of data
	 * @return KErrNone or an error code
	 *
	 * For more information on this function see the specification
	 * of the following functions in the Symbian OS Library:
	 * - CData::Seek(), which is used in CCafSisDataProvider::Seek(),
	 * - RFile::Seek(), which is used in CFileSisDataProvider::Seek()
	 */
	IMPORT_C virtual TInt Seek(TSeek aMode, TInt64& aPos);

	IMPORT_C virtual ~CDesDataProvider();

private:

	/** 
	 * The constructor.
	 */
	CDesDataProvider(const TDesC8& aDes);

	/** Des to read the data from */
	TPtrC8 iDes;

	/** Position **/
	TInt32 iPos;	
	};

} // namespace Swi

#endif // #ifndef __DESSISDATAPROVIDER_H
