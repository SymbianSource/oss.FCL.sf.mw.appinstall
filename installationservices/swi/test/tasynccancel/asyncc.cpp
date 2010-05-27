/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "asyncc.h"
#include "asynccancelstep.h"



// REComSession::FinalClose() needs to be run in order for a memory leak 
// not to occur if using logo in the sis file to be installed. 
// This shouldn't be the case since the test is never using ECom 
// explicitly, meaning that it should be dealt with within the 
// installation source itself. The behaviour isn't related to cancellation 
// since it occurs when installation isn't cancelled at all
#include <ecom/ecom.h>


extern CConsoleBase* gConsole; // write all your messages to this


// Try uninstalling a program with the given UID
// If the program is detected to be installed than cancellation failed => return error
TInt TryUninstall(TUid aUid)
	{
	CUIScriptAdaptor* ui = CUIScriptAdaptor::NewLC();
	CInstallPrefs* prefs = CInstallPrefs::NewLC();

	TInt err = Swi::Launcher::Uninstall(*ui, aUid);
	
	CleanupStack::PopAndDestroy(prefs);
	CleanupStack::PopAndDestroy(ui);
	
	// program not installed => cancelled correctly
	if (err==KErrNotFound)
		{
		err=0;
		}
	else
		{
		err=1;
		}
	
	return err;
	}




TInt CAsyncCancelStep::DoTestL(TInt aWhenToCancelLow, TInt aWhenToCancelHigh, TInt aHowManyCancelations, TInt aMinWhenToCancelInc,
			 const TPtrC& aSisToInstall, TUid aUid,
			 CInstallPrefs* aPrefs)
    {
    gConsole=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(gConsole);



	MY_INFO_PRINTF1( _L("Installing sis file: ") );
	MY_INFO_PRINTF1( aSisToInstall );
	MY_INFO_PRINTF1( _L("\n") );
	

	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(scheduler);
    

    CCancelTimer* CancelTimer = CCancelTimer::NewL(aPrefs);

	TInt err=0;
	
	// Run the installation for the first time (without cancelling)
	// to see how long it takes
	TInt firstRun=1;
	TInt whenToCancel=100000000;
	
	// This initialisation doesn't make any difference since the variable
	// is initialised later. But it removes compiler warnings
	TInt whenToCancelInc=1;

	
	
	if (aWhenToCancelLow==0) 
		{
		aWhenToCancelLow=1; // impossible to cancel at 0
		}
	
	
	// Uninstall if already installed
	err=TryUninstall( aUid );
	err=0; // ignore the error


	do 
		{
		CleanupStack::PushL(CancelTimer);
		// Set up installation
		CancelTimer->StartL(aSisToInstall,whenToCancel,firstRun);
		CleanupStack::Pop(CancelTimer);
		
		// it is useful to run the complete installation twice
		// since when OCSP checking is enabled the first installation
		// takes a lot more time to execute than the later ones
		if (firstRun==1) 
			{
			MY_INFO_PRINTF1(_L("Running a complete installation (first time) : "));
			}
		else if (firstRun==2) 
			{
			MY_INFO_PRINTF1(_L("Running a complete installation (second time): "));
			}
		else
			{
			MY_INFO_PRINTF2(_L("Running installation, cancelling at %d microseconds: "),whenToCancel);
			}
	
		// Start installation process	
		scheduler->Start();
		// Test ended => Deque the cancel timer
		CancelTimer->Deque();
		
		
		MY_INFO_PRINTF2(_L("\tiStatus: %d\t"),CancelTimer->InstallerStatus());
		

		if (firstRun>0)
			{
			MY_INFO_PRINTF2(_L("%d microseconds\n"),CancelTimer->Time());
			
			if (CancelTimer->CancelationSuccess()!=1)
				{
				MY_ERR_PRINTF2(_L("<<< FAILED >>> with %d error code\n"),CancelTimer->CancelationSuccess());
				break;
				}
			
			// Determine at what intervals the cancellation should be performed
			// in order to achieve the wanted number of cancellations
			if (aWhenToCancelHigh>CancelTimer->Time())
				{
				aWhenToCancelHigh=CancelTimer->Time();
				}				
			whenToCancelInc= (aWhenToCancelHigh-aWhenToCancelLow)/aHowManyCancelations;
			if (whenToCancelInc<aMinWhenToCancelInc) 
				{
				whenToCancelInc=aMinWhenToCancelInc;
				}
			whenToCancel=aWhenToCancelLow;
			
			// Uninstall
			err=TryUninstall( aUid  );
			err=0;
			}
		else
			{
			MY_INFO_PRINTF1(_L("Completed: "));
			whenToCancel+= whenToCancelInc;
			
			if (CancelTimer->FinishedInstallation())
				{
				err=TryUninstall( aUid );
				}				
		
			if (CancelTimer->CancelationSuccess())
				{
				MY_INFO_PRINTF1(_L("OK\n"));
				}				
			else 
				{
				MY_ERR_PRINTF1(_L("\t<<< FAILED >>>\n"));
				}
			
			if (
				CancelTimer->CancelationSuccess()!=1 ||
			   	CancelTimer->FinishedInstallation() ||
			   	whenToCancel>aWhenToCancelHigh
			   )
				{
				break;
				}

			}
	

		if (firstRun==1)
			{
			firstRun=2;
			}
		else 
			{
			firstRun=0;
			}

		} while (1);

	
		
	err=0;
	if (CancelTimer->CancelationSuccess()!=1) 
		{
		err=1;
		}
	else
		{
		err=0;
		}
	
	


	// if a delay is needed, edit this code
	{
	CDelayTimer* DelayTimer=new(ELeave) CDelayTimer; 
	DelayTimer->ConstructL();
	CActiveScheduler::Add(DelayTimer);
	DelayTimer->After(TTimeIntervalMicroSeconds32(1));
	CActiveScheduler::Start();
	delete DelayTimer;
	}

	
	
	// clean-up
	delete scheduler;
	delete CancelTimer;
	
	if (err)
		{
		MY_ERR_PRINTF1(_L("\n\n\tAt least one test <<< FAILED >>>\n"));
		}		
	else 
		{
		MY_INFO_PRINTF1(_L("\n\n\tAll tests PASSED\n"));
		}
	
	MY_INFO_PRINTF1(_L("\tCancelTest compleated execution\n"))	;
#ifdef WAITFORINPUTATEND	
	MY_INFO_PRINTF1(KTxtPressAnyKey);
	gConsole->Getch(); // get and ignore character
#endif
	CleanupStack::PopAndDestroy(gConsole); // close gConsole


	// for explanation see the top of this file, at #include <ecom/ecom.h>
	REComSession::FinalClose();
	//

	return err;
    }


// End of file
