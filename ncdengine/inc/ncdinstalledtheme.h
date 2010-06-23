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
* Description:   Contains MNcdInstalledTheme interface
*
*/


#ifndef M_NCD_INSTALLED_THEME_H
#define M_NCD_INSTALLED_THEME_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 * 
 *
 *  @see MNcdNodeInstall 
 *
 *  
 */
class MNcdInstalledTheme : public virtual MCatalogsBase
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdInstalledThemeUid };


    /**
     * Returns the name of the theme.
     *
     * @note Valid only if MNcdNodeInstall::IsInstalledL() returns ETrue
     *
     * 
     * @see MNcdNodeContentInfo
     * @return Installed name of the item.
     */
    virtual const TDesC& Theme() const = 0;



protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdInstalledTheme() {}

    };


#endif // M_NCD_INSTALLED_THEME_H
