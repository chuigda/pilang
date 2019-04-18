#ifndef CONFIG_H
#define CONFIG_H

/* Basis */
/** Please note that PiLang is distributed under WTFPL v2, you may use 
 * it for any usage. As a result, a convenient renaming machanism is 
 * granted.
*/
#define BAS_LANGUAGE_NAME "PiLang"
#define BAS_AUTHOR        "ICEY(iceyself@iceyey.tech)"
#define BAS_DISCRIPTION   "A programming language for ordinary people"
#define BAS_LICENSE       "WTFPL v2+"

/* Version */
#define VER_NAME "Halley"

#define VER_PLCFRONT_MAJOR 0
#define VER_PLCFRONT_MINOR 3
#define VER_PLCFRONT_REVISE 0

#define VER_PLI_MAJOR 0
#define VER_PLI_MINOR 5
#define VER_PLI_REVISE 1

/* PiLang Intepreter configurations */
#define PLI_HEAP_INIT_SIZE     114514
#define PLI_STACK_SIZE         65536
#define PLI_STACKFRAME_COUNT   512

#define PLI_FFI_FUNC_SLOT_SIZE 1024

#endif
