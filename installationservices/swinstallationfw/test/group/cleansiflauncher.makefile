# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
# cleansiflauncher.makefile cleans the production version of the SIF Launcher.
#

do_nothing:
	rem do nothing

#
# The targets invoked by abld...
#

MAKMAKE : do_nothing
FREEZE : do_nothing
LIB : do_nothing
RESOURCE : do_nothing
CLEANLIB : do_nothing
FINAL : do_nothing
SAVESPACE : do_nothing
CLEAN : do_nothing
RELEASABLES : do_nothing

BLD : 
	echo **************************************************
	echo Removing SIF Launcher files after production build
	echo **************************************************
	call cleansiflauncher.bat $(PLATFORM) $(CFG)