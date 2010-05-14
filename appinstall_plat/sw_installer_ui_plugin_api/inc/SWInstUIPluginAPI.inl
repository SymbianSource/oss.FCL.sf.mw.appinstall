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
* Description:   This module contains the implementation of 
*                CSWInstUIPluginAPI class member functions.
*
*/


using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstUIPluginAPI::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
inline SwiUI::CSWInstUIPluginAPI* SwiUI::CSWInstUIPluginAPI::NewL( const TDesC8& aMIME )
    {	   
    TEComResolverParams params;
    params.SetDataType( aMIME );

    TAny* ptr = REComSession::CreateImplementationL( KCSWInstUIPluginInterfaceUid,
                                                     _FOFF( CSWInstUIPluginAPI,
                                                            iDestructKey ),params );
     
    return reinterpret_cast<CSWInstUIPluginAPI*> ( ptr );
    }
    
// Destructor
inline SwiUI::CSWInstUIPluginAPI::~CSWInstUIPluginAPI()
    {
    REComSession::DestroyedImplementation( iDestructKey );    
    }

//  End of File  
