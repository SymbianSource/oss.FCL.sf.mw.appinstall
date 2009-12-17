/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CSWInstUninstallRequest class 
*
*                This class implements functionality for install request.
*
*/


#ifndef SWINSTUNINSTALLREQUEST_H
#define SWINSTUNINSTALLREQUEST_H

//  INCLUDES
#include <e32base.h>
#include <SWInstDefs.h>
#include "SWInstRequestObject.h"

namespace SwiUI
{

// FORWARD DECLARATIONS

class CSWInstUIPluginAPI;

// CLASS DECLARATION

/**
* This class implements functionality for install request.
*
* @since 3.0
*/
class CSWInstUninstallRequest : public CSWInstRequestObject
    {
    public:  // Constructors and destructor
     
        /**
        * Two-phased constructor.
        * @param aMessage - Message object having this request.
        */
        static CSWInstUninstallRequest* NewL( const RMessage2& aMessage );

        /**
        * Two-phased constructor.
        * @param aMessage - Message object having this request.
        */
        static CSWInstUninstallRequest* NewL( TInt aObjectId );
    
        /**
        * Destructor.
        */
        virtual ~CSWInstUninstallRequest();

    public: // New functions

        /**
        * Perform uninstallation
        * @since 3.0
        * @param aUid - Uid of the package to uninstall.
        * @patam aMIME - Mime of the package to uninstall.
        */
        void Uninstall( const TUid& aUid, const TDesC8& aMIME );

        /**
        * Perform silent uninstallation
        * @since 3.0
        * @param aUid - Uid of the package to uninstall.
        * @patam aMIME - Mime of the package to uninstall.
        * @param aOptions - Options for uninstallation
        */
        void SilentUninstall( const TUid& aUid, 
                              const TDesC8& aMIME, 
                              TUninstallOptions& aOptions );        

        /**
        * Perform a custom uninstallation.
        * @since 3.0
        * @param aOperation - Operation to be performed.
        * @param aParams - Parameters of the operation.
        * @param aMIME - Mime of the package to uninstall.
        */
        void CustomUninstall( TOperation aOperation, 
                              const TDesC8& aParams, 
                              const TDesC8& aMIME );   

        /**
        * Perform a silent custom uninstallation.
        * @since 3.0
        * @param aOperation - Operation to be performed.
        * @param aOptions - Options for the silent operation.
        * @param aParams - Parameters of the operation.
        * @param aMIME - Mime of the package to uninstall.
        */
        void SilentCustomUninstall( TOperation aOperation, 
                                    TUninstallOptions& aOptions,
                                    const TDesC8& aParams, 
                                    const TDesC8& aMIME );            

    protected: // Functions from base classes

        /**
        * From CSWInstRequestObject, Cancels the current request.
        * @since 3.0
        */
        void CancelRequest();
        
        /**
        * From CSWInstRequestObject, Gets called when the request is completed.
        * @since 3.0
        */
        void RequestCompleteL( TInt aResult );       

    private:
        
        /**
        * C++ default constructor.
        * @param aMessage - Message object having this request.
        */
        CSWInstUninstallRequest( const RMessage2& aMessage );

        /**
        * C++ default constructor.
        * @param aMessage - Message object having this request.
        */
        CSWInstUninstallRequest( TInt aObjectId );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();   

        /**
        * Performs the actual uninstallation.
        * @since 3.0
        */
        void DoUninstallL( const TDesC8& aMIME );  

        /**
        * Perform a custom uninstallation.
        * @since 3.0
        * @param aOperation - Operation to be performed.
        * @param aParams - Parameters of the operation.
        * @param aMIME - Mime of the package to uninstall.
        */
        void DoCustomUninstallL( TOperation aOperation, 
                                const TDesC8& aParams, 
                                const TDesC8& aMIME );   

        /**
        * Perform a silent custom uninstallation.
        * @since 3.0
        * @param aOperation - Operation to be performed.
        * @param aOptions - Options for the silent operation.
        * @param aParams - Parameters of the operation.
        * @param aMIME - Mime of the package to uninstall.
        */
        void DoSilentCustomUninstallL( TOperation aOperation, 
                                      TUninstallOptions& aOptions,
                                      const TDesC8& aParams, 
                                      const TDesC8& aMIME );           

    private: // Data
        
        TUid iUid;        
        TUninstallOptions iOptions;
    };
}

#endif      // SWINSTUNINSTALLREQUEST_H   
            
// End of File
