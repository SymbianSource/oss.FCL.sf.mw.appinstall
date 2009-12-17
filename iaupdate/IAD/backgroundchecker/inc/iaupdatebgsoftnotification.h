/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef IAUPDATEBGSOFTNOTIFICATION_H
#define IAUPDATEBGSOFTNOTIFICATION_H

// INCLUDES
#include <e32base.h>
#include <AknDynamicSoftNotifier.h>
#include "iaupdatebgcheckermode.h"

// FORWARD DECLARATION
class TAknDynamicSoftNotificationParams;
class CIAUpdateBGInternalFileHandler;

class MIAUpdateBGSoftNotificationCallBack
    {
    public:
        virtual void SoftNotificationCallBack( TBool aIsAccepted ) = 0;
        virtual TIAUpdateBGMode CurrentMode() = 0;
    };


// CLASS DECLARATION
/**
 *  Controls the native soft notifications.
 *  This class delegates method calls to AvKon soft notification.
 *
 *  @since 3.2
 */
class CIAUpdateBGSoftNotification : public CBase, public MAknDynamicSoftNoteObserver
    {
    public:  // Constructors and destructor

        /**
         * Static constructor
         */
        static CIAUpdateBGSoftNotification* NewL( MIAUpdateBGSoftNotificationCallBack* aCallback, CIAUpdateBGInternalFileHandler* aInternalFile );

        /**
         * Destructor.
         */
        virtual ~CIAUpdateBGSoftNotification();

    public: // From MAknDynamicSoftNotifierObserver

        /**
         * Dynamic soft notification was accepted by user.
         *
         * @param aNoteId Identification of notification.
         */
        void NotificationAccepted( TInt aIdentifier );

        /**
         * Dynamic soft notification was canceled by user.
         *
         * @param aNoteId Identification of notification.
         */
        void NotificationCanceled( TInt aIdentifier );

    public: // New functions
        
        void StartObservingIfNeededL();
        
        
        /**
         * Add a custom soft notification. If a custom soft notification with
         * exactly the same parameters already exists,
         * its count is increased by aCount.
         *
         * @param aNotificationId identifier for this notification
         * @param aCount addition count
         */
        void ShowSoftNotificationL();

        /**
         * Cancels custom soft notification.
         *
         * @param aNotificationId identifier for this notification
         */
        void RemoveSoftNotificationL( TInt aNotifierID );

        /**
         * Set a text for a soft notification.
         *
         * @param aSingularText singular text for soft notification
         * @param aPluralText plural text for soft notification
         */
        void SetTextL( const TDesC& aText, const TDesC& aGroupText );

        /**
         * Set labels for soft notification's softkeys.
         *
         * @param aSoftkey1Label Label for the softkey 1
         * @param aSoftkey2Label Label for the softkey 2
         */
        void SetSoftkeyLabelsL( const TDesC& aSoftkey1Label,
            const TDesC& aSoftkey2Label );

        /**
         * Set an image for a soft notification.
         *
         * @param aImage image for soft notification
         */
        void SetImageL( const TDesC8& aImage );

        /**
         * Returns the notification id.
         *
         * @return notification id
         */
        TInt Id();

    private:  // Constructors
        /**
         * C++ constructor.
         *
         * @param aAppId View activation application id.
         * @param aNotificationId Notification id.
         * @param aEventSource Event source used for posting events from
         *        native to Java side.
         */
        CIAUpdateBGSoftNotification( MIAUpdateBGSoftNotificationCallBack* aCallback, CIAUpdateBGInternalFileHandler* aInternalFile );

        /**
         * 2nd phase constructor.
         */
        void ConstructL();

    private:  // New methods
   
        /**
         * Set assigned member data to custom notification parameters
         *
         * @param aParam custom soft notification params to fill
         */
        void FillNotificationParams( TAknDynamicSoftNotificationParams& aParam );

    private:  // Data
        /// Own. AvKon custom soft notifier
        CAknDynamicSoftNotifier* iNotifier;
        /// View activation application id
        TUid iAppId;
        /// Notification Id
        TInt iNotificationId;
        /// Own. Softkey 1 label
        HBufC* iSoftkey1;
        /// Own. Softkey 2 label
        HBufC* iSoftkey2;
        /// Own. Note label when single dialog is shown
        HBufC* iLabel;
        /// Own. Note label when notifications are groupped
        HBufC* iGroupLabel;
        /// Own. Image data byte array
        HBufC8* iImageData;
        
        MIAUpdateBGSoftNotificationCallBack* iCallback;
        
        CIAUpdateBGInternalFileHandler* iInternalFile; //not owned

    };

#endif // IAUPDATEBGSOFTNOTIFICATION_H


