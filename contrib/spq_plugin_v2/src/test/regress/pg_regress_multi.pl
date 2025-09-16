#!/usr/bin/perl -w
#----------------------------------------------------------------------
#
# pg_regress_multi.pl - Test runner for Citus
#
# Portions Copyright (c) Citus Data, Inc.
# Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
# Portions Copyright (c) 1994, Regents of the University of California
#
# src/test/regress/pg_regress_multi.pl
#
#----------------------------------------------------------------------

use strict;
use warnings;

use Fcntl;
use Getopt::Long;
use File::Basename;
use File::Spec::Functions;
use File::Path qw(make_path remove_tree);
use Config;
use POSIX qw( WNOHANG mkfifo );
use Cwd 'abs_path';

my $regressdir = (File::Spec->splitpath(__FILE__))[1];


sub Usage()
{
    print "pg_regress_multi - Citus test runner\n";
    print "\n";
    print "Usage:\n";
    print "  pg_regress_multi [MULTI OPTIONS] -- [PG REGRESS OPTS]\n";
    print "\n";
    print "Multi Options:\n";
    print "  --isolationtester   	Run isolationtester tests instead of plain tests\n";
    print "  --vanillatest       	Run postgres tests with spq loaded as shared preload library\n";
    print "  --bindir            	Path to postgres binary directory\n";
    print "  --libdir            	Path to postgres library directory\n";
    print "  --postgres-builddir 	Path to postgres build directory\n";
    print "  --postgres-srcdir   	Path to postgres build directory\n";
    print "  --pgxsdir           	Path to the PGXS directory\n";
    print "  --load-extension    	Extensions to install in all nodes\n";
    print "  --server-option     	Config option to pass to the server\n";
    print "  --valgrind          	Run server via valgrind\n";
    print "  --valgrind-path     	Path to the valgrind executable\n";
    print "  --valgrind-log-file	Path to the write valgrind logs\n";
    print "  --gs_ctl-timeout    	Timeout for gs_ctl\n";
    print "  --connection-timeout	Timeout for connecting to worker nodes\n";
    print "  --worker-count             Number of workers in Citus cluster (default: 2)\n";
    print "  --pg-regress-single        Path of pg_regress_single \n";
    exit 1;
}

my $TMP_CHECKDIR = 'tmp_check';
my $TMP_BINDIR = 'tmp-bin';
my $MASTERDIR = 'master';
my $MASTER_FOLLOWERDIR = 'master-follower';

# Option parsing
my $isolationtester = 0;
my $vanillatest = 0;
my $followercluster = 0;
my $bindir = "";
my $libdir = undef;
my $pgxsdir = "";
my $postgresBuilddir = "";
my $spqAbsSrcdir = "";
my $postgresSrcdir = "";
my $majorversion = "";
my $synchronousReplication = "";
my @extensions = ();
my @userPgOptions = ();
my %functions = ();
my $valgrind = 0;
my $valgrindPath = "valgrind";
my $valgrindLogFile = "valgrind_test_log.txt";
my $pgCtlTimeout = undef;
my $connectionTimeout = 5000;
my $conninfo = "";
my $publicWorker1Host = "localhost";
my $publicWorker2Host = "localhost";
my $workerCount = 2;
my $pgregressSingle = "";

my $serversAreShutdown = "TRUE";

GetOptions(
    'isolationtester' => \$isolationtester,
    'vanillatest' => \$vanillatest,
    'follower-cluster' => \$followercluster,
    'bindir=s' => \$bindir,
    'libdir=s' => \$libdir,
    'pgxsdir=s' => \$pgxsdir,
    'postgres-builddir=s' => \$postgresBuilddir,
    'postgres-srcdir=s' => \$postgresSrcdir,
    'spq_abs_srcdir=s' => \$spqAbsSrcdir,
    'majorversion=s' => \$majorversion,
    'load-extension=s' => \@extensions,
    'server-option=s' => \@userPgOptions,
    'valgrind' => \$valgrind,
    'valgrind-path=s' => \$valgrindPath,
    'valgrind-log-file=s' => \$valgrindLogFile,
    'gs_ctl-timeout=s' => \$pgCtlTimeout,
    'connection-timeout=s' => \$connectionTimeout,
    'conninfo=s' => \$conninfo,
    'worker-1-public-hostname=s' => \$publicWorker1Host,
    'worker-2-public-hostname=s' => \$publicWorker2Host,
    'worker-count=i' => \$workerCount,
    'pg-regress-single=s' => \$pgregressSingle,
    'help' => sub { Usage() });

my $fixopen = "$bindir/postgres.fixopen";
my @gs_ctl_args = ();
if (-e $fixopen)
{
	push(@gs_ctl_args, "-p");
	push(@gs_ctl_args, $fixopen);
}

# Update environment to include [DY]LD_LIBRARY_PATH/LIBDIR/etc -
# pointing to the libdir - that's required so the right version of
# libpq, spq et al is being picked up.
#
# XXX: There's some issues with el capitan's SIP here, causing
# DYLD_LIBRARY_PATH not being inherited if SIP is enabled. That's a
# known problem, present in postgres itself as well.
if (defined $libdir)
{
    $ENV{LD_LIBRARY_PATH} = "$libdir:".($ENV{LD_LIBRARY_PATH} || '');
    $ENV{DYLD_LIBRARY_PATH} = "$libdir:".($ENV{DYLD_LIBRARY_PATH} || '');
    $ENV{LIBPATH} = "$libdir:".($ENV{LIBPATH} || '');
    $ENV{PATH} = "$libdir:".($ENV{PATH} || '');
}

# Put $bindir to the end of PATH. We want to prefer system binaries by
# default (as e.g. new libpq and old gsql can cause issues), but still
# want to find binaries if they're not in PATH.
if (defined $bindir)
{
    $ENV{PATH} = ($ENV{PATH} || '').":$bindir";
}


# Most people are used to unified diffs these days, rather than the
# context diffs pg_regress defaults to.  Change default to avoid
# everyone having to (re-)learn how to change that setting.  Also add
# a bit more context to make it easier to locate failed test sections.
#
# Also, ignore whitespace, without this the diffs on windows are unreadable
$ENV{PG_REGRESS_DIFF_OPTS} = '-dU10 -w';

my $plainRegress = "$ENV{CODE_BASE}/src/test/regress/pg_regress";
if ($pgregressSingle ne "" ) {
    my $plainRegress = $pgregressSingle;
}

my $isolationRegress = "";
my $pgConfig = "";

$isolationRegress = "${postgresBuilddir}/src/test/isolation/pg_isolation_regress";
$pgConfig = "$bindir/pg_config";

if (-x "$pgxsdir/src/test/isolation/pg_isolation_regress")
{
    $isolationRegress = "$pgxsdir/src/test/isolation/pg_isolation_regress";
}

if ($isolationtester && ! -f "$isolationRegress")
{
    die <<"MESSAGE";

isolationtester not found at $isolationRegress.

isolationtester tests can only be run when source (detected as ${postgresSrcdir})
and build (detected as ${postgresBuilddir}) directory corresponding to $bindir
are present.

Additionally isolationtester in src/test/isolation needs to be built,
which it is not by default if tests have not been run. If the build
directory is present locally
"make -C ${postgresBuilddir} all" should do the trick.
MESSAGE
}

my $vanillaRegress = catfile("${postgresBuilddir}", "src", "test", "regress", "pg_regress");
my $vanillaSchedule = catfile(dirname("${pgxsdir}"), "regress", "parallel_schedule");

if ($vanillatest && ! (-f "$vanillaRegress" or -f "$vanillaSchedule"))
{
    die <<"MESSAGE";

pg_regress (for vanilla tests) not found at $vanillaRegress.

Vanilla tests can only be run when source (detected as ${postgresSrcdir})
and build (detected as ${postgresBuilddir}) directory corresponding to $bindir
are present.
MESSAGE
}

# If pgCtlTimeout is defined, we will set related environment variable.
# This is generally used with valgrind because valgrind starts slow and we
# need to increase timeout.
if (defined $pgCtlTimeout)
{
    $ENV{PGCTLTIMEOUT} = "$pgCtlTimeout";
}

# We don't want valgrind to run gs_ctl itself, as that'd trigger a lot
# of spurious OS failures, e.g. in bash. So instead we have to replace
# the postgres binary with a wrapper that exec's valgrind, which in
# turn then executes postgres.  That's unfortunately at the moment the
# only reliable way to do this.
sub replace_postgres
{
    if (-e catfile("$bindir", "postgres.orig"))
    {
	print "wrapper exists\n";
    }
    else
    {
	print "moving $bindir/postgres to $bindir/postgres.orig\n";
	rename catfile("$bindir", "postgres"), catfile("$bindir", "postgres.orig")
	    or die "Could not move postgres out of the way";
    }

    sysopen my $fh, catfile("$bindir", "postgres"), O_CREAT|O_TRUNC|O_RDWR, 0700
	or die "Could not create postgres wrapper at $bindir/postgres";
    print $fh <<"END";
#!/bin/bash
exec $valgrindPath \\
    --quiet \\
    --suppressions=${postgresSrcdir}/src/tools/valgrind.supp \\
    --trace-children=yes --track-origins=yes --read-var-info=no \\
    --leak-check=no \\
    --error-markers=VALGRINDERROR-BEGIN,VALGRINDERROR-END \\
    --max-stackframe=16000000 \\
    --log-file=$valgrindLogFile \\
    --fullpath-after=/ \\
    $bindir/postgres.orig \\
    "\$@"
END
    close $fh;
}

sub write_settings_to_postgres_conf
{
    my ($pgOptions, $pgConfigPath) = @_;
    open(my $fd, ">>", $pgConfigPath);

    foreach (@$pgOptions)
    {
      print $fd "$_\n";
    }

    close $fd;
}

# revert changes replace_postgres() performed
sub revert_replace_postgres
{
    if (-e catfile("$bindir", "postgres.orig"))
    {
	print "wrapper exists, removing\n";
	print "moving $bindir/postgres.orig to $bindir/postgres\n";
	rename catfile("$bindir", "postgres.orig"), catfile("$bindir", "postgres")
	    or die "Could not move postgres back";
    }
}

sub generate_hba
{
    my $nodename = shift;

    open(my $fh, ">", catfile($TMP_CHECKDIR, $nodename, "data", "pg_hba.conf"))
        or die "could not open pg_hba.conf";
    print $fh "local all        all                    trust\n";
    print $fh "host all         alice,bob 127.0.0.1/32 md5\n";
    print $fh "host all         alice,bob ::1/128      md5\n";
    print $fh "host all         all       127.0.0.1/32 trust\n";
    print $fh "host all         all       ::1/128      trust\n";
    print $fh "host replication postgres  127.0.0.1/32 trust\n";
    print $fh "host replication postgres  ::1/128      trust\n";
    close $fh;
}

# always want to call initdb under normal postgres, so revert from a
# partial run, even if we're now not using valgrind.
revert_replace_postgres();

my $host = "localhost";
my $user = `whoami`;
chomp($user);
my $dbname = "postgres";

print "Spq will be loaded as: $user\n";

# Set some default configuration options
my $masterPort = 58637;
my $reservePortPerNode = 2;

my @workerHosts = ();
my @workerPorts = ();
my $testPassWord = "Huawei\@123";


if ( $conninfo )
{
    my %convals = split /=|\s/, $conninfo;
    if (exists $convals{user})
    {
        $user = $convals{user};
    }
    if (exists $convals{host})
    {
        $host = $convals{host};
    }
    if (exists $convals{port})
    {
        $masterPort = $convals{port};
    }
    if (exists $convals{dbname})
    {
        $dbname = $convals{dbname};
    }

    open my $in, '<', "bin/normalize.sed" or die "Cannot open normalize.sed file\n";
    open my $out, '>', "bin/normalize_modified.sed" or die "Cannot open normalize_modified.sed file\n";

    while ( <$in> )
    {
        print $out $_;
    }

    close $in;


    print $out "\n";
    print $out "s/\\bdbname=regression\\b/dbname=<db>/g\n";
    print $out "s/\\bdbname=$dbname\\b/dbname=<db>/g\n";
    print $out "s/\\b$user\\b/<user>/g\n";
    print $out "s/\\bpostgres\\b/<user>/g\n";
    print $out "s/\\blocalhost\\b/<host>/g\n";
    print $out "s/\\b$host\\b/<host>/g\n";
    print $out "s/\\b576[0-9][0-9]\\b/xxxxx/g\n";
    print $out "s/", substr("$masterPort", 0, length("$masterPort")-2), "[0-9][0-9]/xxxxx/g\n";


    my $worker1host = `gsql "$conninfo" -qtAX -c "SELECT nodename FROM pg_dist_node ORDER BY nodeid LIMIT 1;"`;
    my $worker1port = `gsql "$conninfo" -qtAX -c "SELECT nodeport FROM pg_dist_node ORDER BY nodeid LIMIT 1;"`;
    my $worker2host = `gsql "$conninfo" -qtAX -c "SELECT nodename FROM pg_dist_node ORDER BY nodeid OFFSET 1 LIMIT 1;"`;
    my $worker2port = `gsql "$conninfo" -qtAX -c "SELECT nodeport FROM pg_dist_node ORDER BY nodeid OFFSET 1 LIMIT 1;"`;

    $worker1host =~ s/^\s+|\s+$//g;
    $worker1port =~ s/^\s+|\s+$//g;
    $worker2host =~ s/^\s+|\s+$//g;
    $worker2port =~ s/^\s+|\s+$//g;

    push(@workerPorts, $worker1port);
    push(@workerPorts, $worker2port);
    push(@workerHosts, $worker1host);
    push(@workerHosts, $worker2host);

    my $worker1hostReplaced = $worker1host;
    my $worker2hostReplaced = $worker2host;

    $worker1hostReplaced =~ s/\./\\\./g;
    $worker2hostReplaced =~ s/\./\\\./g;

    print $out "s/\\b$worker1hostReplaced\\b/<host>/g\n";
    print $out "s/\\b$worker2hostReplaced\\b/<host>/g\n";
}
else
{
    for (my $workerIndex = 1; $workerIndex <= $workerCount; $workerIndex++) {
        my $workerPort = $masterPort + ($workerIndex * $reservePortPerNode);
        push(@workerPorts, $workerPort);
        push(@workerHosts, "localhost");
    }
}

for my $port (@workerPorts)
{
    print "Spq will launch node on port: $port\n";
}

my $followerCoordPort = 9070;
my @followerWorkerPorts = ();
for (my $workerIndex = 1; $workerIndex <= $workerCount; $workerIndex++) {
    my $workerPort = $followerCoordPort + $workerIndex * $reservePortPerNode;
    push(@followerWorkerPorts, $workerPort);
}

my @pgOptions = ();

# Postgres options set for the tests
push(@pgOptions, "listen_addresses='${host}'");
push(@pgOptions, "fsync=off");
if (! $vanillatest)
{
    push(@pgOptions, "extra_float_digits=0");
}

my $sharedPreloadLibraries = "spq";

# check if pg_stat_statements extension is installed
# if it is add it to shared preload libraries
my $sharedir = `$pgConfig --sharedir`;
chomp $sharedir;
my $pg_stat_statements_control = catfile($sharedir, "extension", "pg_stat_statements.control");
if (-e $pg_stat_statements_control)
{
    $sharedPreloadLibraries .= ',pg_stat_statements';
}

# check if hll extension is installed
# if it is add it to shared preload libraries
my $hll_control = catfile($sharedir, "extension", "hll.control");
if (-e $hll_control)
{
    $sharedPreloadLibraries .= ',hll';
}
push(@pgOptions, "shared_preload_libraries='${sharedPreloadLibraries}'");

# Help with debugging
push(@pgOptions, "log_error_verbosity = 'verbose'");

# Allow CREATE SUBSCRIPTION to work
push(@pgOptions, "wal_level='logical'");

# Faster logical replication status update so tests with logical replication
# run faster
push(@pgOptions, "wal_receiver_status_interval=0");

# Faster logical replication apply worker launch so tests with logical
# replication run faster. This is used in ApplyLauncherMain in
# src/backend/replication/logical/launcher.c.

if ($majorversion >= "14") {
    # disable compute_query_id so that we don't get Query Identifiers
    # in explain outputs
    push(@pgOptions, "compute_query_id=off");

    # reduce test flappiness and different PG14 plans
    if (!$vanillatest) {
        push(@pgOptions, "enable_incremental_sort=off");
    }
}

# Citus options set for the tests
push(@pgOptions, "spq.shard_count=4");
push(@pgOptions, "spq.max_adaptive_executor_pool_size=4");
# push(@pgOptions, "spq.defer_shard_delete_interval=-1");
# push(@pgOptions, "spq.repartition_join_bucket_count_per_node=2");
# push(@pgOptions, "spq.sort_returning='on'");
push(@pgOptions, "spq.node_connection_timeout=${connectionTimeout}");
push(@pgOptions, "spq.explain_analyze_sort_method='taskId'");
# push(@pgOptions, "spq.enable_manual_changes_to_shards=on");
# push(@pgOptions, "spq.allow_unsafe_locks_from_workers=on");
# push(@pgOptions, "spq.stat_statements_track = 'all'");
# push(@pgOptions, "spq.enable_change_data_capture=on");

# Some tests look at shards in pg_class, make sure we can usually see them:
push(@pgOptions, "spq.show_shards_for_app_name_prefixes='pg_regress'");

# we disable slow start by default to encourage parallelism within tests
push(@pgOptions, "spq.executor_slow_start_interval=0ms");
push(@pgOptions, "enable_opfusion = off");

# we set some GUCs to not break postgres vanilla tests
if($vanillatest)
{
    # we enable hiding the spq dependent objects from pg meta class queries to not break postgres vanilla test behaviour
    push(@pgOptions, "spq.hide_spq_dependent_objects=true");

    # we disable spq related unwanted messages to not break postgres vanilla test behaviour.
    push(@pgOptions, "spq.enable_unsupported_feature_messages=false");

    # we disable some restrictions for local objects like local views to not break postgres vanilla test behaviour.
    push(@pgOptions, "spq.enforce_object_restrictions_for_local_objects=false");
}
else
{
    # We currently need this config for isolation tests and security label tests
    # this option loads a security label provider, which we don't want in vanilla tests
    push(@pgOptions, "spq.running_under_spq_test_suite=true");
}

if ($followercluster)
{
    # follower clusters don't work well when automatically generating certificates as the
    # followers do not execute the extension creation sql scripts that trigger the creation
    # of certificates
    push(@pgOptions, "spq.node_conninfo='sslmode=prefer'");
    push(@pgOptions, "max_wal_senders=10");
    push(@pgOptions, "hot_standby=on");
    push(@pgOptions, "wal_level='replica'");
}


# disable automatic distributed deadlock detection during the isolation testing
# to make sure that we always get consistent test outputs. If we don't  manually
# (i.e., calling a UDF) detect the deadlocks, some sessions that do not participate
# in the deadlock may interleave with the deadlock detection, which results in non-
# consistent test outputs.
# since we have CREATE/DROP distributed tables very frequently, we also set
# shard_count to 4 to speed up the tests.
if($isolationtester)
{
    push(@pgOptions, "spq.worker_min_messages='warning'");
    push(@pgOptions, "spq.log_distributed_deadlock_detection=on");
    push(@pgOptions, "spq.shard_count=4");
    push(@pgOptions, "spq.metadata_sync_interval=1000");
    push(@pgOptions, "spq.metadata_sync_retry_interval=100");
    push(@pgOptions, "client_min_messages='warning'"); # pg12 introduced notice showing during isolation tests

    # Disable all features of the maintenance daemon. Otherwise queries might
    # randomly show temporarily as "waiting..." because they are waiting for the
    # maintenance daemon.
    push(@pgOptions, "spq.distributed_deadlock_detection_factor=-1");
    push(@pgOptions, "spq.recover_2pc_interval=-1");
    push(@pgOptions, "spq.enable_statistics_collection=false");
    push(@pgOptions, "spq.defer_shard_delete_interval=-1");
    push(@pgOptions, "spq.stat_statements_purge_interval=-1");
    push(@pgOptions, "spq.background_task_queue_interval=-1");
}

# Add externally added options last, so they overwrite the default ones above
for my $option (@userPgOptions)
{
    push(@pgOptions, $option);
}

# define functions as signature->definition
%functions = ();
if (!$conninfo)
{
	#%functions = ('fake_fdw_handler()', 'fdw_handler AS \'spq\' LANGUAGE C STRICT;');
}
else
{
    # when running the tests on a cluster these will be created with run_command_on_workers
    # so extra single quotes are needed
    #%functions = ('fake_fdw_handler()', 'fdw_handler AS \'\'spq\'\' LANGUAGE C STRICT;');
}


# Cleanup leftovers and prepare directories for the run
if (-e catfile($TMP_CHECKDIR, $TMP_BINDIR))
{
    remove_tree(catfile($TMP_CHECKDIR, $TMP_BINDIR)) or die "Could not remove $TMP_BINDIR directory";
}

if (-e catfile($TMP_CHECKDIR, $MASTERDIR))
{
    remove_tree(catfile($TMP_CHECKDIR, $MASTERDIR)) or die "Could not remove $MASTERDIR directory";
}

for my $port (@workerPorts)
{
    if (-e catfile($TMP_CHECKDIR, "worker.$port"))
    {
	remove_tree(catfile($TMP_CHECKDIR, "worker.$port")) or die "Could not remove worker directory";
    }
}

if (-e catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR))
{
    remove_tree(catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR)) or die "Could not remove $MASTER_FOLLOWERDIR directory";
}

for my $port (@followerWorkerPorts)
{
    if (-e catfile($TMP_CHECKDIR, "follower.$port"))
    {
	remove_tree(catfile($TMP_CHECKDIR, "follower.$port")) or die "Could not remove worker directory";
    }
}

for my $tablespace ("ts0", "ts1", "ts2")
{
    if (-e catfile($TMP_CHECKDIR, $tablespace))
    {
	remove_tree(catfile($TMP_CHECKDIR, $tablespace)) or die "Could not remove tablespace directory";
    }
    system("mkdir", ("-p", catfile($TMP_CHECKDIR, $tablespace))) == 0
            or die "Could not create vanilla testtablespace dir.";
}


# Prepare directory in which 'gsql' has some helpful variables for locating the workers
make_path(catfile($TMP_CHECKDIR, $TMP_BINDIR)) or die "Could not create $TMP_BINDIR directory $!\n";

my $gsql_name = "gsql";
sysopen my $fh, catfile($TMP_CHECKDIR, $TMP_BINDIR, $gsql_name), O_CREAT|O_TRUNC|O_RDWR, 0700
	or die "Could not create gsql wrapper";
print $fh catfile($bindir, "gsql")." ";
print $fh "--variable=master_port=$masterPort ";
print $fh "--variable=follower_master_port=$followerCoordPort ";
print $fh "--variable=default_user=$user ";
print $fh "--variable=SHOW_CONTEXT=always ";
print $fh "--variable=abs_srcdir=$spqAbsSrcdir ";
for my $workeroff (0 .. $#workerPorts)
{
	my $port = $workerPorts[$workeroff];
	print $fh "--variable=worker_".($workeroff+1)."_port=$port ";
}
for my $workeroff (0 .. $#workerHosts)
{
	my $host = $workerHosts[$workeroff];
	print $fh "--variable=worker_".($workeroff+1)."_host=\"$host\" ";
}
print $fh "--variable=master_host=\"$host\" ";
print $fh "--variable=public_worker_1_host=\"$publicWorker1Host\" ";
print $fh "--variable=public_worker_2_host=\"$publicWorker2Host\" ";
for my $workeroff (0 .. $#followerWorkerPorts)
{
	my $port = $followerWorkerPorts[$workeroff];
	print $fh "--variable=follower_worker_".($workeroff+1)."_port=$port ";
}

print $fh "--variable=dev_null=\"/dev/null\" ";
print $fh "--variable=temp_dir=\"/tmp/\" ";
print $fh "--variable=gsql=\"gsql\" ";
print $fh "\"\$@\"\n"; # pass on the commandline arguments
close $fh;


if (!$conninfo)
{
    make_path(catfile($TMP_CHECKDIR, $MASTERDIR, 'log')) or die "Could not create $MASTERDIR directory";
    for my $port (@workerPorts)
    {
        make_path(catfile($TMP_CHECKDIR, "worker.$port", "log"))
            or die "Could not create worker directory";
    }

    if ($followercluster)
    {
        make_path(catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR, 'log')) or die "Could not create $MASTER_FOLLOWERDIR directory";
        for my $port (@followerWorkerPorts)
        {
            make_path(catfile($TMP_CHECKDIR, "follower.$port", "log"))
                or die "Could not create worker directory";
        }
    }

    # Create new data directories, copy workers for speed
    # --allow-group-access is used to ensure we set permissions on private keys
    # correctly
    system(catfile("$bindir", "gs_initdb"), 
	   ("--encoding", "UTF8",
	    "--locale", "POSIX",
	    "--nodename", "spq",
	    "-w", $testPassWord,
	    catfile($TMP_CHECKDIR, $MASTERDIR, "data"))) == 0
        or die "Could not create $MASTERDIR data directory";

    generate_hba("master");

    for my $port (@workerPorts)
    {
	system("cp", ("-a", catfile($TMP_CHECKDIR, $MASTERDIR, "data"), catfile($TMP_CHECKDIR, "worker.$port", "data"))) == 0
	    or die "Could not create worker data directory";
    }
}


# Routine to shutdown servers at failure/exit
sub ShutdownServers()
{
    if (!$conninfo && $serversAreShutdown eq "FALSE")
    {
        system(catfile("$bindir", "gs_ctl"),
               (@gs_ctl_args, 'stop', '-w', '-D', catfile($TMP_CHECKDIR, $MASTERDIR, 'data'))) == 0
            or warn "Could not shutdown worker server";

        for my $port (@workerPorts)
        {
            system(catfile("$bindir", "gs_ctl"),
                   (@gs_ctl_args, 'stop', '-w', '-D', catfile($TMP_CHECKDIR, "worker.$port", "data"))) == 0
                or warn "Could not shutdown worker server";
        }

        if ($followercluster)
        {
            system(catfile("$bindir", "gs_ctl"),
                   (@gs_ctl_args, 'stop', '-w', '-D', catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR, 'data'))) == 0
                or warn "Could not shutdown worker server";

            for my $port (@followerWorkerPorts)
            {
                system(catfile("$bindir", "gs_ctl"),
                       (@gs_ctl_args, 'stop', '-w', '-D', catfile($TMP_CHECKDIR, "follower.$port", "data"))) == 0
                    or warn "Could not shutdown worker server";
            }
        }
        $serversAreShutdown = "TRUE";
    }
}

# Set signals to shutdown servers
$SIG{INT} = \&ShutdownServers;
$SIG{QUIT} = \&ShutdownServers;
$SIG{TERM} = \&ShutdownServers;
$SIG{__DIE__} = \&ShutdownServers;

# Shutdown servers on exit only if help option is not used
END
{
    if ($? != 1)
    {
        ShutdownServers();
    }

    # At the end of a run, replace redirected binary with original again
    if ($valgrind)
    {
        revert_replace_postgres();
    }
}

# want to use valgrind, replace binary before starting server
if ($valgrind)
{
    replace_postgres();
}


# Signal that servers should be shutdown
$serversAreShutdown = "FALSE";

# enable synchronous replication if needed
if ($followercluster)
{
    $synchronousReplication = "-c synchronous_standby_names='FIRST 1 (*)' -c synchronous_commit=remote_apply";
}

# Start servers
if (!$conninfo)
{
    write_settings_to_postgres_conf(\@pgOptions, catfile($TMP_CHECKDIR, $MASTERDIR, "data/postgresql.conf"));
    if(system(catfile("$bindir", "gs_ctl"),
        (@gs_ctl_args, 'start', '-w',
         '-o', " -c port=$masterPort $synchronousReplication",
         '-D', catfile($TMP_CHECKDIR, $MASTERDIR, 'data'),
	 '-l', catfile($TMP_CHECKDIR, $MASTERDIR, 'log', 'postmaster.log'))) != 0)
    {
        system("tail", ("-n20", catfile($TMP_CHECKDIR, $MASTERDIR, "log", "postmaster.log")));
        die "Could not start master server";
    }

    for my $port (@workerPorts)
    {
        write_settings_to_postgres_conf(\@pgOptions, catfile($TMP_CHECKDIR, "worker.$port", "data/postgresql.conf"));
        if(system(catfile("$bindir", "gs_ctl"),
            (@gs_ctl_args, 'start', '-w',
                '-o', " -c port=$port $synchronousReplication",
                '-D', catfile($TMP_CHECKDIR, "worker.$port", "data"),
                '-l', catfile($TMP_CHECKDIR, "worker.$port", "log", "postmaster.log"))) != 0)
        {
            system("tail", ("-n20", catfile($TMP_CHECKDIR, "worker.$port", "log", "postmaster.log")));
            die "Could not start worker server";
        }
    }
}

# Setup the follower nodes
if ($followercluster)
{
    system(catfile("$bindir", "pg_basebackup"),
           ("-D", catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR, "data"), "--host=$host", "--port=$masterPort",
            "--username=$user", "-R", "-X", "stream", "--no-sync")) == 0
      or die 'could not take basebackup';

    for my $offset (0 .. $#workerPorts)
    {
        my $workerPort = $workerPorts[$offset];
        my $followerPort = $followerWorkerPorts[$offset];
        system(catfile("$bindir", "pg_basebackup"),
               ("-D", catfile($TMP_CHECKDIR, "follower.$followerPort", "data"), "--host=$host", "--port=$workerPort",
                "--username=$user", "-R", "-X", "stream")) == 0
            or die "Could not take basebackup";
    }

    write_settings_to_postgres_conf(\@pgOptions, catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR, "data/postgresql.conf"));
    if(system(catfile("$bindir", "gs_ctl"),
           (@gs_ctl_args, 'start', '-w',
            '-o', " -c port=$followerCoordPort",
           '-D', catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR, 'data'), '-l', catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR, 'log', 'postmaster.log'))) != 0)
    {
        system("tail", ("-n20", catfile($TMP_CHECKDIR, $MASTER_FOLLOWERDIR, "log", "postmaster.log")));
        die "Could not start master follower server";
    }

    for my $port (@followerWorkerPorts)
    {
        write_settings_to_postgres_conf(\@pgOptions, catfile($TMP_CHECKDIR, "follower.$port", "data/postgresql.conf"));
        if(system(catfile("$bindir", "gs_ctl"),
               (@gs_ctl_args, 'start', '-w',
                '-o', " -c port=$port",
                '-D', catfile($TMP_CHECKDIR, "follower.$port", "data"),
                '-l', catfile($TMP_CHECKDIR, "follower.$port", "log", "postmaster.log"))) != 0)
        {
          system("tail", ("-n20", catfile($TMP_CHECKDIR, "follower.$port", "log", "postmaster.log")));
          die "Could not start follower server";
        }
    }
}

sub exec_sql_without_results
{
    my ($node_port, $dbname, $sql) = @_;
    return system(catfile($bindir, "gsql"),
	          ('-X',
                   '-p', $node_port,
		   '-U', $user,
		   "-d", $dbname,
                   '-c', $sql)) == 0;
}

###
# Create database, extensions, types, functions and fdws on the workers,
# pg_regress won't know to create them for us.
###
if (!$conninfo)
{
    for my $port (@workerPorts)
    {
	print "create database regression on node $port\n";
	exec_sql_without_results($port, "postgres", "CREATE DATABASE regression;")
            or die "Could not create regression database on worker port $port.";

        my $firstLib = `gsql -p "$port" -U "$user" -d regression -AXqt \\
                        -c "SHOW shared_preload_libraries;" | cut -d ',' -f1`;
        ($firstLib =~ m/^spq$/)
            or die "Could not find spq as first library in shared_preload_libraries on worker $port.";

        for my $extension (@extensions)
        {
             print "create extension $extension on node $port\n";
	    exec_sql_without_results($port, "regression", "CREATE EXTENSION IF NOT EXISTS $extension;")
                or die "Could not create extension $extension on worker port $port.";
        }

        foreach my $function (keys %functions)
        {
            print "create function $function on node $port\n";
	    exec_sql_without_results($port, "regression", "CREATE FUNCTION $function RETURNS $functions{$function};")
                or die "Could not create function $function on worker port $port";
        }
    }
}
else
{
    my $spqFirstLibCount = `gsql -p "$masterPort" -U "$user" -d "$dbname" -AXqt \\
                                -c "SELECT run_command_on_workers('SHOW shared_preload_libraries;');" \\
                                | cut -d ',' -f4 | grep -e '\"spq' | wc -l`;
    ($workerCount == $spqFirstLibCount)
        or die "Could not find spq as first library in shared_preload_libraries on workers.";

    for my $extension (@extensions)
    {
	exec_sql_without_results($masterPort,
		                 $dbname,
				 "SELECT run_command_on_workers('CREATE EXTENSION IF NOT EXISTS $extension;');")
            or die "Could not create extension $extension on workers";
    }

    foreach my $function (keys %functions)
    {
	exec_sql_without_results($masterPort,
		                 $dbname,
				 "SELECT run_command_on_workers('CREATE FUNCTION $function RETURNS $functions{$function};');")
           or die "Could not create function $function on workers.";
    }
}

print "prepare pg_regress, user $user, port: $masterPort\n";
# Prepare pg_regress arguments
my @arguments = (
    '--port', $masterPort,
    '--user', $user,
    '--top-builddir', $ENV{CODE_BASE},
    '--psqldir', catfile($TMP_CHECKDIR, $TMP_BINDIR),
    '-p', $masterPort,
    '-d', 1,
    '-c', 0,
    '--single_node'
);

# Add load extension parameters to the argument list
for my $extension (@extensions)
{
    push(@arguments, "--load-extension=$extension");
}

# Append remaining ARGV arguments to pg_regress arguments
push(@arguments, @ARGV);

my $startTime = time();

my $exitcode = 0;

sub RunVanillaTests
{
    ###
    # We want to add is_spq_depended_object function to the default db.
    # But without use-existing flag, pg_regress recreates the default db
    # after dropping it if exists. Thus, we set use-existing flag and
    # manually create the default db, spq extension and is_spq_depended_object
    # function. When use-existing flag is set, pg_regress does not drop
    # default db.
    ###

    $ENV{VANILLATEST} = "1";
    my $dbName = "regression";

    # create default db
    exec_sql_without_results($masterPort, "postgres",  "CREATE DATABASE $dbName;")
            or die "Could not create $dbName database on master";

    # alter default db's lc_monetary to C
    exec_sql_without_results($masterPort, $dbName, "ALTER DATABASE $dbName SET lc_monetary TO 'C';")
            or die "Could not create $dbName database on master";

    # create extension spq
    exec_sql_without_results($masterPort, $dbName, "CREATE EXTENSION spq;")
            or die "Could not create spq extension on master";

    # we do not want to expose that udf other than vanilla tests
    my $spq_depended_object_def = "CREATE OR REPLACE FUNCTION
                                        pg_catalog.is_spq_depended_object(oid,oid)
                                        RETURNS bool
                                        LANGUAGE C
                                        AS 'spq', \$\$is_spq_depended_object\$\$;";
    exec_sql_without_results($masterPort, $dbName, $spq_depended_object_def)
                or die "Could not create FUNCTION is_spq_depended_object on master";

    # we need to set regress path to find test input files(sqls and schedule)
    # we need to add regress.so path to dlpath because some tests need to find that lib
    my $pgregressInputdir="";
    my $dlpath="";
    if (-f "$vanillaSchedule")
    {
	$pgregressInputdir=catfile(dirname("$pgxsdir"), "regress");
        $dlpath=dirname("$pgxsdir")
    }
    else
    {
	$pgregressInputdir=catfile("$postgresSrcdir", "src", "test", "regress");
        $dlpath=$pgregressInputdir
    }

    # output dir
    my $pgregressOutputdir = "$spqAbsSrcdir/pg_vanilla_outputs/$majorversion";

    # prepare output and tablespace folder
    system("rm", ("-rf", "$pgregressOutputdir")) == 0
            or die "Could not remove vanilla output dir.";
    system("mkdir", ("-p", "$pgregressOutputdir/testtablespace")) == 0
            or die "Could not create vanilla testtablespace dir.";
    system("mkdir", ("-p", "$pgregressOutputdir/expected")) == 0
            or die "Could not create vanilla expected dir.";
    system("mkdir", ("-p", "$pgregressOutputdir/sql")) == 0
            or die "Could not create vanilla sql dir.";

    if ($majorversion >= "16")
    {
        $exitcode = system("$plainRegress",
                            ("--dlpath", $dlpath),
                            ("--inputdir",  $pgregressInputdir),
                            ("--outputdir",  $pgregressOutputdir),
                            ("--expecteddir",  $pgregressOutputdir),
                            ("--schedule",  catfile("$pgregressInputdir", "parallel_schedule")),
                            ("--use-existing"),
                            ("--host","$host"),
                            ("--port","$masterPort"),
                            ("--user","$user"),
                            ("--dbname", "$dbName"));
    }
    else
    {
        $exitcode = system("$plainRegress",
                            ("--dlpath", $dlpath),
                            ("--inputdir",  $pgregressInputdir),
                            ("--outputdir",  $pgregressOutputdir),
                            ("--schedule",  catfile("$pgregressInputdir", "parallel_schedule")),
                            ("--use-existing"),
                            ("--host","$host"),
                            ("--port","$masterPort"),
                            ("--user","$user"),
                            ("--dbname", "$dbName"));
    }
}

# Finally run the tests
if ($vanillatest)
{
    RunVanillaTests();
}
elsif ($isolationtester)
{
    push(@arguments, "--dbname=regression");
    $exitcode = system("$isolationRegress", @arguments)
}
else
{
    if ($conninfo)
    {
        push(@arguments, "--dbname=$dbname");
        push(@arguments, "--use-existing");
    }

    print "Run pg_regression with following paramters: \n";
    print "$plainRegress ";
    for my $arg (@arguments) {
        print "$arg ";
    }
    print "\n";

    $exitcode = system("$plainRegress", @arguments);
}

system ("copy_modified");
my $endTime = time();

if ($exitcode == 0) {
    print "Finished in ". ($endTime - $startTime)." seconds. \n";
    exit 0;
}
else {
    die "Failed in ". ($endTime - $startTime)." seconds. \n";
}
