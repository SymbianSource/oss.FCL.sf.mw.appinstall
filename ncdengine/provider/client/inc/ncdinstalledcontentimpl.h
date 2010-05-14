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
* Description:   Contains CNcdInstalledContent class
*
*/


#ifndef NCD_INSTALLED_CONTENT_H
#define NCD_INSTALLED_CONTENT_H


// For streams
#include <s32mem.h>

#include "catalogsinterfacebase.h"
#include "ncdinstalledcontent.h"

class CNcdNodeInstallProxy;

/**
 * Base class for installed content 
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdInstalledContent: public CCatalogsInterfaceBase,
                            public MNcdInstalledContent                     
    {

public: // MNcdInstalledContent

    /**
     * @see MNcdInstalledContent::IsLaunchable()
     */
    virtual TBool IsLaunchable() const;


    /**
     * @see MNcdInstalledContent::ContentType()
     */
    virtual MNcdInstalledContent::TInstalledContentType ContentType() const;
    
    
public: // New methods

    /**
     * Sets the launchable state for the object
     *
     * @param aLaunchable
     */
    virtual void SetLaunchable( TBool aLaunchable );
        
    
public: // MCatalogsBase

    /**
     * @note In addition to using the AddRef of the base class,
     * this function increases the internal reference count of the owner.
     * This way, the owner will not be deleted before the reference count
     * of this object is zero. 
     * The owner should not be deleted before this class object's 
     * reference count is back to zero. The owner should
     * handle the deletion of this class object separately for example 
     * when its destructor is called.
     *
     * @see MCatalogsBase::AddRef()
     */
    virtual TInt AddRef();
    
    /**
     * @note In addition to using the Rlease of the base class,
     * this function uses the Release of the owner.
     * This way, the owner will know if it can be deleted in other words if
     * this class object is not used anymore.  
     * The owner should not be deleted before this class object's 
     * reference count is back to zero. The owner should
     * handle the deletion of this class object separately for example 
     * when its destructor is called.
     *
     * @see MCatalogsBase::Release()
     */
    virtual TInt Release();
       
protected:

    /**
     * Constructor
     *
     */
    CNcdInstalledContent( 
        CNcdNodeInstallProxy& aParent, 
        TInstalledContentType aType = EInstalledContent );


    /**
     * ConstructL
     */
    void ConstructL();

    /**
     * Destructor
     *
     * Informs the node manager that this node is deleted.
     */
    virtual ~CNcdInstalledContent();

    /**
     * @return CNcdNodeInstallProxy& reference to the class that owns
     * this class.
     */
    CNcdNodeInstallProxy& ContentOwner() const;
    
    
private:
    // Prevent if not implemented
    CNcdInstalledContent( const CNcdInstalledContent& aObject );
    CNcdInstalledContent& operator =( const CNcdInstalledContent& aObject );
            

private: // data

    // Not owned
    CNcdNodeInstallProxy& iOwner;
    
    TInstalledContentType iContentType;
    TBool iLaunchable;
    };


#endif // NCD_INSTALLED_CONTENT_H
