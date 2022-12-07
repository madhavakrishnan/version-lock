call "data/common.gnuplot" "8.0in,1.8in"

set output "`echo $OUT`"

eval mpSetup(3, 1)

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
set title '(a) YCSB-A' offset 0,-22
plot "data/zipfian/ycsb-a.dat" \
	 using ($2):xtic(1) 	title 'Version locking' lt rgb C1, \
	 "" using ($3) title      ''  lt rgb C2, \
	 "" using ($4) title      ''  lt rgb C3


eval mpNext
set key at 5,50
unset ylabel
set yrange [0:50]
set ytic 10
set title '(b) YCSB-B' offset 0,-22
plot "data/zipfian/ycsb-b.dat" \
	 using ($2):xtic(1) 	title '' lt rgb C1, \
	 "" using ($3) title      'Lock-free'  lt rgb C2, \
	 "" using ($4) title      ''  lt rgb C3


eval mpNext
set key at 5,60
set yrange [0:60]
set ytic 10
set title '(c) YCSB-C' offset 0,-22
plot "data/zipfian/ycsb-c.dat" \
	 using ($2):xtic(1) 	title '' lt rgb C1, \
	 "" using ($3) title      ''  lt rgb C2, \
	 "" using ($4) title      'RW-lock'  lt rgb C3


