/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "dumpchainvaliditytool.h"
#include "options.h"
#include "swicertstore.h"
#include "siscertificatechain.h"
#ifdef _MSC_VER
#include <new>
#include <new.h>
#endif /* _MSC_VER */
using namespace std;

#ifdef _MSC_VER
// House-keeping to allow ::new to throw rather than return NULL in MSVC++
int throwingHandler(size_t)
{
	throw std::bad_alloc();
	return 0;
}
#endif /* _MSC_VER */

//entry point
int main(int argc,char *argv[])
	{
#ifdef _MSC_VER
	_set_new_handler (throwingHandler);	// Make ::new throw rather than return NULL
#endif /* _MSC_VER */

	if(argc < 2)
		{
		Options::DisplayUsage();
		return 0;
		}

	OpenSSL_add_all_algorithms ();
	OpenSSL_add_all_ciphers ();
	OpenSSL_add_all_digests ();

	DumpChainValidityTool* Tool=0;
	Options* options=0;

	try
		{
		Tool = new  DumpChainValidityTool();
		options = new Options (argc,argv);
		Tool->Run(*options);
		}

	catch (Exceptions& aErr)
		{
		Options::DisplayError (aErr);
		delete options;
		delete Tool;
		return aErr;
		}
	
	catch (bad_alloc&)
		{
		cout << "Error Allocating Memory " << endl ;
		}

	delete options;
	delete Tool;
	return 0;
	}
