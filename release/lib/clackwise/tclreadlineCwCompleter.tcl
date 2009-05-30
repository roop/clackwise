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

