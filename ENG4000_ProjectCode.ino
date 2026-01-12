#include <Stepper.h>
#define VOLTAGE_SENSOR A1  // Voltage divider connected to A1
#define CURRENT_SENSOR A0  // ACS712 current sensor
#define TEMP_SENSOR A2     // LM35 temperature sensor
#define IRRADIANCE_SENSOR A3 // Light sensor (Irradiance)

const int LDR1 = A0;  // LDR Sensor 1
const int LDR2 = A1;  // LDR Sensor 2
const int LDR3 = A2;  // LDR Sensor 3
const int LDR4 = A3;  // LDR Sensor 4

const int stepsPerRevolution = 2048;
const int deviation = 50;  // LDR values deviation

// Define rotation limits
const float degreesPerStep = 360.0 / stepsPerRevolution; // Steps to degrees conversion

const int horizontalMaxAngle = 60; // Horizontal rotation range
const int horizontalMinAngle = 0;
const int verticalMaxAngle = 50;    // Vertical rotation range
const int verticalMinAngle = 0;

// Convert max angles to steps
const int horizontalStepsMax = horizontalMaxAngle / degreesPerStep;
const int horizontalStepsMin = horizontalMinAngle / degreesPerStep;
const int verticalStepsMax = verticalMaxAngle / degreesPerStep;
const int verticalStepsMin = verticalMinAngle / degreesPerStep;

int ldr1, ldr2, ldr3, ldr4;

int maxLDRValue = 0;
int totalLDRValue;

int currentHorizontalAngle = 0;
int currentVerticalAngle = 0;

Stepper stepperX(stepsPerRevolution, 8, 10, 9, 11);
Stepper stepperY1(stepsPerRevolution, 0, 2, 1, 3);
Stepper stepperY2(stepsPerRevolution, 4, 6, 5, 7);

void setup() {
    Serial.begin(9600);

    stepperX.setSpeed(10); // Set speed for horizontal motor
    stepperY1.setSpeed(10); // Set speed for vertical motor
    stepperY2.setSpeed(10); // Set speed for vertical motor

    Serial.println("Stepper Motor Limited Angle Test");
}

void loop() {

  // Getting LDR sensor values
  ldr1 = analogRead(LDR1);
  ldr2 = analogRead(LDR2);
  ldr3 = analogRead(LDR3);
  ldr4 = analogRead(LDR4);

  sendDataToNodeRED();

  maxLDRValue = max(max(ldr1, ldr2), max(ldr3, ldr4));


  totalLDRValue = maxLDRValue - deviation;

  Serial.print("Current Horizontal Angle: "); Serial.println(currentHorizontalAngle);
  Serial.print("Current Vertical Angle: "); Serial.println(currentVerticalAngle);


  if (ldr1 < totalLDRValue || ldr2 < totalLDRValue || ldr3 < totalLDRValue || ldr4 < totalLDRValue) {

    if (maxLDRValue == ldr1) moveToLDR1();
    if (maxLDRValue == ldr2) moveToLDR2();
    if (maxLDRValue == ldr3) moveToLDR3();
    if (maxLDRValue == ldr4) moveToLDR4();

  }

  delay(1000);  // 1 second delay

}

void moveToLDR1() {
  stepperX.step(horizontalStepCalculation(1));
  stepperY1.step(verticalStepCalculation(1));
  stepperY2.step(verticalStepCalculation(1));
}

void moveToLDR2() {
  stepperX.step(horizontalStepCalculation(2));
  stepperY1.step(verticalStepCalculation(2));
  stepperY2.step(verticalStepCalculation(2));
}

void moveToLDR3() {
  stepperX.step(horizontalStepCalculation(3));
  stepperY1.step(verticalStepCalculation(3));
  stepperY2.step(verticalStepCalculation(3));
}

void moveToLDR4() {
  stepperX.step(horizontalStepCalculation(4));
  stepperY1.step(verticalStepCalculation(4));
  stepperY2.step(verticalStepCalculation(4));
}

int horizontalStepCalculation(int LDR) {

  int targetHorAngle;

  switch (LDR) {
    case 1: targetHorAngle = -20; break;
    case 2: targetHorAngle = 20; break;
    case 3: targetHorAngle = -20; break;
    case 4: targetHorAngle = 20; break;
  }

  int tempCurrentHorizontalAngle = currentHorizontalAngle;
  int horizontalSteps = targetHorAngle / degreesPerStep;

  currentHorizontalAngle += horizontalSteps * degreesPerStep;

  if (currentHorizontalAngle > horizontalMaxAngle) {
    horizontalSteps = (horizontalMaxAngle - tempCurrentHorizontalAngle) / degreesPerStep;
    currentHorizontalAngle = horizontalMaxAngle;
  }

  if (currentHorizontalAngle < horizontalMinAngle) {
    horizontalSteps = -(tempCurrentHorizontalAngle / degreesPerStep);
    currentHorizontalAngle = horizontalMinAngle;
  }

  if (horizontalSteps != 0) {
  Serial.print("Moving "); Serial.print(horizontalSteps); Serial.println(" horizontally.");
  }

  return horizontalSteps;

}

int verticalStepCalculation(int LDR) {

  int targetVerAngle;

  switch (LDR) {
    case 1: targetVerAngle = 20; break;
    case 2: targetVerAngle = -20; break;
    case 3: targetVerAngle = -20; break;
    case 4: targetVerAngle = 20; break;
  }

  int tempCurrentVerticalAngle = currentVerticalAngle;
  int verticalSteps = targetVerAngle / degreesPerStep;

  currentVerticalAngle += verticalSteps * degreesPerStep;

  if (currentVerticalAngle > verticalMaxAngle) {
    verticalSteps = (verticalMaxAngle - tempCurrentVerticalAngle) / degreesPerStep;
    currentVerticalAngle = verticalMaxAngle;
  }

  if (currentVerticalAngle < verticalMinAngle) {
    verticalSteps = -(tempCurrentVerticalAngle / degreesPerStep);
    currentVerticalAngle = verticalMinAngle;
  }

  if (verticalSteps != 0) {
  Serial.print("Moving "); Serial.print(verticalSteps); Serial.println(" vertically.");
  }

  return verticalSteps;

}

void sendDataToNodeRED() {

  // Read Voltage (Adjust divider values based on real setup)
  float rawVoltage = analogRead(VOLTAGE_SENSOR);
  float voltage = (rawVoltage * 5.0 / 1023.0) * (100.0 / 10.0); // Adjust based on resistor values

  // Read Current from ACS712
  float rawCurrent = analogRead(CURRENT_SENSOR);
  float current = (rawCurrent - 512) * (5.0 / 1023.0) / 0.185; // Adjust based on ACS712 model

  // Read Temperature from LM35
  float tempRaw = analogRead(TEMP_SENSOR);
  float temperature = (tempRaw * 5.0 / 1023.0) * 100.0; // LM35 outputs 10mV per degree Celsius

  // Calculate Power (P = V * I)
  float power = voltage * current;

  // Simulated Total Energy (Sum over time)
  static float totalEnergy = 0;
  totalEnergy += (power * 2 / 3600.0); // Convert W to kWh (every 2 seconds)

  // Simulated Daily Energy
  static float dailyEnergy = 0;
  dailyEnergy += (power * 2 / 3600.0); // Reset daily at midnight

  // Efficiency (Assuming max power is 5000W)
  float efficiency = (power / 5000.0) * 100.0;

  // Adjusted for 5V lithium-ion range
  int batteryPercentage = map(voltage, 3.0, 4.2, 0, 100);

  // JSON Format Output
  Serial.print("{");
  Serial.print("\"power\":"); Serial.print(power); Serial.print(",");
  Serial.print("\"dailyEnergy\":"); Serial.print(dailyEnergy); Serial.print(",");
  Serial.print("\"totalEnergy\":"); Serial.print(totalEnergy); Serial.print(",");
  Serial.print("\"efficiency\":"); Serial.print(efficiency); Serial.print(",");
  Serial.print("\"LDR1\":");
  Serial.print(ldr1);
  Serial.print(",\"LDR2\":");
  Serial.print(ldr2);
  Serial.print(",\"LDR3\":");
  Serial.print(ldr3);
  Serial.print(",\"LDR4\":");
  Serial.print(ldr4); Serial.print(",");
  Serial.print("\"temperature\":"); Serial.print(temperature); Serial.print(",");
  Serial.print("\"voltage\":"); Serial.print(voltage); Serial.print(",");
  Serial.print("\"current\":"); Serial.print(current); Serial.print(",");
  Serial.print("\"battery\":"); Serial.print(batteryPercentage);
  Serial.println("}");

  delay (1000);
  
}