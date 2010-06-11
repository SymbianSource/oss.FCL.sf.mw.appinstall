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
* Description:   This file contains the header file of the CSWDocument class 
*
*                This class is a part of the standard application framework.
*                Declares the document class for this application.
*
*/


#ifndef SWDOCUMENT_H
#define SWDOCUMENT_H

//  INCLUDES
#include <AknDoc.h>

namespace SwiUI
{

// CLASS DECLARATION

/**
* This class is a part of the standard application framework.
* Declares the document class for this application.
*/
class CSWInstDocument : public CAknDocument
    {
    public:  // Constructors and destructor
        
        /**
	* Two-phased constructor.
	*/
	static CSWInstDocument* NewL( CEikApplication& aApp );
	
	/**
        * Destructor.
        */
	virtual ~CSWInstDocument();

    public: // Functions from base classes

        /**
        * From CEikDocument, Called by the framework to open a file.
        * @since 3.0
        * @param aDoOpen - Open an existing file if ETrue, otherwise create a 
        * new file.
        * @param aFilename - The file to open or create.  
        * @param aFs - The file server session to use.
        * @return File store object referring to the newly-opened file.
        */
        CFileStore* OpenFileL( TBool aDoOpen, const TDesC& aFilename, RFs& aFs );

        /**
        * From CEikDocument, Called by the framework to open a file.
        * @since 3.0
        */
        void OpenFileL(CFileStore*& aFileStore, RFile& aFile);

        /**
        * From CEikDocument, Creates CSWAppUi object.
        * @since 3.0
        * @return CSWAppUi object
        */
        CEikAppUi* CreateAppUiL();

    private:
        
        /**
        * Symbian OS default constructor.
        */
        CSWInstDocument( CEikApplication& aApp );

    };
}

#endif      // SWINSTDOCUMENT_H   
            
// End of File
