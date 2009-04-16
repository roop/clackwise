puts "Loading ClackwiseLiberty module ...\n"
load ${clackwise_root_path}/lib/libClackwiseLiberty.so ClackwiseLiberty

# Activate tcl readline if found
if {![catch {package require tclreadline}]} {
	::tclreadline::Loop
}

