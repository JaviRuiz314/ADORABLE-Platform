#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_PWMServoDriver.h>
#include "minikame.h"

// Wifi Access Point configuration
const char *ssid = "Orange-8EE0";
const char *password = "E2242365";
IPAddress server(192, 168, 1, 107);

//Pins
int sensorIRPin[4] = {13, 12, 14, 27};

//FUNCTIONS
void sendHelloMessage();
void lineFollowerTask();
void decideDirection(int sensorIRArray[]);
void checkCentralLine();
void turnRight();
void turnLeft();
void readCode();
int parseOrder(String command);
String receiveHelloMessage();
String waitResponse();
String sendMessage(char message[]);

MiniKame robot;
WiFiClient client;
int codeBar[3];
char message[255];
String destination;
String input;

//STATUS
int communitcationStatus = 0;
int status = 1;

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    do
    {
        Serial.println("Connecting to WiFi..");
    } while (WiFi.status() != WL_CONNECTED);
    Serial.println("Connected to WiFi");
    bool serverConnection = false;
    while (serverConnection == false)
    {

        Serial.println("Trying to connect with server...");
        serverConnection = client.connect(server, 50007);
    }

    robot.init();
}

void loop()
{

    String response;
    String command;
    String actualCode;
    int order = 0;
    bool newConn = true;
    while (client.connected())
    {
        switch (status)
        {
        case 1:
        {
            if (newConn)
            {
                Serial.println("Sending Hello message...");
                char message[255] = "HELLO/COMMADS/WALK/DANCE";
                response = sendMessage(message);
                newConn = false;
            }
            else
            {
                char message[255] = "HELLO/";
                response = sendMessage(message);
            }

            destination = response.substring(6, response.length());
            delay(1000);
            status = 2;
            communitcationStatus = 0;
            break;
        }

        case 2:
        {
            while (communitcationStatus == 0)
            {
                lineFollowerTask();
            }
            switch (communitcationStatus)
            {
            case 1:
            {
                readCode();
                communitcationStatus = 2;
                break;
            }

            case 2:
            {
                sprintf(message, "POSITION/%d%d%d%d", codeBar[0], codeBar[1], codeBar[2], codeBar[3]);
                response = sendMessage(message);
                command = response.substring(8, response.length());
                communitcationStatus = 3;
                delay(3000);
                break;
            }

            case 3:
            {
                order = parseOrder(command);
                command = "";
                break;
            }
            }
            break;
        }

        case 3:
        {
            switch (order)
            {
            case 1:
            {
                robot.walk(2, 1200);
                communitcationStatus = 0;
                status = 2;
                break;
            }

            case 2:
            {
                robot.dance(5, 1200);
                communitcationStatus = 2;
                status = 1;
                break;
            }            
            case 3:
            {
                Serial.println("Orden no disponible");
                break;
            }
            }
        break;
        }
        }
    }
}

void lineFollowerTask()
{
    int sensorIRArray[4];
    for (int i = 0; i <= 3; i++)
    {
        sensorIRArray[i] = digitalRead(sensorIRPin[i]);
    };
    if (sensorIRArray[0] == 0 && sensorIRArray[3] == 0)
    {
        checkCentralLine();
    }
    else if (sensorIRArray[0] == 1 || sensorIRArray[3] == 1)
    {
        decideDirection(sensorIRArray);
    }
};

void checkCentralLine()
{
    int rightSensorPin = digitalRead(sensorIRPin[1]);
    int leftSensorPin = digitalRead(sensorIRPin[2]);
    if (rightSensorPin != 1 || leftSensorPin != 1)
    {
        if (rightSensorPin != 1)
        {
            while (rightSensorPin != 1)
                ;
            {
                robot.turnR(1, 1200);
                robot.walk(1, 1200);
                rightSensorPin = digitalRead(sensorIRPin[1]);
            }
        }
        else if (leftSensorPin != 1)
        {
            while (leftSensorPin != 1)
            {
                robot.turnL(1, 1200);
                robot.walk(1, 1200);
                leftSensorPin = digitalRead(sensorIRPin[2]);
            }
        }
    }
    else
    {
        robot.walk(1, 1200);
    }
}

void decideDirection(int sensorIRArray[])
{
    if (sensorIRArray[0] == 1 && sensorIRArray[3] == 0)
    {
        do
        {
            robot.turnL(3, 1200);
            sensorIRArray[3] = digitalRead(sensorIRPin[3]);
        } while (sensorIRArray[3] != 1);
    }
    else if (sensorIRArray[0] == 0 && sensorIRArray[3] == 1)
    {
        do
        {
            robot.turnR(3, 1200);
            sensorIRArray[0] = digitalRead(sensorIRPin[0]);
        } while (sensorIRArray[0] != 1);
    }
    if (sensorIRArray[0] == 1 && sensorIRArray[3] == 1)
    {
        communitcationStatus = 1;
    }
}

void readCode()
{
    int position = 0;

    while (position <= 4)
    {
        while (digitalRead(sensorIRPin[3]) == 1)
        {
            checkCentralLine();
        }
        if (digitalRead(sensorIRPin[3]) == 0)
        {
            codeBar[position] = digitalRead(sensorIRPin[0]);
            position++;
        }
        while (digitalRead(sensorIRPin[3]) == 0)
        {
            checkCentralLine();
        }
    }
}

String sendMessage(char message[])
{
    String responseReceived = "";
    int count = 0;
    for (count = 0; count <= 5; count++)
    {
        client.write(message);
        client.flush();
        responseReceived = waitResponse();
        if (responseReceived.length() != 0)
        {
            break;
        }
    }
    if (count == 5)
    {
        Serial.println("ERROR EN LA RECEPCION DE MENSAJE");
        robot.dance(20, 1200);
    }
    else
    {
        client.write("ACK/");
        client.flush();
    }
    return responseReceived;
}

String waitResponse()
{
    int initTime = millis();
    bool timeout = false;
    String message = "";
    while (!timeout && message.length() <= 0)
    {
        message = client.readStringUntil('\n');
        Serial.println(message);
        Serial.println(message.length());
        int clock = millis() - initTime;
        if (clock >= 10000)
        {
            timeout = true;
        }
    }
    if (message.length() > 0)
    {
        return message;
    }
    else
    {
        return "";
    }
}

int parseOrder(String command)
{
    int order;
    status = 3;

    if (command == "WALK")
    {
        return order = 1;
    }
    else if (command == "DANCE")
    {
        return order = 2;
    }
    else
    {
        return order = 3;
    }
}