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
* Description:   Definition of MNcdPurchaseInstallInfo interface and
*                implementation class.
*
*/


#ifndef M_NCDPURCHASEINSTALLINFO_H
#define M_NCDPURCHASEINSTALLINFO_H


#include <e32base.h>


/**
 *  Interface for purchase install information.
 *
 *  
 */
class MNcdPurchaseInstallInfo
    {
public:

    /**
     * Get name and path of installed file.
     * 
     * 
     * @return File name.
     */
    virtual const TDesC& Filename() const = 0;
    
    /**
     * Get application UID of installed application.
     * 
     * 
     * @return TUid Application UID.
     */
    virtual TUid ApplicationUid() const = 0;
    
    /**
     * Get application version of installed application.
     * 
     * 
     * @return Application version.
     */
    virtual const TDesC& ApplicationVersion() const = 0;
    
    /**
     * Get name of installed theme.
     * 
     * 
     * @return Theme name.
     */
    virtual const TDesC& ThemeName() const = 0;
    
protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdPurchaseInstallInfo() {}

    };


#endif // M_NCDPURCHASEINSTALLINFO_H
