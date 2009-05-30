namespace eval tclreadline {

	proc cw_completer {part start end line} {
		if {$start == 0} {
			# complete the command
			set commands [array names ::clackwise_commands]
			return [cw_expand_part $part $commands]
		} elseif {[string range $part 0 0] == "-"} {
			# complete the option
			set command [string range $line 0 [expr [string wordend $line 0] - 1]]
			set options {}
			foreach optstr [lindex $::clackwise_commands($command) 2] {
				set option [regsub {.arg$} [lindex $optstr 0] ""]
				lappend options "-[lindex $option 0]"
			}
			return [cw_expand_part $part $options]
		}
		return "";
	}

	proc cw_expand_part {part sample_space} {
		set matches {}
		set common ""
		set firsttime 1
		foreach i [lsearch -regexp -all $sample_space ^$part] {
			set match [lindex $sample_space $i]
			set matchlen [string length $match]
			if {$firsttime} {
				set common "$match "
				set firsttime 0
			} else {
				for {set j 0} {$j < $matchlen} {incr j} {
					if {[string index $match $j] != [string index $common $j]} {
						break
					}
				}
				set common [string range $common 0 [expr $j - 1]]
			}
			lappend matches $match
		}
		return [linsert $matches 0 $common]
	}

}

