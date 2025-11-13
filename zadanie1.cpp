#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>

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

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int** allocArr(int n) {
    int** arr = new int*[n];
    for (int i = 0; i < n; ++i) {
        arr[i] = new int[n];
        for (int j = 0; j < n; ++j) arr[i][j] = 0;
    }
    return arr;
}

void freeArr(int** arr, int n) {
    if (!arr) return;
    for (int i = 0; i < n; ++i) delete[] arr[i];
    delete[] arr;
}

void printArr(int** arr, int n) {
    std::cout << "Матрица смежности (" << n << " x " << n << "):\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) std::cout << arr[i][j] << " ";
        std::cout << "\n";
    }
}

void toAdjList(int** arr, int n) {
    std::cout << "Список смежности:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << i << ": ";
        for (int j = 0; j < n; ++j) if (arr[i][j]) std::cout << j << " ";
        std::cout << "\n";
    }
}

int** generateGraph(int n) {
    int** arr = allocArr(n);
    for (int i = 0; i < n; ++i)
        for (int j = i; j < n; ++j)
            arr[i][j] = arr[j][i] = rand() % 2;
    return arr;
}

int main() {
    clearScreen();
    srand(static_cast<unsigned int>(time(nullptr)));

    std::cout << "Генерация двух графов и вывод их матриц и списков смежности\n";

    int n1 = isInteger("Введите количество вершин G1: ");
    while (n1 < 2) n1 = isInteger("Минимум 2. Повторите: ");

    int n2 = isInteger("Введите количество вершин G2: ");
    while (n2 < 2) n2 = isInteger("Минимум 2. Повторите: ");

    int** M1 = generateGraph(n1);
    int** M2 = generateGraph(n2);

    std::cout << "\nМатрица смежности G1:\n";
    printArr(M1, n1);
    std::cout << "\nСписок смежности G1:\n";
    toAdjList(M1, n1);

    std::cout << "\nМатрица смежности G2:\n";
    printArr(M2, n2);
    std::cout << "\nСписок смежности G2:\n";
    toAdjList(M2, n2);

    freeArr(M1, n1);
    freeArr(M2, n2);

    std::cout << "\n\n";
    
    return 0;
}