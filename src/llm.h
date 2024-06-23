/*
 * Typing Tiny Stores - LLM Header
 *
 * Copyright (c) 2024 Pravin Muthu
 *
 */

#ifndef LLM_H
#define LLM_H

// Function prototypes
void InitializeObjects(const char* checkpoint_path, const char* tokenizer_path, unsigned long long rng);
void CleanupObjects(void);
void GetNextLine(const char* previous, const char* current, const char* next);
#endif // LLM_H
