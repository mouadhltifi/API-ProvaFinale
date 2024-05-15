# API-ProvaFinale

Final Project of the "Algoritmi e Principi dell'Informatica" Course - 2022-2023 Edition.

### Key Features

1. **AVL Tree Implementation**:
   - The stations are stored in an AVL tree, which ensures balanced and efficient searching, insertion, and deletion operations.
   - Functions for right and left rotations, balancing, and maintaining the height of the nodes are implemented to keep the tree balanced.
   - The `addStation`, `removeStation`, and `findStation` functions manage the stations within the AVL tree.

2. **Linked List for Cars**:
   - Each station maintains a linked list of cars.
   - Functions such as `addCar`, `removeCar`, and `findMaxRange` manage the cars within each station.
   - The linked list implementation allows efficient insertion and deletion of cars within a station.

3. **Command Handling**:
   - The `main` function reads commands from the standard input and calls the appropriate handler functions.
   - Supported commands include `aggiungi-stazione`, `demolisci-stazione`, `aggiungi-auto`, `rottama-auto`, `pianifica-percorso`, and `stampa`.
   - Each command triggers specific operations like adding or removing stations and cars, or printing the station details in order.

4. **Route Planning**:
   - The code includes functions to plan routes between stations, either forwards or backwards.
   - It ensures that the routes are correctly identified and handles cases where no route is available.

### Peculiarities

1. **Memory Management**:
   - The code dynamically allocates and deallocates memory for stations and cars using `malloc` and `free`.
   - Careful attention is given to updating parent pointers when stations are removed.

2. **Input and Output Handling**:
   - The code uses `scanf` for reading input and `printf` for outputting results, which makes it suitable for command-line interaction.
   - Error handling is included to manage incorrect inputs gracefully.

3. **Efficiency Considerations**:
   - The AVL tree ensures that the operations related to stations are performed in logarithmic time, maintaining efficiency even as the number of stations grows.
   - The linked list allows efficient management of cars within each station, although it may not be as efficient for large numbers of cars compared to more complex data structures.

4. **Modular Structure**:
   - The code is divided into several functions, each responsible for a specific task, improving readability and maintainability.
   - Separate sections for AVL tree functions and linked list functions are clearly marked.

### Functions Overview

- **AVL Tree Functions**:
  - `rightRotate`, `leftRotate`: Perform rotations to maintain tree balance.
  - `getBalance`, `height`: Calculate balance factors and heights of nodes.
  - `addStation`, `removeStation`: Add or remove stations in the AVL tree.
  - `findStation`, `minValueStation`: Search for stations and find the minimum value station.

- **Linked List Functions**:
  - `addCar`, `removeCar`: Add or remove cars in a station.
  - `findMaxRange`: Find the car with the maximum range in a station.

- **Command Handlers**:
  - `handle_aggiungi_stazione`, `handle_demolisci_stazione`: Handle adding and demolishing stations.
  - `handle_aggiungi_auto`, `handle_rottama_auto`: Handle adding and scrapping cars.
  - `handle_pianifica_percorso`: Handle route planning.
  - `printInOrder`: Print stations in order.

### Usage

Compile the code using a C compiler:
```sh
gcc -o station_manager main2.c
```

Run the program and provide commands through standard input:
```sh
./station_manager
```

Example commands:
```
aggiungi-stazione 10 100
aggiungi-auto 10 50
stampa
pianifica-percorso 10 20
```
