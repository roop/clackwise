INSTALL_DIR = ../../../../release

all: $(INSTALL_DIR)/bin/cw_shell

$(INSTALL_DIR)/bin/cw_shell:
	cd tcl/unix && ./configure --prefix $(INSTALL_DIR) --bindir $(INSTALL_DIR) && make && make install && mv $(INSTALL_DIR)/tclsh8.4 $(INSTALL_DIR)/cw_shell_exec
	echo 'LD_LIBRARY_PATH=`dirname $$0`/lib TCL_LIBRARY=`dirname $$0`/lib/tcl8.4 `dirname $$0`/cw_shell_exec $$@' > tcl/unix/$(INSTALL_DIR)/cw_shell
	chmod a+x tcl/unix/$(INSTALL_DIR)/cw_shell

clean:
	cd tcl/unix && make clean && rm -rf $(INSTALL_DIR)/release/* $(INSTALL_DIR)/release/lib/tcl*

