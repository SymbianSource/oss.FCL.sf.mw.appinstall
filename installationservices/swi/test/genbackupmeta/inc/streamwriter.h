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
* For writing data to a file in accordance to symbian descriptor format.
* @internalComponent
*
*/



#ifndef	STREAMWRITER_H
#define	STREAMWRITER_H

#include "symbiantypes.h"
#include <fstream>
#include <iostream>

/**
* The stream writer class currently creates the descriptor information for 
the descriptor provided.
*/
class CStreamWriter
	{
	public:
		CStreamWriter(const std::string& aFileName);
		~CStreamWriter();
		
		void Write( char* aData, int aLength );
		void Externalize( const char* aData, const int aLength, const bool aWide );
		void Externalize( const int aLength, const bool aWide );
		TUint32 CreateDescriptorInfo( const TUint32 aLength, const bool aIsWide );
		
	private:
		CStreamWriter(const CStreamWriter&);
		CStreamWriter& operator=( const CStreamWriter& );
		TUint32 GetBitLength(const TUint32 length);

	private:
		std::ofstream iFileStream; 
	};

#endif // __STREAMWRITER_H__