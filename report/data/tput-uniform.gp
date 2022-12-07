call "data/common.gnuplot" "4.4in,3.6in"

set output "`echo $OUT`"

eval mpSetup(2, 2)

set key maxrows 2
set key samplen 2
#set xtics rotate by 45 right
#set tics font "Times New Roman,21"
#set ylabel font "Times New Roman,23"
set key font "Times New Roman,14"
set ylabel "Mops/sec"
set xlabel '\# Threads'
set style histogram clustered gap 1 title textcolor lt -1
set style data histograms
set style fill solid border -1
set boxwidth 1
set border back

C1 = "#A00000"
C2 = "#00A000"
C3 = "#5060D0"
C4 = "#F25900"
C5 = "#008ABB"
C6 = "#002E00"


eval mpNext
set key outside top right vertical
set key at 5,12
set yrange [0:12]
set ytic 2
set title '(a) YCSB-A' offset 0,-19
plot "data/uniform/ycsb-a.dat" \
	 using ($2):xtic(1) 	title 'Version locking' lt rgb C1, \
	 "" using ($3) title      'Lock-free'  lt rgb C2, \
	 "" using ($4) title      ''  lt rgb C3


eval mpNext
set key outside top right vertical
set key at 5,40
unset ylabel
set yrange [0:40]
set ytic 5
set title '(b) YCSB-B' offset 0,-19
plot "data/uniform/ycsb-b.dat" \
	 using ($2):xtic(1) 	title '' lt rgb C1, \
	 "" using ($3) title      ''  lt rgb C2, \
	 "" using ($4) title      'RW-lock'  lt rgb C3


eval mpNext
set ylabel "Mops/sec"
set yrange [0:40]
set ytic 5
set title '(c) YCSB-C' offset 0,-19
plot "data/uniform/ycsb-c.dat" \
	 using ($2):xtic(1) 	title '' lt rgb C1, \
	 "" using ($3) title      ''  lt rgb C2, \
	 "" using ($4) title      ''  lt rgb C3


eval mpNext
unset ylabel
set yrange [0:60]
set ytic 10
set title '(d) YCSB-D' offset 0,-19
plot "data/uniform/ycsb-d.dat" \
	 using ($2):xtic(1) 	title '' lt rgb C1, \
	 "" using ($3) title      ''  lt rgb C2, \
	 "" using ($4) title      ''  lt rgb C3


