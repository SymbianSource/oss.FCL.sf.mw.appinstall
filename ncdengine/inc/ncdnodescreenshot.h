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
* Description:   Contains MNcdNodeScreenshot interface
*
*/


#ifndef M_NCD_NODE_SCREENSHOT_H
#define M_NCD_NODE_SCREENSHOT_H


#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdDownloadOperation;
class MNcdDownloadOperationObserver;


/**
 *  This interface is provided for the node if it contains screenshots.
 *  The node may contains multiple screenshots.
 *
 *  
 */
class MNcdNodeScreenshot : public virtual MCatalogsBase
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeScreenshotUid };


    /**
     * Gives the number of the screenshots for the node.
     * 
     * @note This count is given according to the protocol information
     * that informs how many screenshots for the node are available. So, the information
     * can be asked before calling LoadScreenshotL. But, to get the
     * data of the wanted screenshot, LoadScreenshotL has to be called.
     *
     * 
     * @return The number of screenshots.
     */
     virtual TInt ScreenshotCount() const = 0;


    /**
     * Gives the mime type of the screenshot data.
     *
     * @note This mime type is set according to the information gotten
     * from the protocol. So, this information is available before calling
     * LoadScreenshotL for the screenshot item of the given index.
     *
     * @note Current servers do not send mime types for screenshots so
     * this always returns KNullDesC
     * 
     * @param aIndex tells which one of the screenshots is wanted.
     * @return The mime type of the screenshot data. The type is
     *  given as a text according to the standards that define MIME types.
     *  If the protocol has not defined
     *  any value, an empty string is returned.
     * @exception Panic ENcdPanicIndexOutOfRange Index out of range.
     */
    virtual const TDesC& ScreenshotMimeType( TInt aIndex ) const = 0;


    /**
     * Downloads the screenshot file. The screenshot is not handled by the engine,
     * the screenshot data should be handled by the user of this interface as it
     * sees fit.
     *
     * @note The reference count of the operation object is increased by one. 
     * So, Release() function of the operation should be called when the operation 
     * is not needed anymore.
     *
     * @note If a download operation has already been started for the specified
     *  screenshot, the already existing operation object is returned.
     *
     * 
     * @param aIndex Index of the screenshot that should be loaded.
     * @param aObserver Observer for the operation.
     * @return Pointer to an operation object that can 
     *  be used to check the progressing of the screenshot loading. Counted,
     *  Release() must be called after use.
     * @exception Leave System wide error codes.
     *  Leave with KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     * @exception Panic ENcdPanicIndexOutOfRange Index out of range.
     */ 
     virtual MNcdDownloadOperation* LoadScreenshotL( TInt aIndex,
         MNcdDownloadOperationObserver* aObserver ) = 0;


    /**
     * Gives the data of the loaded screenshot.
     *
     *  The MIME type of the screenshot may be found automatically
     *  from the screenshot data by Symbian methods.
     *
     * 
     * @param aIndex is the index of the screenshot.
     * @return Data of the loaded screenshot. Ownership is
     *  transferred. NULL if the data has not been loaded.
     * @exception Leave System wide error codes.
     * @exception Panic ENcdPanicIndexOutOfRange Index out of range.
     */
     virtual HBufC8* ScreenshotDataL( TInt aIndex ) const = 0;


protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdNodeScreenshot() {}
    
    };


#endif // M_NCD_NODE_SCREENSHOT_H
