import time




def main():
    """Main programm"""
    print("Hello World")
    print(time.time_ns())

    transition(0, 100, 2)



def transition(dac, lastValue, newValue, timneSpan):
    """
    transition between the analog values
    Do this to prevent jumping pointer
    """

    intItterations = timneSpan/0.1
    blnCountUp = False
    currentValue = lastValue
    print("Iterations: " + str(intItterations) + "  " + str(currentValue) + " ")


    if (lastValue < newValue):
        blnCountUp = True

        valueDiff = (abs(newValue) - abs(lastValue)) / intItterations

    else:

        valueDiff = (abs(lastValue) - abs(newValue)) / intItterations

    for idx in range(int(intItterations)):

        print(str(idx))
        if blnCountUp:
            currentValue = currentValue + valueDiff
        else:
            currentValue = currentValue - valueDiff
    
        print(str(currentValue))
        dac.value = currentValue
        time.sleep(0.1)
    


if __name__ == '__main__':

    #Start the main programm
    main()
