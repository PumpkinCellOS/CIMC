#pragma once

#include <types.h>

u16 atoi(const char* str);
bool isspace(const char c);
bool isalpha(const char c);
bool isalnum(const char c);
char* itoa_s(char* buf, u16 bufsz, u16 val, u16 base);
void strcmp(const char* s1, const char* s2);
void strncpy(char* dst, u16 dstsize, const char* src);
const char* strerror(errno_t err);
u16 strlen(const char* str);
