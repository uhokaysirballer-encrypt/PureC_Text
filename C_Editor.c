/*
 * SIMPLE CONSOLE TEXT EDITOR
 * Pure C - Save, load, edit text files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024

char lines[MAX_LINES][MAX_LINE_LENGTH];
int lineCount = 0;
char filename[256] = "";
int modified = 0;

/* ============================================
   DISPLAY TEXT BUFFER
   ============================================ */
void DisplayBuffer(void) {
    int i;
    system("cls");
    printf("========================================\n");
    printf("  SIMPLE TEXT EDITOR - %s%s\n", 
           filename[0] ? filename : "[Untitled]",
           modified ? " *" : "");
    printf("========================================\n");
    printf("  Ctrl+S: Save  |  Ctrl+O: Open  |  ESC: Exit\n");
    printf("========================================\n\n");
    
    for (i = 0; i < lineCount; i++) {
        printf("%4d | %s\n", i + 1, lines[i]);
    }
    
    printf("\n----------------------------------------\n");
}

/* ============================================
   SAVE FILE
   ============================================ */
void SaveFile(void) {
    FILE* fp;
    int i;
    
    if (!filename[0]) {
        printf("Enter filename: ");
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = 0;
    }
    
    fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Cannot save file!\n");
        return;
    }
    
    for (i = 0; i < lineCount; i++) {
        fprintf(fp, "%s\n", lines[i]);
    }
    
    fclose(fp);
    modified = 0;
    printf("Saved %d lines to %s\n", lineCount, filename);
}

/* ============================================
   OPEN FILE (renamed to avoid conflict)
   ============================================ */
void LoadAFile(void) {
    FILE* fp;
    char temp[256];
    
    printf("Enter filename to open: ");
    fgets(temp, sizeof(temp), stdin);
    temp[strcspn(temp, "\n")] = 0;
    
    fp = fopen(temp, "r");
    if (!fp) {
        printf("File not found. adding magic to new file.\n");
        strcpy(filename, temp);
        lineCount = 0;
        modified = 1;
        return;
    }
    
    strcpy(filename, temp);
    lineCount = 0;
    
    while (lineCount < MAX_LINES && fgets(lines[lineCount], MAX_LINE_LENGTH, fp)) {
        lines[lineCount][strcspn(lines[lineCount], "\n")] = 0;
        lineCount++;
    }
    
    fclose(fp);
    modified = 0;
    printf("Loaded %d lines from %s\n", lineCount, filename);
}

/* ============================================
   EDIT A LINE
   ============================================ */
void EditLine(void) {
    int lineNum;
    char input[MAX_LINE_LENGTH];
    
    printf("Edit line number (1-%d, 0 to add new): ", lineCount > 0 ? lineCount : 1);
    scanf("%d", &lineNum);
    getchar();
    
    if (lineNum == 0 || lineNum > lineCount) {
        if (lineCount >= MAX_LINES) {
            printf("Maximum lines reached!\n");
            return;
        }
        lineNum = lineCount + 1;
    }
    
    printf("Line %d: ", lineNum);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    if (lineNum > lineCount) {
        lineCount++;
    }
    
    strcpy(lines[lineNum - 1], input);
    modified = 1;
}

/* ============================================
   DELETE A LINE
   ============================================ */
void DeleteLine(void) {
    int lineNum, i;
    
    if (lineCount == 0) {
        printf("Buffer is empty!\n");
        return;
    }
    
    printf("Delete line number (1-%d): ", lineCount);
    scanf("%d", &lineNum);
    getchar();
    
    if (lineNum < 1 || lineNum > lineCount) {
        printf("Invalid line number!\n");
        return;
    }
    
    for (i = lineNum - 1; i < lineCount - 1; i++) {
        strcpy(lines[i], lines[i + 1]);
    }
    
    lineCount--;
    modified = 1;
    printf("Line %d deleted.\n", lineNum);
}

/* ============================================
   INSERT A LINE
   ============================================ */
void InsertLine(void) {
    int lineNum, i;
    char input[MAX_LINE_LENGTH];
    
    if (lineCount >= MAX_LINES) {
        printf("Maximum lines reached!\n");
        return;
    }
    
    printf("Insert at line number (1-%d): ", lineCount + 1);
    scanf("%d", &lineNum);
    getchar();
    
    if (lineNum < 1 || lineNum > lineCount + 1) {
        printf("Invalid line number!\n");
        return;
    }
    
    for (i = lineCount; i >= lineNum; i--) {
        strcpy(lines[i], lines[i - 1]);
    }
    
    printf("Line %d: ", lineNum);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    strcpy(lines[lineNum - 1], input);
    lineCount++;
    modified = 1;
    printf("Line inserted.\n");
}

/* ============================================
   MAIN MENU
   ============================================ */
int main(void) {
    int running = 1;
    char choice[10];
    
    SetConsoleTitle("Simple Text Editor");
    
    printf("SIMPLE TEXT EDITOR\n");
    printf("==================\n\n");
    printf("Open existing file? (y/n): ");
    
    fgets(choice, sizeof(choice), stdin);
    choice[strcspn(choice, "\n")] = 0;
    
    if (choice[0] == 'y' || choice[0] == 'Y') {
        LoadAFile();
    }
    
    while (running) {
        DisplayBuffer();
        
        printf("[E]dit  [I]nsert  [D]elete  [S]ave  [O]pen  [N]ew  [Q]uit\n");
        printf("Choice: ");
        
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;
        
        switch (choice[0]) {
            case 'e':
            case 'E':
                EditLine();
                break;
            case 'i':
            case 'I':
                InsertLine();
                break;
            case 'd':
            case 'D':
                DeleteLine();
                break;
            case 's':
            case 'S':
                SaveFile();
                break;
            case 'o':
            case 'O':
                if (modified) {
                    printf("Unsaved changes! Save first? (y/n): ");
                    fgets(choice, sizeof(choice), stdin);
                    if (choice[0] == 'y' || choice[0] == 'Y') SaveFile();
                }
                LoadAFile();
                break;
            case 'n':
            case 'N':
                if (modified) {
                    printf("Unsaved changes! Save first? (y/n): ");
                    fgets(choice, sizeof(choice), stdin);
                    if (choice[0] == 'y' || choice[0] == 'Y') SaveFile();
                }
                filename[0] = 0;
                lineCount = 0;
                modified = 0;
                printf("New file born.\n");
                break;
            case 'q':
            case 'Q':
                if (modified) {
                    printf("Unsaved changes! Save first? (y/n): ");
                    fgets(choice, sizeof(choice), stdin);
                    if (choice[0] == 'y' || choice[0] == 'Y') SaveFile();
                }
                running = 0;
                break;
        }
        
        if (running) {
            printf("\nPress any key to continue...");
            getch();
        }
    }
    
    printf("\nGoodbye!\n");
    return 0;
}
