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



#ifndef IAUPDATE_RESULTS_INFO_H
#define IAUPDATE_RESULTS_INFO_H

/**
 * TIAUpdateResultsInfo
 *
 * This class is used as a bundle for different counters
 * that describe a success of an operation.
 */
class TIAUpdateResultsInfo
	{

public:
	TInt  iCountSuccessfull;
	TInt  iCountCancelled;
	TInt  iCountFailed;
	TBool iFileInUseError;
	TBool iRebootAfterInstall;
	
public:
	TIAUpdateResultsInfo( TInt aCountSuccessfull,
	                      TInt aCountCancelled,
	                      TInt aCountFailed, 
	                      TBool aFileInUseError,
	                      TBool aRebootAfterInstall )
	                      : iCountSuccessfull( aCountSuccessfull ),
	                        iCountCancelled( aCountCancelled ),
	                        iCountFailed( aCountFailed ), 
	                        iFileInUseError( aFileInUseError ),
	                        iRebootAfterInstall( aRebootAfterInstall )
	    {
	    }
		
	};

#endif // IAUPDATE_RESULTS_INFO_H