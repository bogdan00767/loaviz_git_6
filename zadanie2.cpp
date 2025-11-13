#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <locale>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

struct AdjList {
    int** lists;
    int* lengths;
    int n;
};

AdjList* createAdjListFromMatrix(int** matrix, int n) {
    AdjList* adj = new AdjList;
    adj->n = n;
    adj->lists = (int**)malloc(n * sizeof(int*));
    adj->lengths = (int*)calloc(n, sizeof(int));

    for (int i = 0; i < n; ++i) {
        int count = 0;
        for (int j = 0; j < n; ++j)
            if (matrix[i][j]) count++;
        adj->lengths[i] = count;
        adj->lists[i] = (int*)malloc(count * sizeof(int));
        int idx = 0;
        for (int j = 0; j < n; ++j)
            if (matrix[i][j]) adj->lists[i][idx++] = j;
    }
    return adj;
}

void printAdjList(AdjList* adj) {
    std::cout << "\nСписки смежности:\n";
    for (int i = 0; i < adj->n; ++i) {
        std::cout << i << ": ";
        if (adj->lengths[i] == 0) std::cout << "нет соседей";
        else {
            for (int j = 0; j < adj->lengths[i]; ++j) {
                std::cout << adj->lists[i][j];
                if (j < adj->lengths[i] - 1) std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
}

// Фри память списка
void freeAdjList(AdjList* adj) {
    if (!adj) return;
    for (int i = 0; i < adj->n; ++i) free(adj->lists[i]);
    free(adj->lists);
    free(adj->lengths);
    delete adj;
}

int** allocArr(int n) {
    int** arr = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i)
        arr[i] = (int*)calloc(n, sizeof(int));
    return arr;
}

void freeArr(int** arr, int n) {
    if (!arr) return;
    for (int i = 0; i < n; ++i) free(arr[i]);
    free(arr);
}

int** copyArr(int** src, int n) {
    int** dst = allocArr(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            dst[i][j] = src[i][j];
    return dst;
}

void printMatrix(int** arr, int n) {
    std::cout << "Матрица смежности:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << i << ": ";
        for (int j = 0; j < n; ++j)
            std::cout << arr[i][j] << " ";
        std::cout << "\n";
    }
}

int** generateGraph(int n) {
    int** arr = allocArr(n);
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            arr[i][j] = arr[j][i] = rand() % 2;
        }
    }
    return arr;
}

// Отождествление
int** mergeVertices(int** arr, int n, int u, int v, int& newN) {
    if (u == v || u < 0 || v < 0 || u >= n || v >= n) return nullptr;
    newN = n - 1;
    int** newArr = allocArr(newN);

    int idx_i = 0;
    for (int i = 0; i < n; ++i) {
        if (i == v) continue;
        int idx_j = 0;
        for (int j = 0; j < n; ++j) {
            if (j == v) continue;
            if (i == u && j == u) newArr[idx_i][idx_j] = arr[u][u] || arr[v][v];
            else if (i == u) newArr[idx_i][idx_j] = arr[u][j] || arr[v][j];
            else if (j == u) newArr[idx_i][idx_j] = arr[i][u] || arr[i][v];
            else newArr[idx_i][idx_j] = arr[i][j];
            idx_j++;
        }
        idx_i++;
    }
    return newArr;
}

// Стягивание
int** contractEdge(int** arr, int n, int u, int v, int& newN) {
    if (arr[u][v] == 0) return nullptr;
    return mergeVertices(arr, n, u, v, newN);
}

// Расщепление матрица
int** splitVertex(int** arr, int n, int v, int& newN) {
    newN = n + 1;
    int** newArr = allocArr(newN);

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            newArr[i][j] = arr[i][j];

    int newVertex = n;

    newArr[v][newVertex] = newArr[newVertex][v] = 1;
    newArr[newVertex][newVertex] = 0;

    for (int i = 0; i < n; ++i) {
        if (i != v) newArr[i][newVertex] = 0;
        if (i != v) newArr[newVertex][i] = 0;
    }

    return newArr;
}

// Расщепление список
AdjList* splitVertexAdjList(AdjList* adj, int v) {
    if (!adj || v < 0 || v >= adj->n) return nullptr;

    int newN = adj->n + 1;
    AdjList* newAdj = new AdjList;
    newAdj->n = newN;
    newAdj->lists = (int**)malloc(newN * sizeof(int*));
    newAdj->lengths = (int*)calloc(newN, sizeof(int));

    for (int i = 0; i < adj->n; ++i) {
        int len = adj->lengths[i];
        newAdj->lists[i] = (int*)malloc(len * sizeof(int));
        for (int j = 0; j < len; ++j)
            newAdj->lists[i][j] = adj->lists[i][j];
        newAdj->lengths[i] = len;
    }

    int newVertex = adj->n;

    newAdj->lengths[v]++;
    newAdj->lists[v] = (int*)realloc(newAdj->lists[v], newAdj->lengths[v] * sizeof(int));
    newAdj->lists[v][newAdj->lengths[v] - 1] = newVertex;

    newAdj->lengths[newVertex] = 1;
    newAdj->lists[newVertex] = (int*)malloc(sizeof(int));
    newAdj->lists[newVertex][0] = v;

    return newAdj;
}

int isInteger(const std::string& message) {
    int value;
    while (true) {
        std::cout << message;
        if (!(std::cin >> value)) {
            std::cout << "Ошибка: введено не число.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (std::cin.peek() != '\n') {
            std::cout << "Ошибка: введено не целое число.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        return value;
    }
}

int main() {
    setlocale(LC_ALL, "RUS");
    srand(time(NULL));
    clearScreen();

    int n = isInteger("Введите количество вершин графа: ");
    while (n < 3) n = isInteger("Минимум 3. Повторите: ");

    int** arr = generateGraph(n);

    std::cout << "Исходный граф:\n";
    printMatrix(arr, n);

    AdjList* currentAdjList = createAdjListFromMatrix(arr, n);
    printAdjList(currentAdjList);

    int newN;
    int** result = copyArr(arr, n);
    int currentSize = n;

    // Отождествление
    std::cout << "\nОтождествление вершин (матрица)\n";
    int u = isInteger("Введите вершину u для отождествления: ");
    int v = isInteger("Введите вершину v для отождествления: ");

    int** temp = mergeVertices(result, currentSize, u, v, newN);
    if (temp) {
        freeArr(result, currentSize);
        result = temp;
        currentSize = newN;
        printMatrix(result, currentSize);

        freeAdjList(currentAdjList);
        currentAdjList = createAdjListFromMatrix(result, currentSize);
        std::cout << "\nОтождествление вершин (списки смежности)\n";
        printAdjList(currentAdjList);
    }

    // Стягивание
    std::cout << "\nСтягивание ребра (матрица)\n";
    u = isInteger("Введите вершину u: ");
    v = isInteger("Введите вершину v: ");

    temp = contractEdge(result, currentSize, u, v, newN);
    if (temp) {
        freeArr(result, currentSize);
        result = temp;
        currentSize = newN;
        printMatrix(result, currentSize);

        freeAdjList(currentAdjList);
        currentAdjList = createAdjListFromMatrix(result, currentSize);
        std::cout << "\nСтягивание ребра (списки смежности)\n";
        printAdjList(currentAdjList);
    }

    // Расщепление
    std::cout << "\nРасщепление вершины (матрица)\n";
    int w = isInteger("Введите вершину для расщепления: ");
    temp = splitVertex(result, currentSize, w, newN);
    if (temp) {
        freeArr(result, currentSize);
        result = temp;
        currentSize = newN;
        printMatrix(result, currentSize);

        AdjList* visualAdj = splitVertexAdjList(currentAdjList, w);
        std::cout << "\nРасщепление вершины (списки смежности)\n";
        printAdjList(visualAdj);
        freeAdjList(visualAdj);
    }

    freeArr(result, currentSize);
    freeArr(arr, n);
    freeAdjList(currentAdjList);

    std::cout << "\n\n";

    return 0;
}