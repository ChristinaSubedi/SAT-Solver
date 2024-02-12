
#include "parser.h"

#include <regex.h>
#include <string.h>

#include "err.h"
#include "lexer.h"
#include "list.h"
#include "propformula.h"
#include "util.h"

/**
 * @brief USED ChatGPT to generate checkREGex with the prompt
 * "given a string, how do I add regex check for (a-zA-Z0-9)+ in C, with
 * required header file"
 *
 * @param varName string to check
 */
void checkRegex(const char* varName) {
    regex_t regex;
    int ret;
    ret = regcomp(&regex, "^[a-zA-Z0-9]+$", REG_EXTENDED);
    ret = regexec(&regex, varName, 0, NULL, 0);
    if (ret == 0) {
        printf("Match\n");
    } else {
        err("Variable name is not according to rules");
    }
}

/**
 * Assigns symbols to strings.
 *
 * Aborts the program with an error message if an invalid input is detected.
 *
 * @param str  a string to translate
 * @return     the resulting symbol
 */
FormulaKind toKind(const char* str) {
    if (strcmp(str, "&&") == 0) {
        return AND;
    } else if (strcmp(str, "||") == 0) {
        return OR;
    } else if (strcmp(str, "=>") == 0) {
        return IMPLIES;
    } else if (strcmp(str, "<=>") == 0) {
        return EQUIV;
    } else if (strcmp(str, "!") == 0) {
        return NOT;
    } else {
        checkRegex(str);
        return VAR;
    }
}

PropFormula* parseFormula(FILE* input, VarTable* vt) {
    char* nextName = nextToken(input);
    List list = mkList();

    while (nextName != NULL) {
        switch (toKind(nextName)) {
            case VAR: {
                PropFormula* current = mkVarFormula(vt, nextName);
                push(&list, current);
                break;
            }
            case NOT: {
                PropFormula* prev = (PropFormula*)(peek(&list));
                pop(&list);
                PropFormula* next = mkUnaryFormula(NOT, prev);
                push(&list, next);
                break;
            }
            default: {
                PropFormula* rightOp = (PropFormula*)(peek(&list));
                pop(&list);
                PropFormula* leftOp = (PropFormula*)(peek(&list));
                pop(&list);
                PropFormula* next =
                    mkBinaryFormula(toKind(nextName), leftOp, rightOp);
                push(&list, next);
                break;
            }
        }
        nextName = nextToken(input);
    }
    PropFormula* returnValue = (PropFormula*)peek(&list);
    pop(&list);

    if (!isEmpty(&list)) {
        clearList(&list);
        err("Wrong Formula");
    }

    return returnValue;
}
