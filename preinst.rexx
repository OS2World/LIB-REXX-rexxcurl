/**/
Parse Arg rexx_exe infile outfile
line = Linein(infile)
Call Stream outfile, 'C', 'OPEN WRITE REPLACE'
Call Lineout outfile, "#!" || rexx_exe
Do While(Lines(infile) > 0)
   line = Linein(infile)
   Call Lineout(outfile, line )
End
Return 0
