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
* Description:   Contains MNcdNodeIcon interface
*
*/


#ifndef M_NCD_NODE_ICON_H
#define M_NCD_NODE_ICON_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdDownloadOperation;
class MNcdDownloadOperationObserver;


/**
 *  Provides functions to handle the icon of the node.
 *  This interface supposes that a node has only one icon.
 *  The MIME type of the icon may be found automatically
 *  from the icon data by Symbian methods.
 *
 *  
 */
class MNcdNodeIcon : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeIconUid };


    /**
     * Gives the icon id. This id can be used by UI
     * to identify icons that have already been retrieved by LoadIconL.
     * So, icons can be more efficiently shared between different nodes.
     * 
     * 
     * @return Id of the loaded icon. If the protocol has not defined
     * any value, an empty string is returned.
     */
    virtual const TDesC& IconId() const = 0;


    /**
     * Gives the mime type of the icon data.
     *
     * @note This mime type is set according to the information gotten
     * from the protocol. So, this information is available before calling
     * LoadIconL for the icon item of the given index.
     *
     * @note Current implementation returns always KNullDesC
     * 
     * @return The mime type of the icon data. The type is
     * given as a text according to the standards that define MIME types.
     * Ownership is transferred. If the protocol has not defined
     * any value, an empty string is returned.
     */
    virtual const TDesC& IconMimeType() const = 0;


    /**
     * Downloads the icon file. The icon is not handled by the engine,
     * the icon data should be handled by the user of this interface as it sees fit.
     *
     * @note The reference count of the operation object is increased by one. 
     * So, Release() function of the operation should be called when the operation 
     * is not needed anymore.
     *
     * @note If a download operation has already been started for the specified
     *  icon, the already existing operation object is returned.
     *
     * 
     * @param aObserver Observer for the operation. NULL if no observer
     *  should be called. Will override previous observer
     *  for the operation, in case the operation already existed. 
     * @return Pointer to an operation object that can 
     * be used to check the progressing of the icon loading operation. Counted,
     * Release() must be called after use.
     * @exception Leave System wide error codes.
     *  KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     */ 
    virtual MNcdDownloadOperation* LoadIconL( 
        MNcdDownloadOperationObserver* aObserver ) = 0;


    /**
     * Gives the data of the loaded icon.
     *
     * 
     * @return Pointer to data of the loaded icon. Ownership is transferred.
     * A NULL pointer if the icon has not been loaded for the node.
     * @exception Leave System wide error codes.
     */
    virtual HBufC8* IconDataL() = 0;


protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodeIcon() {}
    
    };


#endif // M_NCD_NODE_ICON_H
