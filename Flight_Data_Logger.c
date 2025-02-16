#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Struct for hold flight data
typedef struct
{
	float altitude;
	float airspeed;
	float heading;
	float temperature;
} FlightData;

//Function to generate random flight data
FlightData generate_flight_data(){

	FlightData Data;
	Data.altitude = 9000 + rand() % 2000;  // altitude between 9000 and 11000 meters
	Data.airspeed = 200 + rand() % 100;    //airspeed between 200 and 300 knots
	Data.heading = rand() % 360;           // heading between 0 and 359 degrees
	Data.temperature = -50 + rand() % 30;  // tempurature betwwen -50 and -20 °

	return Data;
}

//Function to log flight data to file
void log_flight_data(FlightData data)
{
	FILE *file = fopen("/home/h/Documents/Codes/Aircraft Flight Data Logger/flight_log.csv", "a");
	if (file == NULL){
		perror("Error opening file");
		return;
	}
    // If file pointer is valid, write to the file
    if (fprintf(file, "%.2f,%.2f,%.2f,%.2f\n", data.altitude, data.airspeed, data.heading, data.temperature) < 0) {
        perror("Error writing to file");
        fclose(file);  // Close the file before returning
        return;
		}
}

int main()
{
	srand(time(NULL)); //Inputed random value.
	FILE *file = fopen("flight_log.csv", "w");

    if (file == NULL) {
        perror("Error opening file for writing headers");
        return 1;  // Exit if the file can't be opened
    }
    fprintf(file, "Altitude,Airspeed,Heading,Temperature\n");
    fclose(file);  // Properly close the file after writing headers

	for (int i = 0; i < 10; ++i) //simulation for 10 flight data entries
	{
		FlightData data = generate_flight_data();
		printf("Altitude: %.2f m, Airspeed: %.2f knots, Heading: %.2f degrees, Temperature: %.2f °C\n",
               data.altitude, data.airspeed, data.heading, data.temperature);
		log_flight_data(data);
	}

	return 0;
}