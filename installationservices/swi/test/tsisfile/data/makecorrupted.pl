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
use Getopt::Std;
use Cwd;
use File::Copy;
my %opt;

# --------------------------------------------------------------------------

sub usage() {
    print STDERR << "EOF";
    usage: $0 [-hvc]
     -h        : this (help) message
     -v        : verbose output
     -c        : clean up signed .sis files
EOF
   exit;
}

# --------------------------------------------------------------------------

# Return the modification time of a file or zero if non-existent
#
sub mtimeFile($) {
    my ($file) = shift;
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
        $atime,$mtime,$ctime,$blksize,$blocks) = stat($file);
    $mtime = 0 if ! defined( $mtime );
    return $mtime;
}

# --------------------------------------------------------------------------

# Create corrupted xx sis files
#
sub createCorruptedXX($$) {
    my $code = shift;
    my $field = shift;

    my $testmakesis = "$ENV{SECURITYSOURCEDIR}/secureswitools/swisistools/test/testmakesis/testmakesis";
    my $outdir = "$ENV{EPOCROOT}epoc32/winscw/c/tswi/tsis/data/corrupted$code";
    mkdir( $outdir, 0777);
    mkdir( "corrupted$code", 0777);
    

    my $file = "pkg.config";
    open( PKG, $file) || die( "Could not open pkg.config: $!" );
    my @pkgfiles = <PKG>;
    close( PKG );

    foreach my $pkgfile (@pkgfiles)
    {
       chomp( $pkgfile );
       my $sisfile = $pkgfile;
       $sisfile =~ s/.pkg$/.sis/;
       my $outsis = "$outdir/$sisfile";
       $sisfile = "corrupted$code/$sisfile";

       # Remove the corrupted SIS file if asked to clean
       if ( $opt{c} ) {
          if ( -f "$outsis" ) {
             unlink( "$outsis" );
             print( "Removed $outsis\n" ) if $opt{v};
          }
          if ( -f "$sisfile" ) {
             unlink( "$sisfile" );
             print( "Removed $sisfile\n" ) if $opt{v};
          }
          next;
       }

       # Otherwise re-generate corrupted SIS file if necessary
       if ( mtimeFile( "$outsis" ) < mtimeFile( "$pkgfile" ) ||
            mtimeFile( "$outsis" ) < mtimeFile( "$testmakesis" )) {
          print("$testmakesis -$field -$code $pkgfile $sisfile\n") if $opt{v};
          system("$testmakesis -$field -$code $pkgfile $sisfile");
          print("copy $sisfile $outsis\n") if $opt{v};
          copy("$sisfile", "$outsis");
       } else {
          print("$outdir/$pkgfile is up-to-date\n") if $opt{v};
       }
    }
}

# --------------------------------------------------------------------------

# Unfortunately we can't sign corrupted SIS files

getopts( "vch:", \%opt ) or usage();
usage() if $opt{h};

print "Generating Corrupted SISX files\n";

mkdir ("/epoc32/winscw/c/tswi/", 0777);
mkdir ("/epoc32/winscw/c/tswi/tsis/", 0777);
mkdir ("/epoc32/winscw/c/tswi/tsis/data", 0777);

createCorruptedXX("bA", "Q1");
createCorruptedXX("bB", "Q1");
# createCorruptedXX("bC", "Q1");
# createCorruptedXX("bE", "Q1");
createCorruptedXX("bF", "Q1");
createCorruptedXX("bH", "Q1");
createCorruptedXX("bK", "Q1");

# Note that the second field length is specified to be corrupted; for an uncompressed SIS file the first
# field will be Contents, and it doesn't appear to cause a problem if this length is wrong.
createCorruptedXX("bL", "Q2");
createCorruptedXX("bM", "Q6");
createCorruptedXX("bN", "Q2");
createCorruptedXX("bS", "Q1");
createCorruptedXX("bT", "Q1");
createCorruptedXX("bU", "Q1");
createCorruptedXX("bX", "Q1");

print "Generating Corrupted SISX files DONE\n";
exit 0;

