/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the 
*                IAUpdateDialogObserver class.
*
*/



#ifndef IAUPDATEDIALOGOBSERVER_H
#define IAUPDATEDIALOGOBSERVER_H

//  INCLUDES

class HbAction;



class IAUpdateDialogObserver
{
    public:
   
    
    virtual void dialogFinished(HbAction *action) = 0;   
};


#endif // IAUPDATEDIALOGOBSERVER_H
            
// End of File
