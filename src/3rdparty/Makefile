all: tcl/install

tcl/install:
	cd tcl/unix && ./configure --prefix ../install && make && make install

clean:
	cd tcl/unix && make clean && rm -rf tcl/install

