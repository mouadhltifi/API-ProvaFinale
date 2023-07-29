#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//ordered linked list node
typedef struct Car {
    int range;
    int quantity;
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

    //path variables
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
    station->prevInPath = NULL;
    station->nextInPath = NULL;
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
    Station* temp = root;

    while (temp != NULL) {
        if (dist < temp->distance)
            temp = temp->left;
        else if (dist > temp->distance)
            temp = temp->right;
        else if (dist == temp->distance)
            return temp;
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

    if (dist < root->distance) {
        root->left = removeStation(root->left, dist);
        if(root->left) root->left->parent = root;
    } else if (dist > root->distance) {
        root->right = removeStation(root->right, dist);
        if(root->right) root->right->parent = root;
    } else {
        if ((root->left == NULL) || (root->right == NULL)) {
            Station* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
                // Update parent pointers
                if (root->left) root->left->parent = root;
                if (root->right) root->right->parent = root;
            }

            free(temp);
        } else {
            Station* temp = minValueStation(root->right);
            root->distance = temp->distance;
            root->right = removeStation(root->right, temp->distance);
            // Update parent pointers
            if (root->left) root->left->parent = root;
            if (root->right) root->right->parent = root;
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
        printf("Station %d with maxRange %d\n", root->distance, root->maxRange);
        printInOrder(root->right);
    }
}


//AVL tree functions -- END

//Linked List functions -- START


int findMaxRange(Station* station) {
    if (station == NULL || station->cars == NULL) {
        return 0;  // Return 0 if there are no cars in this station.
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


int insertCar(Station* station, int range) {
    Car* newCar = (Car*)malloc(sizeof(Car));

    newCar->range = range;
    newCar->quantity = 1;
    newCar->next = NULL;
    newCar->prev = NULL;

    // cars stored in senso discendente
    // if the list of cars is empty or the new car has the smallest range

    if (station->cars == NULL || range > station->maxRange) {
        newCar->next = station->cars;

        if (station->cars != NULL)
            station->cars->prev = newCar;

        station->cars = newCar;
        station->maxRange = range;
    }

    else if(range == station->maxRange) {
        station->cars->quantity++;
        free(newCar);

    }

    // ci sono già delle macchine con range maggiore
    else {
        Car* current = station->cars;
        while (current->next != NULL && current->next->range > range)
            current = current->next;

        if(current->next != NULL && current->next->range == range) {
            current->next->quantity++;
            free(newCar);
            return 1;
        }

        else {
            newCar->next = current->next;
            newCar->prev = current;

            if (current->next != NULL)
                current->next->prev = newCar;

            current->next = newCar;
            return 1;
        }

    }
    return -1;
}


int removeCar(Station* station, int range) {
    Car* temp = station->cars;

    while (temp != NULL && temp->range > range) {
        temp = temp->next;
    }

    if (temp == NULL || temp->range != range) {
        //printf("No car with range = %d found in this station\n", range);
        return -1;
    }

    temp->quantity--;

    //was the last unit with that range
    if (temp->quantity == 0) {

        if(station->maxRange== range) {
            station->maxRange = (temp->next == NULL) ? 0 : temp->next->range;
        }

        if (temp->prev != NULL)
            temp->prev->next = temp->next;
        else
            station->cars = temp->next;

        if (temp->next != NULL)
            temp->next->prev = temp->prev;

        free(temp);
    }


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
    //insertStation returns the head of the tree
    *head = insertStation(*head, distance);

    Station* newStation = findStation(*head, distance);

    if (newStation == NULL) {
        printf("Unable to find the station that you just added\n");
        return 0;
    }

    int carRange;

    fflush(stdin);
    fflush(stdout);

    for (int i = 0; i < numCars; i++) {
        if (scanf("%d", &carRange) != 1){
            printf("error reading cars from aggiungi-stazione");
            return -1;
        }

        insertCar(newStation, carRange);
    }

    fflush(stdin);
    fflush(stdout);

    newStation->distance = distance;

    return 1;
}

int removeAllCarsInStation(Station *station) {
    Car* current = station->cars;
    Car* temp = NULL;
    while (current != NULL) {
        temp = current->next;
        free(current);
        current = temp;
    }
    return 1;
}

int deleteStation(Station **head, int distance) {
    Station *station = findStation(*head, distance);
    int carRemoval = 0;
    if (station == NULL) {
        //printf("unable to find station to be deleted.\n");
        return -1;
    }

    carRemoval = removeAllCarsInStation(station);

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

Station* findNext(Station* root, Station* temp) {
    Station *n = temp;
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
        //printf("Station not found.\n");
        return NULL;
    }

    Station* nextStation = findNext(*head, station);
    if (nextStation == NULL) {
        //printf("No next station found.\n");
        return NULL;
    }

    return nextStation;
}

Station* findPrevious(Station* root, Station* temp) {
    Station *n = temp;
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
        //printf("Station not found.\n");
        return NULL;
    }

    Station* prevStation = findPrevious(*head, station);
    if (prevStation == NULL) {
        //printf("No previous station found.\n");
        return NULL;
    }

    return prevStation;
}

void printPathForwards(Station* head){
    Station* current = head;
    Station* temp =NULL;

    while(current!=NULL){
        //print station number
        printf("%d",current->distance);

        temp= current;
        current = current->nextInPath;

        if (current!=NULL){
            printf(" ");
        }
        temp->prevInPath = NULL;
        temp ->nextInPath = NULL;
    }
    printf("\n");

}

void printPathBackwards(Station* tail){
    Station* current = tail;
    Station* temp =NULL;

    while(current!=NULL){
        //print station number
        printf("%d",current->distance);

        temp= current;
        current = current->prevInPath;

        if (current!=NULL){
            printf(" ");
        }

        temp->prevInPath = NULL;
        temp ->nextInPath = NULL;
    }
    printf("\n");

}

void cleanPathForwards(Station* head){
    Station* current = head;
    Station* temp =NULL;

    while(current!=NULL){
        temp= current;
        current = current->nextInPath;
        temp->nextInPath = NULL;
        temp ->prevInPath = NULL;
    }
}

void cleanPathBackwards(Station* tail){
    Station* current = tail;
    Station* temp =NULL;

    while(current!=NULL){
        temp = current;
        current = current->prevInPath;
        temp->prevInPath = NULL;
        temp ->nextInPath = NULL;
    }
}

void optimizeForwards (Station** head,  Station* tail){
    Station* a = tail->prevInPath;

    if (a==NULL) return;

    Station* temp = a->nextInPath;

    if (temp == NULL) return;

    Station* b = a->prevInPath;

    while(b!=NULL){

        while(temp != NULL && temp->distance < a->distance){

            if (temp->distance - temp->maxRange <= a->nextInPath->distance &&
                temp->distance >= b->distance- b->maxRange){
                // sostituisco temp ad a

                a->nextInPath->prevInPath = temp;
                b->nextInPath = temp;

                temp->prevInPath = b;
                temp->nextInPath = a->nextInPath;

                a->nextInPath = NULL;
                a->prevInPath = NULL;

                a = temp;
                break;
            }

            temp = findNextStation(head, temp->distance);
        }

        a=b;
        temp = a->nextInPath;
        b=a->prevInPath;
    }

}

void optimizeBackwards (Station** head,  Station* tail){
    Station* a = tail->prevInPath;
    if (a==NULL) return;
    Station* temp = findPreviousStation(head, a->distance);
    if (temp == NULL) return;
    Station* b = a->prevInPath;

    while(b!=NULL){
        while(temp != NULL && temp->distance != b->distance){
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
        }

        a=b;
        temp = findPreviousStation(head, a->distance);
        b=a->prevInPath;
    }

}

//ritorna 1 se percorso trovato
int findPathForwards(Station** head, int startDistance, int endDistance) {

    if (*head == NULL) {
        puts("head is NULL in findPathForward");
        return -1;
    }

    Station* startStation = findStation(*head, startDistance);
    Station* endStation = findStation(*head, endDistance);

    if (startStation == NULL || endStation == NULL) {
        puts("Start or end is NULL");
        return -1;
    }

    int tempMaxReach = 0;
    Station* tempMaxReachStation = NULL;

    Station* currentStation = startStation;
    Station *nextStation = findNextStation(head, currentStation->distance);

    while(currentStation != NULL && nextStation!= NULL){


        while (nextStation != NULL && nextStation->distance <= currentStation->distance + currentStation->maxRange){

            if (nextStation->distance + nextStation->maxRange >= endDistance){
                //printf("nextStation->distance + nextStation->maxRange >= endDistance\n");
                nextStation->prevInPath = currentStation;
                currentStation->nextInPath = nextStation;

                currentStation = nextStation;
                nextStation = endStation;
            }

            if (nextStation->distance == endDistance){
                //printf("nextStation->distance == endDistance\n");
                nextStation->prevInPath = currentStation;
                currentStation->nextInPath = nextStation;

                optimizeBackwards(head, nextStation);
                printPathForwards(startStation);
                return 1;
            }

            else if (nextStation->distance > tempMaxReach){
                tempMaxReach = nextStation->distance;
                tempMaxReachStation = nextStation;
            }

            nextStation = findNextStation(head, nextStation->distance);

        }


        if (tempMaxReachStation==NULL || tempMaxReachStation->distance == currentStation->distance){
            //printf("tempMaxReachStation == currentStation\n");
            cleanPathForwards(startStation);
            return -1;
        }

        tempMaxReachStation -> prevInPath = currentStation;
        currentStation -> nextInPath = tempMaxReachStation;

        currentStation = tempMaxReachStation;
        nextStation = findNextStation(head, currentStation->distance);

    }



    cleanPathForwards(startStation);
    return -1;
}


int findPathBackwards(Station** head, int startDistance, int endDistance) {

    if (*head == NULL) {
        puts("head is NULL in findPathBackwards");
        return -1;
    }

    Station* startStation = findStation(*head, startDistance);

    Station* endStation = findStation(*head, endDistance);

    if (startStation == NULL || endStation == NULL) {
        puts("Start or end is NULL");
        return -1;
    }



    int tempMaxReach = startDistance;
    Station* tempMaxReachStation = startStation;

    Station* currentStation = startStation;

    Station *nextStation = findPreviousStation(head, currentStation->distance);



    while(currentStation != NULL && nextStation!= NULL){

        while (currentStation!=0 && nextStation != NULL && nextStation->distance >= currentStation->distance - currentStation->maxRange){

            if(nextStation == NULL|| nextStation->distance < currentStation->distance-currentStation
                    ->maxRange) break;

            if (nextStation->distance >= currentStation->distance - currentStation->maxRange &&
                nextStation->distance - nextStation->maxRange <= endDistance){

                //printf("nextStation->distance + nextStation->maxRange >= endDistance\n");
                nextStation->prevInPath = currentStation;
                currentStation->nextInPath = nextStation;

                currentStation = nextStation;
                nextStation = endStation;

                nextStation->prevInPath = currentStation;
                currentStation->nextInPath = nextStation;


                optimizeForwards(head, nextStation);
                printPathForwards(startStation);

                return 1;
            }

            else if (nextStation->distance >= currentStation->distance - currentStation->maxRange &&
                     nextStation->distance - nextStation->maxRange < tempMaxReach){
                tempMaxReach = nextStation->distance - nextStation->maxRange;
                tempMaxReachStation = nextStation;
            }

            nextStation = findPreviousStation(head, nextStation->distance);
            if(nextStation == NULL|| nextStation->distance < currentStation->distance-currentStation
            ->maxRange) break;
        }


        if (tempMaxReachStation==NULL || tempMaxReachStation->distance == currentStation->distance){
            //printf("tempMaxReachStation == currentStation\n");
            cleanPathForwards(startStation);
            return -1;
        }

        tempMaxReachStation -> prevInPath = currentStation;
        currentStation -> nextInPath = tempMaxReachStation;

        currentStation = tempMaxReachStation;
        nextStation = findPreviousStation(head, currentStation->distance);

    }



    cleanPathForwards(startStation);
    return -1;
}


//Specific use case functions -- END

void handle_aggiungi_stazione(Station **head) {
    int dist, numCars;
    if(scanf("%d %d", &dist, &numCars) != 2){
        puts("error reading distance and numCars in aggiungi-stazione");
        return;
    }

    fflush(stdin);
    fflush(stdout);

    if(findStation(*head, dist) != NULL){
        puts("non aggiunta");
        return;
    }

    if (addStation(head, dist, numCars)) {
        puts("aggiunta");
    }
}

void handle_demolisci_stazione(Station **head) {
    int dist;
    if(scanf("%d", &dist) != 1){
        printf("error reading distance in demolisci-stazione\n");
        return;
    }

    if (deleteStation(head, dist)==1){
        puts("demolita");
    } else {
        puts("non demolita");
    }
}

void handle_aggiungi_auto(Station *head) {
    int dist, carRange;
    if(scanf("%d %d", &dist, &carRange) != 2) {
        printf("error reading distance and carRange in aggiungi-auto\n");
        return;
    }
    Station* station = findStation(head, dist);
    if (station == NULL) {
        puts("non aggiunta");
    }
    else if (insertCar(station, carRange)) {
        puts("aggiunta");
    }
}

void handle_rottama_auto(Station *head) {
    int dist, carRange;
    if(scanf("%d %d", &dist, &carRange) != 2) {
        printf("error reading distance and carRange in rottama-auto\n");
        return;
    }

    Station* station = findStation(head, dist);

    if (station == NULL) {
        puts("non rottamata");
    } else if (removeCar(station, carRange)==1){
        puts("rottamata");
    } else {
        puts("non rottamata");
    }
}

void handle_pianifica_percorso(Station **head) {
    int start, end;
    if(scanf("%d %d", &start, &end) != 2) {
        printf("error reading startDist and endDist in pianifica-percorso\n");
        return;
    }

    int temp = -1;

    if (start < end) {
        temp = findPathForwards(head, start, end);
    }
    else if (start == end) {
        temp = 0;
        printf("%d\n", start);
    }
    else if (start > end) {
        temp = findPathBackwards(head, start, end);
    }

    if (temp != 1 && temp != 0) {
        puts("nessun percorso");
    }
}

int main() {
    char cmd[30];

    Station * head = NULL;

    while (scanf("%s", cmd)!= EOF) {

        if (strcmp(cmd, "aggiungi-stazione") == 0) {
            handle_aggiungi_stazione(&head);
            fflush(stdin);
            fflush (stdout);
        } else if (strcmp(cmd, "demolisci-stazione") == 0) {
            handle_demolisci_stazione(&head);
            fflush(stdin);
            fflush(stdout);
        } else if (strcmp(cmd, "aggiungi-auto") == 0) {
            handle_aggiungi_auto(head);
            fflush(stdin);
            fflush(stdout);
        } else if (strcmp(cmd, "rottama-auto") == 0) {
            handle_rottama_auto(head);
            fflush(stdin);
            fflush(stdout);
        } else if (strcmp(cmd, "pianifica-percorso") == 0) {
            handle_pianifica_percorso(&head);
            fflush(stdin);
            fflush(stdout);
        } else if (strcmp(cmd, "stampa") == 0) {
            printInOrder(head);
            fflush(stdin);
        }
    }

    return 0;
}