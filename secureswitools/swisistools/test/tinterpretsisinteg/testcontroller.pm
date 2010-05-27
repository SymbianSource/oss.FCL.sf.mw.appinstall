#
# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# controls the execution of tests.
#

package testcontroller;
use strict;
use Cwd;
use POSIX qw(strftime);
use File::Copy;

my $CURR_WORK_DIR = getcwd();
my $logFile = 0;
my $C_DRIVE = $CURR_WORK_DIR."\\cdrive";
my $Z_DRIVE = $CURR_WORK_DIR."\\romdrive";
my $USIF = 0;
my $testType = 0;

# constructor for TestController
sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
    $logFile = shift;
	$testType = shift;
	my $self  = {};
    $self->{'verbose'} = 0;
    $self->{'testlist'} = {};
    my @testorder;
    $self->{'testorder'} = \@testorder;
    my @failures;
    $self->{'failures'} = \@failures;
    $self->{'passed'} = 0;
    $self->{'run'} = 0;
    $self->{'basedir'} = "baseResults"; #default
    $self->{'testroutine'} = \&dummyTestRoutine;
    $self->{'debug'} = 0;
    bless ($self, $class);
    return $self;
}

#routines to setup attributes of the test controller
sub setBaseDir {
    my ($self, $dir) = @_;
    $self->{'basedir'} = $dir;
}

sub setOrder {
    my ($self, $order) = @_;
    $self->{'testorder'} = $order;
}

sub setTestRoutine {
    my ($self, $routine) = @_;
    $self->{'testroutine'} = $routine;
}

sub setVerbose {
    my ($self) = @_;
    $self->{'verbose'} = 1;
}

sub isDebug {
    my ($self) = @_;
    return $self->{'debug'};
}

# add test to test list preserving the order that it was added. The tests are
# stored in a hash which loses the order, so separate array is used to preserve
# the order
sub addTest { 
    my ($self, $testEntry) = @_;
    my $testlist = $self->{'testlist'};
    my $order = $self->{'testorder'};
    my $testid = $testEntry->{'id'};
    $testlist->{$testid} = $testEntry;
    push @$order, $testid;
}

sub CopyScrDb()
	{
	my $epocRoot = $ENV{'EPOCROOT'};
	my $ScrBackup = $epocRoot."epoc32\\winscw\\c\\tswi\\tinterpretsisinteg\\backupscr.db";
	my $ScrDest = $Z_DRIVE."\\sys\\install\\scr\\provisioned\\scr.db";
	copy($ScrBackup, $ScrDest);
	}

sub CreateDir()
	{
	my $current_dir = getcwd();
	chdir($Z_DRIVE);
	mkdir("sys");
	chdir("sys");
	mkdir("install");
	chdir("install");
	mkdir("scr");
	chdir("scr");	
	mkdir("provisioned");
	chdir("provisioned");	
	chdir $current_dir;
	}

sub InitDbEnv()
	{
	CreateDir();
	CopyScrDb();
	}

sub CopyRegFile()
	{
	my $current_dir = getcwd();
	chdir($Z_DRIVE);
	mkdir("system");
	chdir("system");
	mkdir("data");

	my $securityDir = $ENV{'SECURITYSOURCEDIR'};
	my $regSource = $securityDir."\\installationservices"."\\swi\\inc\\sisregistry_5.3.txt";
	my $regDest = $Z_DRIVE."\\system\\data\\sisregistry_5.3.txt";
	copy($regSource, $regDest);
	chmod(0666,$regDest);

	chdir $current_dir;
	}

# main routine to run all the tests.
sub runTests {
    my ($self) = shift;
    my $order = $self->{'testorder'};
    my $testList = $self->{'testlist'};
    my $now_str = strftime "%a %b %d %Y at %H:%M:%S", localtime;
    my $timeStart = "Tests run on $now_str \n";
    WriteLog($timeStart);
    	
		
    foreach my $testid (@$order) {
	if (exists ($testList->{$testid}))
	{
		mkdir($C_DRIVE);
		mkdir($Z_DRIVE);
		if($testType eq "usifnative")
			{
			CopyRegFile();
			}
		elsif($testType eq "usif")
			{
			InitDbEnv();
			}
		$self->{'run'}++;
	    my $testEntry = $testList->{$testid};
	    my $test = "test $testEntry->{'id'}";
	    WriteLog($test);
	    my $line = " - " . $testEntry->{'title'} . " " if $self->{'verbose'};
	    WriteLog($line);
	    WriteLog("... \n");
	    my $routine = $self->{'testroutine'};
	    my $result = &$routine($testEntry, $self->{'basedir'}, $self->{'failures'}, $logFile );
	    if ($result)
	    {
		WriteLog("Passed\n\n");
		$self->{'passed'}++;
	    }
	    else
	    {
		WriteLog("Failed($result) !!\n\n"); # test routine should have added reason for failure
	    }
	}
	else
	{
	    print "Test id '" . $testid . "' does not exist\n";
	}
    }

    # all tests have been run, display summary of results
    if ($self->{'run'} > 0)
    {
	my $failures = 0;
	my $numFails = 0;
	if ($self->{'passed'} == $self->{'run'})
	{
	    print "\n------------------------------------------\n";
	    print "All Tests Passed (". $self->{'run'} . " tests)\n";
	}
	else 
	{
	    $failures = $self->{'failures'};
	    $numFails = @$failures;
	    print "\n----------------- ". $numFails . " failures ----------\n";
	    print join("\n", @$failures);
	    print "\n------------------------------------------\n";
	}
	my $run = "Run: " . $self->{'run'}."\n";
	my $passed = "Passed: ".$self->{'passed'}."\n";
	my $total = $numFails." tests failed out of ".$self->{'run'}."\n\n";
	WriteLog($run);
	WriteLog($passed);
	WriteLog($total);
    }
    else
    {
	print "No Tests Run\n";
    }
    
    # Now we should delete the dirs
    RemoveDir($C_DRIVE);
    RemoveDir($Z_DRIVE);
    
    $now_str = strftime "%a %b %d %Y at %H:%M:%S", localtime;
    print "Finished ", $now_str, "\n";
}


# debug function
sub displayTestList {
    my ($self) = shift;
    print "Test List\n";
    my $testlist = $self->{'testlist'};
    foreach my $test (values %$testlist) {
	print "--- Test -----\n";
	while (my ($key, $value) = each %$test)
	{
	    print "$key->$value\n";
	}
    }
}

sub dummyTestRoutine {
    my ($testEntry, $basedir, $failures) = @_;
    die "No test routines to run tests have been enabled\n";
}

#
#Function to write log into file
#
sub WriteLog {
	my ($log) = @_;
	my $fh;

	unless( open($fh, ">> $logFile")) {
		printf STDERR "Can\'t open $logFile:$!\n";
		return;
	}
	printf $fh $log;
	printf $log;
	close $fh;
}

sub RemoveDir($)
{
	my $dir = $_[0];
	if(! (-e $dir))
	{
		return;
		}

	opendir(DIRHANDLE, $dir) || die "can't open file $dir";
	my $name;
	
	foreach $name (sort readdir(DIRHANDLE)) {
		if ($name ne "." && $name ne "..") {
			if (-d "$dir/$name") {
				&RemoveDir("$dir/$name");
				rmdir("$dir/$name");
			} else {
				unlink("$dir/$name");
			}
		}
	}
	closedir(DIRHANDLE);
	rmdir($dir);
}

1;
