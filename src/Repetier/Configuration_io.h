/*

This file defines io solutions used. This is the lowest level and is the base
for all higher level functions using io operations. At several places we need
subsets of these list of operations. To make configuration easy and easy to 
understand, we use a technique called "x macros". This requires that only
predefined macro names for IO are used here. Do NOT add anything else here
or compilation/functionality will break.

Rules:
1. Each definition will create a class that is named like the first parameter.
This class is later used as input to templates building higher functions. By
convention the names should start with IO followed by something that helps you
identify the function. 
2. Do not use a semicolon at the end. Macro definition gets different meanings
and will add the semicolon if required.

*/

/* Define motor pins here. Each motor needs a setp, dir and enable pin. */

ENDSTOP_NONE(endstopNone)

// X Motor

IO_OUTPUT(IOX1Step, ORIG_X_STEP_PIN)
IO_OUTPUT(IOX1Dir, ORIG_X_DIR_PIN)
IO_OUTPUT_INVERTED(IOX1Enable, ORIG_X_ENABLE_PIN)

// Y Motor

IO_OUTPUT(IOY1Step, ORIG_Y_STEP_PIN)
IO_OUTPUT(IOY1Dir, ORIG_Y_DIR_PIN)
IO_OUTPUT_INVERTED(IOY1Enable, ORIG_Y_ENABLE_PIN)

// Z Motor

IO_OUTPUT(IOZ1Step, ORIG_Z_STEP_PIN)
IO_OUTPUT_INVERTED(IOZ1Dir, ORIG_Z_DIR_PIN)
IO_OUTPUT_INVERTED(IOZ1Enable, ORIG_Z_ENABLE_PIN)

// // E0 Motor

// IO_OUTPUT(IOE0Step, ORIG_E0_STEP_PIN)
// IO_OUTPUT(IOE0Dir, ORIG_E0_DIR_PIN)
// IO_OUTPUT_INVERTED(IOE0Enable, ORIG_E0_ENABLE_PIN)

// E1 Motor

IO_OUTPUT(IOE1Step, ORIG_E1_STEP_PIN)
IO_OUTPUT(IOE1Dir, ORIG_E1_DIR_PIN)
IO_OUTPUT_INVERTED(IOE1Enable, ORIG_E1_ENABLE_PIN)

// Define your endstops inputs

IO_INPUT(IOEndstopXMax, ORIG_X_MAX_PIN)
IO_INPUT(IOEndstopYMax, ORIG_Y_MAX_PIN)
IO_INPUT(IOEndstopZMax, ORIG_Z_MAX_PIN)

// Define our endstops solutions
// You need to define all min and max endstops for all
// axes except E even if you have none!

ENDSTOP_NONE(endstopXMin)
ENDSTOP_SWITCH_HW(endstopAMax, IOEndstopXMax, X_AXIS)
ENDSTOP_NONE(endstopYMin)
ENDSTOP_SWITCH_HW(endstopBMax, IOEndstopYMax, Y_AXIS)
ENDSTOP_NONE(endstopZMin)
ENDSTOP_SWITCH_HW(endstopZMax, IOEndstopZMax, Z_AXIS)

ENDSTOP_MERGE2(endstopXMax, endstopAMax, endstopBMax)
ENDSTOP_MERGE2(endstopYMax, endstopAMax, endstopBMax)

// Define fans

IO_OUTPUT(IOFan1, ORIG_FAN_PIN)
IO_OUTPUT(IOFan2, ORIG_FAN2_PIN)
IO_PWM_SOFTWARE(Fan1NoKSPWM, IOFan1, 0)
IO_PWM_SOFTWARE(Fan2NoKSPWM, IOFan2, 0)
// IO_PWM_HARDWARE(Fan1PWM, 37,5000)
// IO_PDM_SOFTWARE(Fan1NoKSPWM, IOFan1) // alternative to PWM signals
IO_PWM_KICKSTART(Fan1PWM, Fan1NoKSPWM, 20)
IO_PWM_KICKSTART(Fan2PWM, Fan2NoKSPWM, 20)

// Define temperature sensors

// Typically they require an analog input (12 bit) so define
// them first.

IO_ANALOG_INPUT(IOAnalogBed1, TEMP_1_PIN, 5)
IO_ANALOG_INPUT(IOAnalogExt1, TEMP_0_PIN, 5)
// IO_ANALOG_INPUT(IOAnalogExt2, TEMP_2_PIN, 5)

// Need a conversion table for epcos NTC
IO_TEMP_TABLE_NTC(TempTableEpcos, Epcos_B57560G0107F000)

// Now create the temperature inputs

IO_TEMPERATURE_TABLE(TempBed1, IOAnalogBed1, TempTableEpcos)
IO_TEMPERATURE_TABLE(TempExt1, IOAnalogExt1, TempTableEpcos)
// IO_TEMPERATURE_TABLE(TempExt2, IOAnalogExt2, TempTableEpcos)

// Use PWM outputs to heat. If using hardware PWM make sure
// that the selected pin can be used as hardware pwm otherwise
// select a software pwm model whcih works on all pins.

#if MOTHERBOARD == 405
IO_PWM_HARDWARE(PWMExtruder1, HEATER_0_PIN, 1000)
// IO_PWM_HARDWARE(PWMExtruder2, HEATER_2_PIN, 1000)
IO_PWM_HARDWARE(PWMBed1, HEATER_1_PIN, 1000)
#else
IO_OUTPUT(IOExtr1, HEATER_0_PIN)
// IO_OUTPUT(IOExtr2, HEATER_2_PIN)
IO_OUTPUT(IOBed1, HEATER_1_PIN)
IO_PWM_SOFTWARE(PWMExtruder1, IOExtr1, 1)
// IO_PWM_SOFTWARE(PWMExtruder2, IOExtr2, 1)
IO_PWM_SOFTWARE(PWMBed1, IOBed1, 1)
#endif
// IO_OUTPUT(IOCooler1, FAN2_PIN)
// IO_PWM_SOFTWARE(PWMCoolerExt1, FAN2_PIN, 0)

// Define all stepper motors used
// STEPPER_SIMPLE(XMotor, IOX1Step, IOX1Dir, IOX1Enable, endstopNone, endstopNone)
// STEPPER_SIMPLE(YMotor, IOY1Step, IOY1Dir, IOY1Enable, endstopNone, endstopNone)
// STEPPER_SIMPLE(ZMotor, IOZ1Step, IOZ1Dir, IOZ1Enable, endstopNone, endstopNone)
// STEPPER_SIMPLE(E1Motor, IOE1Step, IOE1Dir, IOE1Enable, endstopNone, endstopNone)
// STEPPER_SIMPLE(E2Motor, IOE2Step, IOE2Dir, IOE2Enable, endstopNone, endstopNone)

STEPPER_TMC2130(XMotor, TMC2130_TCOOLTHRS_X, IOX1Step, IOX1Dir, IOX1Enable, endstopXMin, endstopXMax, ORIG_X_CS_PIN)
STEPPER_TMC2130(YMotor, TMC2130_TCOOLTHRS_Y, IOY1Step, IOY1Dir, IOY1Enable, endstopYMin, endstopYMax, ORIG_Y_CS_PIN)
STEPPER_TMC2130(ZMotor, TMC2130_TCOOLTHRS_Z, IOZ1Step, IOZ1Dir, IOZ1Enable, endstopZMin, endstopZMax, ORIG_Z_CS_PIN)
STEPPER_TMC2130(E1Motor, 100, IOE1Step, IOE1Dir, IOE1Enable, endstopNone, endstopNone, ORIG_E1_CS_PIN)

// Heat manages are used for every component that needs to
// control temperature. Higher level classes take these as input
// and simple heater like a heated bed use it directly.

HEAT_MANAGER_PID('B', HeatedBed1, TempBed1, PWMBed1, 120, 255, 5, 30000, 12.0, 33.0, 290.0, 80, 255)
HEAT_MANAGER_PID('E', HeaterExtruder1, TempExt1, PWMExtruder1, 260, 255, 10, 20000, 20.0, 0.6, 65.0, 40, 220)
// HEAT_MANAGER_PID('E', HeaterExtruder2, TempExt2, PWMExtruder2, 260, 255, 10, 20000, 20.0, 0.6, 65.0, 40, 220)

// Coolers are stand alone functions that allow it to control
// a fan with external sensors. Many extruders require a cooling
// fan pointer to the extruder to prevent heat rising up.
// These can be controlled by the cooler. Since it is
// independent you just tell what part needs cooling.
// Other use cases are board cooling and heated chambers.

// Define tools. They get inserted into a tool array in configuration.h
// Typical tools are:
// TOOL_EXTRUDER(name, offx, offy, offz, heater, stepper, resolution, yank, maxSpeed, acceleration, advance, startScript, endScript)

TOOL_EXTRUDER(ToolExtruder1, 0, 0, 0, HeaterExtruder1, E1Motor, 1.75, 147.0, 5, 30, 5000, 40, "M117 Extruder 1", "")
// TOOL_EXTRUDER(ToolExtruder2, 16.775, 0.615, -0.97, HeaterExtruder2, E2Motor, 1.75, 147.0, 5, 30, 5000, 40, "M117 Extruder 2\nM400\nM340 P0 S1950 R600\nG4 P300", "M340 P0 S1050 R600\nG4 P300")
