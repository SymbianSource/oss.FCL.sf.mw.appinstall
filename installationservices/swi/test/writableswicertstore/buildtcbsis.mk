#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

MAKEDATA :
!IF "$(PLATFORM)" == "GCCXML" || "$(PLATFORM)" == "TOOLS"
	cd
	echo ----------------
	echo  Do nothing ...
	echo ----------------
!ELSE
	cd
	echo ---------------------------------------
	echo Building SwiCertstore test sis files...
	echo ---------------------------------------

	perl preparesis.pl $(PLATFORM) $(CFG) C
!ENDIF

DO_NOTHING:
	rem do nothing

#
# The targets invoked by abld...
#

MAKMAKE : DO_NOTHING
FREEZE : DO_NOTHING
LIB : DO_NOTHING
RESOURCE : DO_NOTHING
CLEANLIB : DO_NOTHING
FINAL : MAKEDATA
RELEASABLES : DO_NOTHING

SAVESPACE : BLD

BLD : DO_NOTHING

CLEAN :
	-cd data
	-del *.sis
	-cd ..
	-rmdir /s /q build-$(PLATFORM)-$(CFG)
