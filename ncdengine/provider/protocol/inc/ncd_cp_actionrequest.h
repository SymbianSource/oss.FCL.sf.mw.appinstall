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
* Description:   MNcdConfigurationProtocolActionRequest declaration
*
*/


#ifndef M_NCDCONFIGURATIONPROTOCOLACTIONREQUEST_H
#define M_NCDCONFIGURATIONPROTOCOLACTIONREQUEST_H

class MNcdConfigurationProtocolQuery;
class MNcdConfigurationProtocolDetail;

class MNcdConfigurationProtocolUpdateDetails
    {
public:
    
    virtual ~MNcdConfigurationProtocolUpdateDetails() {}

    virtual const TDesC& Id() const = 0;
    virtual const TDesC& Version() const = 0;
    virtual const TDesC& Uri() const = 0;

    };

class MNcdConfigurationProtocolActionRequest
    {
public:
    enum TNcdProtocolActionRequestType
        {
        ETypeAdd,
        ETypeRemove,
        ETypeClear,
        ETypeUpdate
        };
    
    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolActionRequest() {}

    /**
     * Is action forced
     * @return True or false
     */
    virtual TBool Force() const = 0;

    /**
     * Action request type
     * @return Action type
     * @see TNcdProtocolActionRequestType
     */
    virtual TNcdProtocolActionRequestType Type() const = 0;

    /**
     * Action target
     * @return Target string or KNullDesC
     */
    virtual const TDesC& Target() const = 0;

    /**
     * Amount of message elements in the action
     * @return Message count
     */
    virtual TInt MessageCount() const = 0;
    
    /**
     * Get message element
     * @param aIndex Message index. Leaves if index is out of bounds.
     * @return Message object reference.
     * @see MessageCount()
     */
    virtual const MNcdConfigurationProtocolQuery& MessageL( TInt aIndex ) const = 0;
    
    /**
     * Amount of details objects
     * @return
     */
    virtual TInt DetailCount() const = 0;
    
    /**
     * Get details object
     * @param aIndex Details index. Leaves if index is out of bounds.
     * @return Details object reference.
     * @see DetailCount()
     */
    virtual const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const = 0;

    /**
     * Get update details. Ownership is NOT transferred.
     * @return Update details pointer or NULL if not available.
     */
    virtual const MNcdConfigurationProtocolUpdateDetails* UpdateDetails() const = 0;
    };

#endif
