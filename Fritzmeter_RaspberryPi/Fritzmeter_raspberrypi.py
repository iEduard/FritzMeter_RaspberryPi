import time
from fritzconnection.lib.fritzstatus import FritzStatus



def main():
    """Main programm"""

    SLEEP_TIMER_SECONDS = 2
    MAX_UPLOAD_MBPS = 20        #Maybe chose 50
    MAX_DOWNLOAD_MBPS = 170     #Maybe chose 200

    #Create an Fritz Status Object
    fc = FritzStatus(address='192.168.178.1', password="mySuperSecretPassword;-)")

    #Start the loop
    while True:
        #transmission_rate: The upstream and downstream values as a tuple in bytes per second. Use this for periodical calling.
        print( "Upload: " + str(convertToMbps(fc.transmission_rate[0])) + "Mbps // Download: " + str(convertToMbps(fc.transmission_rate[0])) + "Mbps" )
        
        time.sleep(SLEEP_TIMER_SECONDS)

def convertMbpsToAnalogOut(maxAnalogOut, currentMbps):
    """
    Convert Mbps to Analog Output over I2C
    """

    False


def convertToMbps(bytesPerSeconds):
    """
    BytesPerSeconds = Input 
    ---
    Convert the Data (bytes per second) to Mbps Mega bits per second 
    """

    #1 Mega bit per Second (Mbps) = 1 000 000 Bits / Second
    #First convert to Bit (1 Byte = 8 Bit) Than convert to Mbps by deviding through 1 000 000
    return (bytesPerSeconds * 8)/1000000



if __name__ == '__main__':

    #Start the main programm
    main()