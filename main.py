import serial as ser
import time
import PySimpleGUI as sg
import numpy as np
import matplotlib.pyplot as plt
import os
from os import path


def convert_function_line_to_hex(line):
    # Define a dictionary to map function names to function numbers
    function_map = {
    'inc_lcd': 0x01,
    'dec_lcd': 0x02,
    'rra_lcd': 0x03,
    'set_delay': 0x04,
    'clear_lcd': 0x05,
    'servo_deg': 0x06,
    'servo_scan': 0x07,
    'sleep': 0x08,
    }

    # Split the line into function name and values (if any)
    parts = line.strip().split()
    function_name = parts[0]
    values = parts[1:]

    # Get the function number from the dictionary, defaulting to 0 if not found
    function_number = function_map.get(function_name, 0)

    # Handle servo_scan values (comma-separated strings)
    if function_name == 'servo_scan':
        # Join the values with an empty string, as they are already in the correct format
        numbers = values[0].split(',')
        number1 = [numbers[0]]
        number2 = [numbers[1]]
        formatted_values1 = [format(int(val), '02X') for val in number1]
        formatted_values2 = [format(int(val), '02X') for val in number2]
        formatted_values = formatted_values1 + formatted_values2
        formatted_values = ''.join(formatted_values)
    else:
        # Format each value in the values list as a two-digit hexadecimal number
        formatted_values = [format(int(val), '02X') for val in values]
        formatted_values = ''.join(formatted_values)

    # Combine function number and formatted values into the final hexadecimal representation
    hex_representation = format(function_number, '02X') + formatted_values

    if not values == []:
        hex_representation = hex_representation.replace(',','')
        print(hex_representation)
    return hex_representation + '\r\n'


def text_to_str(file_content_C):
    lines = file_content_C.split('\n') # Split the file_content into lines
    # Convert each line to hexadecimal representation
    hex_values = [convert_function_line_to_hex(line) for line in lines]
    hex_values = ''.join(hex_values)
    return hex_values


def filter_objects(arr1, arr2):
    for i in range(len(arr2)):
        if arr2[i] != 0:
            if 5 < i < len(arr2) - 5:
                for j in range(i-6, i+6):
                    arr1[j] = 0
            elif i <= 5:
                for j in range(i+6):
                    arr1[j] = 0
            else:
                for j in range(i-6, len(arr1)):
                    arr1[j] = 0
    return arr1


def process_script_data(lst):
    left_angle = lst[0]
    right_angle = lst[-1]
    lst = lst[1:-1]
    lst2 = [0] * 61
    for i in range(left_angle // 3, right_angle // 3):
        lst2[i] = lst[i - left_angle // 3]
    return process_ultrasonic_samples(lst2)


def filter_objects(arr1, arr2):
    for i in range(len(arr2)):
        if arr2[i] != 0:
            if 5 < i < len(arr2) - 5:
                for j in range(i-6, i+6):
                    arr1[j] = 0
            elif i <= 5:
                for j in range(i+6):
                    arr1[j] = 0
            else:
                for j in range(i-6, len(arr1)):
                    arr1[j] = 0
    return arr1


def process_script_data(lst):
    left_angle = lst[0]
    right_angle = lst[-1]
    lst = lst[1:-1]
    lst2 = [0] * 61
    for i in range(left_angle // 3, right_angle // 3):
        lst2[i] = lst[i - left_angle // 3]
    return process_ultrasonic_samples(lst2)


def process_ultrasonic_samples(arr):
    default_value = 0
    size = len(arr)
    final_arr = [default_value] * size
    threshold = 10
    i = 0
    arr = arr + [1000, 1000, 1000]
    print(len(arr))

    while i < len(arr)-3:

        pointer1 = arr[i]
        if pointer1 == 0:
            i += 1
            continue
        tmp_threshold = int(threshold + 0.1 * pointer1)
        count_good = 1
        count_bed = 3
        sum_arr = pointer1
        for j in range(i+1, len(arr)):        # j is running from the index we start the search until his break
            pointer2 = arr[j]
            if abs(pointer2 - pointer1) < tmp_threshold:  # we don't have a jump in the values
                count_bed = 3
                sum_arr += pointer2
                count_good += 1

            else:
                count_bed -= 1
                if count_bed == 0:
                    j -= 3  # go back to the index that not belong to our session
                    if count_good > 2:
                        index = (j + i) // 2
                        final_arr[index] = sum_arr // count_good
                    i = j + 1  # the next index after our last session
                    break

    return final_arr


def create_costume_array_telemeter(input_array, size):
    custome_array = [0] * size
    input_array[0] = round(input_array[0] / 3)
    custome_array[input_array[0]] = input_array[1]
    return custome_array

def create_costume_array(input_array, size):
    custome_array = [0] * size
    i = 0
    while (i < len(input_array)):
        custome_array[input_array[i]] = input_array[i + 1]
        i += 2
    return custome_array


def process_ultrasonic_samples(arr):
    default_value = 0
    size = len(arr)
    final_arr = [default_value] * size
    threshold = 5
    i = 0
    arr = arr + [1000, 1000]

    while i < len(arr)-3:

        pointer1 = arr[i]
        if pointer1 == 0:
            i += 1
            continue
        tmp_threshold = int(threshold + 0.1 * pointer1)
        count_good = 1
        count_bed = 2
        sum_arr = pointer1
        for j in range(i + 1, len(arr)):        # j is running from the index we start the search until his break
            pointer2 = arr[j]
            if abs(pointer2 - pointer1) < tmp_threshold:  # we don't have a jump in the values
                count_bed = 2
                sum_arr += pointer2
                count_good += 1

            else:
                count_bed -= 1
                if count_bed == 0:
                    j -= 2  # go back to the index that not belong to our session
                    if count_good > 2:
                        index = (j + i) // 2
                        final_arr[index] = sum_arr // count_good
                    i = j + 1  # the next index after our last session
                    break

    return final_arr


def plot_one_array(arr1, source):  # source is "light" or "object"
    color1 = 'k'
    label1 = ' '
    if source == "light":
        color1 = 'y'
        label1 = 'Light source'
    elif source == "object":
        color1 = 'k'
        label1 = 'Object'

    # Filter out 0 values and corresponding theta values
    radii1 = np.array(arr1)
    theta = np.linspace(0, np.pi, len(radii1))
    mask = radii1 != 0
    radii1 = radii1[mask]
    theta = theta[mask]

    # Create the figure and axes for the polar plot
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})

    # Plot the data as radial lines with different colors
    ax.plot(theta, radii1, marker='o', linestyle=' ', color=color1, label=label1)

    # Set the labels for the radial grid lines
    #max_radius = max(arr1)
    # ax.set_rticks(np.arange(0, max_radius + 1, 5), labels=np.arange(0, max_radius + 1, 5))

    # Set the title of the plot
    ax.set_title("Sonar Grid (180 degrees)")

    # Set the limit for theta to 180 degrees
    ax.set_thetamin(0)
    ax.set_thetamax(180)

    # Add labels for each data point
    for i, radius in enumerate(radii1):
        angle = theta[i] * 180 / np.pi  # Convert radians to degrees
        ax.text(theta[i], radius, f"{angle:.0f}° {radius}", ha='center', va='bottom')

    # Show the legend
    ax.legend()

    # Show the plot
    plt.show(block=False)
    plt.pause(0.1)


def plot_two_arrays(arr1, arr2):

    # Create an array of angles from 0 to 180 degrees
    theta = np.linspace(0, 180, len(arr1))

    # Create a mask for non-zero values in arr1
    mask1 = np.array(arr1) != 0
    # Filter out zero values in arr1 and corresponding theta values
    radii1 = np.array(arr1)[mask1]
    theta1 = theta[mask1]

    # Create a mask for non-zero values in arr2
    mask2 = np.array(arr2) != 0
    # Filter out zero values in arr2 and corresponding theta values
    radii2 = np.array(arr2)[mask2]
    theta2 = theta[mask2]

    # Create the figure and axes for the polar plot
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})

    # Plot the data as radial lines with different colors
    ax.plot(np.radians(theta1), radii1, marker='o', linestyle=' ', color='k', label='Objects')
    ax.plot(np.radians(theta2), radii2, marker='o', linestyle=' ', color='y', label='Light Source')

    # Set the title of the plot
    ax.set_title("Sonar Grid (180 degrees)")

    # Set the limit for theta to 180 degrees
    ax.set_thetamin(0)
    ax.set_thetamax(180)

    # Add labels for each data point in arr1
    for i, radius in enumerate(radii1):
        angle = theta1[i]
        ax.text(np.radians(angle), radius, f"{angle:.0f}° {radius}", ha='center', va='bottom')

    # Add labels for each data point in arr2
    for i, radius in enumerate(radii2):
        angle = theta2[i]
        ax.text(np.radians(angle), radius, f"{angle:.0f}° {radius}", ha='center', va='bottom')

    # Show the legend
    ax.legend()

    # Show the plot
    plt.show(block=False)


class PortError(Exception):
    """Raised when the port not found"""
    pass


# Automatic PORT search
def port_search(between=None):
    # find the right com that connect between the pc and controller
    ports = serial.tools.list_ports.comports()
    for desc in sorted(ports):
        if "MSP430" in desc.description:
            return desc.device
    raise PortError


def show_window(layout_num, window):
    for i in range(1, 7):
        if i == layout_num:
            window[f'COL{layout_num}'].update(visible=True)
        else:
            window[f'COL{i}'].update(visible=False)


def send_state(message=None, file_option=False):
    s.reset_output_buffer()
    if file_option:
        bytesMenu = message

    elif isinstance(message, int):
        bytesMenu = message.to_bytes(1, 'big')

    else:
        bytesMenu = bytes(message, 'ascii')
    s.write(bytesMenu)


def read_from_MSP(state):
    print(" Im in read massage ")
    finish_flag = False
    final_message = []
    while True:
        while s.in_waiting > 0:  # while the input buffer isn't empty
            if (state == 'Objects') or (state == 'light') or (state == 'both') or (state == 'script'):
                Ob_dist = int.from_bytes(s.read(1), byteorder='big')
                if Ob_dist == 250:
                    finish_flag = True
                    break

            final_message.append(Ob_dist)
            print(final_message)
            print(len(final_message))
            time.sleep(0.1)

        if finish_flag:
            s.read_all()
            return final_message


def GUI():
    global enableTX
    global Objects_dist
    global LDR_dist
    global Telemeter_list
    global plot_list
    global Both_dist
    burn_flag = False

    sg.theme('Reddit')

    layout_main = [
        [sg.Text("DCS Final Project - Ron and Matan", size=(40, 2), justification='center', font='Helvetica 17')],
        [sg.Button("Objects Detector System", key='_ObjectDetector_', size=(35, 2), font='Helvetica 17')],
        [sg.Button("Telemeter", key='_Telemeter_', size=(35, 2), font='Helvetica 17')],
        [sg.Button("Light Source Detector System", key='_LightDetector_', size=(35, 2), font='Helvetica 17')],
        [sg.Button("Light Source and Objects Detector System", key='_LightObjectsDetector_', size=(35, 2), font='Helvetica 17')],
        [sg.Button("Script Mode", key='_Script_', size=(35, 2), font='Helvetica 17')],
        [sg.Button("LDR Calibration", key='_Calibration_', size=(35, 2), font='Helvetica 17')]
    ]

    layout_objectdetector = [
        [sg.Text("Objects Detector System", size=(35, 2), justification='center', font='Helvetica 17')],
        [sg.Button("Start Scan", key='_Scan_', size=(18, 1), font='Helvetica 17')],
        [sg.Text('Please enter the maximum distance in cm (default - 450[cm])', font = 'Helvetica 13'), sg.Input(key='-MAXDIST-')],
        [sg.Button("Submit", key='_Submit_0', size=(18, 1), font='Helvetica 17')],
        [sg.Button("Back", key='_BackMenu_', size=(5, 1), font='Helvetica 17', pad=(300, 180))]
    ]

    layout_telemeter = [
        [sg.Text("Telemeter", size=(35, 2), justification='center', font='Helvetica 17')],
        [sg.Text('Please enter the wanted degree'), sg.Input(key= '-NUMBER-')],
        [sg.Button("Submit", key='_Submit_1', size=(18, 1), font='Helvetica 17')],
        [sg.Button("Back", key='_BackMenu_', size=(5, 1), font='Helvetica 17', pad=(300, 180))]
    ]

    layout_lightdetector = [
        [sg.Text("Light Source Detector System", size=(35, 2), justification='center', font='Helvetica 17')],
        [sg.Button("Start Scan", key='_Scan1_', size=(18, 1), font='Helvetica 17')],
        [sg.Button("Back", key='_BackMenu_', size=(5, 1), font='Helvetica 17', pad=(300, 180))]
    ]

    layout_lightdbjectsdetector = [
        [sg.Text("Light Source and Objects Detector System", size=(35, 2), justification='center', font='Helvetica 17')],
        [sg.Button("Start Scan", key='_Scan2_', size=(18, 1), font='Helvetica 17')],
        [sg.Text('Please enter the maximum distance for Objects to detect in cm (default - 450[cm])', font='Helvetica 10'), sg.Input(key='-MAXDIST1-')],
        [sg.Button("Submit", key='_Submit_2', size=(18, 1), font='Helvetica 17')],
        [sg.Button("Back", key='_BackMenu_', size=(5, 1), font='Helvetica 17', pad=(300, 180))]
    ]

    file_viewer = [[sg.Text("File Folder", font='Helvetica 17'),
                    sg.In(size=(20, 1), enable_events=True, key='_Folder_'),
                    sg.FolderBrowse()],
                   [sg.Listbox(values=[], enable_events=True, size=(40, 23), key="_FileList_")],
                   [sg.Button('Back', key='_BackMenu_', size=(5, 1), font='Helvetica 17'),
                    sg.Button('Write to Flash', key='_Write_', size=(15, 1), font='Helvetica 17')],
                    [sg.Text(' ', key='_ACK_', size=(35, 1), font='Helvetica 10')]]

    file_description = [
        [sg.Text("File Description", size=(35, 1), justification='center', font='Helvetica 17')],
        [sg.Text(size=(42, 1), key="_FileHeader_", font='Helvetica 10')],
        [sg.Multiline(size=(42, 15), key="_FileContent_")],
        [sg.HSeparator()],
        [sg.Text("Executed List", size=(35, 1), justification='center', font='Helvetica 17')],
        [sg.Listbox(values=[], enable_events=True, size=(42, 4), key="_ExecutedList_")],
        [sg.Button('Execute', key='_Execute_', size=(17, 1), font='Helvetica 17'),
        sg.Button('Clear', key='_Clear_', size=(17, 1), font='Helvetica 17')]
    ]

    layout_script = [[sg.Column(file_viewer),
                      sg.VSeparator(),
                      sg.Column(file_description)]
                     ]

    layout = [[sg.Column(layout_main, key='COL1', ),
               sg.Column(layout_objectdetector, key='COL2', visible=False),
               sg.Column(layout_telemeter, key='COL3', visible=False),
               sg.Column(layout_lightdetector, key='COL4', visible=False),
               sg.Column(layout_lightdbjectsdetector, key='COL5', visible=False),
               sg.Column(layout_script, key='COL6', visible=False)]]

    global window
    # Create the Window
    window = sg.Window(title='Light source and object proximity detector system', element_justification='c', layout=layout,
                       size=(700, 600))
    execute_list = []
    empty_list = []
    file_name = ''
    file_size = ''
    file_path = ''
    while True:
        s.read_all()
        event, values = window.read()
        if event == "_Calibration_":
            send_state('C')

        if event == "_ObjectDetector_":
            send_state('O')  # Objects Detector
            show_window(2, window)
            while "_BackMenu_" not in event:
                event, values = window.read()
                if event == "_Submit_0":
                    send_state((int(values['-MAXDIST-'])//2))
                if event == "_Scan_":
                    send_state(226)
                    #time.sleep(5)          ## it takes 20 - 25 sec to scan
                    Objects_dist = read_from_MSP('Objects')
                    Objects_dist = [x * 2 for i, x in enumerate(Objects_dist)]
                    print(Objects_dist)
                    Objects_dist = process_ultrasonic_samples(Objects_dist)
                    print(Objects_dist)
                    plot_one_array(Objects_dist, 'object')

        if event == "_Telemeter_":
            send_state('T')
            show_window(3, window)
            while "_BackMenu_" not in event:
                event, values = window.read()
                if event == "_Submit_1":
                    send_state(int(values['-NUMBER-']))
                    Telemeter_list = read_from_MSP('Objects')
                    Telemeter_list[1] *= 2
                    Telemeter_list[0] = (int(values['-NUMBER-']))
                    Telemeter_list = create_costume_array_telemeter(Telemeter_list, 61)
                    s.read_all()
                    print(Telemeter_list)
                    plot_one_array(Telemeter_list, 'object')

        if event == "_LightDetector_":
            send_state('L')  # Calibration
            show_window(4, window)
            while "_BackMenu_" not in event:
                event, values = window.read()
                if "_Scan1_" in event:
                    send_state(226)  # Start light scan
                    LDR_dist = read_from_MSP('light')
                    print(LDR_dist)
                    LDR_dist = [x * 2 for i, x in enumerate(LDR_dist)]
                    print(LDR_dist)
                    LDR_dist = process_ultrasonic_samples(LDR_dist)
                    print(LDR_dist)
                    plot_one_array(LDR_dist, 'light')

        if event == "_LightObjectsDetector_":
            send_state('D')  # Objects Detector
            show_window(5, window)
            while "_BackMenu_" not in event:
                event, values = window.read()
                if event == "_Submit_2":
                    send_state((int(values['-MAXDIST1-'])//2))
                if event == "_Scan2_":
                    send_state(226)
                    Both_dist = read_from_MSP('both')
                    print(Both_dist)
                    Both_dist = [x * 2 for i, x in enumerate(Both_dist)]
                    Objects_dist = Both_dist[0:122:2]
                    LDR_dist = Both_dist[1:122:2]
                    print(Objects_dist)
                    Objects_dist = process_ultrasonic_samples(Objects_dist)
                    print(Objects_dist)
                    print(LDR_dist)
                    LDR_dist = process_ultrasonic_samples(LDR_dist)
                    print(LDR_dist)
                    Objects_dist = filter_objects(Objects_dist, LDR_dist)
                    plot_two_arrays(Objects_dist,LDR_dist)

        if event == "_Script_":
            burn_index = 0
            send_state('S')  # Script
            show_window(6, window)

        if event == '_Folder_':
            selected_folder = values['_Folder_']
            try:
                window["_FileContent_"].update('')
                window["_FileHeader_"].update('')
                file_path = ''
                file_list = os.listdir(selected_folder)
            except Exception as e:
                file_list = []
            files_name = [f for f in file_list if
                      os.path.isfile(os.path.join(selected_folder, f)) and f.lower().endswith(".txt")]
            window["_FileList_"].update(files_name)

        if event == '_FileList_':
            try:
                file_path = os.path.join(values["_Folder_"], values["_FileList_"][0])
                file_size = path.getsize(file_path)
                try:
                    with open(file_path, "rt", encoding='utf-8') as f:
                        file_content = f.read()
                except Exception as e:
                    print("Error: ", e)
                file_name = values["_FileList_"][0]
                window["_FileHeader_"].update(f"File name: {file_name} | Size: {file_size} Bytes")
                window["_FileContent_"].update(file_content)
            except Exception as e:
                print("Error: ", e)
                window["_FileContent_"].update('')

        if event == '_Write_':
            send_state(f"{file_name}\n")
            execute_list.append(f"{file_name}")
            time.sleep(0.1)
            try:
                with open(file_path, "rb") as f:  # Open file in binary read mode
                    file_content_b = f.read()
                    tmp = file_content_b.decode('utf-8')
                    file_content_b = text_to_str(tmp)
                    file_content_b = file_content_b[:-2]
                    file_content_b = bytes(file_content_b, 'utf-8')
            except Exception as e:
                print("Error: ", e)
            send_state(file_content_b + bytes('Z', 'utf-8'), file_option=True)
            print(file_content_b)
            # message_send(file_content_b[start:finish], file=True)
            print(file_name)
            print(f"{file_size}")
            # time.sleep(0.1)
            time.sleep(0.5)
            #  send_state('Q')
            if (burn_index == 0):
                ptr_state = 'a'
            elif (burn_index == 1):
                ptr_state = 'b'
            elif (burn_index == 2):
                ptr_state = 'd'
            #    send_state(str(burn_index))  # send burn index - 0,1,2
            burn_index += 1
            send_state(ptr_state)
            try:
                while True:
                    while s.in_waiting > 0:  # while the input buffer isn't empty
                        burn_ack = int.from_bytes(s.read(1), byteorder='big')
                        if burn_ack == 4:
                            burn_flag = True
                            break
                    if burn_flag:
                        break
            except:
                print("error")
            if burn_ack == 4: # EOT
                window['_ACK_'].update('"'+file_name+'"'+' burned successfully!')
                window.refresh()
                print(burn_ack)
            print("burn file index: " + ptr_state)
            time.sleep(0.3)
            window['_ExecutedList_'].update(execute_list)

        if event == '_ExecutedList_':
            file_name_to_execute = values["_ExecutedList_"][0]
            file_index = execute_list.index(file_name_to_execute)  # for send state - 0,1,2
            print(file_index)
            if (file_index == 0):
                exe_state = 'L'
            elif (file_index == 1):
                exe_state = 'U'
            elif (file_index == 2):
                exe_state = 'V'

        if event == '_Execute_':
            if file_name == 'Script3.txt':
                i = 4
            else:
                i = 5
            send_state(exe_state)
            s.reset_input_buffer()
            s.reset_output_buffer()
            window['_FileHeader_'].update('"' + file_name_to_execute + '"' + " execute window")
            while(i > 0):
                plot_list = read_from_MSP('script')
                if len(plot_list) == 2:
                    plot_list[1] *= 2
                    plot_list = create_costume_array_telemeter(plot_list, 61)
                else:
                    plot_list = [x * 2 if 1 <= i <= len(plot_list) - 2 else x for i, x in enumerate(plot_list)]
                    plot_list = process_script_data(plot_list)
                plot_one_array(plot_list, 'object')
                s.read_all()
                i = i - 1

        if event == '_Clear_':
            window['_ExecutedList_'].update(empty_list)
            window['_ACK_'].update(' ')

        if event == sg.WIN_CLOSED:
            break

        if event is not None and "_BackMenu_" in event:
            send_state(255)
            show_window(1, window)
        if event is not None and "_BackScript_" in event:
            show_window(5, window)
    window.close()


if __name__ == '__main__':
    s = ser.Serial('COM1', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=0)  # timeout of 1 sec so that the read and write operations are blocking,
    # after the timeout the program continues
    enableTX = True
    s.flush()  ## clear ports
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()
    Objects_dist = []
    LDR_dist = []
    Both_dist = []
    Telemeter_list = [0,0]
    plot_list = []

    GUI()
