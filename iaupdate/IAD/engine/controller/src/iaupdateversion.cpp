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



#include "iaupdateversion.h"
#include "iaupdateutils.h"

 
EXPORT_C TIAUpdateVersion::TIAUpdateVersion()
    {
    SetToFloor();
    }


EXPORT_C TIAUpdateVersion::TIAUpdateVersion( const TVersion& aVersion )
    {
    iMajor = aVersion.iMajor;
    iMinor = aVersion.iMinor;
    iBuild = aVersion.iBuild;
    }
    
    
EXPORT_C void TIAUpdateVersion::InternalizeL( const TDesC& aString )
    {
    iMajor = 0;
    iMinor = 0;
    iBuild = 0;    
    IAUpdateUtils::DesToVersionL( aString, iMajor, iMinor, iBuild );
    }
    

EXPORT_C void TIAUpdateVersion::SetToFloor()
    {
    iMajor = 0;
    iMinor = 0;
    iBuild = 0;
    }
	

EXPORT_C void TIAUpdateVersion::SetToRoof()
    {
    iMajor = 127;
    iMinor = 127;
    iBuild = 32767;    
    }
    
    
EXPORT_C const TIAUpdateVersion &TIAUpdateVersion::operator =( const TVersion& aVersion )
    {
    iMajor = aVersion.iMajor;
    iMinor = aVersion.iMinor;
    iBuild = aVersion.iBuild;
    
    return *this;    
    }


EXPORT_C TBool TIAUpdateVersion::operator ==( const TIAUpdateVersion& aVer ) const
    {
    if( iMajor == aVer.iMajor 
        && iMinor == aVer.iMinor 
        && iBuild == aVer.iBuild )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
    
    
EXPORT_C TBool TIAUpdateVersion::operator !=( const TIAUpdateVersion& aVer ) const
    {
    return !( *this == aVer );
    }
   
    
EXPORT_C TBool TIAUpdateVersion::operator <=( const TIAUpdateVersion& aVer ) const
    {
    return !( *this > aVer );
    }
  
    
EXPORT_C TBool TIAUpdateVersion::operator >=( const TIAUpdateVersion& aVer ) const
    {
    return !( *this < aVer );
    }
   
    
EXPORT_C TBool TIAUpdateVersion::operator<( const TIAUpdateVersion& aVer ) const
    {
    TBool result = EFalse;
    
    if( iMajor < aVer.iMajor )
        {
        result = ETrue;
        }
    else if( iMajor == aVer.iMajor )
        {
        if( iMinor < aVer.iMinor )
            {
            result = ETrue;
            }
        else if( iMinor == aVer.iMinor )
            {
            if( iBuild < aVer.iBuild )
                {
                result = ETrue;
                }
            }
        }

    return result;
    }

    
EXPORT_C TBool TIAUpdateVersion::operator >( const TIAUpdateVersion& aVer ) const
    {
    TBool result = EFalse;
    
    if( iMajor > aVer.iMajor )
        {
        result = ETrue;
        }
    else if( iMajor == aVer.iMajor )
        {
        if( iMinor > aVer.iMinor )
            {
            result = ETrue;
            }
        else if( iMinor == aVer.iMinor )
            {
            if( iBuild > aVer.iBuild )
                {
                result = ETrue;
                }
            }
        }

    return result;
    }
