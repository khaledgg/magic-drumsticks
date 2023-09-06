#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

const int historySize = 5;
float z_history[historySize] = {0};
const float tapThreshold = 35; // This threshold might need tuning
unsigned long lastTapTime = 0;
const unsigned long cooldownTime = 300; // Cooldown to avoid double-detection


const int buzzerPin = 3;

void setup(void) {
	Serial.begin(115200);

	// Try to initialize!
	if (!mpu.begin()) {
		Serial.println("Failed to find MPU6050 chip");
		while (1) {
		  delay(10);
		}
	}

	mpu.setAccelerometerRange(MPU6050_RANGE_2_G); //2, 4, 8, 16
	mpu.setGyroRange(MPU6050_RANGE_250_DEG); // 250, 500, 1000, 2000
	mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); //5, 10, 21, 44, 94, 184 (Higher the faster but more noise)

	// setup buzzer
	pinMode(buzzerPin, OUTPUT);

	delay(100);
}

void loop() {
	/* Get new sensor events with the readings */
	sensors_event_t a, g, temp;
	mpu.getEvent(&a, &g, &temp);

  digitalWrite(buzzerPin, LOW);
	if (tap_detector(&a)){
		Serial.println("TAP DETECTED");
		digitalWrite(buzzerPin, HIGH);
	}

	// /* Print out the values */
	// Serial.print(a.acceleration.x);
	// Serial.print(",");
	// Serial.print(a.acceleration.y);
	// Serial.print(",");
	// Serial.print(a.acceleration.z);
	// Serial.print(", ");
	// Serial.print(g.gyro.x);
	// Serial.print(",");
	// Serial.print(g.gyro.y);
	// Serial.print(",");
	// Serial.print(g.gyro.z);
	// Serial.println("");

	delay(10);
}

bool tap_detector(sensors_event_t* a){
	// detect if the imu on drumstick "taps" by looking for a change in z

	unsigned long currentTime = millis();

    if (currentTime - lastTapTime < cooldownTime) {
        return false;
    }

	for (int i = 0; i < historySize - 1; i++) {
			z_history[i] = z_history[i + 1]; //move history window
	}

    z_history[historySize - 1] = a->acceleration.z; //update history window

    if (abs(z_history[historySize - 1] - z_history[0]) > tapThreshold) {
        lastTapTime = currentTime;
        return true;
    }

    return false;
}
