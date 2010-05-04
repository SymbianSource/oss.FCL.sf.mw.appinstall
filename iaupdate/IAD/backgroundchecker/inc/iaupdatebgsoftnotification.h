/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hbdevicenotificationdialogsymbian.h>
#include "iaupdatebgcheckermode.h"


// FORWARD DECLARATION
class MIAUpdateBGSoftNotificationCallBack
    {
    public:
        virtual void SoftNotificationCallBack( TBool aIsAccepted ) = 0;
        virtual TIAUpdateBGMode CurrentMode() = 0;
    };


// CLASS DECLARATION
/**
 *  Controls the background indicator notifications. 
 *
 */
class CIAUpdateBGSoftNotification : public CBase, 
                                    public MHbDeviceNotificationDialogObserver
    {
    public:  // Constructors and destructor

        /**
         * Static constructor
         */
        static CIAUpdateBGSoftNotification* NewL( 
                MIAUpdateBGSoftNotificationCallBack* aCallback ); 

        /**
         * Destructor.
         */
        virtual ~CIAUpdateBGSoftNotification();
        
    public: // New functions
        
         /**
          * Shows notification and  indicator. 
          *
          * @param aNrOfUpdates number of updates
          */
         void ShowNotificationL();
         
         /**
          * Shows indicator. 
          *
          * @param aMode indicator mode (EFirstTimeMode / ENormalMode)
          */
         void ShowIndicatorL();
         
         /**
          * Removes indicator. 
          *
          * @param aNrOfUpdates number of updates
          */
         void RemoveIndicatorL();

        /**
         * Set a text for a soft notification.
         *
         * @param aTitle title for soft notification
         * @param aText  text for soft notification
         */
        void SetTextL( const TDesC& aTitle, const TDesC& aText ); 

        /**
        * Set an image path for a soft notification.
        *
        * @param aImage image path for soft notification
        */
        void SetImagePathL( const TDesC& aImage );
        
        /**
        * Set number of updates a soft notification.
        *
        * @param aNrOfUpdates 0 - first time, >< 0 normal case
        */
        void SetNrOfUpdates( const TInt& aNrOfUpdates);


    private:  // Constructors
        /**
         * C++ constructor.
         *
         * @param aCallback notification callback
         */
        CIAUpdateBGSoftNotification( 
                MIAUpdateBGSoftNotificationCallBack* aCallback); 

        /**
         * 2nd phase constructor.
         */
        void ConstructL();

    private:  // New methods
   
        /**
         * Set assigned member data to custom notification parameters
         *
         */
        void FillNotificationParams();
        
        /**
         * Notification dialog activationobserver
         *
         * @param aDialog notification dialog
         */
        virtual void NotificationDialogActivated(
                const CHbDeviceNotificationDialogSymbian* aDialog );
        /**
         * Notification dialog activationobserver
         *
         * @param aDialog notification dialog
         * @param aCompletionCode completion code
         */
        virtual void NotificationDialogClosed( 
                const CHbDeviceNotificationDialogSymbian* aDialog, 
                TInt aCompletionCode );
        
        /**
         * Enable/disable indicator
         *
         * @param aEnabled ETrue-enabled, EFalse-disabled
         */        
        void SetIndicatorEnabled(TBool aEnabled);
        
        /**
         * Get indicator enablation state
         *
         * @return aEnabled ETrue-enabled, EFalse-disabled
         */ 
        TBool IsIndicatorEnabled();
        
        /**
         * Set number of updates
         *
         * @param aNrOfUpdates number of updates 
         */ 
        // void SetNrOfUpdates( const int aNrOfUpdates );
        
        /**
         * Get number of updates
         *
         * @return  number of updates 
         */ 
        int GetNrOfUpdates();
        
    private:  // Data
        // Note title
        HBufC* iTitle; 
        // Note text
        HBufC* iText; 
        /// Image data byte array
        HBufC8* iImageData;
        // Notification callback
        MIAUpdateBGSoftNotificationCallBack* iCallback;
        // Image path
        HBufC* iImagePath;
        // Number of updates
        int iNrOfUpdates;
        // Indictor activation state
        TBool iActivateIndicator;
        //Notification dialog
        CHbDeviceNotificationDialogSymbian* iNotificationDialog;

    };

#endif // IAUPDATEBGSOFTNOTIFICATION_H


