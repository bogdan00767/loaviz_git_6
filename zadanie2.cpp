#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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

int** allocArr(int n) {
    int** arr = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i) {
        arr[i] = (int*)calloc(n, sizeof(int));
    }
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

void printArrWithNames(int** arr, int n, std::string* names) {
    std::cout << "Матрица смежности с именами вершин:\n";
    for (int i = 0; i < n; ++i) {
        std::cout.width(3);
        std::cout << i << ": ";
        for (int j = 0; j < n; ++j) {
            std::cout.width(3);
            std::cout << arr[i][j];
        }
        std::cout << "  (" << names[i] << ")\n";
    }
}

// Генерация случайного неориентированного графа
int** generateGraph(int n) {
    int** arr = allocArr(n);
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            arr[i][j] = arr[j][i] = rand() % 2;
        }
    }
    return arr;
}

// Отождествление вершин u и v
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
            if (i == u && j == u) {
                // петля сохраняется, не добавляем ребро u-v
                newArr[idx_i][idx_j] = arr[u][u] || arr[v][v];
            } else if (i == u) {
                newArr[idx_i][idx_j] = arr[u][j] || arr[v][j];
            } else if (j == u) {
                newArr[idx_i][idx_j] = arr[i][u] || arr[i][v];
            } else {
                newArr[idx_i][idx_j] = arr[i][j];
            }
            idx_j++;
        }
        idx_i++;
    }
    return newArr;
}

// Стягивание ребра u-v
int** contractEdge(int** arr, int n, int u, int v, int& newN) {
    if (arr[u][v] == 0) return nullptr;
    return mergeVertices(arr, n, u, v, newN);
}

// Расщепление вершины v
int** splitVertex(int** arr, int n, int v, int& newN) {
    newN = n + 1;
    int** newArr = allocArr(newN);
    // копируем старую матрицу
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            newArr[i][j] = arr[i][j];

    // копируем связи вершины v на новую вершину
    for (int j = 0; j < n; ++j) {
        if (j != v && arr[v][j]) {
            if (j % 2 == 0) newArr[v][j] = newArr[j][v] = 0;
            else newArr[newN - 1][j] = newArr[j][newN - 1] = 1;
        }
    }
    newArr[v][newN - 1] = newArr[newN - 1][v] = 1; // соединяем старую и новую вершины
    newArr[newN - 1][newN - 1] = arr[v][v];       // петля копируется
    return newArr;
}

int main() {
    srand(time(NULL));
    clearScreen();

    int n = isInteger("Введите количество вершин графа: ");
    while (n < 3) n = isInteger("Минимум 3. Повторите: ");

    int** arr = generateGraph(n);
    std::string* names = new std::string[n];
    for (int i = 0; i < n; ++i) names[i] = std::to_string(i);

    printArrWithNames(arr, n, names);

    int newN;
    int** result = copyArr(arr, n);
    std::string* resultNames = new std::string[n];
    for (int i = 0; i < n; ++i) resultNames[i] = names[i];

    // === Отождествление вершин ===
    std::cout << "\n=== Отождествление вершин ===\n";
    int u, v;
    while (true) {
        u = isInteger("Введите вершину u для отождествления: ");
        v = isInteger("Введите вершину v для отождествления: ");
        if (u >= 0 && u < n && v >= 0 && v < n && u != v) break;
        std::cout << "Ошибка: неверные вершины, повторите.\n";
    }

    int** temp = mergeVertices(result, n, u, v, newN);
    if (temp) {
        std::string* tempNames = new std::string[newN];
        int idx = 0;
        for (int i = 0; i < n; ++i)
            if (i != v) tempNames[idx++] = (i == u) ? resultNames[u] + "+" + resultNames[v] : resultNames[i];
        freeArr(result, n);
        delete[] resultNames;
        result = temp;
        resultNames = tempNames;
        n = newN;
        printArrWithNames(result, n, resultNames);
    }

    // === Стягивание ребра ===
    std::cout << "\n=== Стягивание ребра ===\n";
    while (true) {
        u = isInteger("Введите вершину u: ");
        v = isInteger("Введите вершину v: ");
        if (u >= 0 && u < n && v >= 0 && v < n && u != v && result[u][v] == 1) break;
        std::cout << "Ошибка: вершины не смежны или неверны.\n";
    }

    temp = contractEdge(result, n, u, v, newN);
    if (temp) {
        std::string* tempNames = new std::string[newN];
        int idx = 0;
        for (int i = 0; i < n; ++i)
            if (i != v) tempNames[idx++] = (i == u) ? resultNames[u] + "+" + resultNames[v] : resultNames[i];
        freeArr(result, n);
        delete[] resultNames;
        result = temp;
        resultNames = tempNames;
        n = newN;
        printArrWithNames(result, n, resultNames);
    }

    // === Расщепление вершины ===
    std::cout << "\n=== Расщепление вершины ===\n";
    while (true) {
        int w = isInteger("Введите вершину для расщепления: ");
        if (w >= 0 && w < n) {
            temp = splitVertex(result, n, w, newN);
            if (temp) {
                std::string* tempNames = new std::string[newN];
                for (int i = 0; i < n; ++i) tempNames[i] = resultNames[i];
                tempNames[newN - 1] = resultNames[w] + "_split";
                freeArr(result, n);
                delete[] resultNames;
                result = temp;
                resultNames = tempNames;
                n = newN;
                printArrWithNames(result, n, resultNames);
            }
            break;
        } else {
            std::cout << "Ошибка: неверная вершина.\n";
        }
    }

    freeArr(result, n);
    delete[] resultNames;
    freeArr(arr, n);
    delete[] names;

    std::cout << "\nРабота завершена.\n";
    return 0;
}