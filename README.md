<p align="center">
<img src="https://github.com/SignalHound/gr-bb60/blob/master/docs/SH-GR.jpg" width="75%" />
</p>

# A [GNU Radio](https://github.com/pothosware/SoapySDR/wiki) OOT module for [Signal Hound Devices](https://signalhound.com/products/)
## System Requirements

- 64-bit Linux operating system
    - Tested on [RF Swift](https://rfswift.io) (`sdr_light`, and `sdr_full` [images](https://rfswift.io/docs/guide/list-of-tools/#sdr-light-tools)) DragonOS Noble 6.14.0-27-generic
- Native USB 3.0 support
## Dependancies

1. Libusb 1.0 
    ~~~
    $ sudo apt-get update
    $ sudo apt-get install libusb-1.0-0
    ~~~
    Note: libusb requires root by default to change this add the following to etc/udev/rules.d/sh.rules
    or download [Spike](https://signalhound.com/spike) and the setup will be done for you
    ~~~
    SUBSYSTEM=="usb", ATTR{idVendor}=="2817", MODE="0666", GROUP="plugdev"
    ~~~
2. FTDI D2XX library (for BB60)
- grab latest 64-bit Linux driver from [FTDI Chip](www.ftdichip.com/Drivers/D2XX.htm) 
- or find a copy of the library in the [Signal Hound SDK](https://signalhound.com/software/signal-hound-software-development-kit-sdk/) 
    - standard path device_apis/bb_series/lib/linux/Ubuntu 18.04/libftd2xx.so
        ~~~
        $ sudo cp libftd2xx* /usr/local/lib
        $ cd /usr/local/lib
        $ sudo chmod 0755 libftd2xx.so
        ~~~
3. Device API's
- grab shared object file from [Signal Hound SDK](https://signalhound.com/software/signal-hound-software-development-kit-sdk/) (BB60 example first)
    ~~~
    $ cd device_apis/bb_series/lib/linux/Ubuntu 18.04
    $ sudo cp libbb_api.* /usr/local/lib
    $ sudo ldconfig -v -n /usr/local/lib
    $ sudo ln -sf /usr/local/lib/libbb_api.so.5 /usr/local/lib/libbb_api.so
    ~~~
    repeat for all devices (required)
    ~~~
    $ cd device_apis/<device>_series/lib/linux/Ubuntu 18.04
    $ sudo cp lib<device>_api.* /usr/local/lib
    $ sudo ldconfig -v -n /usr/local/lib
    $ sudo ln -sf /usr/local/lib/lib<device>_api.so.* /usr/local/lib/lib<device>_api.so
    ~~~
4. GNU Radio Companion
    ~~~
    $ sudo apt-get install gnuradio
    $ sudo apt-get install gnuradio-dev cmake libspdlog-dev clang-format
    ~~~

## Installation

1. Clone this repository.
2. Run the following commands from the cloned repo:
    ~~~
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ sudo make install
    $ sudo ldconfig
    ~~~
Note: MATLAB runtime LD_LIBRARY_PATH environment variable may cause conflicts with cmake

## Usage

- Open [GNU Radio Companion](https://www.gnuradio.org/) select device block from module tree
~~~
-Signal Hound
|___Sink
|   |____VSG60: IQ Sink
|___Source
    |___BB60: IQ Source
    |___SP145: IQ Source
    |___SM200/SM435: IQ Source
~~~

