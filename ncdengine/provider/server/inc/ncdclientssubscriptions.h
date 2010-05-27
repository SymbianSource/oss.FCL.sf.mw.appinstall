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
* Description:  
*
*/


#ifndef C_NCD_CLIENTS_SUBSCRIPTIONS_H
#define C_NCD_CLIENTS_SUBSCRIPTIONS_H

#include <e32base.h>

class CNcdSubscriptionGroup;
class CNcdSubscriptionsSourceIdentifier;

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdClientsSubscriptions : public CBase
    {


public:


    static CNcdClientsSubscriptions* NewL( TUid aId );

    static CNcdClientsSubscriptions* NewLC( TUid aId );

    virtual ~CNcdClientsSubscriptions();


    /**
     * Function that adds a subscription group to
     * this client's subscriptions.
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void AddGroupL( CNcdSubscriptionGroup* aGroup );
    
    /**
     * Function that removes a subscription group from
     * this client's subscriptions.
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */    
    void RemoveGroup( CNcdSubscriptionGroup* aGroup );
    
    /**
     * Returns reference to all subscription groups of
     * the client whose subscriptions the instance of
     * this class represents.
     */
    RPointerArray<CNcdSubscriptionGroup>& Groups();
    
    /**
     * Returns uid that the owner of these subscriptions
     * has. The uid can be whatever desired uid that identifies
     * the client or group. At the moment it is the familyid
     * of the client.
     */
    TUid ClientId() const;


protected:


private:


    CNcdClientsSubscriptions( TUid aId );

    void ConstructL();


private: // data

    /**
     * All subscriptions of this client
     */
    RPointerArray<CNcdSubscriptionGroup> iClientsSubscriptions;

    /**
     * Client identifier
     */
    TUid iId;

    };



#endif // C_NCD_CLIENTS_SUBSCRIPTIONS_H
