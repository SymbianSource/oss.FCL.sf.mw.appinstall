#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use strict;
use File::Copy;
use Getopt::Std;
use Cwd;

my $makesis = "$ENV{EPOCROOT}epoc32/tools/makesis.exe";
my $signsis = "$ENV{EPOCROOT}epoc32/tools/signsis.exe";
my $cert_path = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis";
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
sub mtime($) {
    my ($file) = shift;
    my ( $dev,$ino,$mode,$nlink,$uid,$gid,$rdev,
      $size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($file);

    $mtime = 0 if ! defined( $mtime );
    return $mtime;
}

# --------------------------------------------------------------------------

# Create signed sis files
#
sub runMakesis() {
   chdir("../tobesigned");
   opendir DIR, "." || die "Cannot open directory: $!\n";
   my @files = grep (/\.pkg$/, readdir(DIR));
   closedir DIR;

   foreach my $pkgfile (@files) {
      my $sisfile = $pkgfile;
      $sisfile =~ s/.pkg$/.sis/;

      if ( $opt{c} ) {
          if ( -f $sisfile ) {
              print "Removing $sisfile\n" if $opt{v};
              unlink("$sisfile");
          }
          next;
      }

      if (   mtime($sisfile) < mtime($pkgfile)
          || mtime($sisfile) < mtime($makesis))
      {
          system ("$makesis $pkgfile $sisfile");
          print "$makesis $pkgfile $sisfile\n";
      } else {
          print "../tobesigned/$sisfile is up-to-date\n";
      }
   }
   chdir("../signedsis");
}

# --------------------------------------------------------------------------

###################################################################
#                                                                 #
# This function is used to trim leading and trailing white spaces #
# (including new-line characters) from the parameter.             #
#                                                                 #
###################################################################
sub trim($)
	{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
	}

# --------------------------------------------------------------------------

###################################################################
#                                                                 #
# Since multiple signing is not supported by makesis, following   #
# method of calling makesis multiple times will be followed       #
# to sign a sis multiple times.                                   #
#                                                                 #
# This function should be modified if makesis provide multiple    #
# signing feature in future.                                      #
#                                                                 #
# This function takes a configuration file (multiple_signing.txt) #
# as input. The configuration file will help us control the order #
# of signing and also to choose certificate-key pair for signing. #
#                                                                 #
# This can function can also be used to sign a sis file with user #
# provided certificate-key pair by using the configuration file   #
#                                                                 #
###################################################################
sub multisign()
	{
 
	my $fileName = "multiple_signing.txt";
	open (FD, $fileName) || die "Error! can't open $fileName\n";

	my @file_data = <FD>;

	my $signed_file;
	my $cert;
	my $key;
	my $passwd;
	my $target_dir = "$ENV{EPOCROOT}epoc32/winscw/c/tswi/tsis/data/multiple_times";
	my $input_file;
	my $output_file;
	my $tmp;
	my $addtoiby = "false";
	my $testsignedsisiby = "..\\..\\scripts\\tsissignedtestdata.iby";

	if( not open( ibyFH, ">>$testsignedsisiby" ) )
		{
		my $dir = cwd();
		print( "\nUnable to open $testsignedsisiby (we are in $dir)\n");
		#die;
		}

	mkdir ($target_dir, 0777);

	foreach my $file_line (@file_data)
		{
		next if ( $file_line =~ /[ \t]*#/i );
		$file_line = trim($file_line);
		next if ( $file_line eq "");
		if($file_line eq "[SIGNING]")
			{
			$addtoiby = "false";
			$signed_file = "";
			$cert = "";
			$key = "";
			$passwd = "";
			$input_file = "tmp1_signed.sis";
			$output_file = "tmp2_signed.sis";
			$tmp = $input_file;
			$target_dir = "$ENV{EPOCROOT}epoc32/winscw/c/tswi/tsis/data/multiple_times";
			next;
			}

		if($file_line eq "[/SIGNING]")
			{
			if($addtoiby eq "true")
				{
				printf (ibyFH "data = ZDRIVE\\tswi\\tsis\\data\\multiple_times\\$signed_file \t\t tswi\\tsis\\data\\multiple_times\\$signed_file\n");
				}
		
			system("move /Y $tmp $target_dir/$signed_file");
			unlink("$output_file");
			next;
			}

		my @argArr = split(/=/,$file_line);
		$argArr[0] = trim($argArr[0]);
		$argArr[1] = trim($argArr[1]);

		if($argArr[0] eq "sisfile")
			{
			$input_file = $argArr[1];
			next;
			}

		if($argArr[0] eq "signedsis")
			{
			$signed_file = $argArr[1];
			next;
			}

		if($argArr[0] eq "outputpath")
			{
			$target_dir = $argArr[1];
			next;
			}

		if($argArr[0] eq "addtoiby")
			{
			$addtoiby = $argArr[1];
			next;
			}

		if($argArr[0] eq "cert")
			{
			$cert = $argArr[1];
			next;
			}

		if($argArr[0] eq "password")
			{
			$passwd = $argArr[1];
			next;
			}

 		if($argArr[0] eq "key")
			{
			$key = $argArr[1];
			
			# pem files have been moved to a separate testcertificates directory for export reasons.
			my $key_path = "";
			if ($key =~ /.pem$/)
			{
				$key_path = "$ENV{'SECURITYSOURCEDIR'}\\installationservices\\switestfw\\testcertificates\\swi\\test\\tsisfile\\data\\signedsis\\";
			}
 
			system("$signsis -s $input_file $output_file $cert_path\\$cert $key_path$key $passwd");

			$input_file = $output_file;
			$output_file = $tmp;
			$tmp = $input_file;
			next;
			}
		}

	close(FD);
	}


getopts( "vch:", \%opt ) or usage();
usage() if $opt{h};

mkdir ("$ENV{EPOCROOT}epoc32/winscw/c/tswi", 0777);
mkdir ("$ENV{EPOCROOT}epoc32/winscw/c/tswi/tsis", 0777);
mkdir ("$ENV{EPOCROOT}epoc32/winscw/c/tswi/tsis/data", 0777);

# Firstly, process .pkg files in the ../tobesigned directory
# .sis files generated are be written into ../tobesigned
runMakesis();

multisign();

# Now iterate through all the .pkg files we've just signed
opendir( DIR, "../tobesigned" ) || die("Unable to open directory $!");
my @pkgfiles = grep (/\.pkg$/i, readdir(DIR));
closedir( DIR );

print "\n SISX files to be signed are: \n @pkgfiles \n";

# Create sis files signed with certificate chains.
for my $i ( @pkgfiles ) {
    $i =~ s/.pkg$/.sis/i;
    print "\nProcessing $i ...\n";

    my @roots = qw(Root5CA SymbianTestRootCADSA SymbianTestRootCARSA SymbianTestRootCANotInStore);
    ROOTDIR: for my $root_dirs (@roots)
    {
        print "   Processing $root_dirs ...\n";

        # Sign with both RSA and DSA keys
        foreach my $type ( "rsa", "dsa" )
        {
            # Skip DSA for test sis file with signing root not in the swicertstore.
            next ROOTDIR if ($type eq "dsa" && $root_dirs =~ /SymbianTestRootCANotInStore/);

            for (my $j = 1; $j <= 3; $j++)
            {
                # We only need one sis file with the signing root not in the swicertstore.
                next ROOTDIR if (($root_dirs =~ /SymbianTestRootCANotInStore/) && (($j != 1) || ($i !~ /logo/i)));

                my $shortdir = $root_dirs;
                $shortdir =~ s/SymbianTestRoot//ig;
                my ($outdir) = "$shortdir/${type}_len$j";
                my ($targetdir) = "$ENV{EPOCROOT}epoc32/winscw/c/tswi/tsis/data/$outdir";

                mkdir ("$ENV{EPOCROOT}epoc32/winscw/c/tswi/tsis/data/$shortdir", 0777);
                mkdir ($targetdir, 0777);

                my $signed_file = "$targetdir/$i";
                $signed_file =~ s/\.sis//i;
                $signed_file .= "_${type}_l$j.sis";

                # Just remove the output file if clean option set
                if ( $opt{c} ) {
                    if ( -f $signed_file ) {
                       print "      Removing $signed_file\n" if $opt{v};
                       unlink($signed_file);
                    }
                    next;
                }

                my $keyfile = "$root_dirs/cert_chain_${type}_len$j/$type$j.key";
            
                my $pemfile = "$cert_path/$root_dirs/cert_chain_${type}_len$j/chain_${type}_len$j.cert.pem";
 
                # Set the signing flag to be either -cr or -cd
                my $flag = "-cr";
                $flag = "-cd" if $type eq "dsa";

                my $sisfile = "../tobesigned/$i";
                my $command = "$signsis -s $flag $sisfile $signed_file $pemfile $keyfile";

                # Only resign the sis file if pemfile, keyfile, signsis.exe
                # or source .sis file has a more recent modification time.
                if (    mtime( "$signed_file" ) < mtime( $keyfile )
                     || mtime( "$signed_file" ) < mtime( $pemfile )
                     || mtime( "$signed_file" ) < mtime( $signsis )
                     || mtime( "$signed_file" ) < mtime( $sisfile ))
                {
                    print "      Processing cert_chain_${type}_len$j ...\n";
                    print "   $command\n"  if $opt{v};
                    system("$command");
                } else {
                    print "      $signed_file is up-to-date\n";
                }
            }
        }
    }
}

exit;

###############################################################
#### Multiple signing is not yet implemented in signsis.exe
###############################################################

mkdir("/epoc32/winscw/c/tswi/tsis/multiple_times", 0777);

# create sis files signed multiple times
for my $i (@pkgfiles)
{

 
    next if ( -d $i);
    next if ( $i !~ /\.sis$/i );

    my ($j, @arr, $signed_file);

    @arr = qw(one two three);

    $signed_file = $i;
    $signed_file =~ s/\.sis//i;
    $signed_file .= "_signed_$arr[0]_time.sis";
    system("$signsis -s $i $signed_file $cert_path\\cert_chain_rsa_len1\\chain_rsa_len1.cert.pem cert_chain_rsa_len1\\rsa1.key bla");

    for ($j=1; $j <= 2; $j++)
    {
	my ($orig_file) = $signed_file;

	if ($signed_file !~ /times/)
	{
	    $signed_file =~ s/time/times/;
	}

	$signed_file =~ s/$arr[$j-1]/$arr[$j]/;

	system("$signsis -s $orig_file $signed_file $cert_path\\cert_chain_rsa_len1\\chain_rsa_len1.cert.pem cert_chain_rsa_len1\\rsa1.key bla");
    }

    for ($j=0; $j<3; $j++)
    {
	system("move *$arr[$j]* \\epoc32\\winscw\\c\\tswi\\tsis\\multiple_times");
    }
}





