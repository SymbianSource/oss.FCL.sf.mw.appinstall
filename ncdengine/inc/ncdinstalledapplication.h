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
* Description:   Contains MNcdInstallApplication interface
*
*/


#ifndef M_NCD_INSTALLED_APPLICATION_H
#define M_NCD_INSTALLED_APPLICATION_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 * 
 *
 *  @see MNcdNodeInstall 
 *
 *  
 */
class MNcdInstalledApplication : public virtual MCatalogsBase
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdInstalledApplicationUid };

    /**
     * Returns the UID of the installed application.
     * 
     * @note Valid only if IsInstalledL() returns ETrue
     *
     * 
     * @see MNcdNodeContentInfo
     * @return UID of the installed application.     
     */
    virtual TUid Uid() const = 0;


    /**
     * Document name that should be given to the application when it is
     * launched
     *
     * 
     * @return Document name
     */
    virtual const TDesC& DocumentName() const = 0;

protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdInstalledApplication() {}

    };


#endif // M_NCD_INSTALLED_APPLICATION_H
