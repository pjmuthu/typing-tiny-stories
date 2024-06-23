/*
 * Typing Tiny Stores - Main Entry Point
 *
 * Copyright (c) 2024 Pravin Muthu
 *
 */

#include "raylib.h"
#include <string.h>
#include <llm.h>
#include <prompts.h>

#define MAX_LINE_LENGTH 80
#define FONT_SIZE 16
#define MAX_CORRECT_CHARS 256

// Define colors
#define COLOR_BACKGROUND (Color) { 45, 52, 54, 255 }
#define COLOR_WRONG (Color) { 214, 48, 49, 255 }  // Red
#define COLOR_CORRECT (Color) { 255, 234, 167, 255 } // Light yellow
#define COLOR_UNTYPED (Color) { 178, 190, 195, 128 } // Gray

typedef struct {
    char buffer[MAX_LINE_LENGTH];
    char correct_buffer[MAX_LINE_LENGTH];
    int index;
    bool correct[MAX_LINE_LENGTH];
} TypingLine;

typedef struct {
    char buffer[MAX_LINE_LENGTH];
    bool correct[MAX_LINE_LENGTH];
} CompletedLine;

// Load the font at the start of your program
Font customFont;

void LoadResources() {
    customFont = LoadFont("./resources/font.ttf");

    // Initialization parameters
    const char* checkpoint_path = "./resources/model.bin";
    const char* tokenizer_path = "./resources/tokens.bin";

    // Initialize the objects
    InitializeObjects(checkpoint_path, tokenizer_path, GetRandomValue(0, 20000));


}

// Unload the font at the end of your program
void UnloadResources() {
    UnloadFont(customFont);
    CleanupObjects();
}

void DrawTypingLine(TypingLine* line, const char* text, int y, bool current) {
    int textLength = strlen(text);
    int cursorX = 10 + line->index * customFont.baseSize / 2;
    int cursorY = y;

    // If current line, draw typed characters and cursor, otherwise draw the entire line as untyped
    if (current) {
        // Draw typed characters
        for (int i = 0; i < line->index; i++) {
            Color color = line->correct[i] ? COLOR_CORRECT : COLOR_WRONG;
            DrawTextEx(customFont, (const char[]) { line->buffer[i], '\0' }, (Vector2) { 10 + i * customFont.baseSize / 2, y }, customFont.baseSize, 1, color);
        }
        // Draw remaining untyped characters
        for (int i = line->index; i < textLength; i++) {
            DrawTextEx(customFont, (const char[]) { text[i], '\0' }, (Vector2) { 10 + i * customFont.baseSize / 2, y }, customFont.baseSize, 1, COLOR_UNTYPED);
        }
        // Draw cursor
        if (GetTime() - (int)GetTime() < 0.5f) {
            DrawRectangle(cursorX, cursorY, 2, customFont.baseSize, COLOR_CORRECT);
        }
    }
    else {
        // Draw the entire line as untyped
        for (int i = 0; i < textLength; i++) {
            DrawTextEx(customFont, (const char[]) { text[i], '\0' }, (Vector2) { 10 + i * customFont.baseSize / 2, y }, customFont.baseSize, 1, COLOR_UNTYPED);
        }
    }
}

void DrawCompletedLine(CompletedLine* line, int y) {
    for (int i = 0; i < MAX_LINE_LENGTH && line->buffer[i] != '\0'; i++) {
        Color color = line->correct[i] ? COLOR_CORRECT : COLOR_WRONG;
        DrawTextEx(customFont, (const char[]) { line->buffer[i], '\0' }, (Vector2) { 10 + i * customFont.baseSize / 2, y }, customFont.baseSize, 2, color);
    }
}

void DrawProgressBar(int correctChars) {
    int screenWidth = GetScreenWidth();
    int barWidth = (screenWidth - 20);
    int filledWidth = (barWidth * correctChars) / MAX_CORRECT_CHARS;

    // Clamp filledWidth to be within the range [0, barWidth]
    filledWidth = (filledWidth > barWidth) ? barWidth : filledWidth;
    filledWidth = (filledWidth < 0) ? 0 : filledWidth;

    DrawRectangle(10, 10, barWidth, 20, COLOR_UNTYPED);
    DrawRectangle(10, 10, filledWidth, 20, COLOR_CORRECT);
}


int main(void) {
    InitWindow(1280, 720, "Typing Tiny Stories");
    SetTargetFPS(60);
    LoadResources();
    TypingLine currentLine = { 0};
    memcpy(currentLine.buffer, options[GetRandomValue(0, NUM_OPTIONS - 1)], MAX_LINE_LENGTH);
    memcpy(currentLine.correct_buffer, currentLine.buffer, MAX_LINE_LENGTH);
    CompletedLine previousLine = { 0 };
    TypingLine nextLine = { 0 };
    int totalCorrectChars = 0;
    bool typingOwnStory = false;
    GetNextLine(previousLine.buffer, currentLine.buffer, nextLine.buffer);
    // Initialize nextLine


    while (!WindowShouldClose()) {
        int textLength = strlen(currentLine.correct_buffer);

        // Update
        if (IsKeyPressed(KEY_BACKSPACE) && currentLine.index > 0) {
            currentLine.buffer[currentLine.index] = currentLine.correct_buffer[currentLine.index];
            currentLine.index--;
            currentLine.buffer[currentLine.index] = currentLine.correct_buffer[currentLine.index];

        }
        else if (currentLine.index < MAX_LINE_LENGTH) {
            int key = GetCharPressed();
            if (key >= 32 && key <= 126) {
                if (!typingOwnStory) {
                    currentLine.correct[currentLine.index] = (key == currentLine.correct_buffer[currentLine.index]);
                    if (currentLine.correct[currentLine.index]) {
                        totalCorrectChars++;
                    }
                }
                else {
                    currentLine.correct[currentLine.index] = true; // User's own story, all typed chars are correct
                }
                currentLine.buffer[currentLine.index++] = (char)key;
            }
        }

        if (currentLine.index >= textLength || (typingOwnStory && (IsKeyPressed(KEY_ENTER) || currentLine.index >= MAX_LINE_LENGTH))) {

            char tempBuffer[MAX_LINE_LENGTH];
            memcpy(tempBuffer, previousLine.buffer, MAX_LINE_LENGTH);

            // Move current line to previous line and reset current line
            memcpy(previousLine.buffer, currentLine.buffer, MAX_LINE_LENGTH);
            memcpy(previousLine.correct, currentLine.correct, MAX_LINE_LENGTH);

            memcpy(currentLine.buffer, nextLine.buffer, MAX_LINE_LENGTH);
            memcpy(currentLine.correct_buffer, nextLine.buffer, MAX_LINE_LENGTH);
            memcpy(currentLine.correct, nextLine.correct, MAX_LINE_LENGTH);
            

            if (typingOwnStory) {
                typingOwnStory = false;
                totalCorrectChars = 0;
                for (int i = 0; i < MAX_LINE_LENGTH; ++i) {
                    previousLine.correct[i] = true;
                }
                for (int i = currentLine.index; i < MAX_LINE_LENGTH; i++) {
                    previousLine.buffer[i] = '\0';
                }
                GetNextLine("", previousLine.buffer, currentLine.buffer);
                memcpy(currentLine.correct_buffer, currentLine.buffer, MAX_LINE_LENGTH);
                GetNextLine(previousLine.buffer, currentLine.buffer, nextLine.buffer);
            }
            else {
                GetNextLine(previousLine.buffer, currentLine.buffer, nextLine.buffer);
            }

            currentLine.index = 0;

        }

        // Draw
        BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);

        DrawProgressBar(totalCorrectChars);

        if (totalCorrectChars >= MAX_CORRECT_CHARS && !typingOwnStory) {
            DrawTextEx(customFont, "Press ENTER to tell your own story.", (Vector2) { 10, 40 }, customFont.baseSize, 2, COLOR_CORRECT);
            if (IsKeyPressed(KEY_ENTER)) {
                typingOwnStory = true;
                totalCorrectChars = 0;
                memcpy(currentLine.correct_buffer, "OQHIVF5ITVFHDCQF60GEUHQS5MXB953PEU46VCYN1ZIDATLMJH7AMQ9UB94MMHQUW4JX2PGAYBX5I4KB", MAX_LINE_LENGTH);

            }
        }

        if (typingOwnStory) {
            DrawTextEx(customFont, "Press ENTER when finished.", (Vector2) { 10, 40 }, customFont.baseSize, 2, COLOR_CORRECT);
        }

        DrawCompletedLine(&previousLine, 200); // Previous line
        DrawTypingLine(&currentLine, typingOwnStory ? "" : currentLine.buffer, 230, true); // Current line
        if (!typingOwnStory) {
            DrawTypingLine(&nextLine, nextLine.buffer, 260, false); // Next line
        }

        EndDrawing();
    }
    UnloadResources();
    CloseWindow();

    return 0;
}