/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of MIAUpdateFwNode class 
*
*/



#ifndef IA_UPDATE_FW_NODE_H
#define IA_UPDATE_FW_NODE_H


#include <e32cmn.h>
#include "iaupdateanynode.h"


class MIAUpdateFwNodeObserver;


/**
 * MIAUpdateFwNode
 * Interface for firmware nodes.
 *
 * @note Implementations of MIAUpdateFwNode interface will always
 * return MIAUpdateAnyNode::ENodeTypeFw for MIAUpdateAnyNode::NodeType 
 * function call.
 *
 * @see MIAUpateAnyNode
 */
class MIAUpdateFwNode : public MIAUpdateAnyNode
    {    
    
public:

    /**
     * Enumerates the possible types of updates
     **/
    enum TFwUpdateType
        {
        
        /** 
         * FOTA DP2
         */
        EFotaDp2,

        /** 
         * FOTI NSU
         */
        EFotiNsu,

        };


    /**
     * @return TFwUpdateType The type of this firmware update.
     **/
    virtual TFwUpdateType FwType() const = 0;

    /**
     * @return const TDesC& Firmware update version info
     * part 1.
     */
    virtual const TDesC& FwVersion1() const = 0;

    /**
     * @return const TDesC& Firmware update version info
     * part 2.
     */
    virtual const TDesC& FwVersion2() const = 0;

    /**
     * @return const TDesC& Firmware update version info
     * part 3.
     */
    virtual const TDesC& FwVersion3() const = 0;

    /**
     * This function needs to be called in order to initialize
     * download related information for the object.
     *
     * @note This function may require long time to finish.
     * This function handles the purchasing of the content if required,
     * in addition to updating purchase related history information 
     * into the databases. Therefore, do not call this unless downloading 
     * is really wanted.
     *
     * @note MIAUpdateFwNodeObserver callback function is called when
     * the asynchronous operation is completed. In case of cancel, the
     * callback is not called.
     *
     * @param aObserver Observer whose callbacks are called when asynchronous
     * operation progresses.
     * @return TBool ETrue if an asynchronous operation was started.
     * EFalse, if no new operations were required.
     * @exception Leaves with KErrInUse if operation is already going on.
     * Else, leaves with system wide error code.
     *
     * @see MIAUpdateFwNode::ContentUrl
     * @see MIAUpdateFwNodeObserver::InitDownloadDataComplete
     */
    virtual TBool InitDownloadDataL( 
        MIAUpdateFwNodeObserver& aObserver ) = 0;

    /**
     * @note To get the correct value for this, InitDownloadDataL needs
     * to be called. If initialization is not done, an empty descriptor
     * is retured.
     *
     * @see MIAUpdateFwNode::InitDownloadDataL
     *
     * @return const TDesC& Download URL for firmware update.
     */
    virtual const TDesC& ContentUrl() const = 0;


protected:

    virtual ~MIAUpdateFwNode() { }
        
    };

#endif  //  IA_UPDATE_FW_NODE_H
