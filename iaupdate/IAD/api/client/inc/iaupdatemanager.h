/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateManager class 
*
*/

#ifndef IA_UPDATE_MANAGER_H
#define IA_UPDATE_MANAGER_H


//INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <coemain.h>

//FORWARD DECLARATIONS
class IAUpdateClient;
class CIAUpdateParameters;
class MIAUpdateObserver;
class CEikonEnv;




/**
 * CIAUpdateManager object provides methods for update actions.
 * Updating will be targeted to the update items that qualify the requirements
 * of CIAUpdateParameters objects. In asynchronous actions, the callback 
 * functions of MIAUpdateObserver objects will be informed about the progress 
 * of update actions.
 *
 * @see MIAUpdateObserver
 * @see CIAUpdateParameters
 * @see CIAUpdate
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CIAUpdateManager ) : public CBase, public MCoeForegroundObserver
    {

public:

    static CIAUpdateManager* NewL( MIAUpdateObserver& aObserver );
    
    static CIAUpdateManager* NewLC( MIAUpdateObserver& aObserver );
    


    ~CIAUpdateManager();
  
    void CheckUpdates( const CIAUpdateParameters& aUpdateParameters );
  
    void ShowUpdates( const CIAUpdateParameters& aUpdateParameters );
 
    void UpdateQuery();
    
    void Update( const CIAUpdateParameters& aUpdateParameters );


private:

    CIAUpdateManager();
    
    void ConstructL( MIAUpdateObserver& aObserver );

    // Prevent these if not implemented
    CIAUpdateManager( const CIAUpdateManager& aObject );
    CIAUpdateManager& operator =( const CIAUpdateManager& aObject );
    
private: // Functions from MCoeForegroundObserver

	/**
	* Handles the application coming to the foreground.
	*/
	void HandleGainingForeground();

	/**
	* Handles the application going into the background.
	*/
	void HandleLosingForeground();


private: // data
   
    // Handles the client server communication.
    IAUpdateClient* iUpdateClient;

    CEikonEnv* iEikEnv; //not owned
    };

#endif // IA_UPDATE_IMPL_H
