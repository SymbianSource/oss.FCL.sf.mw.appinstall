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



#ifndef IA_UPDATE_PLATFORM_DEPENDENCY_H
#define IA_UPDATE_PLATFORM_DEPENDENCY_H


#include <e32base.h>

#include "iaupdateversion.h"


/**
 * CIAUpdatePlatformDependency contains and provides
 * platform specific dependency information.
 *
 * @since S60 v3.2
 */
class CIAUpdatePlatformDependency : public CBase
    {
    
public:

    /**
     * @return CIAUpdatePlatformDependency* Created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformDependency* NewL();

    /**
     * @see CIAUpdatePlatformDependency::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdatePlatformDependency* NewLC();


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdatePlatformDependency();
    

    /**
     * Checks if the dependency platform information specifies
     * an acceptable s60 platform. Also, check if the platform version
     * is acceptable.
     *
     * @return TBool ETrue if platform information is acceptable. 
     * EFalse if platform information is not acceptable.
     * @exception Leaves with system wide error code.
     *
     * @since S60 v3.2
     */
    TBool AcceptablePlatformL() const;

    /**
     * Sets the platform information.
     *
     * @param aPlatform The platform information.
     *
     * @since S60 v3.2
     */
    void SetPlatformL( const TDesC8& aPlatform );
    

    /**
     * @return const TIAUpdateVersion& Version floor of the platform
     * dependency.
     *
     * @since S60 v3.2
     */
    const TIAUpdateVersion& VersionFloor() const;

    /**
     * @param aVersion Version floor of the platform
     * dependency.
     *
     * @since S60 v3.2
     */
    void SetVersionFloor( const TVersion& aVersion );
    
    
    /**
     * return const TIAUpdateVersion& Version roof of the platform
     * dependency.
     *
     * @since S60 v3.2
     */
    const TIAUpdateVersion& VersionRoof() const;

    /**
     * @param aVersion Version roof of the platform
     * dependency.
     *
     * @since S60 v3.2
     */
    void SetVersionRoof( const TVersion& aVersion );
        

    /**
     * Resets the dependency information to the default values.
     *
     * @since S60 v3.2
     */
    void Reset();
    
    
protected:

    /**
     * Constructor
     *
     * @since S60 v3.2
     */
    CIAUpdatePlatformDependency();
    
    /**
     * 2nd. phase constructor.
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();
    

private:

    // Prevent these if not implemented.
    CIAUpdatePlatformDependency( const CIAUpdatePlatformDependency& aObject );
    CIAUpdatePlatformDependency& operator =( const CIAUpdatePlatformDependency& aObject );
    

private: // data

    TIAUpdateVersion iVersionFloor;
    TIAUpdateVersion iVersionRoof;    

    HBufC8* iPlatform;
    
    };
    
#endif // IA_UPDATE_PLATFORM_DEPENDENCY_H
