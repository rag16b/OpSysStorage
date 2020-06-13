__Github Username Legend:
Urakuna- Ryan Gutierrez
jmaham23 - Joshua Story
Elegade & Elegade7 - Zachary Gutierrez

malloc(): memory corruption (fast): 0x00000000017ce030 *** 
	suspect: Input/Output Redirection, while loop attempts to pull Redirection token from character array, setting the corresponding index to NULL which seems to cause issues in memory. The issue seems to arise when passing a pointer that is NULL. Causing a memory access violation. 