#
# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#!perl -w
use strict;
use File::Copy;

# Construct appropriate directories

mkdir("/epoc32/drive_d", 0777)			  if ( ! -d "/epoc32/drive_d" );
mkdir("/epoc32/drive_d/system", 0777)		  if ( ! -d "/epoc32/drive_d/system" );
mkdir("/epoc32/drive_d/system/install", 0777)	  if ( ! -d "/epoc32/drive_d/system/install" );
mkdir("/epoc32/winscw/c/system", 0777)		  if ( ! -d "/epoc32/winscw/c/system" );
mkdir("/epoc32/winscw/c/system/install", 0777)	  if ( ! -d "/epoc32/winscw/c/system/install" );
mkdir("/epoc32/winscw/c/PlatformTest", 0777)	  if ( ! -d "/epoc32/winscw/c/PlatformTest" );

print "Running makeandsign.pl\n\n";
system ("perl makeandsign.pl");
print "\nmakeandsign.pl DONE\n\n";

mkdir("/epoc32/winscw/c/tswi/tsis/data", 0777)	  if ( ! -d "/epoc32/winscw/c/tswi/tsis/data");
mkdir("/epoc32/winscw/c/tswi/tsis/scripts", 0777) if ( ! -d "/epoc32/winscw/c/tswi/tsis/scripts");

copy("../scripts/tsis.script","/epoc32/winscw/c/tswi/tsis/scripts");
copy("../scripts/tsis.ini","/epoc32/winscw/c/tswi/tsis/scripts");

print "Running makecorrupted.pl\n\n";
system ("perl makecorrupted.pl");
print "\nmakecorrupted.pl DONE\n\n";

copy("../scripts/tsis_signed.script","/epoc32/winscw/c/tswi/tsis/scripts");
copy("../scripts/tsis_signed.ini","/epoc32/winscw/c/tswi/tsis/scripts");

print "Running signsis.pl\n\n";
chdir ("signedsis" );
system ("perl signsis.pl");
print "\nsignsis.pl DONE\n";

chdir("..");

copy("../scripts/tsistestdata.iby","../../../inc");
copy("../scripts/tsistestdata.iby","/epoc32/rom/include");

exit 0;

