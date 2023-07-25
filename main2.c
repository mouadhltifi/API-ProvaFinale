#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 99999999

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
    int pathFlag;
    struct Station* prevInPath;
    struct Station* nextInPath;

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
    station->pathFlag = 0;
    station->cars = NULL;
    station->left = NULL;
    station->right = NULL;
    station->parent = NULL;
    station->height = 1;
    return station;
}

Station* insertStation(Station* node, int dist) {
    if (node == NULL) {
        return(createStation(dist));
    }

    if (dist < node->distance) {
        node->left  = insertStation(node->left, dist);
        node->left->parent = node;
    }
    else if (dist > node->distance) {
        node->right = insertStation(node->right, dist);
        node->right->parent = node;
    }

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    if (balance > 1 && dist < node->left->distance) {
        return rotateRight(node);
    }

    if (balance < -1 && dist > node->right->distance) {
        return rotateLeft(node);
    }

    if (balance > 1 && dist > node->left->distance) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    if (balance < -1 && dist < node->right->distance) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
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

// Utility function to find the maximum value node in a tree
Station* maxValueStation(Station* node) {
    Station* current = node;

    while (current->right != NULL)
        current = current->right;

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

void printInOrder(Station* root) {
    if (root != NULL) {
        printInOrder(root->left);
        printf("Station at distance %d with maxRange %d\n", root->distance, root->maxRange);
        printInOrder(root->right);
    }
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

int addStation(Station **head, int distance, int numCars) {
    *head = insertStation(*head, distance);

    Station *newStation = findStation(*head, distance);
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

int removeAllCarsInStation(Station *station) {
    Car* current = station->cars;
    while (current != NULL) {
        removeCar(station, current->range);
        current = current->next;
    }

    return 1;
}

int deleteStation(Station **head, int distance) {
    Station *station = findStation(*head, distance);
    int carRemoval = 1;
    if (station == NULL) {
        printf("Station to be deleted not found.\n");
        return 0;
    }

    if (station->cars != NULL) {
        carRemoval = removeAllCarsInStation(station);
    }

    *head = removeStation(*head, distance);

    return carRemoval;
}

void printAllStations (Station **head) {
    if (*head == NULL) {
        printf("No stations found.\n");
        return;
    }

    printInOrder(*head);
}

Station* findNext(Station* root, Station* n) {
    if (n->right != NULL)
        return minValueStation(n->right);

    Station* p = n->parent;
    while (p != NULL && n == p->right) {
        n = p;
        p = p->parent;
    }

    return p;
}


Station* findNextStation(Station** head, int distance) {
    Station* station = findStation(*head, distance);
    if (station == NULL) {
        printf("Station not found.\n");
        return NULL;
    }

    Station* nextStation = findNext(*head, station);
    if (nextStation == NULL) {
        printf("No next station found.\n");
        return NULL;
    }

    return nextStation;
}

Station* findPrevious(Station* root, Station* n) {
    if (n->left != NULL)
        return maxValueStation(n->left);

    Station* p = n->parent;
    while (p != NULL && n == p->left) {
        n = p;
        p = p->parent;
    }

    return p;
}

Station* findPreviousStation(Station** head, int distance) {
    Station* station = findStation(*head, distance);
    if (station == NULL) {
        printf("Station not found.\n");
        return NULL;
    }

    Station* prevStation = findPrevious(*head, station);
    if (prevStation == NULL) {
        printf("No previous station found.\n");
        return NULL;
    }

    return prevStation;
}


void printPath(Station* current){
    Station* temp =NULL;

    while(current!=NULL){
        //print station number
        printf("%d",current->distance);
        if (current->prevInPath!=NULL){
            printf("->");
        }
        temp= current;
        current = current->prevInPath;
        temp->prevInPath = NULL;
        temp ->nextInPath = NULL;
    }
    printf("\n");

}

void cleanPath(Station* current){
    Station* temp =NULL;

    while(current!=NULL){
        temp= current;
        current = current->prevInPath;
        temp->prevInPath = NULL;
        temp ->nextInPath = NULL;
    }
}

void optimizeBackwards (Station** head,  Station* tail){
    Station* a = tail->prevInPath;
    Station* temp = a;
    if (a==NULL){
        return;
    }
    Station* b = a->prevInPath;

    while(b!=NULL){
        while(temp->distance != b->distance){
            if (temp->distance + temp->maxRange >= a->nextInPath->distance){
                // sostituisco temp ad a
                a->nextInPath->prevInPath = temp;
                b->nextInPath = temp;
                temp->prevInPath = b;
                temp->nextInPath = a->nextInPath;
                a->nextInPath = NULL;
                a->prevInPath = NULL;
                a = temp;
            }


            temp = findPreviousStation(head, temp->distance);
            if (temp==NULL){
                break;
            }
        }

        a=b;
        b=b->prevInPath;
        temp = a;
    }

}

//ritorna 1 se percorso trovato
int findPathForwards(Station** head, int startDistance, int endDistance) {

    if (*head == NULL) {
        printf("Head is NULL\n");
        return -1;
    }

    Station* startStation = findStation(*head, startDistance);
    Station* endStation = findStation(*head, endDistance);

    if (startStation == NULL || endStation == NULL) {
        printf("Start or end is NULL\n");
        return -1;
    }

    if (startDistance == endDistance) {
        printf("Start equals end\n");
        return 1;
        //stampa stazione singola
    }






    int tempMaxReach = 0;
    Station* tempMaxReachStation = NULL;
    Station* currentStation = startStation;



    Station *nextStation = findNextStation(head, currentStation->distance);

    while(currentStation!= NULL && currentStation->distance <= endStation->distance){










        while(currentStation!= NULL && currentStation->distance <= startStation->distance + startStation->maxRange){


            if(nextStation!=NULL && nextStation->distance == endStation->distance){

                //due funzioni separate per printbackwards e cleanPathPointers
                //TODO
                optimizeBackwards(head, nextStation);

                printPath(nextStation);

                return 0;

            }


            else if(nextStation->distance + nextStation->maxRange > tempMaxReach){
                tempMaxReach = nextStation->distance + nextStation->maxRange;
                tempMaxReachStation = nextStation;
            }


            currentStation=nextStation;
            nextStation = findNextStation(head, currentStation->distance);

        }

        if(nextStation!=NULL && nextStation->distance == endStation->distance){
            return 0;

            //due funzioni separate per printbackwards e cleanPathPointers


            //return 1;
            //stampa percorso in backwartds
        }


        if(tempMaxReachStation == NULL || tempMaxReachStation->distance == currentStation->distance){
            printf("No next station found.\n");
            cleanPath(currentStation);
            return -1;
        }

        tempMaxReachStation->prevInPath = currentStation;
        currentStation = tempMaxReachStation;

    }


    return -1;
}

//Specific use case functions -- END

int main() {
    char cmd[30];
    int dist, numCars, carRange, ret;
    //int modified;

    Station * head = NULL;


    while ((ret = scanf("%s", cmd)) && *cmd != EOF) {

        if(ret != 1) break;

        else if (strcmp(cmd, "aggiungi-stazione") == 0) {

            if(scanf("%d %d", &dist, &numCars) != 2){
                printf("error reading distance and numCars from aggiungi-stazione");
                break;
            }

            if(findStation(head, dist) != NULL){
                puts("non aggiunta");
                break;
            }

            else if (addStation(&head, dist, numCars)) puts("aggiunta");

        }


        else if (strcmp(cmd, "demolisci-stazione") == 0) {
            if(scanf("%d", &dist) != 1){
                break;
            }
            if (deleteStation(&head, dist)){
                puts("demolita");
            }
            else {
                puts("non demolita");
            }
        }


        else if (strcmp(cmd, "aggiungi-auto") == 0) {
            if(scanf("%d %d", &dist, &carRange) != 2) break;
            Station* station = findStation(head, dist);
            if (station == NULL) {
                puts("non aggiunta");
                break;
            }
            else if (insertCar(station, carRange)) {
                puts("aggiunta");
            }
            else {
                puts("non aggiunta");
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

            if (findPathForwards(&head, start, end) == 1 || findPathForwards(&head, end, start) == 0) {
                printf("Percorso trovato.\n");
            }
            else {
                printf("Percorso non trovato.\n");
            }
        }

        else if (strcmp(cmd, "stampa-stazioni") == 0) {
            printInOrder(head);
        }
    }

    return 0;
}