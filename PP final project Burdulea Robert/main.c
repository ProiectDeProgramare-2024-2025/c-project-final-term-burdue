#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LEN 50
#define MAX_QUESTIONS 100
#define MAX_TEXT_LEN 200
#define HISTORY_FILE "history.txt"
#define QUESTIONS_FILE "questions.txt"
#define MAX_HISTORY 100

// Color codes
#define GREEN "\x1B[32m"
#define RED "\x1B[31m"
#define BLUE "\x1B[34m"
#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"  // Reset to default color

typedef struct {
    char question[MAX_TEXT_LEN];
    char options[4][MAX_TEXT_LEN];
    char correct;
} Question;

typedef struct {
    char name[MAX_NAME_LEN];
    float score;
} HistoryEntry;

Question questions[MAX_QUESTIONS];
HistoryEntry history[MAX_HISTORY];
int totalQuestions = 0;
int totalHistory = 0;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseAndClear() {
    printf("\nPress ENTER to continue...");
    getchar();
    clearScreen();
}

void saveHistory(const char* name, float score) {
    FILE* file = fopen(HISTORY_FILE, "a");
    if (file == NULL) {
        printf("Error opening history file!\n");
        return;
    }
    fprintf(file, "%s %.1f\n", name, score);
    fclose(file);
}

void loadHistory() {
    totalHistory = 0;
    FILE* file = fopen(HISTORY_FILE, "r");
    if (file == NULL) return;

    while (fscanf(file, "%s %f", history[totalHistory].name, &history[totalHistory].score) == 2) {
        totalHistory++;
        if (totalHistory >= MAX_HISTORY) break;
    }
    fclose(file);
}

void viewHistory() {
    clearScreen();
    printf("=== Game History ===\n");
    loadHistory();
    if (totalHistory == 0) {
        printf("No history found.\n");
    } else {
        for (int i = 0; i < totalHistory; i++) {
            printf("Player: %s, Score: %.1f\n", history[i].name, history[i].score);
        }
    }
    pauseAndClear();
}

int compareScores(const void* a, const void* b) {
    HistoryEntry* h1 = (HistoryEntry*)a;
    HistoryEntry* h2 = (HistoryEntry*)b;
    if (h2->score > h1->score) return 1;
    else if (h2->score < h1->score) return -1;
    else return 0;
}

void viewLeaderboard() {
    clearScreen();
    printf("=== Leaderboard ===\n");
    loadHistory();

    if (totalHistory == 0) {
        printf("No players yet.\n");
    } else {
        qsort(history, totalHistory, sizeof(HistoryEntry), compareScores);
        int top = totalHistory < 5 ? totalHistory : 5;
        for (int i = 0; i < top; i++) {
            printf("%d. " BLUE "%s" RESET " - %.1f points\n", i + 1, history[i].name, history[i].score);
        }
    }
    pauseAndClear();
}

void help() {
    clearScreen();
    printf("=== Help ===\n");
    printf("1. Play Game: Answer quiz questions from file. One wrong answer = game over.\n");
    printf("2. Game History: See all past player scores.\n");
    printf("3. Leaderboard: View top 5 players with highest scores.\n");
    printf("4. Help: Shows this help menu.\n");
    printf("5. Exit: Quit the application.\n");
    pauseAndClear();
}

int loadQuestions() {
    FILE* file = fopen(QUESTIONS_FILE, "r");
    if (file == NULL) {
        printf("Error opening questions file!\n");
        return 0;
    }
    while (fgets(questions[totalQuestions].question, MAX_TEXT_LEN, file)) {
        questions[totalQuestions].question[strcspn(questions[totalQuestions].question, "\n")] = 0;
        for (int i = 0; i < 4; i++) {
            fgets(questions[totalQuestions].options[i], MAX_TEXT_LEN, file);
            questions[totalQuestions].options[i][strcspn(questions[totalQuestions].options[i], "\n")] = 0;
        }
        char correctLine[MAX_TEXT_LEN];
        fgets(correctLine, MAX_TEXT_LEN, file);
        questions[totalQuestions].correct = toupper(correctLine[0]);
        totalQuestions++;
        if (totalQuestions >= MAX_QUESTIONS) break;
    }
    fclose(file);
    return totalQuestions;
}

void playGame() {
    clearScreen();
    printf("=== Play Game ===\n");

    char name[MAX_NAME_LEN];
    int valid = 0;

    // Input and validate name
    while (!valid) {
        printf("Enter your name (letters only): ");
        scanf("%s", name);
        getchar();

        valid = 1;
        for (int i = 0; name[i] != '\0'; i++) {
            if (!isalpha(name[i])) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf(RED "Invalid input. Please use letters only.\n" RESET);
        }
    }

    float score = 0;
    for (int i = 0; i < totalQuestions; i++) {
        printf("\nQ%d: " YELLOW "%s" RESET "\n", i + 1, questions[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%d. " BLUE "%s" RESET "\n", j + 1, questions[i].options[j]);
        }
        printf("Your answer (A/B/C/D, or H for 50/50 help): ");
        char answer;
        scanf(" %c", &answer);
        getchar();

        if (answer == 'H' || answer == 'h') {
            printf("50/50 Help: The correct answer is %c.\n", questions[i].correct);
            printf("You now get 0.5 points if correct.\n");
            printf("Your answer (A/B/C/D): ");
            scanf(" %c", &answer);
            getchar();
            if (toupper(answer) == questions[i].correct) {
                printf(GREEN "CORRECT!" RESET "\n");
                printf("+0.5 points.\n");
                score += 0.5f;
            } else {
                printf(RED "Wrong! The correct answer was %c.\n" RESET, questions[i].correct);
                printf("\nGame over! You scored %.1f points.\n", score);
                saveHistory(name, score);
                pauseAndClear();
                return;
            }
        } else {
            if (toupper(answer) == questions[i].correct) {
                printf(GREEN "CORRECT!" RESET "\n");
                printf("+1 point.\n");
                score += 1.0f;
            } else {
                printf(RED "Wrong! The correct answer was %c.\n" RESET, questions[i].correct);
                printf("\nGame over! You scored %.1f points.\n", score);
                saveHistory(name, score);
                pauseAndClear();
                return;
            }
        }
    }

    printf("\nCongratulations! You finished all questions. Total score: %.1f points.\n", score);
    saveHistory(name, score);
    pauseAndClear();
}


void showMainMenu() {
    int choice;
    do {
        printf("=== Main Menu ===\n");
        printf("1. " BLUE "Play Game" RESET "\n");
        printf("2. " BLUE "Game History" RESET "\n");
        printf("3. " BLUE "Leaderboard" RESET "\n");
        printf("4. " BLUE "Help" RESET "\n");
        printf("5. " BLUE "Exit" RESET "\n");
        printf("Choose an option: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Redirecting to Help...\n");
            help();
            continue;
        }
        getchar();

        switch (choice) {
            case 1:
                playGame();
                break;
            case 2:
                viewHistory();
                break;
            case 3:
                viewLeaderboard();
                break;
            case 4:
                help();
                break;
            case 5:
                printf("Exiting... Goodbye!\n");
                break;
            default:
                printf("Invalid option. Redirecting to Help...\n");
                help();
        }
    } while (choice != 5);
}

int main() {
    clearScreen();
    if (loadQuestions() == 0) {
        printf("No questions loaded. Please check '%s' file.\n", QUESTIONS_FILE);
        return 1;
    }
    showMainMenu();
    return 0;
}
