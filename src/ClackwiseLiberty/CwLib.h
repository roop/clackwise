/*

Copyright (c) 2008, 2009 Roopesh Chander <roop@forwardbias.in>

This file is part of Clackwise. <http://clackwise.googlecode.com>

Clackwise is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

Clackwise is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
version 2.1 along with Clackwise.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __DOTLIB_H
#define __DOTLIB_H

#include "Liberty/CwLibGroup.h"

namespace Clackwise {

class CwLib
	: public CwLibGroup
{
public:
	CwLib();
	CwLib(const QString& name);
	~CwLib();
	CwLib(const CwLib &other);
	CwLib& operator=(const CwLib & other);
	bool read(const QString& filename);
	bool write(const QString& filename);
};

}
#endif
