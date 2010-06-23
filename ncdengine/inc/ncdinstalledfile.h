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
* Description:   Contains MNcdInstalledFile interface
*
*/


#ifndef M_NCD_INSTALLED_FILE_H
#define M_NCD_INSTALLED_FILE_H

#include <f32file.h>
#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"


/**
 * 
 *
 *  @see MNcdNodeInstall 
 *
 *  
 */
class MNcdInstalledFile : public virtual MCatalogsBase
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdInstalledFileUid };


    /**
     * Gives the mime type of a directly accessible installed content file.
     *
     * @note Valid only if MNcdNodeInstall::IsInstalledL() returns ETrue
     *
     * 
     * @return The mime type of the download content data. The type is
     *  given as a text according to the standards that define MIME types.
     *  If protocol has not defined any value, an empty string is returned.
     */
    virtual const TDesC& MimeType() const = 0;


    /**
     * Gives a handle to directly accessible installed content file. The handle
     * may be used to launch an application or do something else that UI seems fit.
     *
     * @note Valid only if MNcdNodeInstall::IsInstalledL() returns ETrue
     *
     * 
     * @return Handle to the file that was installed and is now
     *  node content. Ownership is transferred. So, Close should
     *  be called when file is not used anymore.
     * @exception Leave System wide error code.
     */
    virtual RFile OpenFileL() = 0;


protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdInstalledFile() {}

    };


#endif // M_NCD_INSTALLED_FILE_H
