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
    virtual bool init() { return true; }
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
    virtual void status(void* status = NULL) {
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

typedef struct {
    const char* name;
    uint8_t version;
    uint8_t conntest;
    uint16_t current;
    uint16_t microsteps;
    int8_t stallguard_thr;
    uint16_t stallguard_result;
    bool ot, otpw;
    uint8_t csact;
} TMC2130DriverStatus;

template <class stepCls, class dirCls, class enableCls>
class TMC2130StepperDriver : public StepperDriverBase {
public:
    TMC2130StepperDriver(const char* label, uint16_t coolStMin, EndstopDriver* minES, EndstopDriver* maxES, uint16_t csPin)
        : StepperDriverBase(minES, maxES)
        , driver(TMC2130Stepper(csPin))
        , name(label)
        , coolstep_sp_min(coolStMin) {}
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
    inline bool init() final {
        Com::printF(PSTR("TMC2130 init "), this->name);
        disable();
        driver.begin(); // Initiate pins and registers
        driver.test_connection();
        if (driver.test_connection() != 0) {
            Com::printFLN(PSTR(": SPI error"));
            return false;
        }
        Com::printFLN(PSTR(": chip version "), driver.version());
        while (!(driver.stst()))
            ;                          // Wait for motor stand-still
        driver.I_scale_analog(false);  // Set current reference source
        driver.interpolate(true);      // Set internal microstep interpolation
        driver.internal_Rsense(false); // External current sense resistor
        driver.chopper_mode(0);
        driver.off_time(5);
        driver.blank_time(2);
        driver.coolstep_min_speed(coolstep_sp_min);
        driver.sgt(0);                  // Netural Stallguard threshold
        driver.diag1_stall(true);       // DIAG1 pin as stall signal (endstop)
        driver.diag1_active_high(true); // StallGuard pulses active high
        enable();
        return true;
    }
    inline bool implementSetMaxCurrent() { return true; }

    inline void setMicrosteps(uint16_t microsteps) final {
        while (!(driver.stst()))
            ;
        driver.microsteps(microsteps);
    }
    inline void setMotorCurrent(uint16_t current) final {
        while (!(driver.stst()))
            ;
        driver.rms_current(current);
    }
    inline void status(void* s) final {
        if (NULL == s)
            return;
        TMC2130DriverStatus* status = (TMC2130DriverStatus*)s;
        status->name = this->name;
        status->version = driver.version();
        status->conntest = driver.test_connection();
        status->current = driver.rms_current();
        status->microsteps = driver.microsteps();
        status->stallguard_thr = driver.sgt();
        status->stallguard_result = driver.sg_result();
        status->ot = driver.ot();
        status->otpw = driver.otpw();
        status->csact = driver.cs_actual();
    }

    inline void beforeHoming() {
        // backup.GCONF = driver.GCONF() & 0x3FFFF;
        // backup.CHOPCONF = driver.CHOPCONF();
        // backup.COOLCONF = driver.COOLCONF();
        // backup.PWMCONF = driver.PWMCONF();
        // backup.TCOOLTHRS = driver.TCOOLTHRS();
        // backup.TPWMTHRS = driver.TPWMTHRS();
        backup.coolstep_speed = driver.coolstep_min_speed();
        backup.stealth_max_sp = driver.stealth_max_speed();
        backup.stealth_state = driver.stealthChop();
        waitForStandstill();                        // Wait for motor stand-still
        driver.stealth_max_speed(0);                // Upper speedlimit for stealthChop
        driver.stealthChop(false);                  // Turn off stealthChop
        driver.coolstep_min_speed(coolstep_sp_min); // Minimum speed for StallGuard trigerring
        driver.sg_filter(false);                    // Turn off StallGuard filtering
        driver.diag1_stall(true);                   // Signal StallGuard on DIAG1 pin
        driver.diag1_active_high(true);             // StallGuard pulses active high
    }

    inline void afterHoming() {
        // driver.GCONF(backup.GCONF);
        // driver.CHOPCONF(backup.CHOPCONF);
        // driver.COOLCONF(backup.COOLCONF);
        // driver.PWMCONF(backup.PWMCONF);
        // driver.TCOOLTHRS(backup.TCOOLTHRS);
        // driver.TPWMTHRS(backup.TPWMTHRS);
        waitForStandstill();
        driver.coolstep_min_speed(backup.coolstep_speed);
        driver.stealth_max_speed(backup.stealth_max_sp);
        driver.stealthChop(backup.stealth_state);
    }

    inline void setSGT(int8_t sgtVal) {
        waitForStandstill();
        driver.sgt(sgtVal);
    }
    TMC2130Stepper driver;

private:
    struct {
        // uint32_t GCONF;
        // uint32_t CHOPCONF;
        // uint32_t COOLCONF;
        // uint32_t PWMCONF;
        // uint32_t TCOOLTHRS;
        // uint32_t TPWMTHRS;
        uint32_t coolstep_speed;
        uint32_t stealth_max_sp;
        bool stealth_state;
    } backup;
    const char* name PROGMEM;
    uint16_t coolstep_sp_min;

    inline bool waitForStandstill() {
        driver.test_connection();
        if (driver.test_connection() != 0) {
            return false;
        }
        while (!(driver.stst()))
            ;
        return true;
    }
};
