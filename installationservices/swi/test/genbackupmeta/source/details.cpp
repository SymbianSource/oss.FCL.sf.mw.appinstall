/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* For containing resource strings.
* @internalComponent
*
*/


#include "details.h"

std::string gCurrentVersion = "GenBackupMeta Version 1, 0, 0, 0";

std::string gDescription =	"A utility tool for generating meta-data for a given set of "
							"Software Installation (SIS) files.";

std::string gCopyright = "Copyright (c) 2006 Symbian Software Ltd. All rights reserved.";

std::string gUsage	=	"Usage : GenBackupMeta [-h|-?] [-v] -u packageuid [-d] drivename -s sisfile store "
						"[-f] file store [-o] outputfilename \n"
						"Options: -h|-? Show help usage. \n"
						"Options: -v Show tool version. \n";
	

std::string gHelp =  "[-u]: Mandatory: Package UID based on which valid input files would be retrieved.\n"
					 "[-d]: Optional: Used for specifying the drive name.\n"
					 "[-s]: Mandatory: File name where the sis files are stored.\n"
					 "[-f]: Optional: File name where files are stored.\n"
					 "[-o]: Optional: Output filename in which meta-data is to be generated.";


std::string gMetaSuccess	= "Meta-Data successfully generated with the following specification:";
std::string gSISName		= "SIS File Names:";
std::string gMetaName		= "Metadata Filename: ";
std::string gMetaLocation	= "Metadata Location: ";