call "data/common.gnuplot" "3.6in,1.8in"

set output "`echo $OUT`"

set xtics rotate by 45 right
set tics font "Times New Roman,21"
set ylabel font "Times New Roman,23"
set key font "Times New Roman,11"
set ylabel "Memory overhead(\\%)"
# set key outside top center horizontal
set key top left vertical
set style histogram clustered gap 1 title textcolor lt -1
set style data histograms
set style fill solid border -1
set boxwidth 1
set yrange [0:]

C1 = "#A00000"
C2 = "#00A000"
C3 = "#5060D0"
C4 = "#F25900"
C5 = "#008ABB"
C6 = "#002E00"

plot "data/sample2/mem.dat" \
       using 5:xtic(1) 	title "~~~~~~~DVI-CPS+VTPtr" lt rgb C2, \
	"" using 4 title      "~~~~~~~DVI-CPI"  lt rgb C3
