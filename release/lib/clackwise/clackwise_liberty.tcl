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

