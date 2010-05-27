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
* Description:   Contains MNcdBundle interface
*
*/


#ifndef M_NCD_BUNDLE_H
#define M_NCD_BUNDLE_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 * Empty interface. The only usage currenly is to check whether a 
 * node is a bundle folder by querying this interface.
 */
class MNcdBundle : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdBundleUid };
    
    virtual const TDesC& ViewType() const = 0;
    
protected:

   /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdBundle() {}

    };


#endif // M_NCD_BUNDLE_H
