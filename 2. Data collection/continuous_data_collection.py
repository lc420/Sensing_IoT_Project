import serial
import csv

# Establish serial connection (replace 'COMX' with your Arduino's port)
ser = serial.Serial('COM8', 115200)  # Adjust baud rate if needed

# Create a CSV file for storing sensor data
csv_file = open('RPY-3-rises.csv', 'w', newline='')
csv_writer = csv.writer(csv_file)

try:
    while True:
        # Read data from the serial port
        data = ser.readline().decode().strip()  # Adjust decoding as per your Arduino output
        
        # Print the received data (optional)
        print(data)
        
        # Split the data into different values (modify as per your data format)
        sensor_values = data.split(',')  # Change the delimiter as per your Arduino output
        
        # Write sensor data to the CSV file
        csv_writer.writerow(sensor_values)
        
except KeyboardInterrupt:
    # Close the CSV file and serial connection on keyboard interrupt
    csv_file.close()
    ser.close()
    print("Data logging stopped - Flat to Full.")
