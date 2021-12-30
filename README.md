# About
Application used to show the current usage of the up and downstream on two analog gauges

![ExampleVideo](./Hardware/Example.gif)


# Hardware

- Raspberry Pi Zero W
- 2x Adafruit MCP4725 12-Bit DAC  https://learn.adafruit.com/mcp4725-12-bit-dac-tutorial
- 2x 5V Analog gauges

![Sketch](./Hardware/Sketch.png)

## Gauges Background

The Background of the gauges can be created by the Hardware/Analog Gauges/CreateGaugesBackground.py Python Skript.

Here there will be two svg files be created- One for the up and one for the download.

Here you can set the max up and download speed and print the result. 

![Sketch](./Hardware/Analog_Gauges/download.svg)

### Using the Python Gauge Background generator

Importing pycairo
To install pycairo we also need the dependencies of the C Headers: 

> brew install cairo pkg-config



# Software

The application is written in python.

## fritzconnection

To connect with the fr!tz box routers we need an TR-064 libtrary to communicate. This Library can be used to retriev all needed informations drom the router.

## adafruit-circuitpython-mcp4725

### Usage / Info 

- GitHub: https://github.com/adafruit/Adafruit_CircuitPython_MCP4725

- Library Definition: https://circuitpython.readthedocs.io/projects/mcp4725/en/latest/api.html
- Create an SPI object: https://circuitpython.readthedocs.io/en/latest/shared-bindings/busio/index.html#busio.I2C

# Pi Konfiguration

After installing the latest Raspbian OS from RaspberryPi.org update the systenm by:

> sudo apt-get update

> sudo apt-get upgrade

## Install the requested libraries

> pip install fritzconnection

> pip install adafruit-circuitpython-mcp4725

## Create and start service

Source: https://www.nerdynat.com/programming/2019/run-python-on-your-raspberry-pi-as-background-service/

> sudo nano /lib/systemd/system/fritzmeter.service

Add the text from ./Software/Fritzmeter.service
Than change the permission

> sudo chmod 644 /lib/systemd/system/fritzmeter.service

Reload the system manager configuration

> sudo systemctl daemon-reload

Start the Service

> sudo systemctl start fritzmeter.service

Enable the service to start at boot 

> sudo systemctl enable fritzmeter.service

So stop the service you can use

>  sudo systemctl stop fritzmeter.service