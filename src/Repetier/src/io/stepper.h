/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

*/

#undef STEPPER_SIMPLE
#undef STEPPER_TMC2130

#if IO_TARGET == 4 // declare variable

#define STEPPER_SIMPLE(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop) \
    extern SimpleStepperDriver<stepPin, dirPin, enablePin> name;

#define STEPPER_TMC2130(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop, csPin) \
    extern TMC2130StepperDriver<stepPin, dirPin, enablePin> name;

#elif IO_TARGET == 6

#define STEPPER_SIMPLE(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop) \
    SimpleStepperDriver<stepPin, dirPin, enablePin> name(&minEndstop, &maxEndstop);

#define STEPPER_TMC2130(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop, csPin) \
    TMC2130StepperDriver<stepPin, dirPin, enablePin> name(#name, &minEndstop, &maxEndstop, csPin);

#elif IO_TARGET == 1 // Init drivers at startup

#define STEPPER_SIMPLE(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop)
#define STEPPER_TMC2130(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop, csPin) \
    name.init();

#elif IO_TARGET == 8 // call eepromHandle if required

#define STEPPER_SIMPLE(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop)
#define STEPPER_TMC2130(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop, csPin)

#elif IO_TARGET == 9 // call updatedDerived to activate new settings

#define STEPPER_SIMPLE(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop)
#define STEPPER_TMC2130(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop, csPin)

#else

#define STEPPER_SIMPLE(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop)
#define STEPPER_TMC2130(name, stepPin, dirPin, enablePin, minEndstop, maxEndstop, csPin)

#endif
