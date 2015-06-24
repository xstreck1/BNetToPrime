# BNetToPrime

This tool converts a Boolean Network in the BoolNet format to a list of all prime implicants for each regulatory function.

### Compiling 
Extra easy to compile:
```shell
cd BNetToPrime
"compiler" main.cpp
```
where "compiler" is either g++ on Unix/Linux, clang on MacOS, or cl on Windows.

The code is C++98 compliant.
### Running
```shell
BNetToPrime [--help][--ver][input [output]] 
  --help displays help
  --ver  displays version
  input  the path to the input file, if missing, 'input.bnet' is used
  output the path to the output file, if missing, 'output.json' is used
```

### Input file
The first line of the input file is expected to be: 
```
targets, factors
```
Following lines are of the form:
```
target, function
```
where "target" is a name of a component and "function" is a boolean function over components with "&" for conjunction, "|" for disjunction, "!" for negation.

### Output file
Output is a json object that holds for each component an array with two elements. 
The first element is a list of all prime implicants of the negation of its regulatory function.
The second element is a list of all prime implicants of its regulatory function.

If the component appears only in a function, a new line with 'component,component' is added.

An empty list means there is no implicant, i.e. the function is not satisfiable.

A list with only an empty object means that the formula is tautology, i.e. it is always satisfiable.

### Example
#### Input
```
targets, factors
A, B & C
B, !A | B
```
#### Output (formatted with whitespace)
```
{
    "A": [
        [
            {
                "C": 0
            },
            {
                "B": 0
            }
        ],
        [
            {
                "B": 1,
                "C": 1
            }
        ]
    ],
    "B": [
        [
            {
                "A": 1,
                "B": 0
            }
        ],
        [
            {
                "B": 1
            },
            {
                "A": 0
            }
        ]
    ],
    "C": [
        [
            {
                "C": 0
            }
        ],
        [
            {
                "C": 1
            }
        ]
    ]
}
```
