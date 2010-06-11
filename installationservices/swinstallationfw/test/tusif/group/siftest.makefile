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
# siftest.amkefile is an extended test makefile which generates
# self-signed SIS files for the USIF integration tests


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
BLD : do_nothing
SAVESPACE : do_nothing
CLEAN : do_nothing
RELEASABLES : do_nothing

FINAL : 
	echo ***************************************************************
	echo maketestsis - Running createsisfiles.bat for $(PLATFORM) $(CFG)
	echo ***************************************************************
	createsisfiles.bat $(PLATFORM) $(CFG)
	perl createleesisfiles.pl $(PLATFORM) $(CFG)
	perl createrefbinfiles.pl $(PLATFORM) $(CFG)
