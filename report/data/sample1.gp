call "data/common.gnuplot" "3.6in,1.8in"

set output "`echo $OUT`"

set style data histograms
set style histogram rowstacked
set boxwidth 0.75
set xtics rotate by 45 right
set style fill solid border -1
set yrange [-10:]
set tics font "Times New Roman,21"
set ylabel font "Times New Roman,23"
set key font "Times New Roman,11"
set ylabel "Performance improvement (\\%)"
set key outside horizontal top center

C1 = "#A00000"
C2 = "#00A000"
C3 = "#5060D0"
C4 = "#F25900"
C5 = "#008ABB"
C6 = "#002E00"

plot 'data/sample1/data.dat' using 2:xtic(1) t "INLN" lt rgb C1, \
     '' using 3 t "SS" lt rgb C2, \
     '' using 4 t "RNT" lt rgb C3, \
     '' using 5 t "CBB" lt rgb C4, \
     '' using 6 t "CFN" lt rgb C5, \
     '' using 7 t "HGP" lt rgb C6
