# Background
The project comprises of two ways of reading in an input file `infections.txt` and detecting 
who was responsible for infecting who with covid in a sequence of infections. 
The first part of the project involves analyzing the infections data by printing a sequence of names for those who were infected - using C, while the next part involves performing the same task in MIPS Assemlbly language.

# Input
The input file `infections.txt` consists of several lines of the format `<Name1 Name2>`, where `Name1` has infected `Name2`. The file always ends with the line: "DONE". 

# Requirements
The first line of the file will always be the first person that is infected with no way of knowing who infected them (the "source"). Afterwards, any infector that appears in the list will always be someone who has already been infected. An infector may infect at most two people. 

# Output
Given the aforementioned requirements, print out the names of the infected people in a In-Order Depth-First-Search fashion.
