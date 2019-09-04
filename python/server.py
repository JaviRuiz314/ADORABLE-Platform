# Echo server program
import socket
import time


class Robot:

    def __init__(robot):
        """Create a new robot"""
        robot.serial_code = ""
        robot.destination = ""
        robot.address = 0


def set_up_database():
    database = {
        '0001': "Despacho 01",
        '0011': "Despacho 02",
        '0100': "Despacho 03",
        '0101': "Despacho 04",
        '0110': "Despacho 05",
        '0111': "Despacho 06",
        '1000': "Despacho 07",
        '1001': "Despacho 08",
        '1010': "Despacho 09",
        '1011': "Despacho 10"
    }
    return database


def set_up_robot_config(robot, serial_code, address):
    robot.serial_code = serial_code
    robot.address = address


def receive_message():
    data = ""
    while len(data) == 0:
        data = conn.recv(255)
    data = data.decode()
    return data


def process_message(incoming_packet):
    fields = incoming_packet.split('/')
    header = fields[0]

    if header == "HELLO":
        return True
    elif header == "POSITION":
        return False


def wait_for_ACK():
    tick = time.time()
    tack = tick + 3000
    ack_received = False
    while time.time() < tack and ack_received == False :
        data = conn.recv(255)
        data = data.decode()
        print(data)
        if data == "ACK/":
            ack_received = True

    return ack_received


def show_menu(incoming_packet):
    command_list = incoming_packet.split('/')
    if len(command_list) > 2:
        number_of_commands = len(command_list) - 2
        order = 1
        print("Orders available for the robot \n")
        while order <= number_of_commands:
            print(str(order) + ": " + command_list[order] + " \n")
            order = order + 1


def identify_code(code_bar):
    for code, position in dataBase.items():
        if code == code_bar:
            return position


def process_code(incoming_packet):
    fields = incoming_packet.split("/")
    code_bar = fields[1]
    position = identify_code(code_bar)
    print("The robot has reade the code: " + code_bar + " it is on the location: " + position + "\n")
    if position == robot01.destination:
        order = "DANCE"
    else:
        order = "WALK"
    return order


def identify_destination(destination):
    for code, position in dataBase.items():
        if position == destination:
            return True
    return False


def input_new_order():
    print("Select a new order: ")
    user_order = input()
    return user_order


def input_new_destination():
    print("Select a new destiny, write shutdown to close server \n")
    print("New destiny: ")
    valid_destination = False
    while valid_destination is False:
        destination = input()
        valid_destination = identify_destination(destination)
        if valid_destination is False:
            print("Select a valid destiny")
    return destination


def send_message(header, body):
    count = 0
    packet = header + "/" + body
    packet = packet.encode()
    while count < 5:
        conn.send(packet)
        ack_received = wait_for_ACK()
        if ack_received is False:
            count = count + 1
        else:
            break
    if count == 5:
        print("ERROR: Missing ACK")


HOST = ''   # Symbolic name meaning all available interfaces
PORT = 50007        # Arbitrary non-privileged port

dataBase = set_up_database()

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(1)
    print("Socket listening")
    conn, addr = s.accept()

    with conn:
        print('Connected by', addr)
        robot01 = Robot()
        set_up_robot_config(robot01, "robot_Kame_01", addr)
        print("Welcome to ADORABLE platform")
        while True:
            print("Waiting for message...")
            message = receive_message()
            new_connection = process_message(message)
            if new_connection is True:
                show_menu(message)
                robot01.destination = input_new_destination()
                if robot01.destination == "shutdown":
                    socket.close()
                send_message("HELLO", robot01.destination)
            else:
                command = process_code(message)
                if robot01.destination == "manual_control":
                    command = input_new_order()
                send_message("COMMAND", command)

