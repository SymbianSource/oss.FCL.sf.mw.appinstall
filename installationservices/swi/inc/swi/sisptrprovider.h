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
 @internalTechnology
 @released
*/
 
#ifndef __SISPTRPROVIDER_H__
#define __SISPTRPROVIDER_H__

namespace Swi
	{

namespace Sis
	{
	
	/**
	 * @publishedPartner
	 * @released
	 */ 
	class TPtrProvider
		{
	public:
		inline TPtrProvider(TPtr8 aDataSource, TUint aPos = 0);
		inline TPtrC8 ReadL(TUint aLength);
		
	private:
		TPtr8 iDataSource;
		TUint iPos;
		};
	
	inline TPtrProvider::TPtrProvider(TPtr8 aDataSource, TUint aPos)
		: iDataSource(aDataSource), iPos(aPos)
		{
		}
		
	inline TPtrC8 TPtrProvider::ReadL(TUint aLength)
		{
		if (aLength + iPos > iDataSource.Length())
			{
			User::Leave(KErrEof);
			}
		
		TPtrC8 ret(iDataSource.Mid(iPos, aLength));
		iPos += aLength;
		return ret;
		}
	
	} /* namespace Swi::Sis */
	
	} /* namespace Swi */

#endif /* __SISPTRPROVIDER_H__ */
