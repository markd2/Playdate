# Notes

Notes analyzing the BASIC source source.


### Variables


* A$(14)
* A1 - 0
* B0 - set to B9  (value that gets decremented?)
* B9 - number of bases  - 2 + rnd(2) + rnd(2)
* D$(128)
* D(9)
* E0
* E2 - 50  (shield percentage?)
* I - 0
* I$(8)
* J - 0
* K0 - set to K9  (value that gets decremented?)
* K4 - 0
* K9 - number of klingons  - 25 + rnd(15) + rnd(15)
* N4 - parameter to gosub 640 for window top
* P0 - 10  (photo torps?)
* Q$(5) = "K EB*"
* Q(64) - 
* Q1 - used for a vtab at 670
* Q2 - used for an htab (4 * q2 + 1) at 670
* Q3 - used to dig into Q() at 670
* Q6
* R5 = Peek(77) * 256 + PEEK(76)  (0x4C 0x4D) - HIMEM
* S(65)
* S3
* S5
* S9 - 3800
* T
* T0 - stardate mantissa (time?)
* T1 - stardate decimal = 0
* T9 - end date (number of year-mission is T9 - T0)  3427
* V1 - 0
* V2 - 0
* V3 - 0
* X - set to 89 at start of game
* X2
* Y - set to 10 at start of game
* Y2 - 0
* Z - 8 (never changed)
