#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

} Station;

// Graph structure
typedef struct GraphNode {
    int dest;
    int weight;
    struct GraphNode* next;
} GraphNode;

typedef struct Graph {
    int numVertices;
    GraphNode** adjLists;
} Graph;

typedef struct MinHeapNode {
    int  v;
    int dist;
} MinHeapNode;

typedef struct MinHeap {
    int size;
    int capacity;
    int *pos;
    MinHeapNode **array;
} MinHeap;

// A utility function to create a new adjacency list node
GraphNode* createNode(int dest, int weight) {
    GraphNode* newNode = malloc(sizeof(GraphNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// A utility function to create a graph of given vertices
Graph* createGraph(int vertices) {
    Graph* graph = malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->adjLists = malloc(vertices * sizeof(GraphNode*));

    for (int i = 0; i < vertices; i++)
        graph->adjLists[i] = NULL;

    return graph;
}

// Adds an edge to an undirected graph
void addEdge(Graph* graph, int src, int dest, int weight) {
    // Add an edge from src to dest.
    GraphNode* newNode = createNode(dest, weight);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;

    // Add an edge from dest to src.
    newNode = createNode(src, weight);
    newNode->next = graph->adjLists[dest];
    graph->adjLists[dest] = newNode;
}

// A utility function to create a new Min Heap GraphNode
MinHeapNode* newMinHeapNode(int v, int dist) {
    MinHeapNode* minHeapNode = (MinHeapNode*) malloc(sizeof(MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// A utility function to create a Min Heap
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// A utility function to swap two nodes of min heap
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// A standard function to heapify at given idx
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist )
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist )
        smallest = right;

    if (smallest != idx) {
        // The nodes to be swapped in min heap
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        // Swap positions
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        // Swap nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// A utility function to check if the given minHeap is ampty or not
int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

// Standard function to extract minimum node from heap
MinHeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    // Store the root node
    MinHeapNode* root = minHeap->array[0];

    // Replace root node with last node
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // Update position of last node
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;

    // Reduce heap size and heapify root
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// Function to decrease dist value of a given vertex v
void decreaseKey(MinHeap* minHeap, int v, int dist) {
    // Get the index of v in heap array
    int i = minHeap->pos[v];

    // Get the node and update its dist value
    minHeap->array[i]->dist = dist;

    // Travel up while the complete tree is not hepified.
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) {
        // Swap this node with its parent
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        // move to parent index
        i = (i - 1) / 2;
    }
}

// A utility function to check if a given vertex 'v' is in min heap or not
bool isInMinHeap(MinHeap *minHeap, int v) {
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

// A utility function used to print the solution
void printArr(int dist[], int n) {
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i)
        printf("%d \t\t %d\n", i, dist[i]);
}


// Dijkstra algorithm implementation
void dijkstra(Graph* graph, int src) {
    int V = graph->numVertices;
    int dist[V];
    MinHeap* minHeap = createMinHeap(V);

    for (int v = 0; v < V; ++v) {
        dist[v] = INF;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
    minHeap->size = V;

    while (!isEmpty(minHeap)) {
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v;
        GraphNode* pCrawl = graph->adjLists[u];

        while (pCrawl != NULL) {
            int v = pCrawl->dest;

            if (isInMinHeap(minHeap, v) && dist[u] != INF &&
                pCrawl->weight + dist[u] < dist[v]) {
                dist[v] = dist[u] + pCrawl->weight;
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }

    printArr(dist, V);
}

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
        //printf("Station not found.\n");
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

//Specific use case functions -- END

int main() {
    char cmd[30];
    int dist, numCars, carRange, ret, modified;

    Station * head = NULL;


    while ((ret = scanf("%s", cmd)) && *cmd != EOF) {

        if(ret != 1) break;

        else if (strcmp(cmd, "aggiungi-stazione") == 0) {

            if(scanf("%d %d", &dist, &numCars) != 2){
                printf("error reading distance and numCars from aggiungi-stazione");
                break;
            }

            if (addStation(&head, dist, numCars)) puts("aggiunta");
            else puts("non aggiunta");

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
                printf("Station not found.\n");
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

            //TODO
        }

        else if (strcmp(cmd, "stampa-stazioni") == 0) {
            printInOrder(head);
        }
    }

    return 0;
}