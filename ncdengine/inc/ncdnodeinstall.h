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


#ifndef M_NCD_NODE_INSTALL_H
#define M_NCD_NODE_INSTALL_H

#include <f32file.h>
#include <e32cmn.h>

#include "catalogsbase.h"
#include "catalogsarray.h"
#include "ncdinterfaceids.h"


class MNcdInstallOperation;
class MNcdInstallOperationObserver;
class MDesCArray;
class MNcdInstalledContent;

// Forward declaration for Usif::TInstallOptions
namespace Usif
    {
    class COpaqueNamedParams;    
    }

/**
 *  Provides functions that can be used to install the node object.
 *  Before installing, the data should have been downloaded using
 *  download interface.
 *
 *  @see MNcdNodeDownload
 *  @see MNcdNodeUriContent
 *
 *  
 */
class MNcdNodeInstall : public virtual MCatalogsBase
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeInstallUid };


    /**
     * Starts the installing operation of the node object.
     * 
     * @note The reference count of the operation object is increased by one. So, Release()
     * function of the operation should be called when operation is not needed anymore.
     * 
     * 
     * @param aObserver Observer for the operation.
     * @return The operation that handles the installation. 
     * This operation can be used to check the progressing of the installation. Counted,
     * Release() must be called after use.
     * @exception Leave System wide error code.
     *  Leaves with KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     */
    virtual MNcdInstallOperation* InstallL( MNcdInstallOperationObserver& aObserver ) = 0;

    /**
     * Starts the silent installing operation of the node object.
     * Because installation is silent, normal installation query dialogs are not shown.
     * But, the caller of this function may use aInstallOptions parameter to define that 
     * text files that are included in SIS packages will be shown in query dialogs during installation.
     * This may be required, for example, to show disclaimers that are included in the SIS packages
     * as text files.
     * 
     * @note The reference count of the operation object is increased by one. So, Release()
     * function of the operation should be called when operation is not needed anymore.
     * 
     * @note Requires TrustedUI capabilities
     * @note Only SIS packages and Java archives and widgets are installed silently, other content
     * is installed exactly like with InstallL
     * 
     * @param aObserver Observer for the operation.
     * @param aInstallOptions These install options are used by the silent install.
     * @return The operation that handles the installation. 
     * This operation can be used to check the progressing of the installation. Counted,
     * Release() must be called after use.
     * @exception Leave System wide error code.
     * Leaves with KErrPermissionDenied if UI does not have TrustedUI capability.
     * Leaves with KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     */
    virtual MNcdInstallOperation* SilentInstallL( 
                MNcdInstallOperationObserver& aObserver,
                Usif::COpaqueNamedParams* aInstallOptions ) = 0;

    /**
     * Checks if the item content has already been installed and is currently present in the
     * device.
     *
     * @note This returns EFalse if some of the content is missing, eg. if one of several
     * image files has been deleted.
     *
     * 
     * @return ETrue if the item content has been installed, EFalse otherwise.
     * @exception Leave System wide error code.     
     */
    virtual TBool IsInstalledL() const = 0;

    
    /**
     * Checks if the item is allowed to be launched. 
     *
     * @note This returns ETrue if at least one of installed themes/applications/files 
     * is launchable
     *
     * 
     * @return ETrue if the item can be launched.
     * @note This method does not check if the item has been installed or if it's
     * currently present.
     */
    virtual TBool IsLaunchable() const = 0;
    
    
    
    /**
     * Returns an array of content installed from the item
     * 
     * @return Array of installed content
     * @exception Leave System wide error code.     
     */
    virtual RCatalogsArray<MNcdInstalledContent> InstalledContentL() = 0;


    /**
     * This function may be used to update install information of the node if the 
     * node content has been externally installed.
     * 
     * @note The use of this function should be avoided in normal install cases.
     * In normal install cases, all the necessary actions are done automatically. 
     * This function should only be used if node content is installed externally 
     * and the content can not be installed by using other functions provided in 
     * this interface. This function is meant only for the application content.
     * The content has to be downloaded by using MNcdNodeDownload interface before
     * using this function.
     *
     * @note When this function is called with KErrNone parameter, the node content 
     * is assumed to be succesfully installed. Therefore, possible content installation 
     * files will be deleted as in normal succesfull install cases. The purchase history 
     * details and installation report information are updated to correspond the given 
     * error code information.
     *
     * @param aErrorCode The error code describes the success of the install.
     * If KErrNone is given, then the installation is assumed to be a success. 
     * If some other error code is given, then the installation is assumed to fail in some way.
     * The details related to the installations are updated according to the error code.
     * @exception Leave System wide error code.     
     */
    virtual void SetApplicationInstalledL( TInt aErrorCode ) = 0;


protected:

    /**
     * Destructor.
     * 
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodeInstall() {}

    };


#endif // M_NCD_NODE_INSTALL_H
