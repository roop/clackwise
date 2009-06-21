# Load cmdline module
lappend auto_path ${clackwise_root_path}/lib/cmdline
package require cmdline

# Load clackwise liberty module
puts "Loading ClackwiseLiberty module ...\n"
load ${clackwise_root_path}/lib/libClackwiseLiberty.so ClackwiseLiberty
source ${clackwise_root_path}/lib/clackwise/clackwise_liberty.tcl

# Load .clackwiserc files
if {![info exists init_disable_clackwiserc] || $init_disable_clackwiserc == 0} {
	set cwrcs {}
	if {[info exists clackwise_root_path]} {
		lappend cwrcs ${clackwise_root_path}/.clackwiserc
	}
	if {[info exists env(HOME)]} {
		lappend cwrcs $env(HOME)/.clackwiserc
	}
	lappend cwrcs .clackwiserc
	foreach cwrc $cwrcs {
		if {[file exists $cwrc]} {
			source $cwrc
		}
	}
}

# Load any -f file
if {[info exists init_eval_file] && $init_eval_file != ""} {
	if {[file exists $init_eval_file]} {
		source $init_eval_file
	} else {
		puts "Error: Can't find $init_eval_file passed as -f"
	}
}

# Load any -x command
if {[info exists init_eval_command] && $init_eval_command != ""} {
	eval $init_eval_command
}

set ::tcl_interactive 1

# Activate tcl readline if found
if {![catch {package require tclreadline}]} {
	rename tclreadline::Print tclreadline::Print_orig
	proc tclreadline::Print {args} {
		if [regexp ^_.*_p_Clackwise__ $::tclreadline::result] {
			show_objects $::tclreadline::result
			return no;
		}
		return [tclreadline::Print_orig $::tclreadline::result];
	}
	source ${clackwise_root_path}/lib/clackwise/tclreadlineCwCompleter.tcl
	::tclreadline::readline customcompleter ::tclreadline::cw_completer
	set tclreadline::historyLength 10000
	::tclreadline::Loop ./.clackwise-history
}

