INPUT
: multrec over if rot swap over + rot 1 - swap multrec then ;
: mult 0 multrec ;
6 8 mult .
OUTPUT
48
