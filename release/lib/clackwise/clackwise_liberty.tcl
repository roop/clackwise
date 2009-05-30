
set ::clackwise_commands(read_lib) {
	{Read a .lib file into memory}
	{filename}
	{
	}
}
proc read_lib {args} {
	set ::argv0 "read_lib"
	set summary [lindex $::clackwise_commands($::argv0) 0]
	set usage [lindex $::clackwise_commands($::argv0) 1]
	set options [lindex $::clackwise_commands($::argv0) 2]
	array set params [::cmdline::getoptions args $options "$usage # $summary"]
	if {[info exists params(__NON_SWITCH_ARGS__)] && [llength $params(__NON_SWITCH_ARGS__)] > 0} {
		set paramcount [llength $params(__NON_SWITCH_ARGS__)]
		set filename [lindex $params(__NON_SWITCH_ARGS__) 0]
		if {$paramcount == 1} {
			if {![file isfile $filename]} {
				error "Error: No such file - $filename";
				return;
			}
			set lib [cw_read_lib $filename]
			if {[CwLibGroup_name $lib] == ""} {
				return ""
			}
			return $lib
		} elseif {$paramcount == 2} {
			error "Error: That's one file too many than what I can handle"
		} else {
			error "Error: That's [expr $paramcount - 1] files too many than what I can handle"
		}
	}
	return "";
}

proc show_objects {collection} {
	set i 0
	set strlist {}
	foreach cw_ptr $collection {
		if {[regexp {^_.*_p_} $cw_ptr]} {
			lappend strlist "\"[object_to_string $cw_ptr]\""
		} else {
			lappend strlist $cw_ptr
		}
		if {[incr i] > 10} {
			break
		}
	}
	puts -nonewline "{"
	puts -nonewline [join $strlist ", "]
	if {$i < [llength $collection]} {
		puts -nonewline " ... "
	}
	puts "}"
}

set ::clackwise_commands(help) {
	{Print help for commands}
	{pattern1 [pattern2 ...]}
	{
		{verbose "Print verbose help"}
	}
}
proc help {args} {
	set ::argv0 "help"
	set summary [lindex $::clackwise_commands($::argv0) 0]
	set usage [lindex $::clackwise_commands($::argv0) 1]
	set options [lindex $::clackwise_commands($::argv0) 2]
	set params(verbose) 0
	array set params [::cmdline::getoptions args $options "$usage # $summary"]
	if {[info exists params(__NON_SWITCH_ARGS__)] && [llength $params(__NON_SWITCH_ARGS__)] > 0} {
		set commands [array names ::clackwise_commands]
		foreach pattern $params(__NON_SWITCH_ARGS__) {
			foreach i [lsearch -glob -all $commands $pattern] {
				set command [lindex $commands $i]
				set s [lindex $::clackwise_commands($command) 0]
				set u [lindex $::clackwise_commands($command) 1]
				set o [lindex $::clackwise_commands($command) 2]
				if {$params(verbose)} {
					set saveargv $::argv0
					set ::argv0 $command
					puts [::cmdline::usage $o $u]
					set ::argv0 $saveargv
				} else {
					set spaces [string repeat " " [expr 25 - [string length $command]]]
					puts "$command$spaces# $s"
				}
			}
		}
	} else {
		puts "Try 'help *'"; #TODO: Print help for commonly used commands
	}
}

set ::clackwise_commands(get_libs) {
	{Get libs from memory}
	{pattern}
	{
		{regexp "Match pattern as regular expression"}
		{exact "Match pattern as exact string"}
	}
}
proc get_libs {args} {
	set ::argv0 "get_libs"
	set summary [lindex $::clackwise_commands($::argv0) 0]
	set usage [lindex $::clackwise_commands($::argv0) 1]
	set options [lindex $::clackwise_commands($::argv0) 2]
	array set params [::cmdline::getoptions args $options "$usage # $summary"]
	set QRegExp_Type $::QRegExp_Wildcard
	if {$params(regexp)} {
		set QRegExp_Type $::QRegExp_RegExp
	} elseif {$params(exact)} {
		set QRegExp_Type $::QRegExp_FixedString
	}
	if {[info exists params(__NON_SWITCH_ARGS__)] && [llength $params(__NON_SWITCH_ARGS__)] > 0} {
		set paramcount [llength $params(__NON_SWITCH_ARGS__)]
		set pattern [lindex $params(__NON_SWITCH_ARGS__) 0]
		if {$paramcount == 1} {
			return [cw_get_libs $pattern $QRegExp_Type]
		} elseif {$paramcount == 2} {
			error "Error: That's one pattern too many than what I can handle"
		} else {
			error "Error: That's [expr $paramcount - 1] patterns too many than what I can handle"
		}
	} else {
		error "Error: $::argv0: No patterns specified";
		return {};
	}
	return {};
}

