// ICEY Baka helpdoc system
// Intelligent helpdoc system invented by ICEY
//   note: This program *MUST* compile with clang -Weverything.
//   if not, contact me.

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/// Predeclarations

typedef enum {
  MDT_Builtin,
  MDT_CompileLoad,
  MDT_Precompiled,
  MDT_FFI_JIT,
  MDT_DynamicLoad
} mod_type_t;

typedef struct modetype_int {
  const char* name;
  int16_t load_time;
  int16_t type;
  float ver;
} mod_t;

typedef struct maybe_answer_int {
  // this must be a NULL terminated array
  const char *kwds[8];
  const char *answer;
  float threshold;
  int : 32;
} mbans_t;

void load_module(mod_t mod);
void load_modules(void);
bool contains(const char *haystack, const char *needle);
void lower_str(char*);
bool is_baka_or_equal(const char *stmt);
bool is_taboo(const char *stmt);
void reject_taboo_input(void);
void reject_baka_input(void);
bool try_answer(const char *ques, const mbans_t *ans);
bool answer_ques(const char *stmt);
void baka_main_loop(void);

/// Module system

void load_module(mod_t mod) {
  switch (mod.type) {
  case MDT_Builtin:       
    printf("loading builtin module: ");
    break;
  case MDT_CompileLoad:   
    printf("compiling module: ");             
    break;
  case MDT_Precompiled:   
    printf("loading precompiled module: ");   
    break;
  case MDT_FFI_JIT:       
    printf("loading ffi module: ");           
    break;
  case MDT_DynamicLoad:   
    printf("dyn loading module: ");           
    break;
  default: 
    fprintf(stderr, "error loading module: %s\n", mod.name); 
    exit(-1);
  }
  printf("%s v%.2lf\n", mod.name, (double)mod.ver);
  sleep((unsigned)mod.load_time);
}

void load_modules() {
  mod_t mods[] = {
    {"core",                      1,  MDT_Builtin,      1.2f},
    {"core-baka",                 1,  MDT_Builtin,      1.1f},
    {"libcore",                   1,  MDT_CompileLoad,  1.1f},
    {"libcore-baka",              2,  MDT_CompileLoad,  1.1f},
    {"libbaka-data",              1,  MDT_Precompiled,  2.1f},
    {"libbaka-data-multiverse",   2,  MDT_Precompiled,  2.05f},
    {"libbaka-data-universe",     2,  MDT_Precompiled,  2.08f},
    {"libffi-baka",               1,  MDT_FFI_JIT,      0.1f},
    {"libffi-dynload",            2,  MDT_FFI_JIT,      0.1f},
    {"crash-report",              1,  MDT_DynamicLoad,  0.23f},
    {"profiler",                  1,  MDT_DynamicLoad,  0.01f}
  };
  
  printf("baka is loading modules . . .\n");
  for (size_t i = 0; i < (sizeof(mods)/sizeof(mods[0])); i++) {
    load_module(mods[i]);
  }
  printf("done loading all modules\n\n");
  sleep(1);
}

/// Module system
/// Answering driver system

bool contains(const char *haystack, const char *needle) {
  return strstr(haystack, needle) != NULL;
}

void lower_str(char *str) {
  for (; *str != '\0'; ++str) *str = (char)tolower(*str);
}

bool is_baka_or_equal(const char *stmt) {
  static const char *known_baka[] = { 
    "baka", "fool", "stupid", "silly", "sb" 
    };
  static const int count = sizeof(known_baka)/sizeof(known_baka[0]);
  for (int i = 0; i < count; i++) {
    if (contains(stmt, known_baka[i])) {
      return true;
    }
  }
  return false;
}

bool is_taboo(const char *stmt) {
  static const char *known_taboo[] = {
    // according to slate.com summary
    "shit", "fuck", "bitch", "damn", "crap", "piss", "dick",
    "cock", "pussy", "asshole", "fag", "bastard", "slut", "douche",
    // variants
    "f*ck", "f**k", "f***", "sh*t", "s**t"
  };
  static const int count = sizeof(known_taboo)/sizeof(known_taboo[0]);
  for (int i = 0; i < count; i++) {
    if (contains(stmt, known_taboo[i])) {
      return true;
    }
  }
  return false;
}

void reject_baka_input() {
  static const char *stmts[] = {
    "int you = baka; // you are baka!",
    "I am not baka! Get away stranger!",
    "You are baka! bakabaka, bakabakabaka!",
    "I am not baka, I am akemimadoka!",
    "...",
  };
  static const int count = sizeof(stmts)/sizeof(stmts[0]);
  printf("%s\n", stmts[rand() % count]);
}

void reject_taboo_input() {
  printf("sorry, swear words are not allowed.\n");
}

bool try_answer(const char *ques, const mbans_t *ans) {
  int hit = 0;
  int i = 0;
  for (; ans->kwds[i] != NULL; i++) {
    if (contains(ques, ans->kwds[i])) {
      ++hit;
    }
  }
  
  float hit_rate = (float)hit / (float)(i-1);
  if (hit_rate >= ans->threshold) {
    printf("%s\n", ans->answer);
    return true;
  }
  return false;
}

bool answer_ques(const char *ques) {
  static const mbans_t answers[] = {
    // quit
    {{"what", "how", "quit", "quit", NULL},
     "press ctrl+d to quit", 0.74f},
     
    // exit
    {{"what", "how", "exit", "exit", NULL}, 
     "press ctrl+d to exit", 0.74f},
     
    // close
    {{"what", "how", "close", "close", "this", NULL}, 
     "press ctrl+d to exit", 0.59f},
     
    // about
    {{"this", "this", "program", "system", "how", "what", NULL}, 
     "simply type your question and you will get answer", 0.66f}
  };
  static const int count = sizeof(answers) / sizeof(answers[0]);
  for (int i = 0; i < count; i++) {
    bool hit = try_answer(ques, answers + i);
    if (hit) return true;
  }
  return false;
}

void baka_main_loop() {
  static char buffer[512];
  srand((unsigned int)time(NULL));
  printf("");
  while (1) {
    printf("baka$ ");
    fflush(stdin);
    fgets(buffer, 511, stdin);
    lower_str(buffer);
    if (buffer[0] == '\0') {
      break;
    }
    else if (buffer[0] == '\n') {
      /// pass
    }
    else if (is_taboo(buffer)) {
      reject_taboo_input();
    }
    else if (is_baka_or_equal(buffer)) {
      reject_baka_input();
    }
    else if (!answer_ques(buffer)) {
      printf("akemimadoka don't know!\n");
    }
    memset(buffer, 0, sizeof(buffer));
  }
}

int main() {
  printf("ICEY Baka helpdoc system\n"
         "Copyright (C) 2018 ICEYSOFT, Inc.\n"
         "license WTFPLv2+: WTFPL version 2 or later\n"
         "this is free software: "
         "you are free to change and redistribute it.\n"
         "there is NO WARRANTY, to the extent permitted by law.\n\n");
  load_modules();
  baka_main_loop();
  return 0;
}
