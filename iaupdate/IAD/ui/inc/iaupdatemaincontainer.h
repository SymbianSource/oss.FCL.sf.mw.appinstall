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
* Description:   This file contains the header file of the CIAUpdateMainContainer class 
*
*/



#ifndef IAUPDATEMAINCONTAINER_H
#define IAUPDATEMAINCONTAINER_H

//INCLUDES
#include <coecntrl.h> // CCoeControl

// FORWARD DECLARATIONS
class CAknDoubleGraphicStyleListBox;
class MIAUpdateContainerObserver;

class MIAUpdateNode;
class MIAUpdateAnyNode;

// CLASS DECLARATION
/**
* This class represents the container of main view of IAUpdate
*
*/
class CIAUpdateMainContainer :  
    public CCoeControl,    
    public MEikListBoxObserver
    {
public:
     
    /**
    * Symbian two phased constructor.
    * @param aRect the rectangle this view will be drawn to
    * @return Instance of the created server object.
    */
    static CIAUpdateMainContainer* NewL( const TRect& aRect, 
                                         MIAUpdateContainerObserver& aObserver );
    
    /**
    * Symbian two phased constructor. Created object is pushed to clenaup stack
    * @param aRect the rectangle this view will be drawn to
    * @return Instance of the created server object.
    */
    static CIAUpdateMainContainer* NewLC( const TRect& aRect, 
                                          MIAUpdateContainerObserver& aObserver );
    /**
    * Destructor
    */
    ~CIAUpdateMainContainer();
    
    /**
    * Handles Mark/Unmark
    * @param aCommand Mark or unmark commannd
    */
    void HandleMarkCommandL( TInt aCommand );
    
    /**
    * Resfresh the list
    * @param aNodes List of nodes
    * @param aError Error code
    */
    void RefreshL( const RPointerArray<MIAUpdateAnyNode>& aNodes,
                   TInt aError );       
    
    /**
    * Returns node that's highlighted in a list
    * @param aNodes List of nodes
    * @return Current (highlighted) node
    */    
    MIAUpdateAnyNode* GetCurrentNode( const RPointerArray<MIAUpdateAnyNode>& aNodes );
    
    /**
    * Set selected items to list  
    * @param aIndices List of indices
    */    
    void SetSelectedIndicesL( const RArray<TInt>& aIndices );
    
    
    /**
     * Clean all the selection
     */ 
    void CleanAllSelection();

    /**
    * Set selected items to list  
    * @param aListBox   List box
    * @param aEventType Event type
    */  
    void HandleListBoxEventL(
                  CEikListBox*  aListBox,
                  TListBoxEvent aEventType );
    
private:  // from CCoeControl

    /**
    * Handle key events by forwarding them to the list box.
    * @param aKeyEvent The key event.
    * @param aType The event type.
    * @result Indicates whether or not the key event was consumed by this control. 
    */
   TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
    * Called when size is changed by framework.
    * Set current size to listbox
    */
    void SizeChanged();

    /**
    * Count of components.
    * @return Count of child components
    */
    TInt CountComponentControls() const;

    /**
    * Component control
    * @param aIndex  Index numer of child component.
    * @return Pointer to a component
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;
      
    /**
    * Get the help context for this control.
    *
    * @param aContext Help context
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
        
    /**
    * Handles a resource relative event
    *
    * @param aType Event type
    */
    void HandleResourceChange( TInt aType );
    
    
    
private:
    
    /** 
    * Constuctor 
    * @param aRect Rectangular this view will be drawn to
    */
    void ConstructL (const TRect& aRect );
    
    /** 
    * C++ constuctor 
    * @param aObserver Observer of the container
    */
    CIAUpdateMainContainer( MIAUpdateContainerObserver& aObserver );
            
    /** 
    * Update Middle soft key label text (Mark/Unmark) 
    */
    void UpdateMSKTextL();
    
    void CreateIconL( CFbsBitmap*& aBitmap,
                      CFbsBitmap*& aMask,
                      const TDesC& aFileName,
                      TInt aBitmapId,
                      TInt aMaskId );
        

private: // Data

    MIAUpdateContainerObserver& iObserver;
    CDesCArray* iItemTextArray;
    CAknDoubleGraphicStyleListBox* iListBox; 
    };

#endif // IAUPDATEMAINCONTAINER_H

// End of File
