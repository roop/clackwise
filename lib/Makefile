
# Copyright (c) 2008, 2009 Roopesh Chander <roop@forwardbias.in>
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


all: bin/cwlib

tmp:
	mkdir -p tmp/

bin:
	mkdir -p bin/

tmp/cwlib.yy.c: cwlib.l cwlib.h tmp/cwlib.bison.hh
	flex  -o tmp/cwlib.yy.c --prefix cwlib_yy cwlib.l

tmp/cwlib.bison.hh: cwlib.y cwlib.h tmp
	bison -d  cwlib.y -o tmp/cwlib.bison.cc -p cwlib_yy --debug  --verbose
tmp/cwlib.bison.cc: tmp/cwlib.bison.hh

bin/cwlib: tmp/cwlib.bison.cc tmp/cwlib.yy.c main.cpp LibData.cpp bin
	g++ -g -Itmp -I. tmp/cwlib.bison.cc tmp/cwlib.yy.c main.cpp LibData.cpp -o bin/cwlib

clean:
	rm -rf tmp/* bin/*
