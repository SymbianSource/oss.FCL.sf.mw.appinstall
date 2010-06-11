/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdInstalledTheme class
*
*/


#ifndef NCD_INSTALLED_THEME_H
#define NCD_INSTALLED_THEME_H



#include "ncdinstalledcontentimpl.h"
#include "ncdinstalledtheme.h"


class CNcdNodeInstallProxy;

/**
 * 
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdInstalledTheme: public CNcdInstalledContent,
                          public MNcdInstalledTheme
    {
public:

    /**
     * NewL
     *
     * @param aParent Parent
     * @param aTheme Theme name. Ownership is transferred
     *
     * @leave KErrArgument if aTheme is an empty descriptor
     * @leave KErrNoMemory if aTheme is NULL
     */
    static CNcdInstalledTheme* NewL( 
        CNcdNodeInstallProxy& aParent,
        HBufC* aTheme );


    /**
     * NewLC
     *
     * @param aParent Parent
     * @param aTheme Theme name. Ownership is transferred
     *
     * @leave KErrArgument if aTheme is an empty descriptor
     * @leave KErrNoMemory if aTheme is NULL
     *     
     */
    static CNcdInstalledTheme* NewLC( 
        CNcdNodeInstallProxy& aParent,
        HBufC* aTheme );
    
    
public: // MNcdInstalledContent


    /**
     * @see MNcdInstalledContent::IsInstalledL()
     */
    TBool IsInstalledL() const;

    

public: // MNcdInstalledTheme

    /**
     * @see MNcdInstalledTheme::Theme()
     */
    const TDesC& Theme() const;
    
    
protected:

    /**
     * Constructor
     *
     */
    CNcdInstalledTheme( 
        CNcdNodeInstallProxy& aParent, 
        HBufC* aTheme );
    

    /**
     * ConstructL
     */
    virtual void ConstructL();


    /**
     * Destructor
     *
     * Informs the node manager that this node is deleted.
     */
    virtual ~CNcdInstalledTheme();



private:
    // Prevent if not implemented
    CNcdInstalledTheme( const CNcdInstalledTheme& aObject );
    CNcdInstalledTheme& operator =( const CNcdInstalledTheme& aObject );
            
private: // data

    HBufC* iTheme;

    };


#endif // NCD_INSTALLED_THEME_H
