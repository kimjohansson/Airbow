int valve_biceps = 51;
int valve_triceps = 50;
int pump = 2;
int pot_angle = A2;
int pot_setpoint = A1;

int angle = 0;
int setpoint = 0;
int old_setpoint = 0;
int stable_setpoint = 0;
int change = 0;

int error = 0;
float pVal = 1.8;
float iVal = 0.02;
float dVal = 0.9;
long add_error = 0;
int last_error = 0;
int abs_error = 0;
int slope = 0;
int t = 0;

int pump_power = 0;

void setup() {
  pinMode(valve_biceps, OUTPUT);
  pinMode(valve_triceps, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(pot_angle, INPUT);
  pinMode(pot_setpoint, INPUT);

  Serial.begin(9600);
}
void loop() {
  int angle = map(constrain(analogRead(pot_angle), 250, 500), 250, 500, 0, 250); // Reads the current angle so that a value of 0 means the Airbow is fully extended and 500 fully flexed.
  int setpoint = map(analogRead(pot_setpoint), 0, 1023, 250, 0);
  if (change > 3) // If the setpoint changes, the cumulative error is reset to the current error.
  {
    add_error = add_error;
  }
  else
  {
    stable_setpoint = setpoint;
  }
  old_setpoint = setpoint;
  int change = abs(setpoint - old_setpoint);

  int error = angle - stable_setpoint; // Calculates the error.
  int abs_error = abs(error);

  float p = abs_error * pVal; // Gives the proportional control value.

  add_error = add_error + error;
  long i = constrain(abs(add_error), 0, 20000) * iVal; // Gives the integral control value, limiting the cumulative error to 13000.

  slope = abs_error - last_error;
  float d = slope * dVal; // Gives the derivative control value.
  last_error = abs_error;

  float pump_pid = p + i - d;
  int pump_power = constrain(pump_pid, 0, 255); // The PID control is given as output to the pump, which is controlled by PWM.
  t++;

  if (error < -1) // When the error is negative, the arm flexes.
  {
    analogWrite(pump, pump_power);
    digitalWrite(valve_biceps, HIGH);
    digitalWrite(valve_triceps, LOW);
  }
  else if (error > 1) // When the error is positive, the arm extends.
  {
    analogWrite(pump, pump_power);
    digitalWrite(valve_biceps, LOW);
    digitalWrite(valve_triceps, HIGH);
  }
  else
  {
    analogWrite(pump, 0);
    add_error = error;
  }
  Serial.print(t);
  Serial.print(",");
  Serial.print(setpoint);
  Serial.print(",");
  Serial.print(angle);
  Serial.print(",");
  Serial.print(pump_power);
  Serial.print(",");
  Serial.println(error);
}
