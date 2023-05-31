#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CAR_RANGE 1000000

typedef struct {
    int range;
} Car;

typedef struct Node {
    Car* car;
    struct Node* next;
} Node;

typedef struct {
    int distance;
    Node* cars;
} Station;

typedef struct {
    Station* stations;
    int len;
} Road;

// Function prototypes.
Road* createRoad(int len);
Station* createStation(int distance);
Car* createCar(int range);
void addStation(Road* road, Station* station);
void removeStation(Road* road, int distance);
void addCar(Station* station, Car* car);
void removeCar(Station* station, Car* car);
void calculateShortestPath(Road* road, int start, int end);

// Function implementations.
Road* createRoad(int len) {
    Road* road = malloc(sizeof(Road));
    road->stations = malloc(sizeof(Station) * len);
    road->len = len;
    return road;
}

Station* createStation(int distance) {
    Station* station = malloc(sizeof(Station));
    station->distance = distance;
    station->cars = NULL;
    return station;
}

Car* createCar(int range) {
    Car* car = malloc(sizeof(Car));
    car->range = range;
    return car;
}

void addStation(Road* road, Station* station) {
    road->stations[road->len++] = *station;
}

void removeStation(Road* road, int distance) {
    // Implement this function.
}

void addCar(Station* station, Car* car) {
    Node* node = malloc(sizeof(Node));
    node->car = car;
    node->next = station->cars;
    station->cars = node;
}

void removeCar(Station* station, Car* car) {
    // Implement this function.
}

void calculateShortestPath(Road* road, int start, int end) {
    int* dp = malloc(sizeof(int) * (end + 1));
    int* prev = malloc(sizeof(int) * (end + 1));
    for (int i = 0; i <= end; ++i) {
        dp[i] = MAX_CAR_RANGE;
        prev[i] = -1;
    }
    dp[start] = 0;

    for (int i = start + 1; i <= end; ++i) {
        for (int j = 0; j < road->len; ++j) {
            Station* station = &road->stations[j];
            if (station->distance > i) break;
            Node* node = station->cars;
            while (node != NULL) {
                if (station->distance + node->car->range >= i) {
                    if (dp[i] > dp[station->distance] + 1) {
                        dp[i] = dp[station->distance] + 1;
                        prev[i] = station->distance;
                    }
                }
                node = node->next;
            }
        }
    }

    int current = end;
    if (prev[current] == -1) {
        printf("No path from %d km to %d km.\n", start, end);
        free(prev);
        free(dp);
        return;
    }
    printf("%d", current);
    while (prev[current] != -1) {
        printf(" <- %d", prev[current]);
        current = prev[current];
    }
    printf("\n");

    free(prev);
    free(dp);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: ./program <input_file>\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Could not open file: %s\n", argv[1]);
        return 1;
    }

    Road* road = createRoad(10);
    char command[20];
    while (fscanf(file, "%s", command) != EOF) {
        if (strcmp(command, "add") == 0) {
            fscanf(file, "%s", command);
            if (strcmp(command, "station") == 0) {
                int distance;
                fscanf(file, "%d", &distance);
                addStation(road, createStation(distance));
                printf("Added station at %d km.\n", distance);
            } else if (strcmp(command, "car") == 0) {
                int distance, range;
                fscanf(file, "%d%d", &distance, &range);
                for (int i = 0; i < road->len; ++i) {
                    if (road->stations[i].distance == distance) {
                        addCar(&road->stations[i], createCar(range));
                        printf("Added car with range %d km to station at %d km.\n", range, distance);
                        break;
                    }
                }
            }
        } else if (strcmp(command, "remove") == 0) {
            // Implement this part.
        } else if (strcmp(command, "calculate") == 0) {
            fscanf(file, "%s", command);
            if (strcmp(command, "path") == 0) {
                int start, end;
                fscanf(file, "%d%d", &start, &end);
                calculateShortestPath(road, start, end);
            }
        }
    }
    fclose(file);
    return 0;
}
