# SensorReading.msg
# A composite sensor reading with temperature, humidity, and metadata.

float64 temperature     # Temperature in degrees Celsius
float64 humidity        # Relative humidity (0.0 – 100.0 %)
string  sensor_id       # Unique identifier for the sensor
builtin_interfaces/Time stamp  # Timestamp of the reading
