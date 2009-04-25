# Load cmdline module
lappend auto_path ${clackwise_root_path}/lib/cmdline
package require cmdline

# Load clackwise liberty module
puts "Loading ClackwiseLiberty module ...\n"
load ${clackwise_root_path}/lib/libClackwiseLiberty.dylib ClackwiseLiberty

# Activate tcl readline if found
if {![catch {package require tclreadline}]} {
	::tclreadline::Loop
}

