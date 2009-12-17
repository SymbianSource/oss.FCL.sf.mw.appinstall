/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdExtendedInstallInfo
*
*/


#ifndef NCD_EXTENDED_INSTALL_INFO_HH
#define NCD_EXTENDED_INSTALL_INFO_HH

// Contains CNcdPurchaseDownloadInfo
#include "ncdutils.h"
#include "ncdinstalledcontent.h"
#include "ncdinstallationservice.h"

/**
 *  A extended implementation for the CNcdPurchaseInstallInfo class.
 *
 *  @lib ncdutils_20019119.dll
 *  @since S60 v3.2
 *  @see MNcdPurchaseInstallInfo
 */
class CNcdExtendedInstallInfo : public CNcdPurchaseInstallInfo
    {
public:

    /**
     * Constructor.
     * 
     * @since S60 v3.2
     * @return Install information class.
     */
    static CNcdExtendedInstallInfo* NewL();

    /**
     * Constructor.
     * 
     * @since S60 v3.2
     * @return Install information class.
     */
    static CNcdExtendedInstallInfo* NewLC();


    /**
     * Copy constructor
     * 
     * @since S60 v3.2
     * @return Install information class.
     */
    static CNcdExtendedInstallInfo* NewLC( 
        const MNcdPurchaseInstallInfo& aInstall,
        const TDesC& aMimeType,
        TBool aLaunchable );


    /**
     * Destructor
     */
    virtual ~CNcdExtendedInstallInfo();    

public:

    virtual void InternalizeL( RReadStream& aStream );
    virtual void ExternalizeL( RWriteStream& aStream );

public:

    /**
     * Content mime type getter
     *
     * @since S60 v3.2     
     * @return Mime type
     */
    const TDesC& MimeType() const;
    
    
    /**
     * Launchable status getter
     *
     * @since S60 v3.2     
     * @return Launchable status
     */
    TBool IsLaunchable() const;
    
    /**
     * Launchable status setter
     *
     * @param aLaunchable Launchable status
     */
    void SetLaunchable( TBool aLaunchable );
    
    /**
     * Content type setter
     */
    void SetContentType( MNcdInstalledContent::TInstalledContentType aType );
    
    /**
     * Content type getter
     */
    MNcdInstalledContent::TInstalledContentType ContentType() const;


    /**
     * Parameter setter
     * 
     * Parameter is content specific. Currently it is only used for
     * setting the document name for the launched application
     */
    void SetParameterL( const TDesC& aParam );
    
    /**
     * Parameter getter
     */
    const TDesC& Parameter() const;
    
    
    void SetInstalledStatus( TNcdApplicationStatus aStatus );
    
    TNcdApplicationStatus InstalledStatus() const;
    
    
    void SetUriExists( TBool aUriExists );
    
    TBool UriExists() const;
    
protected:  

    /**
     * Constructor
     */
    CNcdExtendedInstallInfo( TBool aLaunchable = EFalse );

    /**
     * ConstructL
     */
    void ConstructL();

    void ConstructL( 
        const MNcdPurchaseInstallInfo& aInstall,
        const TDesC& aMimeType );

private:

    // Prevent these if they are not implemented
    CNcdExtendedInstallInfo( const CNcdExtendedInstallInfo& aObject );
    CNcdExtendedInstallInfo& operator =( 
        const CNcdExtendedInstallInfo& aObject );

private:

    HBufC* iMimeType;
    TBool iLaunchable;
    MNcdInstalledContent::TInstalledContentType iContentType;
    HBufC* iParameter;
    TNcdApplicationStatus iInstalledStatus;
    TBool iUriExists;
    };

#endif // NCD_EXTENDED_INSTALL_INFO_HH
