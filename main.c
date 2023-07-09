#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATIONS 512

typedef struct Station {
    int distance;
    int maxRange;
} Station;

Station* stations[MAX_STATIONS];
int numStations = 0;

Station* createStation(int distance, int maxRange) {
    Station* newStation = (Station*)malloc(sizeof(Station));
    newStation->distance = distance;
    newStation->maxRange = maxRange;
    return newStation;
}

Station* findStation(int distance) {
    for (int i = 0; i < numStations; i++) {
        if (stations[i]->distance == distance) {
            return stations[i];
        }
    }
    return NULL;
}

int addStation(int distance, int maxRange) {
    if (findStation(distance) == NULL) {
        stations[numStations++] = createStation(distance, maxRange);
        return 1;
    }
    return 0;
}

void removeStation(int distance) {
    for (int i = 0; i < numStations; i++) {
        if (stations[i]->distance == distance) {
            free(stations[i]);
            stations[i] = stations[--numStations];
            return;
        }
    }
}

void planRoute(int startDistance, int endDistance) {
    char output[512] = {0};
    Station* start = findStation(startDistance);
    Station* end = findStation(endDistance);
    if (start == NULL || end == NULL || start->maxRange == 0) {
        printf("nessun percorso\n");
        return;
    }

    int direction = (startDistance < endDistance) ? 1 : -1;
    int route[MAX_STATIONS];
    int routeLen = 0;
    route[routeLen++] = startDistance;

    while (start->distance != endDistance) {
        Station* next = NULL;
        for (int i = 0; i < numStations; i++) {
            if (direction == 1 && stations[i]->distance > start->distance && stations[i]->distance <= start->distance + start->maxRange) {
                if (next == NULL || stations[i]->distance > next->distance)
                    next = stations[i];
            }
            else if (direction == -1 && stations[i]->distance < start->distance && stations[i]->distance >= start->distance - start->maxRange) {
                if (next == NULL || stations[i]->distance < next->distance)
                    next = stations[i];
            }
        }
        if (next == NULL) {
            printf("nessun percorso\n");
            return;
        }
        route[routeLen++] = next->distance;
        start = next;
    }
    for (int i = 0; i < routeLen; i++) {
        char temp[30];
        sprintf(temp, "%d ", route[i]);
        strcat(output, temp);
    }
    // remove the trailing space
    output[strlen(output) - 1] = '\0';
    printf("%s", output); // Here we have removed the newline character.
}


int main() {
    char cmd[900];
    int dist, numCars, carRange, ret;

    while ((ret = scanf("%s", cmd)) && *cmd != EOF) {
        if(ret != 1) break;
        if (strcmp(cmd, "aggiungi-stazione") == 0) {
            int maxRange = 0;
            if(scanf("%d %d", &dist, &numCars) != 2) break;
            for (int i = 0; i < numCars; i++) {
                if(scanf("%d", &carRange) != 1) break;
                if (carRange > maxRange)
                    maxRange = carRange;
            }
            if (addStation(dist, maxRange)) puts("aggiunta");
            else puts("non aggiunta");
        } else if (strcmp(cmd, "demolisci-stazione") == 0) {
            if(scanf("%d", &dist) != 1) break;
            Station* s = findStation(dist);
            if (s) {
                removeStation(dist);
                puts("demolita");
            } else {
                puts("non demolita");
            }
        }
        else if (strcmp(cmd, "aggiungi-auto") == 0) {
            if(scanf("%d %d", &dist, &carRange) != 2) break;
            Station* s = findStation(dist);
            if (s && carRange > s->maxRange) {
                s->maxRange = carRange;
                puts("aggiunta");
            } else {
                puts("non aggiunta");
            }
        } else if (strcmp(cmd, "rottama-auto") == 0) {
            if(scanf("%d %d", &dist, &carRange) != 2) break;
            Station* s = findStation(dist);
            if (s && s->maxRange == carRange) {
                s->maxRange = 0;
                puts("rottamata");
            } else {
                puts("non rottamata");
            }
        } else if (strcmp(cmd, "pianifica-percorso") == 0) {
            int start, end;
            if(scanf("%d %d", &start, &end) != 2) break;
            planRoute(start, end);
        }
    }

    for (int i = 0; i < numStations; i++) {
        free(stations[i]);
    }
    return 0;
}
