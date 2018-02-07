#include "api.h"
#include "uart.h"

const int leftMotor1 = 1, leftMotor2 = 2;
const int rightMotor1 = 3, rightMotor2 = 4;
const motor_brake_mode_e brakeMode = E_MOTOR_BRAKE_COAST;
const motor_encoder_units_e encMode = E_MOTOR_ENCODER_ROTATIONS;

void initMotors() {
  motor_set_position(leftMotor1, 0);
  motor_set_position(leftMotor2, 0);
  motor_set_position(rightMotor1, 0);
  motor_set_position(rightMotor2, 0);

  motor_set_brake_mode(leftMotor1, brakeMode);
  motor_set_brake_mode(leftMotor2, brakeMode);
  motor_set_brake_mode(rightMotor1, brakeMode);
  motor_set_brake_mode(rightMotor2, brakeMode);

  motor_encoder_set_units(leftMotor1, encMode);
  motor_encoder_set_units(leftMotor2, encMode);
  motor_encoder_set_units(rightMotor1, encMode);
  motor_encoder_set_units(rightMotor2, encMode);
}

void setDrive(int16_t leftVel, int16_t rightVel) {
  motor_set_velocity(leftMotor1, leftVel);
  motor_set_velocity(leftMotor2, leftVel);
  motor_set_velocity(rightMotor1, rightVel);
  motor_set_velocity(rightMotor2, rightVel);
}

void opcontrol() {
  bool lastY = false, joystickMode = true;
  int16_t leftRPM = 0, rightRPM = 0;

  uint8_t packetID = 0;
  int32_t value = 0;
  
  initMotors();
  writeUart(0xF5, 50505);
  
  while (true) {
    readUart(&packetID, &value);
    switch (packetID) {
      case 0x1:
        leftRPM = value / 360.0f;
        break;
      case 0x2:
        rightRPM = value / 360.0f;
        break;
      case 0x16:
        break;
      case 0x17:
        break;
      case 0x18:
        break;
    }

    if (controller_get_digital(CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_Y) && !lastY) {
      lastY = true;
      joystickMode = !joystickMode;
    } else {
      lastY = false;
    }

    if (joystickMode) {
      setDrive(controller_get_analog(CONTROLLER_MASTER, E_CONTROLLER_ANALOG_LEFT_Y),
               controller_get_analog(CONTROLLER_MASTER, E_CONTROLLER_ANALOG_RIGHT_Y));
    } else {
      setDrive(leftRPM, rightRPM);
    }
    
    delay(15);
  }
}
