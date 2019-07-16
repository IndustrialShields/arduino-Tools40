/*
   Copyright (c) 2018 Boot&Work Corp., S.L. All rights reserved

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Counter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
Counter::Counter(uint32_t preset) {
	_value = 0UL;
	_preset = preset;
	_lastUp = LOW;
	_lastDown = LOW;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int Counter::update(int up, int down, int reset) {
	if ((up == HIGH) && (_lastUp == LOW)) {
		++_value;
	}
	_lastUp = up;

	if ((down == HIGH) && (_lastDown == LOW)) {
		--_value;
	}
	_lastDown = down;

	if (reset == HIGH) {
		_value = 0UL;
	}

	return _value == _preset ? HIGH : LOW;
}
