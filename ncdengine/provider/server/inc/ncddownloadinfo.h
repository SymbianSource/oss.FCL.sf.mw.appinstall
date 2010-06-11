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
* Description:   CNcdDownloadInfo
*
*/


#ifndef NCD_DOWNLOAD_INFO_HH
#define NCD_DOWNLOAD_INFO_HH

// Contains CNcdPurchaseDownloadInfo
#include "ncdutils.h"
#include "ncdnodedependency.h"

class MNcdPreminetProtocolEntityDependency;
class MNcdPreminetProtocolDownload;

/**
 *  A extended implementation for the CNcdPurchaseDownloadInfo class.
 *  This class can be initialized by passing the protocol class object
 *  to the constructors.
 *
 *  @lib ncdutils_20019119.dll
 *  @since S60 v3.2
 *  @see MNcdPurchaseDownloadInfo
 */
class CNcdDownloadInfo : public CNcdPurchaseDownloadInfo
    {
public:

    /**
     * Constructor.
     * 
     * @since S60 v3.2
     * @return Download information class.
     */
    static CNcdDownloadInfo* NewL();

    /**
     * Constructor.
     * 
     * @since S60 v3.2
     * @return Download information class.
     */
    static CNcdDownloadInfo* NewLC();


    /**
     * Creates a download information class object from the information 
     * that the protocol has provided.
     * 
     * @since S60 v3.2
     * @return Download information class.
     */     
    static CNcdDownloadInfo* NewL( 
        const MNcdPreminetProtocolEntityDependency& aProtocolDependency );

    /**
     * Creates a download information class object from the information 
     * that the protocol has provided.
     * 
     * @since S60 v3.2
     * @return Download information class.
     */
    static CNcdDownloadInfo* NewLC( 
        const MNcdPreminetProtocolEntityDependency& aProtocolDependency );

    /**
     * Creates a download information class object from the information 
     * that the protocol has provided.
     * 
     * @since S60 v3.2
     * @return Download information class.
     */
    static CNcdDownloadInfo* NewLC( 
        const MNcdPreminetProtocolDownload& aProtocolDownload );


    /**
     */
    static CNcdDownloadInfo* NewLC( 
        const MNcdPurchaseDownloadInfo& aDownload,
        TUid aUid,
        const TDesC& aVersion );



    /**
     * Destructor
     */
    virtual ~CNcdDownloadInfo();    

public:

    virtual void InternalizeL( RReadStream& aStream );
    virtual void ExternalizeL( RWriteStream& aStream );

public:

    /**
     * Content Name getter
     *
     * @since S60 v3.2     
     * @return Name
     */
    const TDesC& ContentName() const;

    /**
     * Content ID getter
     *
     * @since S60 v3.2     
     * @return Id
     */
    TUid ContentId() const;
    
    
    /**
     * Content version getter
     *
     * @since S60 v3.2     
     * @return Version
     */
    const TDesC& ContentVersion() const;
    
    
    /**
     * Dependency state getter
     */
    TNcdDependencyState DependencyState() const;
    
    /**
     * Dependency state setter
     */
    void SetDependencyState( TNcdDependencyState aState );
    

protected:  

    /**
     * Constructor
     */
    CNcdDownloadInfo();

    /**
     * Alternative private 2nd phase constructor.
     */
    virtual void ConstructL( 
        const MNcdPreminetProtocolEntityDependency& aProtocolDependency );

    void ConstructL( 
        const MNcdPreminetProtocolDownload& aProtocolDownload );

    void ConstructDownloadL( 
        const MNcdPreminetProtocolDownload& aProtocolDownload );

    void ConstructL( 
        const MNcdPurchaseDownloadInfo& aDownload,
        TUid aUid,
        const TDesC& aVersion );


private:

    // Prevent these if they are not implemented
    CNcdDownloadInfo( const CNcdDownloadInfo& aObject );
    CNcdDownloadInfo& operator =( const CNcdDownloadInfo& aObject );

private:

    TUid iUid;
    HBufC* iVersion;
    TNcdDependencyState iDependencyState;
    };

#endif // NCD_DOWNLOAD_INFO_HH
