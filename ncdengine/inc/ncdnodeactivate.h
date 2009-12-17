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
* Description:   Contains MNcdNodeInstall interface
*
*/


#ifndef M_NCD_NODE_ACTIVATE_H
#define M_NCD_NODE_ACTIVATE_H

#include <f32file.h>
#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 *  Provides functionality for activating the content represented by the node.
 *  Well-defined only for simple nodes that contain exactly one activateable item,
 *  such as image (set as wallpaper), audio file (set as ringing tone) or theme.
 *  
 *  Before activating, the data should have been downloaded and installed.
 *  download interface.
 *
 *  @see MNcdNodeDownload
 *  @see MNcdNodeInstall
 *
 *  
 */
class MNcdNodeActivate : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeActivateUid };


    /**
     * Activates the item represented by the node.
     *
     * For example, if the node holds a suitable image, it is set as wallpaper. If the
     * node holds a suitable audio file, it is set as ringing tone. If the node holds a
     * theme file, it is set as the current theme for the device.
     *
     * If the node has no content suitable for activation, it should not offer this
     * interface in the first place.
     * 
     * 
     * @exception Leave System wide error code.
     */
    virtual void ActivateL() = 0;

protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodeActivate() {}

    };


#endif // M_NCD_NODE_ACTIVATE_H
