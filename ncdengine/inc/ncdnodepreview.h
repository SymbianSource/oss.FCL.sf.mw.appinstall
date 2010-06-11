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
* Description:   Contains MNcdNodePreview
*
*/


#ifndef M_NCD_NODE_PREVIEW_H
#define M_NCD_NODE_PREVIEW_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdDownloadOperation;
class MNcdDownloadOperationObserver;
class RFile;

/**
 *  This interface is provided if the node contains preview content.
 *  One node may contain multiple preview items.
 *
 *  
 */
class MNcdNodePreview : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodePreviewUid };


    /**
     * Gives the number of previews of the node.
     *
     * @note This count is given according to the protocol information
     * that informs how many previews are available. So, the information
     * can be asked before calling LoadPreviewL. But, to get the
     * data of the wanted preview, LoadPreviewL has to be called.
     *
     * @return The number of preview items.
     */
    virtual TInt PreviewCount() const = 0;


    /**
     * IsPreviewLoadedL informs if the preview has already been loaded for the
     * node. Then, call of LoadPreviewL is not necessary.
     *
     * @param aIndex tells which one of the previews is wanted.
     * @return ETrue if the preview has already been loaded, EFalse otherwise.
     * @exception Panic ENcdPanicIndexOutOfRange Array index out of range.
     */
    virtual TBool IsPreviewLoadedL( TInt aIndex ) const = 0;


    /**
     * Gives the mime type of the preview data.
     *
     * This mime type is set according to the information gotten
     * from the protocol if any. So, this information is available before 
     * calling LoadPreviewL for the preview item of the given index. 
     *
     * @note If the mime type is not received in protocol responses, it is
     * taken from Content-Type HTTP header after the preview has been downloaded
     * successfully
     *
     * @param aIndex tells which one of the previews is wanted.
     * @return The mime type of the preview data. The type is
     * given as a text according to the standards that define MIME types.
     * Ownership is transferred. If the protocol has not defined
     * any value, KNullDesC is returned.
     * @exception Panic ENcdPanicIndexOutOfRange Array index out of range.
     */
    virtual const TDesC& PreviewMimeType( TInt aIndex ) const = 0;


    /**
     * Loads the preview data from the internet.
     *
     * @note The reference count of the operation object is increased by one. 
     * So, Release() function of the operation should be called when operation 
     * is not needed anymore.
     *
     * @param aIndex tells which one of the previews is wanted.
     * @param aObserver Observer for the operation.
     * @return Pointer to an operation object that handles
     *  the preview data downloading. This operation can be used
     *  to check the progressing of the preview download. Counted, Release() must
     *  be called after use.
     * @exception Leave System wide error codes.
     *  KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     * @exception Panic ENcdPanicIndexOutOfRange Array index out of range.
     */
    virtual MNcdDownloadOperation* LoadPreviewL( TInt aIndex,
        MNcdDownloadOperationObserver* aObserver ) = 0;


    /**
     * Gives the preview data file.
     *
     * @note RFile handle can be used to get the unifying id for example
     * using RFile::FullName.
     * @note The MIME type of the preview may be found automatically
     * from the preview data by Symbian methods.
     *
     * @param aIndex tells which one of the previews is wanted. 
     * @return Handle to a file containing the preview data. Ownership is transferred.
     *  So, the user has to call Close when the file is not needed anymore.
     * @exception KErrNotFound if the corresponding preview data 
     *  has not been downloaded for the specified preview item.
     * @exception Leave System wide error codes.
     * @exception Panic ENcdPanicIndexOutOfRange Array index out of range.
     */
    virtual RFile PreviewFileL( TInt aIndex ) const = 0;


protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodePreview() {}

    };


#endif // M_NCD_NODE_PREVIEW_H
