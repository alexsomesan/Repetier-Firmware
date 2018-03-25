#include <TMC2130Stepper.h>
class EndstopDriver;

class StepperDriverBase {
public:
    StepperDriverBase(EndstopDriver* minES, EndstopDriver* maxES)
        : minEndstop(minES)
        , maxEndstop(maxES)
        , direction(true) {}
    virtual ~StepperDriverBase() {}
    inline EndstopDriver* getMinEndstop() { return minEndstop; }
    inline EndstopDriver* getMaxEndstop() { return maxEndstop; }
    /// Allows initialization of driver e.g. current, microsteps
    virtual void init() {
        Com::printFLN(PSTR("StepperDriverBase initialization"));
    }
    /// Executes the step if endstop is not triggered. Return tru eif endstop is triggered
    virtual bool stepCond() = 0;
    /// Always executes the step
    virtual void step() = 0;
    /// Set step signal low
    virtual void unstep() = 0;
    /// Set direction, true = max direction
    virtual void dir(bool d) = 0;
    /// Enable motor driver
    virtual void enable() = 0;
    /// Disable motor driver
    virtual void disable() = 0;
    // Return true if setting microsteps is supported
    virtual bool implementSetMicrosteps() { return false; }
    // Return true if setting current in software is supported
    virtual bool implementSetMaxCurrent() { return false; }
    /// Set microsteps. Must be a power of 2.
    virtual void setMicrosteps(uint16_t) {}
    /// Set max current as range 0..65536
    virtual void setMotorCurrent(uint16_t) {}
    /// Set current as percentage of maximum 0..100
    virtual void setMotorCurrentPercent(uint8_t) {}
    // Called before homing starts. Can be used e.g. to disable silent mode
    // or otherwise prepare for endstop detection.
    virtual void beforeHoming() {}
    virtual void afterHoming() {}
    virtual void status() {
        Com::printFLN(PSTR("not implemented"));
    }
    EndstopDriver* minEndstop;
    EndstopDriver* maxEndstop;
    bool direction;
    // uint32_t position;
};

/// Plain stepper driver with optional endstops attached.
template <class stepCls, class dirCls, class enableCls>
class SimpleStepperDriver : public StepperDriverBase {
public:
    SimpleStepperDriver(EndstopDriver* minES, EndstopDriver* maxES)
        : StepperDriverBase(minES, maxES) {}
    inline bool stepCond() final {
        if (direction) {
            if (!maxEndstop->update()) {
                stepCls::on();
                return false;
            }
        } else {
            if (!minEndstop->update()) {
                stepCls::on();
                return false;
            }
        }
        return true;
    }
    inline void step() final {
        stepCls::on();
    }
    inline void unstep() final {
        stepCls::off();
    }
    inline void dir(bool d) final {
        dirCls::set(d);
        direction = d;
    }
    inline void enable() final {
        enableCls::on();
    }
    inline void disable() final {
        enableCls::off();
    }
};

/// TMC2130 stepper driver with SPI configuration.
template <class stepCls, class dirCls, class enableCls>
class TMC2130StepperDriver : public StepperDriverBase {
public:
    TMC2130StepperDriver(EndstopDriver* minES, EndstopDriver* maxES, uint16_t csPin)
        : StepperDriverBase(minES, maxES)
        , driver(new TMC2130Stepper(csPin)) {}
    inline bool stepCond() final {
        if (direction) {
            if (!maxEndstop->update()) {
                stepCls::on();
                return false;
            }
        } else {
            if (!minEndstop->update()) {
                stepCls::on();
                return false;
            }
        }
        return true;
    }
    inline void step() final {
        stepCls::on();
    }
    inline void unstep() final {
        stepCls::off();
    }
    inline void dir(bool d) final {
        dirCls::set(d);
        direction = d;
    }
    inline void enable() final {
        enableCls::on();
    }
    inline void disable() final {
        enableCls::off();
    }
    inline void init() final {
        Com::printFLN(PSTR("TMC2130 initialization"));
        while (!(driver->stst()))
            ;                         // Wait for motor stand-still
        driver->begin();              // Initiate pins and registeries
        driver->I_scale_analog(true); // Set current reference source
        driver->interpolate(false);    // Set internal microstep interpolation
        driver->internal_Rsense(false);
        driver->rms_current(500);
        driver->microsteps(16);
        // driver->pwm_ampl(tmc_pwm_ampl);           // Chopper PWM amplitude
        // driver->pwm_grad(tmc_pwm_grad);           // Velocity gradient for chopper PWM amplitude
        // driver->pwm_autoscale(tmc_pwm_autoscale); // Chopper PWM autoscaling
        // driver->pwm_freq(tmc_pwm_freq);           // Chopper PWM frequency selection
        // driver->stealthChop(stealthchop);         // Enable extremely quiet stepping
        // driver->sg_stall_value(sgt);              // StallGuard sensitivity
    }
    inline bool implementSetMaxCurrent() { return true; }

    inline void setMicrosteps(uint16_t microsteps) final {
        while (!(driver->stst()))
            ;
        driver->microsteps(microsteps);
    }
    inline void setMotorCurrent(uint16_t current) final {
        while (!(driver->stst()))
            ;
        driver->rms_current(current);
    }
    inline void status() final {
        Com::printFLN(PSTR("TMC2130 driver version "), driver->version());
        Com::printFLN(PSTR("\tRMS current "), driver->rms_current());
        Com::printFLN(PSTR("\tMicrosteps "), driver->microsteps());
        Com::printFLN(PSTR("\tStallguard value "),driver->sg_result());
        Com::printFLN(PSTR("\tOver temperature "), driver->ot());
        Com::printFLN(PSTR("\tOver temperature prewarn "), driver->otpw());
    }

private:
    TMC2130Stepper* driver;
};
