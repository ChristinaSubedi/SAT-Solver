#include "tseitin.h"

#include <stdio.h>

#include "err.h"
#include "util.h"

/**
 * Inserts a clause with one literal into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    a literal
 */
void addUnaryClause(VarTable* vt, CNF* cnf, Literal a) {
    Clause* clause = mkTernaryClause(vt, a, 0, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with two literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 */
void addBinaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b) {
    Clause* clause = mkTernaryClause(vt, a, b, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with three literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 * @param c    the third literal
 */
void addTernaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b, Literal c) {
    Clause* clause = mkTernaryClause(vt, a, b, c);
    addClauseToCNF(cnf, clause);
}

/**
 * Adds clauses for a propositional formula to a CNF.
 *
 * For a propositional formula pf, clauses that are added that are equivalent to
 *
 *     x <=> pf
 *
 * where x is usually a fresh variable. This variable is also returned.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param pf   a propositional formula
 * @return     the variable x, as described above
 */

/**
 * @brief generated with the help of ChatGPT
 * gave it the incomplete function of addClause with only switch case
 * of AND, VAR and NOT done and told it to complete the rest
 * also entered the project description and other functions for refernce
 */
VarIndex addClauses(VarTable* vt, CNF* cnf, const PropFormula* pf) {
    switch (pf->kind) {
        case NOT: {
            PropFormula* operand = pf->data.single_op;
            VarIndex operandVar = addClauses(vt, cnf, operand);

            VarIndex newIndex = mkFreshVariable(vt);

            addBinaryClause(vt, cnf, -newIndex, -operandVar);
            addBinaryClause(vt, cnf, newIndex, operandVar);

            return newIndex;
        }
        case VAR: {
            return pf->data.var;
        }
        case AND: {
            //(¬𝑥𝑖 ∨ 𝑎)∧(¬𝑥𝑖 ∨𝑏)∧(¬𝑎∨¬𝑏∨𝑥𝑖)

            PropFormula* left = pf->data.operands[0];
            VarIndex leftVar = addClauses(vt, cnf, left);

            PropFormula* right = pf->data.operands[1];
            VarIndex rightVar = addClauses(vt, cnf, right);

            VarIndex newIndex = mkFreshVariable(vt);

            addBinaryClause(vt, cnf, -newIndex, leftVar);
            addBinaryClause(vt, cnf, -newIndex, rightVar);
            addTernaryClause(vt, cnf, -leftVar, -rightVar, newIndex);

            return newIndex;
        }

        case OR: {
            PropFormula* left = pf->data.operands[0];
            VarIndex leftVar = addClauses(vt, cnf, left);

            PropFormula* right = pf->data.operands[1];
            VarIndex rightVar = addClauses(vt, cnf, right);

            VarIndex newIndex = mkFreshVariable(vt);

            addBinaryClause(vt, cnf, newIndex, -leftVar);
            addBinaryClause(vt, cnf, newIndex, -rightVar);
            addTernaryClause(vt, cnf, leftVar, rightVar, -newIndex);

            return newIndex;
        }

        case IMPLIES: {
            PropFormula* left = pf->data.operands[0];
            VarIndex leftVar = addClauses(vt, cnf, left);

            PropFormula* right = pf->data.operands[1];
            VarIndex rightVar = addClauses(vt, cnf, right);

            VarIndex newIndex = mkFreshVariable(vt);

            addBinaryClause(vt, cnf, newIndex, -rightVar);
            addBinaryClause(vt, cnf, newIndex, leftVar);
            addTernaryClause(vt, cnf, rightVar, -newIndex, -leftVar);

            return newIndex;
        }

        case EQUIV: {
            PropFormula* left = pf->data.operands[0];
            VarIndex leftVar = addClauses(vt, cnf, left);

            PropFormula* right = pf->data.operands[1];
            VarIndex rightVar = addClauses(vt, cnf, right);

            VarIndex newIndex = mkFreshVariable(vt);

            addTernaryClause(vt, cnf, leftVar, -rightVar, -newIndex);
            addTernaryClause(vt, cnf, -leftVar, rightVar, -newIndex);
            addTernaryClause(vt, cnf, -leftVar, -rightVar, newIndex);
            addTernaryClause(vt, cnf, leftVar, rightVar, newIndex);

            return newIndex;
        }
        // Handle other cases if needed
        default:
            break;
    }

    // Return a default value in case of error
    return -1;
}

CNF* getCNF(VarTable* vt, const PropFormula* f) {
    CNF* res = mkCNF();

    VarIndex x = addClauses(vt, res, f);

    addUnaryClause(vt, res, x);

    return res;
}
