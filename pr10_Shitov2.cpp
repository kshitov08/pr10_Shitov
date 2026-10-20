// pr10_Shitov2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstring>

using namespace std;

const int WIDTH = 20;
const int HEIGHT = 20;

struct SnakeNode {
    int x;
    int y;
    SnakeNode* next;
};

struct SaveSnake {
    char playerName[50];
    int score;
    int dirX;
    int dirY;
    int foodX;
    int foodY;
    int length;
    int difficult;
};

struct Record {
    char name[50];
    int score;
    int difficult;
};

SnakeNode* head = nullptr;
int score = 0;
int foodX, foodY;
int dirX, dirY;
bool gameOver = false;
bool isPaused = false;
char playerName[50] = "";
int currentDifficult = 1;

int countForRec = 1;

void addHead(int x, int y) {
    SnakeNode* newNode = new SnakeNode;
    newNode->x = x;
    newNode->y = y;
    newNode->next = head;
    head = newNode;
}

void removeTail() {
    SnakeNode* current = head;
    while (current->next->next != nullptr) {
        current = current->next;
    }
    delete current->next;
    current->next = nullptr;
}

void clearSnake() {
    while (head != nullptr) {
        SnakeNode* temp = head;
        head = head->next;
        delete temp;
    }
    head = nullptr;
}

void saveRecord(const char* name, int finalScore, int difficult) {
    if (finalScore == 0) {
        cout << "\nСчёт равен 0, рекорд не сохранён.\n";
        Sleep(1500);
        return;
    }

    ofstream file("records.bin", ios::binary | ios::app);
    if (!file.is_open()) {
        cout << "Не удалось открыть файл рекордов!\n";
        Sleep(2000);
        return;
    }

    Record rec;
    strncpy_s(rec.name, name, 49);
    rec.name[49] = '\0';
    rec.score = finalScore;
    rec.difficult = difficult;

    file.write((char*)&rec, sizeof(Record));
    file.close();

    cout << "Рекорд сохранён в таблицу!\n";
    Sleep(1500);
}

void showRecords() {
    ifstream file("records.bin", ios::binary);
    if (!file.is_open()) {
        cout << "\nТаблица рекордов пуста.\n";
        Sleep(2000);
        return;
    }

    cout << "\nТаблица рекордов\n";

    Record rec;
    bool hasRecords = false;

    while (file.read((char*)&rec, sizeof(Record))) {
        hasRecords = true;
        cout << countForRec << ". " << rec.name << " | " << rec.score << " | ";
        countForRec++;

        if (rec.difficult == 1) cout << "Легко";
        else if (rec.difficult == 2) cout << "Нормально";
        else if (rec.difficult == 3) cout << "Сложно";

        cout << "\n";
    }

    file.close();
    cout << "\nНажмите любую клавишу для выхода...";
    _getch();
}

void saveGame() {
    ofstream file("savegame.bin", ios::binary);

    if (!file.is_open()) {
        cout << "\nНе удалось открыть файл для сохранения!\n";
        Sleep(2000);
        return;
    }

    SaveSnake saveSnake;
    strncpy_s(saveSnake.playerName, playerName, 49);
    saveSnake.playerName[49] = '\0';
    saveSnake.score = score;
    saveSnake.dirX = dirX;
    saveSnake.dirY = dirY;
    saveSnake.foodX = foodX;
    saveSnake.foodY = foodY;
    saveSnake.difficult = currentDifficult;

    int len = 0;
    SnakeNode* current = head;
    while (current != nullptr) {
        len++;
        current = current->next;
    }
    saveSnake.length = len;

    file.write((char*)&saveSnake, sizeof(SaveSnake));

    current = head;
    while (current != nullptr) {
        file.write((char*)&current->x, sizeof(int));
        file.write((char*)&current->y, sizeof(int));
        current = current->next;
    }

    file.close();
}

bool loadGame() {
    ifstream file("savegame.bin", ios::binary);
    if (!file.is_open()) return false;

    clearSnake();

    SaveSnake saveSnake;
    file.read((char*)&saveSnake, sizeof(SaveSnake));

    if (saveSnake.length <= 0 || saveSnake.length > 1000) {
        file.close();
        return false;
    }
    strncpy_s(playerName, saveSnake.playerName, 49);
    playerName[49] = '\0';
    score = saveSnake.score;
    dirX = saveSnake.dirX;
    dirY = saveSnake.dirY;
    foodX = saveSnake.foodX;
    foodY = saveSnake.foodY;
    currentDifficult = saveSnake.difficult;

    int* xs = new int[saveSnake.length];
    int* ys = new int[saveSnake.length];
    for (int i = 0; i < saveSnake.length; i++) {
        file.read((char*)&xs[i], sizeof(int));
        file.read((char*)&ys[i], sizeof(int));
    }

    for (int i = saveSnake.length - 1; i >= 0; i--) {
        addHead(xs[i], ys[i]);
    }

    delete[] xs;
    delete[] ys;
    file.close();
    return true;
}

void generateFood() {
    bool valid = false;
    int attempts = 0;
    while (!valid && attempts < 1000) {
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
        valid = true;
        SnakeNode* current = head;
        while (current != nullptr) {
            if (current->x == foodX && current->y == foodY) {
                valid = false;
                break;
            }
            current = current->next;
        }
        attempts++;
    }
}

void draw() {
    system("cls");
    for (int i = 0; i < WIDTH + 1; i++) cout << "# ";
    cout << "\n";

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0) cout << "#";

            bool isSnake = false;
            SnakeNode* current = head;
            while (current != nullptr) {
                if (current->x == x && current->y == y) {
                    if (current == head) cout << "O ";
                    else cout << "o ";
                    isSnake = true;
                    break;
                }
                current = current->next;
            }

            if (!isSnake) {
                if (x == foodX && y == foodY) cout << "* ";
                else cout << "  ";
            }

            if (x == WIDTH - 1) cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < WIDTH + 1; i++) cout << "# ";
    cout << "\n";

    cout << "Счёт: " << score << "\n";
    cout << "Управление: стрелочки. P - пауза + сохранение\n";
}

void processInput() {
    if (_kbhit()) {
        int key = _getch();
        if (key == 224) {
            key = _getch();
            switch (key) {
            case 72:
                if (dirY != 1) { dirX = 0; dirY = -1; }
                break;
            case 80:
                if (dirY != -1) { dirX = 0; dirY = 1; }
                break;
            case 75:
                if (dirX != 1) { dirX = -1; dirY = 0; }
                break;
            case 77:
                if (dirX != -1) { dirX = 1; dirY = 0; }
                break;
            }
        }
        else {
            switch (key)
            {
            case 'p': case 'P': case 'з': case 'З':
                isPaused = true;
                break;
            }
        }
    }
}

void update() {
    if (gameOver) return;
    if (isPaused) return;

    if (head == nullptr) {
        gameOver = true;
        return;
    }

    int newX = head->x + dirX;
    int newY = head->y + dirY;

    if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) {
        cout << endl << "Столкновение со стеной! newX=" << newX << " newY=" << newY << "\n";
        Sleep(2000);
        gameOver = true;
        return;
    }

    SnakeNode* current = head;
    while (current->next != nullptr) {
        if (current->x == newX && current->y == newY) {
            cout << endl << "Столкновение с собой!\n";
            Sleep(2000);
            gameOver = true;
            return;
        }
        current = current->next;
    }

    addHead(newX, newY);

    if (newX == foodX && newY == foodY) {
        score += 10;
        generateFood();
    }
    else {
        removeTail();
    }
}

void startNewGame(bool isNewGame) {
    clearSnake();
    int speed = 0;
    int choice = 0;
    gameOver = false;
    isPaused = false;
    if (isNewGame)
    {
        score = 0;
        dirX = 1;
        dirY = 0;


        int startX = WIDTH / 2;
        int startY = HEIGHT / 2;
        do
        {
            cout << "Введите ваше имя: ";
            cin >> playerName;
        } while (strlen(playerName) == 0);
        do
        {
            cout << endl << "Выберите сложность: " << endl <<
                "легко - 1" << endl << "нормально - 2" << endl << "сложно - 3" << endl << ">>>";
            cin >> choice;
            if (cin.fail())
            {
                cin.clear();
                choice = 20;
            }
            cin.ignore(10000, '\n');
            try
            {
                if (choice == 1) speed = 250;
                else if (choice == 2) speed = 160;
                else if (choice == 3) speed = 40;
                else
                {
                    cout << endl << "Неверный выбор, пожалуйста выберите другое";
                }
            }
            catch (const exception& e)
            {
                cerr << "Ошибка: " << e.what() << endl;
            }
        } while (choice != 1 && choice != 2 && choice != 3);
        currentDifficult = choice;
        addHead(startX - 2, startY);
        addHead(startX - 1, startY);
        addHead(startX, startY);

        generateFood();

        draw();
        cout << endl << "Нажмите чтобы начать" << endl;
        _getch();
    }
    else
    {
        if (!loadGame()) {
            cout << "\nНе удалось загрузить игру!\n";
            Sleep(1500);
            return;
        }

        if (strlen(playerName) == 0) {
            do {
                cout << "Введите ваше имя: ";
                cin >> playerName;
            } while (strlen(playerName) == 0);
        }

        if (currentDifficult == 1) speed = 250;
        else if (currentDifficult == 2) speed = 150;
        else if (currentDifficult == 3) speed = 30;
        system("cls");
        draw();
        cout << "\nИгра загружена! Нажмите любую клавишу чтобы продолжить\n";
        _getch();
    }
    while (!gameOver) {
        draw();
        processInput();
        if (isPaused) {
            saveGame();
            cout << "\n Идет сохранение... Возобновление через 3 секунды...\n";
            Sleep(3000);
            isPaused = false;
            continue;
        }
        update();
        Sleep(speed);
    }

    draw();
    cout << "\n=== ИГРА ОКОНЧЕНА ===\n";
    cout << "Ваш счёт: " << score << "\n";
    saveRecord(playerName, score, currentDifficult);
    cout << "Нажмите любую клавишу для возврата в меню...";
    _getch();
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(0));

    int choice;
    do {
        system("cls");
        cout << "===== ИГРА ЗМЕЙКА =====" << endl <<
            "1. Новая игра" << endl <<
            "2. Загрузить игру" << endl <<
            "3. Таблица рекордов" << endl <<
            "4. Выход" << endl;
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            choice = 20;
        }
        cin.ignore(10000, '\n');
        try
        {
            if (choice == 1)
            {

                startNewGame(true);
            }
            else if (choice == 2)
            {
                startNewGame(false);
            }
            else if (choice == 3)
            {
                countForRec = 1;
                showRecords();
            }
            else if (choice == 4)
            {
                clearSnake();
                cout << "Выход...\n";
            }
            else
            {
                cout << "Неверный выбор!\n";
                Sleep(1300);
            }
        }
        catch (const exception& e)
        {
            cerr << "Ошибка: " << e.what() << endl;
        }
    } while (choice != 4);

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
