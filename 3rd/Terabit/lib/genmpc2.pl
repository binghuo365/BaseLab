use Cwd;
use File::Basename;
use File::Copy;
use File::Find;
use File::Path;
use strict;


my @list_mpc ;
my $inc_dir;

sub write_line
{
    my $filename  = $_[0];
    my $line = $_[1];

    #print "Write file ${filename} ...\n";

    open (FH, ">" , $filename ) || die "can not open $filename : $!" ;

    print FH "${line}\n";
    
    truncate  (FH, tell(FH));
    close FH;
}

sub write_mwc
{
	my $dir = getcwd ();

	my $filename = "${dir}/build.mwc";

	print ( "Generating mwc : ${filename}\n" );

    open (FH, ">" , $filename ) || die "can not open $filename : $!" ;

    print FH "workspace  \{ \n";

	foreach my $x ( @list_mpc )
	{

	    print FH "     $x \n";
	}
    
    print FH "\}\n";
	
    
    truncate  (FH, tell(FH));
    close FH;
}

sub write_mpc
{
    my $refArrCPP = $_[0];
    my $refArrH = $_[1];

	my $dir = getcwd ();
    my $proj = basename($dir);


    my $x;

	print ( "Generating mpc: ${dir}/build.mpc \n" );
    push  ( @list_mpc, "${dir}/build.mpc" );     

    open (FH, ">" , "${dir}/build.mpc" ) || die "can not open ${dir}/build.mpc : $!" ;

    print FH  <<"EOL1";
project ($proj) {

    staticname = ${proj}
    includes += ${inc_dir}
    includes += ${inc_dir}/stlport

    Source_Files {
EOL1

	foreach $x ( @{$refArrCPP})
	{
		print FH "      $x\n";
    }

    print FH  <<"EOL2";
    }

    Header_Files {
EOL2

	foreach  $x ( @{$refArrH})
	{
		print FH "      $x\n";
    }

    print FH  <<"EOL3";
    }
}

EOL3


    truncate  (FH, tell(FH));
    close FH;
    
}

sub process_dir 
{
	my $dir = $_[0];
    my $stl_path = $_[1];

	my $curdir = getcwd ();
	chdir $dir ;

    my $i = 0;

	my @arrayCPP ;
    my @arrayH;

    foreach my $z (glob("./*"))
    {
        my $y = basename ($z);

        if ( -d $z)
	    {
            if ( $y =~ m!^doc! ||
                 $y =~ m!^include! ||
                 $y =~ m!^test! )
            {
                 next;
            }

            if ( $y =~ m/^stlport/ )
			{
				mkpath ( "${inc_dir}/${y}", 1);
	            process_dir ($z, $y);
			}
			elsif (defined ($stl_path))
			{
				mkpath ( "${inc_dir}/${stl_path}/${y}", 1);
                process_dir ($z, "${stl_path}/${y}" );
			}
            else
			{
	            process_dir ($z);
			}

            $i = $i+1;
            next;
		}

		if ( $y =~ m/\.cpp$/ ||  $y =~ m/\.c$/ )
		{
            if ($y !~ m/\.t\.cpp$/)
			{
	            push (@arrayCPP, $y);
			}
		}

		if ( $y =~ m/\.h$/ )
        {
	          push (@arrayH, $y);
	    }

		if (defined ($stl_path))
		{
			link ( $z, "${inc_dir}/${stl_path}/${y}");
		}
        elsif  ($y =~ m/\.h$/ )
        {
			link ( $z, "${inc_dir}/${y}");
		}
    }

	if (@arrayCPP != 0)
	{
    	write_mpc (\@arrayCPP, \@arrayH);
	}

	chdir $curdir;
}

sub do_main 
{
    if (@ARGV != 0)
	{
		chdir ($ARGV[0]);
	}

	my $dir =  getcwd ();

    $inc_dir = "${dir}/include";

	mkpath ("${inc_dir}", 1) ;
	
	process_dir ($dir);
	write_mwc ;
}


do_main ;
