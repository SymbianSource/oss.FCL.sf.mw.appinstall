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
* Description:   Contains MNcdNodeSkin interface
*
*/


#ifndef M_NCD_NODE_SKIN_H
#define M_NCD_NODE_SKIN_H


#include <e32cmn.h>
#include <f32file.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdFileDownloadOperation;
class MNcdFileDownloadOperationObserver;


/**
 *  This interface is provided for the node if the node contains a skin.
 *
 *  
 */
class MNcdNodeSkin : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeSkinUid };


    /**
     * Gives the skin id. This id can be used by UI
     * to identify icons that have already been retrieved by LoadSkinL.
     * 
     * 
     * @return Id of the skin. If the protocol has not defined
     *  any value, an empty string is returned.
     */
    virtual const TDesC& SkinId() const = 0;


    /**
     * Returns the time stamp that informs when the skin was downloaded. 
     * The client may use this value to check if a new skin is wanted.
     *
     * 
     * @return Time when the skin vas downloaded. TTime is set to
     *  zero if the time stamp was not given by the protocol.
     */
    virtual TTime SkinTimeStamp() const = 0;


    /**
     * Downloads the skin file. The skin is not handled by the engine, the file
     * should be handled by the user of this interface as it sees fit.
     *
     * @note The reference count of the operation object is increased by one. 
     * So, Release function of the operation should be called when the operation 
     * is not needed anymore.
     *
     * @note The file will be moved to the target location after it has been 
     * fully downloaded. Any existing file with the same name will be overwritten.     
     *
     * 
     * @param aTargetFileName Full path and name for the target file.
     * @param aObserver Observer for the operation.
     * @return  Operation which can be monitored to check the
     *  processing of the skin loading. Counted, Release() must be called after use.
     * @exception Leave System wide error codes.
     *  KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     */ 
    virtual MNcdFileDownloadOperation* LoadSkinL( 
        const TDesC& aTargetFileName,
        MNcdFileDownloadOperationObserver& aObserver ) = 0;


protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodeSkin() {}
    
    };


#endif // M_NCD_NODE_SKIN_H
