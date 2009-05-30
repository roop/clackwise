proc read_lib {filename} {
    if {[regexp -- {^-h(elp)?$} $filename]} {
        puts "Usage: read_lib filename"
        return;
    }
    if {![file isfile $filename]} {
        puts "Error: No such file - $filename";
        return;
    }
    set lib [cw_read_lib $filename]
    return $lib;
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

set ::clackwise_commands(get_libs) {
	{Get libs from memory}
	{pattern1 [pattern2 ...]}
	{
		{regexp "Match patterns as regular expressions"}
		{exact "Match patterns as exact strings"}
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
		array set ret {}
		foreach pattern $params(__NON_SWITCH_ARGS__) {
			foreach result [cw_get_libs $pattern $QRegExp_Type] {
				array set ret "$result 1"
			}
		}
		return [array names ret];
	} else {
		error "Error: $::argv0: No patterns specified";
		return {};
	}
	return {};
}

