#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For sleep function in Linux
#include <string.h>
#include <arpa/inet.h> // For socket
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

// Struct for holding flight data
#pragma pack(1)
typedef struct { 
    float altitude;
    float airspeed;
    float heading;
    float temperature;
} FlightData;
#pragma pack()

// Function to generate random flight data
FlightData generate_flight_data()
{
    FlightData Data;
    Data.altitude = 9000 + rand() % 2000;  // Altitude between 9000 and 11000 meters
    Data.airspeed = 200 + rand() % 100;    // Airspeed between 200 and 300 knots
    Data.heading = rand() % 360;           // Heading between 0 and 359 degrees
    Data.temperature = -50 + rand() % 30;  // Temperature between -50 and -20 °C
    return Data;
}

// Function to log flight data to a binary file
void log_flight_data(FlightData data)
{
    FILE *file = fopen("flight_log.bin", "ab");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    if (fwrite(&data, sizeof(FlightData), 1, file) != 1)
    {
        perror("Error writing to file");
    }

    if (fclose(file) != 0)
    {
        perror("Error closing file");
    }
}

// Function to read and display binary flight log data
void read_flight_data()
{
    FILE *file = fopen("flight_log.bin", "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    FlightData data;
    printf("\nFlight Log Data:\n");

    while (1)
    {
        size_t read = fread(&data, sizeof(FlightData), 1, file);
        printf("Bytes read: %zu\n", read);
        if (read == 0)
        {
            if (feof(file))
                break; // End of file reached
            else
            {
                perror("Error reading file");
                break;
            }
        }
        printf("Altitude: %.2f m, Airspeed: %.2f knots, Heading: %.2f degrees, Temperature: %.2f °C\n",
               data.altitude, data.airspeed, data.heading, data.temperature);
    }

    fclose(file);
}

// Function to simulate heading direction using ASCII arrows
void display_heading(float heading)
{
    if (heading < 22.5 || heading >= 337.5)
        printf("\u2191 North\n");
    else if (heading < 67.5)
        printf("\u2197 Northeast\n");
    else if (heading < 112.5)
        printf("\u2192 East\n");
    else if (heading < 157.5)
        printf("\u2198 Southeast\n");
    else if (heading < 202.5)
        printf("\u2193 South\n");
    else if (heading < 247.5)
        printf("\u2199 Southwest\n");
    else if (heading < 292.5)
        printf("\u2190 West\n");
    else
        printf("\u2196 Northwest\n");
}

// Function to send flight data over a UDP socket
void send_flight_data(FlightData data)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (sendto(sock, &data, sizeof(FlightData), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error sending data");
    }

    close(sock);
}

int main()
{
    srand(time(NULL)); // Seed random number generator
    float prev_altitude = 10000; // Initial altitude

    for (int i = 0; i < 10; i++) // Simulate 10 flight data entries
    {
        FlightData data = generate_flight_data();
        printf("Altitude: %.2f m, Airspeed: %.2f knots, Heading: %.2f degrees, Temperature: %.2f °C\n",
               data.altitude, data.airspeed, data.heading, data.temperature);

        // Check for abnormal conditions
        if (prev_altitude - data.altitude > 500)
        {
            printf("WARNING: Rapid descent!\n");
        }
        if (data.airspeed > 350)
        {
            printf("ALERT: Overspeed!\n");
        }
        prev_altitude = data.altitude;

        // Display heading
        display_heading(data.heading);

        // Log Data and send to ground control
        log_flight_data(data);
        send_flight_data(data);

        sleep(1);
    }

    printf("\nReading saved flight data...\n");
    read_flight_data();

    return 0;
}
