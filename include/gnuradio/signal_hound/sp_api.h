// Copyright (c).2023, Signal Hound, Inc.
// For licensing information, please see the API license in the software_licenses folder

/*!
 * \file sp_api.h
 * \brief API functions for the SP145 spectrum analyzer.
 *
 * This is the main file for user-accessible functions for controlling the
 * SP145 spectrum analyzer.
 *
 */

#ifndef SP_API_H
#define SP_API_H

#if defined(_WIN32) // Windows
    #ifdef SP_EXPORTS
        #define SP_API __declspec(dllexport)
    #else
        #define SP_API
    #endif

    // bare minimum stdint typedef support
    #if _MSC_VER < 1700 // For VS2010 or earlier
        typedef signed char        int8_t;
        typedef short              int16_t;
        typedef int                int32_t;
        typedef long long          int64_t;
        typedef unsigned char      uint8_t;
        typedef unsigned short     uint16_t;
        typedef unsigned int       uint32_t;
        typedef unsigned long long uint64_t;
    #else
        #include <stdint.h>
    #endif

    #define SP_DEPRECATED(comment) __declspec(deprecated(comment))
#else // Linux
    #include <stdint.h>
    #define SP_API __attribute__((visibility("default")))

    #if defined(__GNUC__)
        #define SP_DEPRECATED(comment) __attribute__((deprecated))
    #else
        #define SP_DEPRECATED(comment) comment
    #endif
#endif

 /** Used for boolean true when integer parameters are being used. Also see #SpBool. */
#define SP_TRUE (1)
/** Used for boolean false when integer parameters are being used. Also see #SpBool. */
#define SP_FALSE (0)

/** Max number of devices that can be interfaced in the API. */
#define SP_MAX_DEVICES (9)

/** Maximum reference level in dBm */
#define SP_MAX_REF_LEVEL (20.0)
/** Tells the API to automatically choose attenuation based on reference level. */
#define SP_AUTO_ATTEN (-1)
/** Valid atten values [0,6] or -1 for auto */
#define SP_MAX_ATTEN (6)

/** Min frequency for sweeps, and min center frequency for I/Q measurements. */
#define SP_MIN_FREQ (9.0e3)
/** Max frequency for sweeps, and max center frequency for I/Q measurements. */
#define SP_MAX_FREQ (15.0e9)

/** Min sweep time in seconds. See #spSetSweepCoupling. */
#define SP_MIN_SWEEP_TIME (1.0e-6)
/** Max sweep time in seconds. See #spSetSweepCoupling. */
#define SP_MAX_SWEEP_TIME (100.0)

/** Min span for device configured in real-time measurement mode */
#define SP_REAL_TIME_MIN_SPAN (200.0e3)
/** Max span for device configured in real-time measurement mode */
#define SP_REAL_TIME_MAX_SPAN (40.0e6)
/** Min RBW for device configured in real-time measurement mode */
#define SP_REAL_TIME_MIN_RBW (2.0e3)
/** Max RBW for device configured in real-time measurement mode */
#define SP_REAL_TIME_MAX_RBW (1.0e6)

/** Max decimation for I/Q streaming. */
#define SP_MAX_IQ_DECIMATION (8192)

/** Maximum number of definable steps in a I/Q sweep */
#define SP_MAX_IQ_SWEEP_STEPS (1000)

/** Minimum fan set point in Celcius */
#define SP_MIN_FAN_SET_POINT (0.0)
/** Maximum fan set point in Celcius */
#define SP_MAX_FAN_SET_POINT (60.0)

/** 
 * Maximum number of I/Q sweeps that can be queued up. 
 * Valid sweep indices between [0,15].
 * See @ref iqSweepList
 */
#define SP_MAX_SWEEP_QUEUE_SZ (16)

/**
 * Status code returned from all SP API functions.
 */
typedef enum SpStatus {
    /** Internal use only. */
    spInternalFlashErr = -101,
    /** Internal use only. */
    spInternalFileIOErr = -100,

    /** Context sensitive GPS error. */
    spGPSErr = -12,
    /**
     * Unable to allocate memory for the measurement. Out of system memory.
     */
    spAllocationErr = -11,
    /** At maximum number of devices that can be interfaced. */
    spMaxDevicesConnectedErr = -10,
    /**
     * Often the result of trying to perform an action while the device is currently
     * making a measurement or not in an idle state, or performing an action
     * that is not supported by the current mode of operation. For instance,
     * requesting a sweep while the device is configured for I/Q streaming.
     */
    spInvalidConfigurationErr = -9,
    /**
     * For standard sweeps, this error indicates another sweep is already being
     * performed. You might encounter this in a multi-threaded application. For
     * queued sweep lists, this indicates the sweep at the given position is
     * already active. Finish this sweep before starting again.
     */
    spSweepAlreadyActiveErr = -8,
    /** Boot error */
    spBootErr = -7,
    /**
     * Indicates USB data framing issues. Data may be corrupt.
     * If error persists, reconfiguration/cycling might be required.
    */
    spFramingErr = -6,
    /**
     * Device disconnected. Will require the device to be closed and reopened
     * to continue. Most likely cause is large USB data loss or cable connectivity
     * issues.
     */
    spConnectionLostErr = -5,
    /** Invalid device handle specified. */
    spInvalidDeviceErr = -4,
    /** One or more required pointer parameters are null. */
    spNullPtrErr = -3,
    /** One or more required parameters found to have an invalid value. */
    spInvalidParameterErr = -2,
    /** 
     * Unable to open device.
     * Verify the device is connected and the LED is solid green.
     */
    spDeviceNotFoundErr = -1,

    /** Function returned successfully. */
    spNoError = 0,

    /** One or more of the provided parameters were clamped to a valid range. */
    spSettingClamped = 1,
    /** 
     * Temperature drift occured since last configuration.
     * Measurements might be uncalibrated. Reconfiguring the device when possible will
     * eliminate this warning.
     */
    spTempDrift = 2,
    /** Measurement includes data which caused an ADC overload (clipping/compression) */
    spADCOverflow = 3,
    /** Measurement is uncalibrated, overrides ADC overflow */
    spUncalData = 4,
    /** Returned when the API was unable to keep up with the necessary processing */
    spCPULimited = 5,
    /** 
     * This warning is returned when recoverable USB data loss occurred. The measurement
     * may be uncalibrated due to the data loss and may need to be ignored. The device
     * can continue to be used without any action/intervention. This warning can appear 
     * in periods of high CPU or USB load. 
     */
    spInvalidCalData = 6
} SpStatus;

/**
 * Boolean type. Used in public facing functions instead of `bool` to improve
 * API use from different programming languages.
 */
typedef enum SpBool {
    /** False */
    spFalse = 0,
    /** True */
    spTrue = 1
} SpBool;

/**
 * Specifies device power state. See @ref powerStates for more information.
 */
typedef enum SpPowerState {
    /** On */
    spPowerStateOn = 0,
    /** Standby */
    spPowerStateStandby = 1
} SpPowerState;

/**
 * Measurement mode
 */
typedef enum SpMode {
    /** Idle, no measurement active */
    spModeIdle = 0,
    /** Swept spectrum analysis */
    spModeSweeping = 1,
    /** Real-time spectrum analysis */
    spModeRealTime = 2,
    /** I/Q streaming */
    spModeIQStreaming = 3,
    /** I/Q sweep list / frequency hopping */
    spModeIQSweepList = 4,
    /** Audio demod */
    spModeAudio = 5,
} SpMode;

/**
 * Detector used for sweep and real-time spectrum analysis.
 */
typedef enum SpDetector {
    /** Average */
    spDetectorAverage = 0,
    /** Min/Max */
    spDetectorMinMax = 1
} SpDetector;

/**
 * Specifies units of sweep and real-time spectrum analysis measurements.
 */
typedef enum SpScale {
    /** dBm */
    spScaleLog = 0,
    /** mV */
    spScaleLin = 1,
    /** Log scale, no corrections */
    spScaleFullScale = 2
} SpScale;

/**
 * Specifies units in which VBW processing occurs for swept analysis.
 */
typedef enum SpVideoUnits {
    /** dBm */
    spVideoLog = 0,
    /** Linear voltage */
    spVideoVoltage = 1,
    /** Linear power */
    spVideoPower = 2,
    /** No VBW processing */
    spVideoSample = 3
} SpVideoUnits;

/**
 * Specifies the window used for sweep and real-time analysis.
 */
typedef enum SpWindowType {
    /** SRS flattop */
    spWindowFlatTop = 0,
    /** Nutall */
    spWindowNutall = 1,
    /** Blackman */
    spWindowBlackman = 2,
    /** Hamming */
    spWindowHamming = 3,
    /** Gaussian 6dB BW window for EMC measurements and CISPR compatibility */
    spWindowGaussian6dB = 4,
    /** Rectangular (no) window */
    spWindowRect = 5
} SpWindowType;

/**
 * Specifies the data type of I/Q data returned from the API. 
 * For I/Q streaming and I/Q sweep lists.
 * See @ref iqDataTypes
 */
typedef enum SpDataType {
    /** 32-bit complex floats */
    spDataType32fc = 0,
    /** 16-bit complex shorts */
    spDataType16sc = 1
} SpDataType;

/**
 * External trigger edge polarity for I/Q streaming.
 */
typedef enum SpTriggerEdge {
    /** Rising edge */
    spTriggerEdgeRising = 0,
    /** Falling edge */
    spTriggerEdgeFalling = 1
} SpTriggerEdge;

/**
 * Internal GPS state
 */
typedef enum SpGPSState {
    /** GPS is not locked */
    spGPSStateNotPresent = 0,
    /** GPS is locked, NMEA data is valid, but the timebase is not being disciplined by the GPS */
    spGPSStateLocked = 1,
    /** GPS is locked, NMEA data is valid, timebase is being disciplined by the GPS */
    spGPSStateDisciplined = 2
} SpGPSState;

/**
 * Used to indicate the source of the timebase reference for the device.
 */
typedef enum SpReference {
    /** Use the internal 10MHz timebase. */
    spReferenceUseInternal = 0,
    /** Use an external 10MHz timebase on the `10 MHz In` port. */
    spReferenceUseExternal = 1
} SpReference;

/**
 * Used to specify the function of the GPIO port. See #spSetGPIOPort.
 */
typedef enum SpGPIOFunction {
    /** 
     * The port will be used for trigger detection while I/Q streaming. 
     * The port is not configured until the next I/Q stream is started.
     * This is the default GPIO function.
     */
    SpGPIOFunctionTrigIn = 0,
    /** The internal PPS signal is routed to the GPIO port. */
    SpGPIOFunctionPPSOut = 1,
    /** Set to logic low. */
    SpGPIOFunctionLogicOutLow = 2,
    /** Set to logic high. */
    SpGPIOFunctionLogicOutHigh = 3,
    /** The port can be used to manually write UART messages. */
    SpGPIOFunctionUARTDirect = 4,
    /** The port will be used for frequency switching during sweep. */
    SpGPIOFunctionUARTSweep = 5,
    /** (Not implemented) The port will be used for switching while I/Q streaming. */
    SpGPIOFunctionUARTDoppler = 6
} SpGPIOFunction;

/**
 * Audio demodulation type.
 */
typedef enum SpAudioType {
    /** AM */
    spAudioTypeAM = 0,
    /** FM */
    spAudioTypeFM = 1,
    /** Upper side band */
    spAudioTypeUSB = 2,
    /** Lower side band */
    spAudioTypeLSB = 3,
    /** CW */
    spAudioTypeCW = 4
} SpAudioType;

/**
 * Available u-blox dynamic platform models. See #spSetGPSPlatformModel for more information.
 */
typedef enum SpGPSPlatformModel {
    /** 
     * Applications with low acceleration, e.g. portable devices. 
     * Suitable for most applications.
     */
    SpGPSPlatformModelPortable = 0,
    /** 
     * Used in timing applications (antenna must be stationary) or other stationary
     * applications. Velocity restricted to 0 m/s. Zero dynamics assumed. 
     * This is the default setting.
     */
     SpGPSPlatformModelStationary = 2,
    /**
     * Applications with low accleration and speed, how a pedestian would move.
     * Low acceleration assumed.
     */
     SpGPSPlatformModelPedestrian = 3,
    /**
     * Used for applications with equivalent dynamics to those of a passenger car.
     * Low vertical acceleration assumed.
     */
     SpGPSPlatformModelAutomotive = 4,
    /**
     * Recommended for applications at sea, with zero vertical velocity.
     * Zero vertical velocity assumed, sea level assumed.
     */
     SpGPSPlatformModelAtSea = 5,
    /**
     * Used for applications with a higher dynamic range and greater vertical acceleration than a passenger car.
     * No 2D position fixes supported
     */
     SpGPSPlatformModelAirborne_1g = 6,
    /**
     * Recommended for typical airborne environment.
     * No 2D position fixes supported.
     */
     SpGPSPlatformModelAirborne_2g = 7
} SpGPSPlatformModel;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function is used to retrieve the serial numbers of all unopened SP
 * devices connected to the PC. The maximum number of serial numbers that can
 * be returned is equal to the value deviceCount points to. The serial numbers 
 * returned can then be used to open specific devices with the #spOpenDeviceBySerial 
 * function. When the function returns successfully, the serials array will contain 
 * _deviceCount_ number of unique SP serial numbers. Only _deviceCount_ values will be
 * modified. Note that this function will only report devices that are not opened
 * in the current process. If a device is opened in another application/process,
 * it will be returned by this function. See @ref multiDeviceProcessNotes.
 *
 * @param[out] serials Pointer to an array of integers. The array must be larger
 * than the number of SP devices connected to the PC.
 *
 * @param[out] deviceCount Initially the value pointed to by deviceCount should be 
 * equal to or less than the size of the serials array. If the function returns
 * successfully the value will be set to the number of serial numbers returned
 * int the serials array.
 *
 * @return
 */
SP_API SpStatus spGetDeviceList(int *serials, int *deviceCount);

/**
 * Claim the first unopened SP device detected on the system. If the device is
 * opened successfully, a handle to the function will be returned through the
 * _device_ pointer. This handle can then be used to refer to this device for
 * all future API calls. This function has the same effect as calling
 * #spGetDeviceList and using the first device found to call
 * #spOpenDeviceBySerial.
 *
 * @param[out] device Returns handle that can be used to interface the device.
 * If this function returns an error, the handle will be invalid.
 *
 * @return
 */
SP_API SpStatus spOpenDevice(int *device);

/**
 * This function is similar to #spOpenDevice except it allows you to specify the
 * device you wish to open. This function is often used in conjunction with
 * #spGetDeviceList when managing several SP devices on one PC.
 *
 * @param[out] device Returns handle that can be used to interface the device.
 * If this function returns an error, the handle will be invalid.
 *
 * @param[in] serialNumber Serial number of the device you wish to open.
 *
 * @return
 */
SP_API SpStatus spOpenDeviceBySerial(int *device, int serialNumber);

/**
 * This function should be called when you want to release the resources for a
 * device. All resources (memory, etc.) will be released, and the device will
 * become available again for use in the current process. The device handle
 * specified will no longer point to a valid device and the device must be
 * re-opened again to be used. This function should be called before the
 * process exits, but it is not strictly required.
 *
 * @param[in] device Device handle.
 *
 * @return
 */
SP_API SpStatus spCloseDevice(int device);

/**
 * Performs a full device preset. When this function returns, the hardware will
 * have performed a full reset, the device handle will no longer be valid, the
 * #spCloseDevice function will have been called for the device handle, and the
 * device will need to be re-opened again. This function can be used to recover
 * from an undesirable device state.
 *
 * @param[in] device Device handle.
 *
 * @return
 */
SP_API SpStatus spPresetDevice(int device);

/**
 * Change the power state of the device. The power state controls the power
 * consumption of the device. See @ref powerStates for more information.
 *
 * @param[in] device Device handle.
 *
 * @param[in] powerState New power state.
 *
 * @return
 */
SP_API SpStatus spSetPowerState(int device, SpPowerState powerState);

/**
 * Retrieves the current power state. See @ref powerStates for more information.
 *
 * @param[in] device Device handle.
 *
 * @param[out] powerState Pointer to #SpPowerState.
 *
 * @return
 */
SP_API SpStatus spGetPowerState(int device, SpPowerState *powerState);

/**
 * This function returns the serial number of an open device.
 *
 * @param[in] device Device handle.
 *
 * @param[out] serialNumber Returns device serial number. 
 *
 * @return
 */
SP_API SpStatus spGetSerialNumber(int device, int *serialNumber);

/**
 * Get the firmware version of the device. The firmware version is of the form
 * `major.minor.revision`. 
 *
 * @param[in] device Device handle.
 *
 * @param[out] major Pointer to int. 
 *
 * @param[out] minor Pointer to int.
 *
 * @param[out] revision Pointer to int. 
 *
 * @return
 */
SP_API SpStatus spGetFirmwareVersion(int device, int *major, int *minor, int *revision);

/**
 * Return operational information of a device.
 *
 * @param[in] device Device handle.
 *
 * @param[out] voltage Pointer to float, to contain device voltage. 
 * Reported in Voltas. Can be `NULL`.
 *
 * @param[out] current Pointer to float, to contain device current. 
 * Can be `NULL`.
 *
 * @param[out] temperature Pointer to float, to contain device temperature.
 * Reported in Celcius. Can be `NULL`.
 *
 * @return
 */
SP_API SpStatus spGetDeviceDiagnostics(int device, float *voltage, float *current, float *temperature);

/**
 * Return the last device adjustment date. This is the date in which the current
 * device adjustments were made.
 *
 * @param[in] device Device handle.
 *
 * @param[out] lastCalDate Last adjustment data as seconds since epoch.
 *
 * @return
 */
SP_API SpStatus spGetCalDate(int device, uint32_t *lastCalDate);

/**
 * Configure the receiver to use either the internal 10MHz reference or use a
 * 10MHz reference present on the 10MHz in port. The device must be in the idle
 * state. For high precision frequency measurements allow adequate settling
 * time after setting the device to use an external reference. The device defaults
 * to using the internal reference after opening the device.
 *
 * @param[in] device Device handle.
 *
 * @param[in] reference New reference state.
 *
 * @return
 */
SP_API SpStatus spSetReference(int device, SpReference reference);

/**
 * Get the current reference state.
 *
 * @param[in] device Device handle.
 *
 * @param[out] reference Returns the last set reference state.
 *
 * @return
 */
SP_API SpStatus spGetReference(int device, SpReference *reference);

/** 
 * Configure the GPIO port function. The device should be idle when calling this function.
 * See #SpGPIOFunction descriptions of each function. Also see @ref gpio.
 * 
 * @param[in] device Device handle.
 * 
 * @param[in] func Set the function of the GPIO port.
 * 
 * @return
 */
SP_API SpStatus spSetGPIOPort(int device, SpGPIOFunction func);

/**
 * Retrieve the current function of the GPIO port.
 *
 * @param[in] device Device handle.
 *
 * @param[out] func Get the function of the GPIO port.
 *
 * @return
 */
SP_API SpStatus spGetGPIOPort(int device, SpGPIOFunction *func);

/**
 * Set the UART baud rate for the GPIO port. If the GPIO port is 
 * configured for UART writes, the baud rate is updated immediately. The
 * requested baud rate may not be able to be achieved exactly, request the
 * actual baud rate with #spGetUARTBaudRate.
 * 
 * @param[in] device Device handle.
 * 
 * @param[in] rate Desired baud rate in Hz.
 * 
 * @return
 */
SP_API SpStatus spSetUARTBaudRate(int device, float rate);

/**
 * Retrieve the current configured UART baud rate.
 *
 * @param[in] device Device handle.
 *
 * @param[out] rate Current baud rate in Hz.
 *
 * @return
 */
SP_API SpStatus spGetUARTBaudRate(int device, float *rate);

/**
 * Write a single byte to the UART on the GPIO port. The port must be
 * configured for direct writes using the #spSetGPIOPort function. The baud 
 * rate of the write is specified using the #spSetUARTBaudRate function.
 *
 * @param[in] device Device handle.
 *
 * @param[in] data Byte to write.
 *
 * @return
 */
SP_API SpStatus spWriteUARTDirect(int device, uint8_t data);

/**
 * Enable whether or not the API automatically updates the timebase calibration value
 * when a valid GPS lock is acquired. This function must be called in an idle
 * state. For accurate timestamping using GPS timestamps, this should be enabled.
 * See @ref autoGPSTimebaseDiscipline for more information.
 *
 * @param[in] device Device handle.
 *
 * @param[in] enabled Send #spTrue to enable.
 *
 * @return
 */
SP_API SpStatus spSetGPSTimebaseUpdate(int device, SpBool enabled);

/**
 * Get GPS timebase update enabled. See @ref autoGPSTimebaseDiscipline for
 * more information.
 *
 * @param[in] device Device handle.
 *
 * @param[out] enabled Returns #spTrue if auto GPS timebase update is enabled.
 *
 * @return
 */
SP_API SpStatus spGetGPSTimebaseUpdate(int device, SpBool *enabled);

/**
 * Return information about the GPS holdover correction. Determine if a
 * correction exists and when it was generated.
 *
 * @param[in] device Device handle.
 *
 * @param[out] usingGPSHoldover Returns whether the GPS holdover value is newer
 * than the factory calibration value. To determine whether the holdover value
 * is actively in use, you will need to use this function in combination with
 * #spGetGPSState. This parameter can be NULL.
 *
 * @param[out] lastHoldoverTime If a GPS holdover value exists on the system,
 * return the timestamp of the value. Value is seconds since epoch. This
 * parameter can be NULL.
 *
 * @return
 */
SP_API SpStatus spGetGPSHoldoverInfo(int device, SpBool *usingGPSHoldover, uint32_t *lastHoldoverTime);

/**
 * Determine the lock and discipline status of the GPS. See the @ref gpsLock
 * section for more information. The GPS state is updated after every measurement,
 * or if no measurement is active, it is updated at most once per second.
 *
 * @param[in] device Device handle.
 *
 * @param[out] GPSState Pointer to #SpGPSState.
 *
 * @return
 */
SP_API SpStatus spGetGPSState(int device, SpGPSState *GPSState);

/**
 * The reference level controls the sensitivity of the receiver by setting the
 * attenuation of the receiver to optimize measurements for signals at or below
 * the reference level. See @ref refLevelAndSensitivity for more information.
 * Attenuation must be set to automatic (-1) to set reference level. This setting
 * is used by all measurements except I/Q sweep lists/frequency hopping.
 *
 * @param[in] device Device handle.
 *
 * @param[in] refLevel Set the reference level of the receiver in dBm.
 *
 * @return
 */
SP_API SpStatus spSetRefLevel(int device, double refLevel);

/**
 * Retrieve the current device reference level.
 *
 * @param[in] device Device handle.
 *
 * @param[out] refLevel Reference level returned in dBm.
 *
 * @return
 */
SP_API SpStatus spGetRefLevel(int device, double *refLevel);

/**
 * Set the device attenuation. See @ref refLevelAndSensitivity for more
 * information. Valid values for attenuation are between [0,6] representing
 * between [0,30] dB of attenuation (5dB steps). Setting the attenuation to SP_AUTO_ATTEN(-1)
 * tells the receiver to automatically choose the best attenuation value for
 * the specified reference level selected. Setting attenuation to a non-auto
 * value overrides the reference level selection.
 *
 * @param[in] device Device handle.
 *
 * @param[in] atten Attenuation value between [-1,6].
 *
 * @return
 */
SP_API SpStatus spSetAttenuator(int device, int atten);

/**
 * Get the device attenuation. See @ref refLevelAndSensitivity for more
 * information.
 *
 * @param[in] device Device handle.
 *
 * @param[out] atten Returns current attenuation value.
 *
 * @return
 */
SP_API SpStatus spGetAttenuator(int device, int *atten);

/**
 * Set sweep center/span.
 *
 * @param[in] device Device handle.
 *
 * @param[in] centerFreqHz New center frequency in Hz.
 *
 * @param[in] spanHz New span in Hz.
 *
 * @return
 */
SP_API SpStatus spSetSweepCenterSpan(int device, double centerFreqHz, double spanHz);

/**
 * Set sweep start/stop frequency.
 *
 * @param[in] device Device handle.
 *
 * @param[in] startFreqHz Start frequency in Hz.
 *
 * @param[in] stopFreqHz Stop frequency in Hz.
 *
 * @return
 */
SP_API SpStatus spSetSweepStartStop(int device, double startFreqHz, double stopFreqHz);

/**
 * Set sweep RBW/VBW parameters.
 *
 * @param[in] device Device handle.
 *
 * @param[in] rbw Resolution bandwidth in Hz.
 *
 * @param[in] vbw Video bandwidth in Hz. Cannot be greater than RBW.
 *
 * @param[in] sweepTime Suggest the total acquisition time of the sweep.
 * Specified in seconds. This parameter is a suggestion and will ensure RBW and
 * VBW are first met before increasing sweep time.
 *
 * @return
 */
SP_API SpStatus spSetSweepCoupling(int device, double rbw, double vbw, double sweepTime);

/**
 * Set sweep detector.
 *
 * @param[in] device Device handle.
 *
 * @param[in] detector New sweep detector.
 *
 * @param[in] videoUnits New video processing units.
 *
 * @return
 */
SP_API SpStatus spSetSweepDetector(int device, SpDetector detector, SpVideoUnits videoUnits);

/**
 * Set the sweep mode output unit type.
 *
 * @param[in] device Device handle.
 *
 * @param[in] scale New sweep mode units.
 *
 * @return
 */
SP_API SpStatus spSetSweepScale(int device, SpScale scale);

/**
 * Set sweep mode window function.
 *
 * @param[in] device Device handle.
 *
 * @param[in] window New window function.
 *
 * @return
 */
SP_API SpStatus spSetSweepWindow(int device, SpWindowType window);

/**
 * This function is used to set the frequency cross over points for the GPIO
 * sweep functionality and the associated GPIO output logic levels for each
 * frequency. See @ref gpio for more information.
 *
 * @param[in] device Device handle.
 *
 * @param[in] freqs Array of frequencies at which the associated data will be 
 * written to the UART. Array must be 'count' length.
 * 
 * @param[in] data Array of bytes. Corresponds to values in freqs array.
 * Array must be 'count' length.
 *
 * @param[in] count Length of freqs and data arrays. Set to zero to disable
 * sweep GPIO switching.
 *
 * @return
 */
SP_API SpStatus spSetSweepGPIOSwitching(int device, double *freqs, uint8_t *data, int count);

/**
 * Disables and clears the current GPIO sweep setup. The effect of this
 * function will be seen the next time the device is configured. See the @ref
 * gpio section for more information.
 *
 * @param[in] device Device handle.
 *
 * @return
 */
SP_API SpStatus spSetSweepGPIOSwitchingDisabled(int device);

/**
 * Set the center frequency and span for real-time spectrum analysis.
 *
 * @param[in] device Device handle.
 *
 * @param[in] centerFreqHz Center frequency in Hz.
 *
 * @param[in] spanHz Span in Hz.
 *
 * @return
 */
SP_API SpStatus spSetRealTimeCenterSpan(int device, double centerFreqHz, double spanHz);

/**
 * Set the resolution bandwidth for real-time spectrum analysis.
 *
 * @param[in] device Device handle.
 *
 * @param[in] rbw Resolution bandwidth in Hz.
 *
 * @return
 */
SP_API SpStatus spSetRealTimeRBW(int device, double rbw);

/**
 * Set the detector for real-time spectrum analysis.
 *
 * @param[in] device Device handle.
 *
 * @param[in] detector New detector.
 *
 * @return
 */
SP_API SpStatus spSetRealTimeDetector(int device, SpDetector detector);

/**
 * Set the sweep and frame units used in real-time spectrum analysis.
 *
 * @param[in] device Device handle.
 *
 * @param[in] scale Scale for the returned sweeps.
 *
 * @param[in] frameRef Sets the reference level of the real-time frame, or, the
 * amplitude of the highest pixel in the frame.
 *
 * @param[in] frameScale Specify the height of the frame in dB. A common value
 * is 100dB.
 *
 * @return
 */
SP_API SpStatus spSetRealTimeScale(int device, SpScale scale, double frameRef, double frameScale);

/**
 * Specify the window function used for real-time spectrum analysis.
 *
 * @param[in] device Device handle.
 *
 * @param[in] window New window function.
 *
 * @return
 */
SP_API SpStatus spSetRealTimeWindow(int device, SpWindowType window);

/**
 * Set the I/Q data type of the samples returned for I/Q streaming.
 *
 * @param[in] device Device handle.
 *
 * @param[in] dataType Data type. See @ref iqDataTypes for more information.
 *
 * @return
 */
SP_API SpStatus spSetIQDataType(int device, SpDataType dataType);

/**
 * Set the center frequency for I/Q streaming.
 *
 * @param[in] device Device handle.
 *
 * @param[in] centerFreqHz Center frequency in Hz.
 *
 * @return
 */
SP_API SpStatus spSetIQCenterFreq(int device, double centerFreqHz);

/**
 * Get the I/Q streaming center frequency.
 *
 * @param[in] device Device handle.
 *
 * @param[in] centerFreqHz Pointer to double.
 *
 * @return
 */
SP_API SpStatus spGetIQCenterFreq(int device, double *centerFreqHz);

/**
 * Set sample rate for I/Q streaming.
 *
 * @param[in] device Device handle.
 *
 * @param[in] decimation Decimation of the I/Q data as a power of 2. See @ref
 * iqSampleRates for more information.
 *
 * @return
 */
SP_API SpStatus spSetIQSampleRate(int device, int decimation);

/**
 * Enable/disable software filtering.
 *
 * @param[in] device Device handle.
 *
 * @param[in] enabled Set to #spTrue to enable software filtering.
 *
 * @return
 */
SP_API SpStatus spSetIQSoftwareFilter(int device, SpBool enabled);

/**
 * Specify the software filter bandwidth in I/Q streaming. See @ref
 * iqSampleRates for more information.
 *
 * @param[in] device Device handle.
 *
 * @param[in] bandwidth The bandwidth in Hz.
 *
 * @return
 */
SP_API SpStatus spSetIQBandwidth(int device, double bandwidth);

/**
 * Configure the external trigger edge detect in I/Q streaming.
 *
 * @param[in] device Device handle.
 *
 * @param[in] edge Set the external trigger edge.
 *
 * @return
 */
SP_API SpStatus spSetIQExtTriggerEdge(int device, SpTriggerEdge edge);

/**
 * Configure how external triggers are reported for I/Q streaming.
 *
 * @param[in] sentinelValue Value used to fill the remainder of the trigger
 * buffer when the trigger buffer provided is larger than the number of
 * triggers returned. The default sentinel value is zero. See the @ref
 * iqStreaming section for more information on triggering.
 *
 * @return
 */
SP_API SpStatus spSetIQTriggerSentinel(double sentinelValue);

/**
 * Controls the USB queue size of I/Q data that is being actively requested by
 * the API. For example, a queue size of 21ms means the API keeps 21ms of
 * data requests active. A larger queue size means a greater tolerance to data
 * loss in the event of an interruption. Because once data is requested, its
 * transfer must be completed, a smaller queue size can give you faster
 * reconfiguration times. For instance, if you wanted to change frequencies
 * quickly, a smaller queue size would allow this. A default (16) is chosen for the
 * best resistance to data loss for both Linux and Windows. If you are on Linux
 * and you are using multiple devices, please see @ref linuxNotes. This setting
 * applies to I/Q streaming only.
 *
 * @param[in] device Device handle
 *
 * @param[in] units Should be a value between [2,16]. Each unit represent 2.1ms. 
 * For example, 4 units = 4 * 2.1 = 8.4ms queue size.
 *
 * @return
 */
SP_API SpStatus spSetIQQueueSize(int device, int units);

/**
 * Set the data type for data returned for I/Q sweep list measurements.
 *
 * @param[in] device Device handle.
 *
 * @param[in] dataType See @ref iqDataTypes for more information.
 *
 * @return
 */
SP_API SpStatus spSetIQSweepListDataType(int device, SpDataType dataType);

/**
 * Set whether the data returns for I/Q sweep list meausurements is full-scale
 * or corrected.
 *
 * @param[in] device Device handle.
 *
 * @param[in] corrected Set to #spFalse for the data to be returned as full scale,
 * and #spTrue to be returned amplitude corrected. See @ref iqDataTypes for more
 * information on how to perform these conversions.
 *
 * @return
 */
SP_API SpStatus spSetIQSweepListCorrected(int device, SpBool corrected);

/**
 * Set the number of frequency steps for I/Q sweep list measurements.
 *
 * @param[in] device Device handle.
 *
 * @param[in] steps Number of frequency steps in I/Q sweep.
 *
 * @return
 */
SP_API SpStatus spSetIQSweepListSteps(int device, int steps);

/**
 * Get the number steps in the I/Q sweep list measurement.
 *
 * @param[in] device Device handle.
 *
 * @param[out] steps Pointer to int.
 *
 * @return
 */
SP_API SpStatus spGetIQSweepListSteps(int device, int *steps);

/**
 * Set the center frequency of the acquisition at a given step for the I/Q
 * sweep list measurement.
 *
 * @param[in] device Device handle.
 *
 * @param[in] step Step at which to configure the center frequency. Should be
 * between [0, _steps_-1] where _steps_ is set in the #spSetIQSweepListSteps
 * function.
 *
 * @param[in] freq Center frequency in Hz.
 *
 * @return
 */
SP_API SpStatus spSetIQSweepListFreq(int device, int step, double freq);

/**
 * Set the reference level for a step for the I/Q sweep list measurement.
 *
 * @param[in] device Device handle.
 *
 * @param[in] step Step at which to configure the reference level. Should be
 * between [0, _steps_-1] where _steps_ is set in the #spSetIQSweepListSteps
 * function.
 *
 * @param[in] level Reference level in dBm. If this is set, attenuation is set
 * to automatic for this step.
 *
 * @return
 */
SP_API SpStatus spSetIQSweepListRef(int device, int step, double level);

/**
 * Set the attenuation for a step for the I/Q sweep list measurement.
 *
 * @param[in] device Device handle.
 *
 * @param[in] step Step at which to configure the attenuation. Should be
 * between [0, _steps_-1] where _steps_ is set in the #spSetIQSweepListSteps
 * function.

 * @param[in] atten Attenuation value between [0,6] representing [0,30] dB of
 * attenuation (5dB steps). Setting the attenuation to -1 forces the
 * attenuation to auto, at which time the reference level is used to control
 * the attenuator instead.
 *
 * @return
 */
SP_API SpStatus spSetIQSweepListAtten(int device, int step, int atten);

/**
 * Set the number of I/Q samples to be collected at a step.
 *
 * @param[in] device Device handle.
 *
 * @param[in] step Step at which to configure the sample count. Should be
 * between [0, _steps_-1] where _steps_ is set in the #spSetIQSweepListSteps
 * function.
 *
 * @param[in] samples Number of samples. Must be greater than 0. There is no
 * upper limit, but keep in mind contiguous memory must be allocated for the
 * capture. Memory allocation for the capture is the responsibility of the user
 * program.
 *
 * @return
 */
SP_API SpStatus spSetIQSweepListSampleCount(int device, int step, uint32_t samples);

/**
 * Set the center frequency for audio demodulation.
 *
 * @param[in] device Device handle.
 *
 * @param[in] centerFreqHz Center frequency in Hz.
 *
 * @return
 */
SP_API SpStatus spSetAudioCenterFreq(int device, double centerFreqHz);

/**
 * Set the audio demodulator for audio demodulation.
 *
 * @param[in] device Device handle.
 *
 * @param[in] audioType Demodulator.
 *
 * @return
 */
SP_API SpStatus spSetAudioType(int device, SpAudioType audioType);

/**
 * Set the audio demodulation filters for audio demodulation.
 *
 * @param[in] device Device handle.
 *
 * @param[in] ifBandwidth IF bandwidth (RBW) in Hz.
 *
 * @param[in] audioLpf Audio low pass frequency in Hz.
 *
 * @param[in] audioHpf Audio high pass frequency in Hz.
 *
 * @return
 */
SP_API SpStatus spSetAudioFilters(int device,
    double ifBandwidth,
    double audioLpf,
    double audioHpf);

/**
 * Set the FM deemphasis for audio demodulation.
 *
 * @param[in] device Device handle.
 *
 * @param[in] deemphasis Deemphasis in us.
 *
 * @return
 */
SP_API SpStatus spSetAudioFMDeemphasis(int device, double deemphasis);

/**
 * This function configures the receiver into a state determined by the mode
 * parameter. All relevant configuration routines must have already been
 * called. This function calls #spAbort to end the previous measurement mode
 * before attempting to configure the receiver. If any error occurs attempting
 * to configure the new measurement state, the previous measurement mode will
 * no longer be active.
 *
 * @param[in] device Device handle.
 *
 * @param[in] mode New measurement mode.
 *
 * @return
 */
SP_API SpStatus spConfigure(int device, SpMode mode);

/**
 * Retrieve the current device measurement mode.
 *
 * @param[in] device Device handle.
 *
 * @param[in] mode Pointer to #SpMode.
 *
 * @return
 */
SP_API SpStatus spGetCurrentMode(int device, SpMode *mode);

/**
 * This function ends the current measurement mode and puts the device into the
 * idle state. Any current measurements are completed and discarded and will
 * not be accessible after this function returns.
 *
 * @param[in] device Device handle.
 *
 * @return
 */
SP_API SpStatus spAbort(int device);

/**
 * Retrieves the sweep parameters for an active sweep measurement mode. This
 * function should be called after a successful device configuration to
 * retrieve the sweep characteristics.
 *
 * @param[in] device Device handle.
 *
 * @param[out] actualRBW Returns the RBW being used in Hz. Can be NULL.
 *
 * @param[out] actualVBW Returns the VBW being used in Hz. Can be NULL.
 *
 * @param[out] actualStartFreq Returns the frequency of the first
 * bin in Hz. Can be NULL.
 *
 * @param[out] binSize Returns the frequency spacing between each frequency bin
 * in the sweep in Hz. Can be NULL.
 *
 * @param[out] sweepSize Returns the length of the sweep (number of frequency
 * bins). Can be NULL.
 *
 * @return
 */
SP_API SpStatus spGetSweepParameters(int device, double *actualRBW, double *actualVBW,
                                     double *actualStartFreq, double *binSize, int *sweepSize);

/**
 * Retrieve the real-time measurement mode parameters for an active real-time
 * configuration. This function is typically called after a successful device
 * configuration to retrieve the real-time sweep and frame characteristics.
 *
 * @param[in] device Device handle.
 *
 * @param[out] actualRBW Returns the RBW used in Hz. Can be NULL.
 *
 * @param[out] sweepSize Returns the number of frequency bins in the sweep. Can
 * be NULL.
 *
 * @param[out] actualStartFreq Returns the frequency of the first bin in the
 * sweep in Hz. Can be NULL.
 *
 * @param[out] binSize Frequency bin spacing in Hz. Can be NULL.
 *
 * @param[out] frameWidth The width of the real-time frame. Can be NULL.
 *
 * @param[out] frameHeight The height of the real-time frame. Can be NULL.
 *
 * @param[out] poi 100% probability of intercept of a signal given the current
 * configuration. Can be NULL.
 *
 * @return
 */
SP_API SpStatus spGetRealTimeParameters(int device, double *actualRBW, int *sweepSize, double *actualStartFreq,
                                        double *binSize, int *frameWidth, int *frameHeight, double *poi);

/**
 * Retrieve the I/Q measurement mode parameters for an active I/Q stream. This
 * function is called after a successful device configuration.
 *
 * @param[in] device Device handle.
 *
 * @param[out] sampleRate The sample rate in Hz. Can be NULL.
 *
 * @param bandwidth The bandwidth of the I/Q capture in Hz. Can be NULL.
 *
 * @return
 */
SP_API SpStatus spGetIQParameters(int device, double *sampleRate, double *bandwidth);


/**
 * Retrieve the I/Q correction factor for an active I/Q stream. This function is
 * called after a successful device configuration.
 *
 * @param[in] device Device handle.
 *
 * @param[out] scale Amplitude correction used by the API to convert from full
 * scale I/Q to amplitude corrected I/Q. The formulas for these conversions are
 * in the @ref iqDataTypes section.
 *
 * @return
 */
SP_API SpStatus spGetIQCorrection(int device, float *scale);

/**
 * Retrieve the corrections used to convert full scale I/Q values to amplitude
 * corrected I/Q values for the I/Q sweep list measurement. A correction is
 * returned for each step configured. The device must be configured for I/Q
 * sweep list measurements before calling this function.
 *
 *
 * @param[in] device Device handle.
 *
 * @param[out] corrections Pointer to an array. Array should have length >=
 * number of steps configured for the I/Q sweep list measurement. A correction
 * value will be returned for each step configured.
 *
 * @return
 */
SP_API SpStatus spIQSweepListGetCorrections(int device, float *corrections);

/**
 * Perform a single sweep. Block until the sweep completes.
 *
 * @param[in] device Device handle.
 *
 * @param[out] sweepMin Can be NULL.
 *
 * @param[out] sweepMax Can be NULL.
 *
 * @param[out] nsSinceEpoch Nanoseconds since epoch. Timestamp representing the
 * end of the sweep. Can be NULL.
 *
 * @return
 */
SP_API SpStatus spGetSweep(int device, float *sweepMin, float *sweepMax, int64_t *nsSinceEpoch);

/**
 * Retrieve a single real-time frame. See @ref realTime for more information.
 *
 * @param[in] device Device handle.
 *
 * @param[out] colorFrame Pointer to memory for the frame.
 * Must be (_frameWidth_ * _frameHeight_) floats in size. Can be NULL.
 *
 * @param[out] alphaFrame Pointer to memory for the frame.
 * Must be (_frameWidth_ * _frameHeight_) floats in size. Can be NULL.
 *
 * @param[out] sweepMin Can be NULL.
 *
 * @param[out] sweepMax Can be NULL.
 *
 * @param[out] frameCount Unique integer which refers to a real-time frame and
 * sweep. The frame count starts at zero following a device reconfigure and
 * increments by one for each frame.
 *
 * @param[out] nsSinceEpoch Nanoseconds since epoch for the returned frame. For
 * real-time mode, this value represents the time at the end of the real-time
 * acquisition and processing of this given frame. It is approximate. Can be
 * NULL.
 *
 * @return
 */
SP_API SpStatus spGetRealTimeFrame(int device, float *colorFrame, float *alphaFrame, float *sweepMin,
                                   float *sweepMax, int *frameCount, int64_t *nsSinceEpoch);

/**
 * Retrieve one block of I/Q data as specified by the user. This function
 * blocks until the data requested is available.
 *
 * @param[in] device Device handle.
 *
 * @param[out] iqBuf Pointer to user allocated buffer of complex values. The
 * buffer size must be at least (_iqBufSize_ * 2 * sizeof(dataTypeSelected)).
 * Cannot be NULL. Data is returned as interleaved contiguous complex samples.
 * For more information on the data returned and the selectable data types, see
 * @ref iqDataTypes.
 *
 * @param[in] iqBufSize Specifies the number of I/Q samples to be retrieved.
 * Must be greater than zero.
 *
 * @param[out] triggers Pointer to user-allocated array of doubles. The buffer
 * must be at least _triggerBufSize_ contiguous doubles. The pointer can also be
 * NULL to indicate you do not wish to receive external trigger information.
 * See @ref iqStreaming section for more information on triggers.
 *
 * @param[in] triggerBufSize Specifies the size of the _triggers_ array. If the
 * _triggers_ array is NULL, this value should be zero.
 *
 * @param[out] nsSinceEpoch Nanoseconds since epoch. The time of the first I/Q
 * sample returned. Can be NULL. See @ref gpsAndTimestamps for more information.
 *
 * @param[in] purge When set to #spTrue, any buffered I/Q data in the API is
 * purged before returned beginning the I/Q block acquisition.
 *
 * @param[out] sampleLoss Set by the API when a sample loss condition occurs.
 * If enough I/Q data has accumulated in the internal API circular buffer, the
 * buffer is cleared and the sample loss flag is set. If purge is set to true,
 * the sample flag will always be set to #SP_FALSE. Can be NULL.
 *
 * @param[out] samplesRemaining Set by the API, returns the number of samples
 * remaining in the I/Q circular buffer. Can be NULL.
 *
 * @return
 */
SP_API SpStatus spGetIQ(int device, void *iqBuf, int iqBufSize, double *triggers, int triggerBufSize,
                        int64_t *nsSinceEpoch, SpBool purge, int *sampleLoss, int *samplesRemaining);

/**
 * Perform an I/Q sweep. Blocks until the sweep is complete. Can
 * only be called if no other I/Q sweeps are queued.
 *
 * @param[in] device Device handle.
 *
 * @param[out] dst Pointer to memory allocated for sweep. The user must
 * allocate this memory before calling this function. Must be large enough to
 * contain all samples for all steps in a sweep. The memory must be contiguous.
 * The samples in the sweep are placed contiguously into the array (step 1
 * samples follow step 0, step 2 follows step 1, etc). Samples are tightly
 * packed. It is the responsibility of the user to properly index the arrays
 * when finished. The array will be cast to the user-selected data type
 * internally in the API.
 *
 * @param[out] timestamps Pointer to memory allocated for timestamps. The user
 * must allocate this memory before calling these functions. Must be an array
 * of _steps_ int64_ts, where _steps_ is the number of frequency steps in the
 * sweep. When the sweep completes each timestamp in the array represents the
 * time of the first sample at that frequency in the sweep. Can be NULL if
 * you do not want timestamps.
 *
 * @return
 */
SP_API SpStatus spIQSweepListGetSweep(int device, void *dst, int64_t *timestamps);

/**
 * Start an I/Q sweep at the given queue position. Up to 16 sweeps can be in
 * queue.
 *
 * @param[in] device Device handle.
 *
 * @param[in] pos Sweep queue position. Must be between [0,15].
 *
 * @param[out] dst Pointer to memory allocated for sweep. The user must
 * allocate this memory before calling this function. Must be large enough to
 * contain all samples for all steps in a sweep. The memory must be contiguous.
 * The samples in the sweep are placed contiguously into the array (step 1
 * samples follow step 0, step 2 follows step 1, etc). Samples are tightly
 * packed. It is the responsibility of the user to properly index the arrays
 * when finished. The array will be cast to the user-selected data type
 * internally in the API.
 *
 * @param[out] timestamps Pointer to memory allocated for timestamps. The user
 * must allocate this memory before calling these functions. Must be an array
 * of _steps_ int64_ts, where _steps_ is the number of frequency steps in the
 * sweep. When the sweep completes each timestamp in the array represents the
 * time of the first sample at that frequency in the sweep. Can be NULL.
 *
 * @return
 */
SP_API SpStatus spIQSweepListStartSweep(int device, int pos, void *dst, int64_t *timestamps);

/**
 * Finish an I/Q sweep at the given queue position. Blocks until the sweep is
 * finished.
 *
 * @param[in] device Device handle.
 *
 * @param[in] pos Sweep queue position. Must be between [0,15].
 *
 * @return
 */
SP_API SpStatus spIQSweepListFinishSweep(int device, int pos);

/**
 * If the device is configured to audio demodulation, use this function to
 * retrieve the next 1000 audio samples. This function will block until the
 * data is ready. Minor buffering of audio data is performed in the API, so it
 * is necessary this function is called repeatedly if contiguous audio data is
 * required. The values returned range between [-1.0, 1.0] representing
 * full-scale audio. In FM mode, the audio values will scale with a change in
 * IF bandwidth.
 *
 * @param[in] device Device handle.
 *
 * @param[out] audio Pointer to array of 1000 32-bit floats.
 *
 * @return
 */
SP_API SpStatus spGetAudio(int device, float *audio);

/**
 * Acquire the latest GPS information which includes a time stamp, location
 * information, and NMEA sentences. The GPS info is updated once per second at
 * the PPS interval. This function can be called while measurements are active.
 * NMEA data can contain null values. When parsing, do not use the null
 * delimiter to mark the end of the message, use the returned _nmeaLen_.
 *
 * @param[in] device Device handle.
 *
 * @param[in] refresh When set to true and the device is not in a streaming
 * mode, the API will request the latest GPS information. Otherwise the last
 * retrieved data is returned.
 *
 * @param[out] updated Will be set to true if the NMEA data has been updated
 * since the last time the user called this function. Can be set to NULL.
 *
 * @param[out] secSinceEpoch Number of seconds since epoch as reported by the
 * GPS NMEA sentences. Last reported value by the GPS. If the GPS is not
 * locked, this value will be set to zero. Can be NULL.
 *
 * @param[out] latitude Latitude in decimal degrees. If the GPS is not locked,
 * this value will be set to zero. Can be NULL.
 *
 * @param[out] longitude Longitude in decimal degrees. If the GPS is not
 * locked, this value will be set to zero. Can be NULL.
 *
 * @param[out] altitude Altitude in meters. If the GPS is not locked, this
 * value will be set to zero. Can be NULL.
 *
 * @param[out] nmea Pointer to user-allocated array of char. The length of this
 * array is specified by the _nmeaLen_ parameter. Can be set to NULL.
 *
 * @param[inout] nmeaLen Pointer to an integer. The integer will initially
 * specify the length of the nmea buffer. If the nmea buffer is shorter than
 * the NMEA sentences to be returned, the API will only copy over _nmeaLen_
 * characters, including the null terminator. After the function returns,
 * _nmeaLen_ will be the length of the copied nmea data, including the null
 * terminator. Can be set to NULL. If NULL, the nmea parameter is ignored.
 *
 * @return
 */
SP_API SpStatus spGetGPSInfo(int device, SpBool refresh, SpBool *updated, int64_t *secSinceEpoch,
                             double *latitude, double *longitude, double *altitude, char *nmea, int *nmeaLen);

/**
 * See @ref writingToGPS. Use this function to send messages to the internal 
 * u-blox M8 GPS. Messages provided are rounded/padded up to the next multiple 
 * of 4 bytes. The padded bytes are set to zero.
 *
 * @param[in] device Device handle.
 *
 * @param[in] mem The message to send to the GPS.

 * @param[in] len The length of the message in bytes.
 *
 * @return
 */
SP_API SpStatus spWriteToGPS(int device, const uint8_t *mem, int len);


/**
 * Sets the dynamic platform model of the internal GPS. Setting the correct model
 * is required to achieve GPS lock. By default, the GPS is configured to stationary. 
 * The device should be idle and not making any measurements when this function is called.
 * This value is reset on device power cycle. 
 * 
 * It is recommended to use stationary and portable for most use cases.
 * 
 * The device must have a valid GPS lock (but does not need to be disciplined) before
 * calling this function.
 * 
 * This function can take up to 2 seconds to complete. 
 *
 * @param[in] device Device handle.
 *
 * @param[in] platformModel The model to use.
 *
 * @return spGPSErr will be returned if this function fails. The main reason this function
 * will fail is if the GPS is not currently locked. You can check the state of the GPS with the
 * #spGetGPSState function. 
 */
SP_API SpStatus spSetGPSPlatformModel(int device, SpGPSPlatformModel platformModel);

/**
 * Specify the temperature of the device at which the internal fan attempts to
 * maintain. The available temperature range is between [0-60] degrees. The fan
 * uses a closed loop algorithm to reach the target temperature. Set to the
 * maximum or minimum set point to force 0 or 100% fan speed.
 * 
 * This function must be called when the device is idle (no measurement mode active).
 *
 * @param[in] device Device handle.
 *
 * @param[in] setPoint Temperature set point in Celcius.
 *
 * @return
 */
SP_API SpStatus spSetFanSetPoint(int device, float setPoint);

/**
 * Get current fan temperature threshold and voltage.
 *
 * @param[in] device Device handle.
 *
 * @param[out] setPoint Temperature set point in Celcius. Can be NULL.
 *
 * @param[out] voltage Voltage in V. Can be NULL.
 *
 * @return
 */
SP_API SpStatus spGetFanSettings(int device, float *setPoint, float *voltage);

/**
 * Retrieve a descriptive string of an #SpStatus enumeration. Useful for
 * debugging and diagnostic purposes.
 *
 * @param[in] status Status code returned from any API function.
 *
 * @return
 */
SP_API const char* spGetErrorString(SpStatus status);

/**
 * Get the API version.
 *
 * @return
 * The returned string is of the form
 *
 * major.minor.revision
 *
 * Ascii periods ('.') separate positive integers. Major/minor/revision are not
 * guaranteed to be a single decimal digit. The string is null terminated. The
 * string should not be modified or freed by the user. An example string is
 * below?
 *
 * ['3' | '.' | '0' | '.' | '1' | '1' | '\0'] = "3.0.11"
 */
SP_API const char* spGetAPIVersion();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef SP_API_H
