import time
from fritzconnection.lib.fritzstatus import FritzStatus
import board
import busio
import adafruit_mcp4725
import threading

def main():
    """Main programm"""

    SLEEP_TIMER_SECONDS = 1
    MAX_UPLOAD_MBPS = 30        #chose the right maximum upload speed depending on your connection
    MAX_DOWNLOAD_MBPS = 200     #chose the right maximum download speed depending on your connection

    #initialize the I2C connection with the sensor
    i2c = busio.I2C(board.SCL, board.SDA)
    dacUpload = adafruit_mcp4725.MCP4725(i2c)
    dacDownload = adafruit_mcp4725.MCP4725(i2c, address=0x63)
    _valueUploadOld = 0
    _valueDownloadOld = 0

    #Create an Fritz Status Object
    fc = FritzStatus(address='192.168.178.1', password="")

    blnLoopActive = True
    _valueUploadOld = 0
    _valueDownloadOld = 0
    _valueUpload = 0
    _valueDownload = 0
    uploadTransitionThread = None
    downlaodTransitionThread = None


    #Start the loop
    while blnLoopActive:


        try:
            #Get the Transmission rate from tegh Fr!tz Router           
            _transmissionRate = fc.transmission_rate

            #Try to join the Thread if allready existing
            if not (uploadTransitionThread is None): 
                uploadTransitionThread.join()
            if not (downlaodTransitionThread is None):
                downlaodTransitionThread.join()

            #Convert to value to set the do dac 
            _valueUpload = convertToAnalogOutput(30, convertToMbps(_transmissionRate[0])) 
            _valueDownload = convertToAnalogOutput(200, convertToMbps(_transmissionRate[1]))

            #Create the threads to performane the transition of the analog gauges
            uploadTransitionThread = threading.Thread(target=transition, args=(dacUpload, _valueUploadOld, _valueUpload, 0.8, "Upload",))
            downlaodTransitionThread = threading.Thread(target=transition, args=(dacDownload, _valueDownloadOld, _valueDownload, 0.8, "Download",))

            #Start the 
            uploadTransitionThread.start()
            downlaodTransitionThread.start()

            #Save the current value 
            _valueUploadOld = _valueUpload
            _valueDownloadOld = _valueDownload
        
        except:
            #Create an Fritz Status Object
            fc = FritzStatus(address='192.168.178.1', password="")
            print("Error occured. We will not try again.")

def convertToAnalogOutput(maxMbps, currentMbps):
    """
    Convert Mbps to Analog Output over I2C
    """
    tmpRetVal = 0
    #if (currentMbps < 1):
    #    tmpRetVal = 0
    #elif (float(currentMbps) > float(maxMbps)):
    #    tmpRetVal = 1
    #else:

    tmpRetVal = 65535/float(maxMbps)*float(currentMbps)

    return tmpRetVal


def convertToMbps(bytesPerSeconds):
    """
    BytesPerSeconds = Input 
    ---
    Convert the Data (bytes per second) to Mbps Mega bits per second 
    """

    #1 Mega bit per Second (Mbps) = 1 000 000 Bits / Second
    #First convert to Bit (1 Byte = 8 Bit) Than convert to Mbps by deviding through 1 000 000
    return (bytesPerSeconds * 8)/1000000


def transition(dac, currentValue, newValue, timneSpan, name):
    """
    transition between the analog values
    Do this to prevent jumping pointer
    """

    intItterations = timneSpan/0.1
    blnCountUp = False
    lastValue = currentValue

    if (lastValue < newValue):
        blnCountUp = True

        valueDiff = (abs(newValue) - abs(lastValue)) / intItterations

    else:

        valueDiff = (abs(lastValue) - abs(newValue)) / intItterations

    for idx in range(int(intItterations)):

        if blnCountUp:
            currentValue = currentValue + valueDiff
        else:
            currentValue = currentValue - valueDiff
    
        #Set the Analog Value
        dac.value = int(currentValue)

        #wait for 100 ms 
        time.sleep(0.1)


if __name__ == '__main__':

    #Start the main programm
    main()
