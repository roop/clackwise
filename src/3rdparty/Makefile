INSTALL_DIR = ../../../../release

all: $(INSTALL_DIR)/bin/cw_shell

$(INSTALL_DIR)/bin/cw_shell:
	cd tcl/unix && ./configure --prefix $(INSTALL_DIR) && make && make install && mv $(INSTALL_DIR)/bin/tclsh8.4 $(INSTALL_DIR)/bin/cw_shell_exec
	echo 'LD_LIBRARY_PATH=`dirname $$0`/../lib `dirname $$0`/cw_shell_exec' > tcl/unix/$(INSTALL_DIR)/bin/cw_shell

clean:
	cd tcl/unix && make clean && rm -rf $(INSTALL_DIR)/release/bin/* $(INSTALL_DIR)/release/lib/tcl*

