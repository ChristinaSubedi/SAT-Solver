#include "dpll.h"

#include "err.h"
#include "list.h"
#include "util.h"
#include "variables.h"

typedef enum Reason { CHOSEN, IMPLIED } Reason;

/**
 * Struct to represent an entry in the assignment stack. Should only be created
 * and freed by pushAssignment and popAssignment.
 */
typedef struct Assignment {
    VarIndex var;
    Reason reason;
} Assignment;

/**
 * Adds a new assignment to the assignment stack.
 *
 * @param stack  an assignment stack
 * @param var    the variable to assign
 * @param r      the reason for the assignment
 */
void pushAssignment(List* stack, VarIndex var, Reason r) {
    Assignment* a = (Assignment*)malloc(sizeof(Assignment));
    a->var = var;
    a->reason = r;
    push(stack, a);
}

/**
 * Removes the head element of an assignment stack and frees it.
 *
 * @param stack  an assignment stack
 */
void popAssignment(List* stack) {
    Assignment* a = (Assignment*)peek(stack);
    free(a);
    pop(stack);
}

/**
 * @brief done with the help of ChatGPT +my editing
 * input was the task description and the related functions
 *
 * @param vt
 * @param stack
 * @param cnf
 * @return int
 */

int unitPropagation(VarTable* vt, List* stack, CNF* cnf) {
    int notFound = 1;
    ListIterator it = mkIterator(&cnf->clauses);
    while (isValid(&it)) {
        Clause* clause = (Clause*)getCurr(&it);
        if (clause->val == UNDEFINED) {
            Literal unitLiteral = getUnitLiteral(vt, clause);
            if (unitLiteral != 0) {
                VarIndex var = abs(unitLiteral);
                TruthValue value = (unitLiteral > 0) ? TRUE : FALSE;
                updateVariableValue(vt, var, value);
                pushAssignment(stack, var, IMPLIED);
                notFound = 0;
            }
        }
        next(&it);
    }
    return notFound;
}

/**
 * Führt eine Iteration des DPLL Algorithmus aus.
 *
 * @param vt       die zugrunde liegende Variablentabelle
 * @param stack    der Zuweisungsstack
 * @param cnf      die zu prüfende Formel
 * @return         1 falls der Algorithmus mit SAT terminieren sollte,
 *                 0 falls der Algorithmus weiterlaufen sollte,
 *                 -1 falls der Algorithmus mit UNSAT terminieren sollte
 */
/**
 * Performs one iteration of the DPLL algorithm.
 *
 * @param vt       the underlying variable table
 * @param stack    an assignment stack
 * @param cnf      the formula to check
 * @return         1 if the algorithm should terminate with SAT,
 *                 0 if the algorithm should continue,
 *                -1 if the algorithm should terminate with UNSAT
 */
int iterate(VarTable* vt, List* stack, CNF* cnf) {
    if (evalCNF(cnf) == TRUE) {
        // Formula evaluates to true, the algorithm is aborted (SAT)
        return 1;
    }

    if (evalCNF(cnf) == FALSE) {
        if (!isEmpty(stack)) {
            Assignment* lastAssignment = (Assignment*)peek(stack);
            if (lastAssignment->reason == IMPLIED) {
                while (!isEmpty(stack) && lastAssignment->reason == IMPLIED) {
                    VarIndex var = lastAssignment->var;
                    updateVariableValue(vt, var, UNDEFINED);
                    popAssignment(stack);
                    if (!isEmpty(stack)) {
                        lastAssignment = (Assignment*)peek(stack);
                    }
                }
            }

            if (!isEmpty(stack)) {
                VarIndex var = lastAssignment->var;
                popAssignment(stack);
                TruthValue newValue =
                    getVariableValue(vt, var) == TRUE ? FALSE : TRUE;
                updateVariableValue(vt, var, newValue);
                pushAssignment(stack, var, IMPLIED);

                return 0;  // Continue with the next iteration
            }
        }

        return -1;  // No reset possible, the formula is unsatisfiable (UNSAT)
    }

    // Check if there is a unit clause
    int unitChange = unitPropagation(vt, stack, cnf);
    if (unitChange == 0) {
        return 0;
    }

    // Select the next free variable and assign it
    Literal nextUndefined = getNextUndefinedVariable(vt);
    if (nextUndefined == 0) {
        return -1;
    }

    VarIndex var = abs(nextUndefined);
    updateVariableValue(vt, var, TRUE);
    pushAssignment(stack, var, CHOSEN);

    // All variables are assigned, but formula is not satisfied (UNSAT)
    return 0;
}

char isSatisfiable(VarTable* vt, CNF* cnf) {
    List stack = mkList();
    int res;
    do {
        res = iterate(vt, &stack, cnf);
    } while (res == 0);

    while (!isEmpty(&stack)) {
        popAssignment(&stack);
    }

    return (res < 0) ? 0 : 1;
}
