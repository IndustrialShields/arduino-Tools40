/*
   Copyright (c) 2017 Boot&Work Corp., S.L. All rights reserved

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

#include "Timer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
Timer::Timer(uint32_t presetTime) {
	_presetTime = presetTime;
	_lastTime = 0UL;
	_lastIn = LOW;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
PulseTimer::PulseTimer(uint32_t presetTime) : Timer(presetTime) {
	_running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int PulseTimer::update(int in) {
	if (!_running) {
		if ((_lastIn == LOW) && (in == HIGH)) {
			_lastTime = millis();
			_running = true;
		}
	} else {
		if (millis() - _lastTime > _presetTime) {
			_running = false;
		}
	}

	_lastIn = in;

	return _running ? HIGH : LOW;
}
