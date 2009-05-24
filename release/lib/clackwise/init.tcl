# Load cmdline module
lappend auto_path ${clackwise_root_path}/lib/cmdline
package require cmdline

# Load clackwise liberty module
puts "Loading ClackwiseLiberty module ...\n"
load ${clackwise_root_path}/lib/libClackwiseLiberty.so ClackwiseLiberty
source ${clackwise_root_path}/lib/clackwise/clackwise_liberty.tcl

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
	::tclreadline::Loop
}

