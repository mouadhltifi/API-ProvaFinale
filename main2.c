#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ordered linked list node
typedef struct Car {
    int range;
    struct Car * next;
    struct Car * prev;
} Car;

//AVL tree node
typedef struct Station {
    //AVL tree variables
    struct Station * left;
    struct Station * right;
    struct Station* parent;
    int height;

    //specific use case variables
    int distance;
    int maxRange;
    Car* cars;

} Station;


//AVL tree functions -- START

int height(Station* N) {
    if (N == NULL) return 0;
    return N->height;
}

int max(int a, int b) {
    return (a > b)? a : b;
}

Station* rotateLeft(Station* x) {
    Station* y = x->right;
    Station* T2 = y->left;

    y->left = x;
    x->right = T2;

    // update parent pointers
    y->parent = x->parent;
    x->parent = y;
    if (T2 != NULL)
        T2->parent = x;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

Station* rotateRight(Station* y) {
    Station* x = y->left;
    Station* T2 = x->right;

    x->right = y;
    y->left = T2;

    // update parent pointers
    x->parent = y->parent;
    y->parent = x;
    if (T2 != NULL)
        T2->parent = y;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

int getBalance(Station* N) {
    if (N == NULL) return 0;
    return height(N->left) - height(N->right);
}

Station* createStation(int dist) {
    Station* station = (Station*)malloc(sizeof(Station));
    station->distance = dist;
    station->maxRange = 0;
    station->cars = NULL;
    station->left = NULL;
    station->right = NULL;
    station->parent = NULL;
    station->height = 1;
    return station;
}

Station* insertStation(Station* station, int dist) {
    if (station == NULL) {
        return createStation(dist);
    }

    if (dist < station->distance) {
        Station* leftChild = insertStation(station->left, dist);
        station->left = leftChild;
        leftChild->parent = station;
    } else if (dist > station->distance) {
        Station* rightChild = insertStation(station->right, dist);
        station->right = rightChild;
        rightChild->parent = station;
    }

    station->height = 1 + max(height(station->left), height(station->right));

    int balance = getBalance(station);

    if (balance > 1 && dist < station->left->distance) {
        return rotateRight(station);
    }

    if (balance < -1 && dist > station->right->distance) {
        return rotateLeft(station);
    }

    if (balance > 1 && dist > station->left->distance) {
        station->left = rotateLeft(station->left);
        return rotateRight(station);
    }

    if (balance < -1 && dist < station->right->distance) {
        station->right = rotateRight(station->right);
        return rotateLeft(station);
    }

    return station;
}

Station* findStation(Station* root, int dist) {
    while (root != NULL) {
        if (dist < root->distance)
            root = root->left;
        else if (dist > root->distance)
            root = root->right;
        else
            return root;
    }

    return NULL;
}


Station* minValueStation(Station* station) {
    Station* current = station;

    while (current->left != NULL)
        current = current->left;

    return current;
}

Station* removeStation(Station* root, int dist) {
    if (root == NULL)
        return root;

    if (dist < root->distance)
        root->left = removeStation(root->left, dist);
    else if (dist > root->distance)
        root->right = removeStation(root->right, dist);
    else {
        if ((root->left == NULL) || (root->right == NULL)) {
            Station* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }

            free(temp);
        } else {
            Station* temp = minValueStation(root->right);
            root->distance = temp->distance;
            root->right = removeStation(root->right, temp->distance);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rotateRight(root);

    if (balance < -1 && getBalance(root->right) <= 0)
        return rotateLeft(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}



//AVL tree functions -- END

//Linked List functions -- START
int insertCar(Station* station, int range) {
    Car* newCar = (Car*)malloc(sizeof(Car));
    newCar->range = range;
    newCar->next = NULL;
    newCar->prev = NULL;

    // if the list of cars is empty or the new car has the smallest range
    if (station->cars == NULL || range < station->cars->range) {
        newCar->next = station->cars;

        if (station->cars != NULL)
            station->cars->prev = newCar;

        station->cars = newCar;
    } else {
        Car* current = station->cars;
        while (current->next != NULL && current->next->range < range)
            current = current->next;

        newCar->next = current->next;
        newCar->prev = current;

        if (current->next != NULL)
            current->next->prev = newCar;

        current->next = newCar;
    }

    return 1;
}

int findMaxRange(Station* station) {
    if (station == NULL || station->cars == NULL) {
        printf("Station not found or no cars in this station.\n");
        return -1;  // Return -1 or another suitable value indicating error.
    }

    int maxRange = 0;
    Car* current = station->cars;
    while (current != NULL) {
        if (current->range > maxRange) {
            maxRange = current->range;
        }
        current = current->next;
    }

    return maxRange;
}


int removeCar(Station* station, int range) {
    Car* temp = station->cars;

    while (temp != NULL && temp->range != range) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("No car with range = %d found in this station\n", range);
        return 0;
    }

    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        station->cars = temp->next;

    if (temp->next != NULL)
        temp->next->prev = temp->prev;

    // if the removed car had the maximum range, update maxRange and maxRangeCar
    if (temp->range == station->maxRange) {
        Car* car = station->cars;
        station->maxRange = (car == NULL) ? 0 : car->range;

        while (car != NULL) {
            if (car->range > station->maxRange) {
                station->maxRange = car->range;
            }
            car = car->next;
        }
    }

    free(temp);

    return 1;
}



Car* findCar(Station* station, int range) {
    Car* temp = station->cars;

    while (temp != NULL && temp->range < range) {
        temp = temp->next;
    }

    if (temp != NULL && temp->range == range) {
        return temp;
    } else {
        printf("No car with range = %d found in this station\n", range);
        return NULL;
    }
}


//Linked List functions -- END

//Specific use case functions -- START

int addStation(Station *head, int distance, int numCars) {
    Station *newStation;
    if (head == NULL) {
        head = insertStation(head, distance);
        newStation = head;
    } else{
        newStation = insertStation(head, distance);;
    }

    Car* tempMaxRangeCar;
    int tempMaxRange = 0;
    int carRange;

    for (int i = 0; i < numCars; i++) {
        if (scanf("%d", &carRange) != 1){
            printf("error reading cars from aggiungi-stazione");
            return -1;
        }

        if (carRange > tempMaxRange) {
            tempMaxRange = carRange;
        }
                insertCar(newStation, carRange);
    }

    newStation->distance = distance;
    newStation->maxRange = tempMaxRange;

    return 1;
}


//Specific use case functions -- END

int main() {
    char cmd[30];
    int dist, numCars, carRange, ret;

    Station * head = NULL;


    while ((ret = scanf("%s", cmd)) && *cmd != EOF) {

        if(ret != 1) break;

        else if (strcmp(cmd, "aggiungi-stazione") == 0) {
            int maxRange = 0;
            if(scanf("%d %d", &dist, &numCars) != 2) break;

            if (addStation(head, dist, numCars)) puts("aggiunta");
            else puts("non aggiunta");

            if(scanf("%d %d", &dist, &numCars) != 2) break;
            for (int i = 0; i < numCars; i++) {
                if(scanf("%d", &carRange) != 1) break;
                if (carRange > maxRange)
                    maxRange = carRange;
            }

        }


        else if (strcmp(cmd, "demolisci-stazione") == 0) {
            if(scanf("%d", &dist) != 1){
                break;
            }
            if (removeStation(head, dist)){
                puts("demolita");
            }
            else {
                puts("non demolita");
            }
        }


        else if (strcmp(cmd, "aggiungi-auto") == 0) {
            if(scanf("%d %d", &dist, &carRange) != 2) break;
            Station* station = findStation(head, dist);
            Car* newCar = (Car*) malloc(sizeof(Car));
            newCar->range = carRange;

            if (insertCar(station, carRange)) {
                puts("aggiunta");
            } else {
                puts("non aggiunta");
                free(newCar);
            }
        }


        else if (strcmp(cmd, "rottama-auto") == 0) {
            if(scanf("%d %d", &dist, &carRange) != 2) break;

            Station* station = findStation(head, dist);


            if (removeCar(station, carRange)){
                puts("rottamata");
            } else {
                puts("non rottamata");
            }
        }


        else if (strcmp(cmd, "pianifica-percorso") == 0) {
            int start, end;
            if(scanf("%d %d", &start, &end) != 2) break;
            //TODO
        }
    }

    return 0;
}