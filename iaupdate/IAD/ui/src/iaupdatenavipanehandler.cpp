/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES

#include "iaupdatenavipanehandler.h"
#include "iaupdatedebug.h"
#include <aknnavilabel.h>
#include <eikspane.h>
#include <avkon.hrh>

/*******************************************************************************
 * class CIAUpdateNaviPaneHandler
 *******************************************************************************/


// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CIAUpdateNaviPaneHandler::~CIAUpdateNaviPaneHandler()
    {
   	if (iNaviDecorator)
		{
		if (iNaviPane && iNavidecoratorPushed)
			{
			iNaviPane->Pop(iNaviDecorator);
			}
	
		delete iNaviDecorator;
		}
		
	if (iNaviPane && iNavipanePushed)
		{
		iNaviPane->Pop(); // restore previous navi pane
		}
    }


// -----------------------------------------------------------------------------
// CIAUpdateNaviPaneHandler::CIAUpdateNaviPaneHandler
//
// -----------------------------------------------------------------------------
//
CIAUpdateNaviPaneHandler::CIAUpdateNaviPaneHandler(CEikStatusPane* aStatusPane) 
    : iStatusPane(aStatusPane)
	{
	iNavidecoratorPushed = EFalse;
	iNavipanePushed = EFalse;
	
	if (iStatusPane)
		{
		TRAP_IGNORE( 
		 iNaviPane = (CAknNavigationControlContainer*)iStatusPane->ControlL( 
		                                 TUid::Uid(EEikStatusPaneUidNavi) ) );
		}
	}


// ----------------------------------------------------------------------------
// CIAUpdateNaviPaneHandler::SetNaviPaneTitleL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateNaviPaneHandler::SetNaviPaneTitleL(const TDesC& aTitle)
	{
	if (!iStatusPane || !iNaviPane)
		{
		return;
		}

    if (iNaviDecorator)
        {
        CAknNaviLabel* naviLabel = static_cast<CAknNaviLabel*>(iNaviDecorator->DecoratedControl());
        if (naviLabel)
        	{
        	naviLabel->SetTextL(aTitle);
        	iNaviDecorator->DrawDeferred();
        	}
        }
     else
    	{
    	iNaviDecorator = iNaviPane->CreateNavigationLabelL(aTitle);
    	iNaviPane->PushL(*iNaviDecorator); // activate navi label in navi pane
    	iNavidecoratorPushed = ETrue;
    	}
	}


    
//  End of File  
