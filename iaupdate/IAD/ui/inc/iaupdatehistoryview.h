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
* Description:   This file contains the header file of the CIAUpdateHistoryView
*                class 
*
*/



#ifndef IA_UPDATE_HISTORY_VIEW_H
#define IA_UPDATE_HISTORY_VIEW_H

#include <aknview.h> 

class CIAUpdateHistoryContainer;
class MIAUpdateHistory;


/*! 
  @class CIAUpdateHistoryView
  
  @discussion An instance of the Application View object for the IAUpdate 
  example application
  */
class CIAUpdateHistoryView : public CAknView
    {

public:
    
    /*!
      @function NewL
  
      @discussion Create a CIAUpdateHistoryView object, which will draw itself to aRect
      @result a pointer to the created instance of CIAUpdateHistoryView
    */
    static CIAUpdateHistoryView* NewL( MIAUpdateHistory& aHistory );

    /*!
      @function NewLC
   
      @discussion Create a CIAUpdateHistoryView object, which will draw itself to aRect
      @result a pointer to the created instance of CIAUpdateHistoryView
    */
    static CIAUpdateHistoryView* NewLC( MIAUpdateHistory& aHistory );


    /*!
      @function ~CIAUpdateHistoryView
      
      @discussion Destroy the object and release all memory objects
    */
    virtual ~CIAUpdateHistoryView();


    void RefreshL();       


private:  // from CAknView

    /*!
      @see CAknView::Id
    */
    virtual TUid Id() const;


    /*!
      @see CAknView::HandleCommandL
    */
    virtual void HandleCommandL( TInt aCommand );
    
    
    /*!
      @see CAknView::DoActivateL
    */
    virtual void DoActivateL( const TVwsViewId &aPrevViewId, 
                              TUid aCustomMessageId, 
                              const TDesC8 &aCustomMessage);
    
    /*!
      @see CAknView::DoDeactivate
    */
    virtual void DoDeactivate();

                        
private: // Methods

    /*!
      @function CIAUpdateHistoryView
      
      @discussion Perform the first phase of two phase construction 
    */
    CIAUpdateHistoryView( MIAUpdateHistory& aHistory );

    
    /*!
      @function ConstructL
      
      @discussion  Perform the second phase construction of a CIAUpdateHistoryView object
    */
    virtual void ConstructL();
    
    // Gives the history object that is used to show the history in the view.
    MIAUpdateHistory& History() const; 

    // Updates the status pane texts.
    void UpdateStatusPaneL();

    
private: // Data

    // Contains the item history info
    MIAUpdateHistory& iHistory;      
    
    // Handles the content of the view.
    CIAUpdateHistoryContainer* iContainer;

    };

#endif // IA_UPDATE_HISTORY_VIEW_H
