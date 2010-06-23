/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PROGRAMSTATUS_H
#define PROGRAMSTATUS_H

#include <e32base.h>

namespace Swi
{  
// Program main state
enum TProgramState
     {           
     EStateIdle,            
     EStateInstalling,
     EStateUninstalling
     };        
/**
 *  CProgramStatus class handles program main status. 
 *
 *  @since S60 3.2
 */
class CProgramStatus : public CBase
    {
    public:

        /**
        * Symbian construction
        */
        static CProgramStatus* NewL( TInt aProcessStatus );
        
        /**
         * Symbian construction
         */
         static CProgramStatus* NewLC( TInt aProcessStatus );        
        
        /**
        * C++ destructor
        */
        ~CProgramStatus();
                
        /**
        * This function sets main status.
        * 
        * @since 3.2  
        * @param aProcessStatus - program status
        */
        void SetProgramStatus( TInt aProcessStatus );
        
        /**
         * This function returns main status.
         * 
         * @since 3.2  
         * @return TInt Program status
         */
        TInt GetProgramStatus();
        
        /**
         * This function sets main status to idle.
         * 
         * @since 3.2  
         */
        void SetProgramStatusToIdle();        

        /**
          * This function defines general PS key for dialogs.
          * If PS key is already set function does nothing.
          * 
          * @since 3.2  
          */
        void DefinePSKeyForUI();
               
    private:
           
        /**
        * 2nd phase constructor
        */
        void ConstructL( TInt aProcessStatus );
                                                            
    private:
        
        // Defines program main status.
        TInt iGeneralProcessStatus;   
        // Current PS key for UI dialog control.
        TInt iPSKeyValue;
    };      

} //namespace Swi

#endif /*PROGRAMSTATUS_*/

//EOF

