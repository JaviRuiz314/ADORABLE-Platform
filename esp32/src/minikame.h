#ifndef minikame_h
#define minikame_h


#include <Octosnake.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN  140
#define SERVOMAX  480

class MiniKame{
public:
    void init();
    void run(float steps, int period);
    void walk(float steps, int period);
    void omniWalk(float steps, int T, bool side, float turn_factor);
    //void backward(float steps, int period);
    void turnL(float steps, int period);
    void turnR(float steps, int period);
    void moonwalkL(float steps, int period);
    void dance(float steps, int period);
    void upDown(float steps, int period);
    void pushUp(float steps, int period);
    void followLine();
    void hello();
    void jump();
    void home();
    void zero();
    void frontBack(float steps, int period);

    void setServo(int id, float target);
    void reverseServo(int id);
    float getServo(int id);
    void moveServos(int time, float target[8]);

private:
    Oscillator oscillator[8];
    Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
    int trim[8];
    bool reverse[8];
    unsigned long _init_time;
    unsigned long _final_time;
    unsigned long _partial_time;
    float _increment[8];
    float _servo_position[8];
    float output[8];
    float angle[8];

    int angToUsec(float value);
    void execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]);
};

#endif
