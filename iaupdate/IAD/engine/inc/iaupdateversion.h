/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   ?Description
*
*/



#ifndef IAUPDATEVERSION_H
#define IAUPDATEVERSION_H

#include <e32base.h>

class TIAUpdateVersion : public TVersion
	{
	
public:

    IMPORT_C TIAUpdateVersion();

	IMPORT_C TIAUpdateVersion( const TVersion& aVersion );
	
    /**
     * Parses passed string for version information.
     * @param aString String representing version 
     * ( <major>.<minor>.<build> )
     **/
     
	IMPORT_C void InternalizeL( const TDesC& aString );
	
	/**
	 * Set this version entitys value to minimum version revision 
	 * possible to represent
	 **/
	IMPORT_C void SetToFloor();
	
	/**
	 * Set this version entitys value to maximum version revision 
	 * possible to represent
	 **/
	IMPORT_C void SetToRoof();

	
	IMPORT_C const TIAUpdateVersion& operator =( const TVersion& aVersion );
	IMPORT_C TBool operator ==( const TIAUpdateVersion& aVer ) const;
	IMPORT_C TBool operator !=( const TIAUpdateVersion& aVer ) const;		
	IMPORT_C TBool operator <=( const TIAUpdateVersion& aVer ) const;
	IMPORT_C TBool operator >=( const TIAUpdateVersion& aVer ) const;
	IMPORT_C TBool operator <( const TIAUpdateVersion& aVer ) const;
	IMPORT_C TBool operator >( const TIAUpdateVersion& aVer ) const;
	
	};


#endif  //  IAUPDATEVERSION_H
