/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdInstalledApplication class
*
*/


#ifndef NCD_INSTALLED_APPLICATION_H
#define NCD_INSTALLED_APPLICATION_H


#include "ncdinstalledcontentimpl.h"
#include "ncdinstalledapplication.h"
#include "catalogsutils.h"


class CNcdNodeInstallProxy;

/**
 * 
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdInstalledApplication: public CNcdInstalledContent,
                                public MNcdInstalledApplication
    {
public:

    /**
     * NewLC
     */
    static CNcdInstalledApplication* NewLC( 
        CNcdNodeInstallProxy& aParent,
        const TUid& aUid,
        const TDesC& aVersion,
        const TDesC& aDocumentName,
        MNcdInstalledContent::TInstalledContentType aType,
        TBool aUriExists );
    
    
public: // MNcdInstalledContent


    /**
     * @see MNcdInstalledContent::IsInstalledL()
     */
    virtual TBool IsInstalledL() const;

    

public: // MNcdInstalledApplication

    /**
     * @see MNcdInstalledApplication::Uid()
     */
    virtual TUid Uid() const;
    
        
    /**
     * @see MNcdInstalledApplication::DocumentName()
     */
    virtual const TDesC& DocumentName() const;
    
protected:

    /**
     * Constructor
     *
     */
    CNcdInstalledApplication( 
        CNcdNodeInstallProxy& aParent, 
        const TUid& aUid,
        MNcdInstalledContent::TInstalledContentType aType,
        TBool aUriExists );
    

    /**
     * ConstructL
     */
    virtual void ConstructL( 
        const TDesC& aVersion,
        const TDesC& aDocumentName );


    /**
     * Destructor
     *
     * Informs the node manager that this node is deleted.
     */
    virtual ~CNcdInstalledApplication();



private:
    // Prevent if not implemented
    CNcdInstalledApplication( const CNcdInstalledApplication& aObject );
    CNcdInstalledApplication& operator =( const CNcdInstalledApplication& aObject );
            
private: // data

    TUid iUid;

    HBufC* iDocumentName;
    TCatalogsVersion iVersion;
    TBool iUriExists;
    };


#endif // NCD_INSTALLED_APPLICATION_H
