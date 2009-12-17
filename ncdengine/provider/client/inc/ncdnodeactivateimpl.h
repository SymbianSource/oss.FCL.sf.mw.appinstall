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
* Description:   Contains CNcdNodeActivate class
*
*/


#ifndef C_NCD_NODE_ACTIVATE_IMPL_H
#define C_NCD_NODE_ACTIVATE_IMPL_H


// For the streams
#include <s32mem.h>

#include "catalogsinterfacebase.h"
#include "ncdnodeactivate.h"

class CNcdNodeMetadataProxy;
class CNcdNodeInstallProxy;
class MNcdInstalledTheme;
class MNcdInstalledFile;
class MNcdDeviceService;

/**
 *  This class implements the functionality for the
 *  MNcdNodeActivate interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeActivate : public CCatalogsInterfaceBase, 
                         public MNcdNodeActivate
    {

public:

    /**
     * @param aMetadata Owns and uses the information of this class object.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeActivate* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeActivate* NewL( CNcdNodeMetadataProxy& aMetadata );

    /**
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeActivate* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeActivate* NewLC( CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Activate may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeActivate();


    /**
     * @return CNcdNodeMetadataProxy& Gives the metadata proxy 
     * that owns this class object.
     */
    CNcdNodeMetadataProxy& Metadata() const;

         

public: // MNcdNodeActivate

    /**
     * @see MNcdNodeActivate::ActivateL
     */ 
    virtual void ActivateL();


protected:

    /**
     * Constructor
     *
     * @param aMetadata Owns and uses the information of this class object.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeActivate* Pointer to the created object 
     * of this class.
     */
    CNcdNodeActivate( CNcdNodeMetadataProxy& aMetadata );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    
    void SetThemeL( MNcdInstalledTheme& aTheme, 
        MNcdDeviceService& aService );


    void SetContentFileL( MNcdInstalledFile& aFile, 
        CNcdNodeInstallProxy& aInstall,
        MNcdDeviceService& aService );

private:

    // Prevent if not implemented
    CNcdNodeActivate( const CNcdNodeActivate& aObject );
    CNcdNodeActivate& operator =( const CNcdNodeActivate& aObject );
    
    
private: // data

    CNcdNodeMetadataProxy& iMetadata;

    };


#endif // C_NCD_NODE_ACTIVATE_IMPL_H

