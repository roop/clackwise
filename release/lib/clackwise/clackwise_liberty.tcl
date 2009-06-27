#
# Copyright (c) 2009 Roopesh Chander <roop@forwardbias.in>
#
# This file is part of Clackwise. <http://clackwise.googlecode.com>
#
# Clackwise is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# Clackwise is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# version 2.1 along with Clackwise.  If not, see <http://www.gnu.org/licenses/>.

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

proc _get_lib_groups {type pattern QRegExp_Type {of ""}} {
	set patterns [list $pattern]
	set slashcount_pattern [expr [string length $pattern] - \
							[string length [string map {/ {}} $pattern]]]
	set slashcount_type [expr [string length $type] - \
						 [string length [string map {/ {}} $type]]]
	if {$QRegExp_Type == $::QRegExp_Wildcard} {
		# make the number of slashes in pattern the same as that in the type
		if {$slashcount_pattern > $slashcount_type} {
			return {};
		}
		if {($slashcount_type > 0) && ($slashcount_pattern < $slashcount_type)} {
			array set pattern_arr {}
			set asterisks_to_add [expr $slashcount_type - $slashcount_pattern]
			for {set d 0} {$d <= $asterisks_to_add} {incr d} {
				if {($d > 0) && ([string index $pattern 0] != "*")} {
					continue;
				}
				if {($d < $asterisks_to_add) && ([string index $pattern end] != "*")} {
					continue;
				}
				set prefix [string repeat "*/" $d]
				set suffix [string repeat "/*" [expr $asterisks_to_add - $d]]
				array set pattern_arr [list [join [list $prefix $pattern $suffix] ""] 1]
			}
			set patterns [array names pattern_arr]
		}
	}
	set ret {}
	foreach pat $patterns {
		set scount_pattern [expr [string length $pat] - \
							[string length [string map {/ {}} $pat]]]
		set scount_type [expr [string length $type] - \
						 [string length [string map {/ {}} $type]]]
		set pl [split $pat /]
		set tl [split $type /]
		if {$scount_pattern == $scount_type} {
			set groups $of
			for {set i 0} {$i < [expr $scount_pattern + 1]} {incr i} {
				set t [lindex $tl $i]
				set p [lindex $pl $i]
				if {$groups == ""} {
					if {$t == "lib"} {
						set groups [cw_get_libs $p $QRegExp_Type]
					} else {
						if {$i == 0} {
							error "Error: $::argv0: Expecting -type '$type' to start with 'lib' instead of '$t'"
							return {};
						}
					}
				} else {
					if {$t == ""} {
						error "Error: $::argv0: Null subtype specified in lib group type '$type'"
					}
					set next_groups {}
					foreach g $groups {
						foreach g2 [CwLibGroup_subgroupsByName $g $t $p $QRegExp_Type] {
							lappend next_groups $g2
						}
					}
					set groups $next_groups
				}
			}
			foreach g $groups {
				lappend ret $g
			}
		} else {
			error "Error: $::argv0: Number of '/'s in pattern \"$pattern\" does not match the number of '/'s in type \"$type\". Cannot perform -regexp/-exact match."
		}
	}
	return $ret
}

set ::clackwise_commands(get_lib_groups) {
	{Get lib groups from memory}
	{pattern}
	{
		{type.arg "no default" "Lib group type (eg. lib/cell, lib/cell/pin/timing)"}
		{of_objects.arg "" "Lib group of which object"}
		{regexp "Match pattern as regular expression"}
		{exact "Match pattern as exact string"}
	}
}
proc get_lib_groups {args} {
	set ::argv0 "get_lib_groups"
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
	if {$params(type) == "no default"} {
		error "Error: $::argv0: -type is required"
	}
	set pattern ""
	if {[info exists params(__NON_SWITCH_ARGS__)] && [llength $params(__NON_SWITCH_ARGS__)] > 0} {
		set paramcount [llength $params(__NON_SWITCH_ARGS__)]
		set pattern [lindex $params(__NON_SWITCH_ARGS__) 0]
		if {$paramcount == 1} {
			return [_get_lib_groups $params(type) $pattern $QRegExp_Type $params(of_objects)]
		} elseif {$paramcount == 2} {
			error "Error: $::argv0: That's one pattern too many than what I can handle"
		} else {
			error "Error: $::argv0: That's [expr $paramcount - 1] patterns too many than what I can handle"
		}
	} else {
		error "Error: $::argv0: No patterns specified";
	}
	return {};
}

set ::clackwise_commands(get_lib_cells) {
	{Get lib cells from memory}
	{pattern}
	{
		{of_objects.arg "" "Cells of which lib"}
		{regexp "Match pattern as regular expression"}
		{exact "Match pattern as exact string"}
	}
}
proc get_lib_cells {args} {
	set ::argv0 "get_lib_cells"
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
	set pattern ""
	if {[info exists params(__NON_SWITCH_ARGS__)] && [llength $params(__NON_SWITCH_ARGS__)] > 0} {
		set paramcount [llength $params(__NON_SWITCH_ARGS__)]
		set pattern [lindex $params(__NON_SWITCH_ARGS__) 0]
		if {$paramcount == 1} {
			if {$params(of_objects) == ""} {
				return [_get_lib_groups "lib/cell" $pattern $QRegExp_Type]
			} else {
				set of_objects_type [CwLibGroup_type [lindex $params(of_objects) 0]]
				if {$of_objects_type == "library"} {
					return [_get_lib_groups "cell" $pattern $QRegExp_Type $params(of_objects)]
				} elseif {$of_objects_type == "pin"} {
					set parents {}
					foreach p $params(of_objects) {
						set parent [lindex [CwLibGroup_parents $p] 0]
						if {[CwLibGroup_type $parent] == "cell"} {
							lappend parents $parent
						}
					}
					return $parents
				} else {
					error "Error: $::argv0: Expected object of type 'library' or 'pin' but got '$of_objects_type' for -of_objects";
				}
			}
		} elseif {$paramcount == 2} {
			error "Error: $::argv0: That's one pattern too many than what I can handle"
		} else {
			error "Error: $::argv0: That's [expr $paramcount - 1] patterns too many than what I can handle"
		}
	} else {
		error "Error: $::argv0: No patterns specified";
	}
	return {};
}
