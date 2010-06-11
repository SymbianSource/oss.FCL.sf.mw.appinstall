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
* Description:   Install operation interface definition
*
*/
	

#ifndef M_NCD_INSTALL_OPERATION_H
#define M_NCD_INSTALL_OPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"


/**
 *  Install operation interface.
 *  
 *  Install operation handles the installing of an installable node.
 *  The operation installs one or more files in predefined order.
 *
 *  The install process is handled outside of Catalogs Engine, in the
 *  platform installer.
 *
 *  The platform installer has it's own ui, (which is initiated
 *  automatically) that gives the user more detailed progress information.
 *
 *  
 *  @see MNcdInstallOperationObserver
 *  @see MNcdNodeInstall
 */
class MNcdInstallOperation : public MNcdOperation
    {

public:
    
    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdInstallOperationUid };
    
    /**
     * Getter for file count. This is the total number of files this operation
     * is installing.
     *
     * 
     * @return Total number of files to be installed.
     */
    virtual TInt FileCount() = 0;
    
    /**
     * Getter for current file index. This is the index of the file that
     * is currently being installed.
     *
     * @return Number of the file currently being installed.
     */
    virtual TInt CurrentFile() = 0;
    
protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdInstallOperation() {}

    };
	
	
#endif //  M_NCD_INSTALL_OPERATION_H
