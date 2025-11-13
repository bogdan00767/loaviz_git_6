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
        arr[i] = (int*)malloc(n * sizeof(int));
        for (int j = 0; j < n; ++j) arr[i][j] = 0;
    }
    return arr;
}

void freeArr(int** arr, int n) {
    if (!arr) return;
    for (int i = 0; i < n; ++i) free(arr[i]);
    free(arr);
}

void printArr(int** arr, int n) {
    std::cout << "Матрица смежности (" << n << " x " << n << "):\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            std::cout << arr[i][j] << " ";
        std::cout << "\n";
    }
}

int** generateGraph(int n) {
    int** arr = allocArr(n);
    for (int i = 0; i < n; ++i)
        for (int j = i; j < n; ++j) {
            arr[i][j] = arr[j][i] = rand() % 2;
        }
    return arr;
}

int** unionGraphs(int** a1, int** a2, int n1, int n2, int& newN) {
    newN = (n1 > n2 ? n1 : n2);
    int** g = allocArr(newN);
    for (int i = 0; i < newN; ++i)
        for (int j = 0; j < newN; ++j) {
            int v1 = (i < n1 && j < n1) ? a1[i][j] : 0;
            int v2 = (i < n2 && j < n2) ? a2[i][j] : 0;
            g[i][j] = v1 || v2;
        }
    return g;
}

int** intersectGraphs(int** a1, int** a2, int n1, int n2, int& newN) {
    newN = (n1 < n2 ? n1 : n2);
    int** g = allocArr(newN);
    for (int i = 0; i < newN; ++i)
        for (int j = 0; j < newN; ++j)
            g[i][j] = a1[i][j] && a2[i][j];
    return g;
}

int** xorGraphs(int** a1, int** a2, int n1, int n2, int& newN) {
    newN = (n1 > n2 ? n1 : n2);
    int** g = allocArr(newN);
    for (int i = 0; i < newN; ++i)
        for (int j = 0; j < newN; ++j) {
            int v1 = (i < n1 && j < n1) ? a1[i][j] : 0;
            int v2 = (i < n2 && j < n2) ? a2[i][j] : 0;
            g[i][j] = (v1 + v2) % 2;
        }
    return g;
}

int main() {
    srand(time(NULL));
    clearScreen();

    std::cout << "Объединение, пересечение, кольцевая сумма\n";

    int n1 = isInteger("Введите количество вершин G1: ");
    while (n1 < 2) n1 = isInteger("Минимум 2. Повторите: ");
    int n2 = isInteger("Введите количество вершин G2: ");
    while (n2 < 2) n2 = isInteger("Минимум 2. Повторите: ");

    int** g1 = generateGraph(n1);
    int** g2 = generateGraph(n2);

    std::cout << "\nG1:\n"; printArr(g1, n1);
    std::cout << "\nG2:\n"; printArr(g2, n2);

    std::cout << "\nВыберите операцию:\n1 — Объединение\n2 — Пересечение\n3 — Кольцевая сумма\n";
    int op;
    while (true) {
        op = isInteger("Введите номер операции: ");
        if (op >= 1 && op <= 3) break;
        std::cout << "Ошибка: неверный выбор\n";
    }

    int newN;
    int** res = nullptr;
    if (op == 1) res = unionGraphs(g1, g2, n1, n2, newN);
    else if (op == 2) res = intersectGraphs(g1, g2, n1, n2, newN);
    else if (op == 3) res = xorGraphs(g1, g2, n1, n2, newN);

    if (res) {
        std::cout << "\nРезультат:\n";
        printArr(res, newN);
        freeArr(res, newN);
    }

    freeArr(g1, n1);
    freeArr(g2, n2);

    std::cout << "\n\n";
    
    return 0;
}