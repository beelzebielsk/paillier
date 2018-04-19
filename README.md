In order to compile this program, open up a terminal and type the
following:

    make

The output should be a program named `main`.

Language Grammar:

~~~
Program -> Declaration+ Instruction+
Declaration -> (
    "input" Identifier /
    "share" Identifier
)
Instruction -> (
    "add" Identifier Operand Operand /
    "mult" Identifier Operand Operand /
    "load" Identifier Operand /
    "output" Identifier Operand
)
// Might put numbers in operand.
Operand -> Identifier
Identifier -> [a-zA-Z][a-zA-Z0-9]+
~~~
