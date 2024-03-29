The boolean satisfiability (SAT) problem is one of the oldest and still most important problems in computer science. It is probably the most prominent representative of the P versus NP millennium problem. And its application is widely used in many fields of computer science, e.g. theorem proving or software verification. The problem statement is to check for a given propsitional formula if it satisfiable or not. A propositional formula is a collection of variables combined by logical operators. These variables can be assigned to true or false. If there is at least one assignment so that the resulting term is equivalent to the logical truth, we say that the formula is satisfiable. The SAT problem is the task of determining for a given propositional formula if it is satisfiable. While noone was ever able to come up with an efficient solution to this problem, it is comparably easy to come up with a basic solution.

To implement this, the solver 
1) reads input files of formulas in Reverse Polish notation (RPN),
2) uses the Tseytin transfor- mation to transform the task in conjuctive normal form and
3) solves it via the Davis-Putnam-Logemann-Loveland (DPLL) algorithm.
