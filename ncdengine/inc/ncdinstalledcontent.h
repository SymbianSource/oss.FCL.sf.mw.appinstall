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
* Description:   Contains MNcdInstalledContent interface
*
*/


#ifndef M_NCD_INSTALLED_CONTENT_H
#define M_NCD_INSTALLED_CONTENT_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 *  Interface for accessing installed content.
 *
 *  This interface provides methods for checking the status of the installed content. 
 *  The actual content interfaces are queried by using QueryInterfaceL(). These interfaces
 *  include MNcdInstalledApplication, MNcdInstalledTheme and MNcdInstalledFile.
 *
 *  @see MNcdNodeInstall 
 *  @see MNcdInstalledApplication
 *  @see MNcdInstalledTheme
 *  @see MNcdInstalledFile
 *
 *  
 */
class MNcdInstalledContent : public virtual MCatalogsBase
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdInstalledContentUid };


    /**
     * Types of installed content
     * 
     * 
     */
    enum TInstalledContentType 
        {
        /**
         * Normal content
         */
        EInstalledContent,
        
        /**
         * Required dependency
         */
        EInstalledDependency,
        
        /**
         * Application that should be launched
         */
        EInstalledLauncher
        };
        
    /**
     * Checks if the item content has already been installed and is currently present in the
     * device.
     *
     * 
     * @return ETrue if the item content has been installed and is present, EFalse otherwise.
     * @exception Leave System wide error code.    
     * @note This is always ETrue for content that is of EInstalledLauncher type 
     */
    virtual TBool IsInstalledL() const = 0;

    
    /**
     * Checks if the content is allowed to be launched. 
     *
     * If this returns EFalse, the content should not be launched even if it has been
     * successfully installed.
     *
     * 
     * @return ETrue if the content can be launched.
     * @note This method does not check if the content has been installed or if it's
     * currently present.
     * @note For EInstalledLauncher content this is both launchable 
     * and installed status
     */
    virtual TBool IsLaunchable() const = 0;
    
    
    /**
     * Content type getter
     *
     * 
     * @return Type of the content
     */
    virtual TInstalledContentType ContentType() const = 0;

protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdInstalledContent() {}

    };


#endif // M_NCD_INSTALLED_CONTENT_H
