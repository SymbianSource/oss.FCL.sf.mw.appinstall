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
* Description:   MNcdConfigurationProtocolServerDetails declaration
*
*/


#ifndef M_NCDCONFIGURATIONPROTOCOLSERVERDETAILS_H
#define M_NCDCONFIGURATIONPROTOCOLSERVERDETAILS_H

#include <e32base.h>

class MNcdConfigurationProtocolDetail;

class MNcdConfigurationProtocolServerDetails
    {
public:
    
    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolServerDetails() {}

    /**
     * Server details version
     * @return Version or KNullDesC
     */
    virtual const TDesC& Version() const = 0;
    
    /**
     * Amount of details
     * @return Details count
     */
    virtual TInt DetailCount() const = 0;
    
    /**
     * Get details element
     * @param aIndex Details index. Leaves if index is out of bounds.
     * @return Details object reference.
     * @see DetailCount()
     */
    virtual const MNcdConfigurationProtocolDetail& DetailL( 
        TInt aIndex ) const = 0;

    /**
     * Amount of capabilities
     * @return Capability count
     */
    virtual TInt CapabilityCount() const = 0;
    
    /**
     * Get capability element
     * @param aIndex Capability index. Leaves if index is out of bounds.
     * @return Capability object reference.
     * @see CapabilityCount()
     */
    virtual const TDesC& CapabilityL( 
        TInt aIndex ) const = 0;
    };


#endif // M_NCDCONFIGURATIONPROTOCOLSERVERDETAILS_H
