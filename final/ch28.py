# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

from numpy import double
import serial
import matplotlib.pyplot as plt 
from statistics import mean 
from genref import genRef

ser = serial.Serial('/dev/tty.usbserial-DT03EUVE', 230400, rtscts=1)
print('Opening port: ')
print(ser.name)


def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()


has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\n\nb: Read current sensor (mA)') # '\t' is a tab
    print('c: Read encoder (counts)  \td: Read encoder (degrees) \te: Reset encoder (to zero)')
    print('f: Set PWM (-100 to 100)')
    print('g: Set current gains \th: Get current gains \tk: Test current gains')
    print('i: Set position gains \tj: Get position gains \tl: Go to angle (deg)')
    print('m: Load step trajectory \tn: Load cubic trajectory \to:Execute trajectory')
    print('p: Unpower Motor  \tr: Get mode \tq: Quit')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()) # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'b'): # Get current (mA)
        n_str = ser.read_until(b'\n')
        n_float = float(n_str) 
        print('The motor current is ' + str(n_float) + ' mA\n')

    elif (selection == 'c'): # Read encoder (counts)
        n_str = ser.read_until(b'\n')
        n_int = int(n_str)
        print('The motor angle is ' + str(n_int) + ' counts\n')

    elif (selection == 'd'): # Read encoder (degrees)
        n_str = ser.read_until(b'\n')
        n_float = float(n_str)
        print('The motor angle is ' + str(n_float) + ' degrees\n')

    elif (selection == 'e'): # Reset encoder
        print('Motor position reset to zero.\n')

    elif (selection == 'f'):
        n_str = input('What PWM value would you like (-100 to 100)? \n')
        n_int = int(n_str)
        ser.write((str(n_int) + '\n').encode())
        if n_int > 0:
            print('PWM has been set to ' + n_str + '%' + ' in the counterclockwise direction.')
        elif n_int < 0:
            print('PWM has been set to ' + str(abs(n_int)) + '%' + ' in the clockwise direction.')

    elif (selection == 'g'):
        # set gains
        kp_str = input('Enter your desired Kp current gain: ') # get the number to send
        kp_float = float(kp_str) # turn it into an float
        ki_str = input('Enter your desired Ki current gain: ')
        ki_float = float(ki_str)
        print('Sending Kp = ' + str(kp_float) + ' and Ki = ' + str(ki_float) + ' to the current controller.\n') # print it to the screen to double check

        ser.write((str(kp_float) + ' ' + str(ki_float)+'\n').encode()) # send the number

    elif (selection == 'h'): # get current gains
        n_str = ser.read_until(b'\n').decode()
        m_str = ser.read_until(b'\n').decode()
        print('The current controller is using Kp = ' + n_str + ' and Ki = ' + m_str + '.\n')

    elif (selection == 'i'):
        # set gains
        kp_str = input('Enter your desired Kp position gain: ') # get the number to send
        kp_float = float(kp_str) # turn it into an float
        ki_str = input('Enter your desired Ki position gain: ')
        ki_float = float(ki_str)
        kd_str = input('Enter your desired Kd position gain: ')
        kd_float = float(kd_str)
        print('Sending Kp = ' + str(kp_float) + ', Ki = ' + str(ki_float) + ', and Kd = ' + str(kd_float) + ' to the position controller.\n') # print it to the screen to double check

        ser.write((str(kp_float) + ' ' + str(ki_float) + ' ' + str(kd_float) + '\n').encode()) # send the number

    elif (selection == 'j'): # get current gains
        n_str = ser.read_until(b'\n').decode()
        m_str = ser.read_until(b'\n').decode()
        l_str = ser.read_until(b'\n').decode()
        print('The position controller is using Kp = ' + n_str + ', Ki = ' + m_str + ' and Kd = ' + l_str)

    elif (selection == 'k'): #test current gains
        read_plot_matrix()

    elif (selection == 'l'): # set angle
        d_angle_s = input('Enter the desired motor angle in degrees: ')
        d_angle_f = float(d_angle_s)
        ser.write((str(d_angle_f) + '\n').encode())
        print('Motor moving to ' + str(d_angle_f) + ' degrees.')

    elif (selection == 'm'):
        ref = genRef('step')
        #print(len(ref))
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        # send 
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())

    elif (selection == 'n'):
        ref = genRef('cubic')
        #print(len(ref))
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        # send 
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())

    elif (selection == 'o'):
        read_plot_matrix()

    elif (selection == 'p'):
        print("Motor set to IDLE")

    elif (selection == 'r'):
        n_str = ser.read_until(b'\n').decode()  # get the mode
        print('Mode: ' + n_str + '\n')

    elif (selection == 'x'):
        # add 2 ints
        n_str = input('Enter first number: ') # get the number to send
        n_int = int(n_str) # turn it into an int
        m_str = input('Enter second number: ')
        m_int = int(m_str)
        print('numbers: ' + str(n_int) + ' and ' + str(m_int)) # print it to the screen to double check

        ser.write((str(n_int) + ' ' + str(m_int)+'\n').encode()) # send the number
        n_str = ser.read_until(b'\n')  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Got back: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True # exit client
        # be sure to close the port
        ser.close()

    else:
        print('Invalid Selection ' + selection_endline)