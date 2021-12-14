# About
Application used to show the current usage of the up and downstream on two analog gauges



## Hardware

- Raspberry Pi Zero W
- 2x Adafruit MCP4725 12-Bit DAC Tutorial https://learn.adafruit.com/mcp4725-12-bit-dac-tutorial
- 2x 5V Analog gauges


## Software

The application written here is in python.

### fritzconnection

To connect with the fr!tz box routers we need an TR-064 libtrary to communicate. This Library can be used to retriev all needed informations drom the router.

#### Installation

To install the application use pip.

> pip install fritzconnection

### adafruit-circuitpython-mcp4725

#### Installation

To install the application use pip.

> pip install adafruit-circuitpython-mcp4725

#### Usage / Info 

- GitHub: https://github.com/adafruit/Adafruit_CircuitPython_MCP4725

- Library Definition: https://circuitpython.readthedocs.io/projects/mcp4725/en/latest/api.html
- Create an SPI object: https://circuitpython.readthedocs.io/en/latest/shared-bindings/busio/index.html#busio.I2C
