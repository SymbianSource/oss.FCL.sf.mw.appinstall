/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateRendezvous
*
*/



#ifndef IA_UPDATE_RENDEZVOUS_H
#define IA_UPDATE_RENDEZVOUS_H


#include <e32base.h>
#include <e32std.h>

class MIAUpdateRendezvousObserver;


/**
 * CIAUpdateRendezvous
 *
 * This is a CActive class that observes when another
 * process completes rendezvous request. 
 * When this observer is informed about the completion, 
 * this class object informs the MIAUpdateRendezvousObserver
 * about the completion.
 *
 * @see MIAUpdateRendezvousObserver
 *
 * @since S60 v3.2
 */
class CIAUpdateRendezvous : public CActive
{

public:

    /**
     * @param aObserver The information about the completion of
     * an operation is forwarded to this observer by calling its callback
     * function.
     * @return CIAUpdateRendezvous* Newly created object.
     *
     * @since S60 v3.2
     */
    static CIAUpdateRendezvous* NewL( MIAUpdateRendezvousObserver& aObserver );

    /**
     * @see NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateRendezvous* NewLC( MIAUpdateRendezvousObserver& aObserver );


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateRendezvous();


    /**
     * Sets this object active for observing a rendezvous operation.
     * 
     * @param aProcessId Process id of the on-going process whose rendezvous
     * will be listened here.
     *
     * @since S60 v3.2
     */
    void StartL( const TProcessId& aProcessId );

    
protected: // CActive

    /**
     * @see CActive::DoCancel
     *
     * Cancels the operation.
     *
     * @since S60 v3.2
     */
    virtual void DoCancel();

    /**
     * @see CActive::RunL
     *
     * When the rendezvous process releases the rendezvous, this function calls
     * the callback function of the observer.
     *
     * @since S60 v3.2
     */
    virtual void RunL();


protected:

    /**
     * @see NewL
     *
     * @since S60 v3.2
     */ 
    CIAUpdateRendezvous( MIAUpdateRendezvousObserver& aObserver );

    /**
     * @see NewL
     *
     * @since S60 v3.2
     */ 
    virtual void ConstructL();
    
    
    /**
     * @return MIAUpdateRendezvousObserver& The information 
     * about the completion of an operation is forwarded to 
     * this observer by calling its callback function.
     * Ownership is not transferred.
     *
     * @since S60 v3.2
     */
    MIAUpdateRendezvousObserver& Observer();

    /**
     * @return RProcess& Handle to the process that is listening to the
     * rendezvous to complete.
     *
     * @since S60 v3.2
     */
    RProcess& RendezvousProcess();


private:

    // Prevent these if not implemented
    CIAUpdateRendezvous( const CIAUpdateRendezvous& aObject );
    CIAUpdateRendezvous& operator =( const CIAUpdateRendezvous& aObject );


private: // data

    // The information about the completion of
    // an operation is forwarded to this observer 
    // by calling its callback function.
    MIAUpdateRendezvousObserver& iObserver;

    // The process handle that is listening the rendezvous.
    RProcess iProcess;
};

#endif // IA_UPDATE_RENDEZVOUS_H
