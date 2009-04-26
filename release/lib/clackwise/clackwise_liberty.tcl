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
