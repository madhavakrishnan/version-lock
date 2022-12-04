call "data/common.gnuplot" "7.4in,1.1in"

set output "`echo $OUT`"

mp_startx=0.04
mp_starty=0.01
mp_height=0.91
mp_rowgap=0.15
mp_colgap=0.03
mp_width=0.91

eval mpSetup(6, 1)

set key default
set key spacing 1.25
#set key at 80,180

#set xrange[1:112]
set xtics 16

eval mpNext
#set format y "%0.2f"
set xlabel '\# threads'
set title '256B' offset -1.5,0
set key at 100, 35
#unset key
set ylabel 'Ops/$\mu$sec' offset -1,0
set yrange [0:35]
plot \
       'data/sample0/256-bytes.dat' index 0 title 'Makalu' with linespoints linestyle 1,\
                     '' index 1 title '' with linespoints linestyle 2,\
                     '' index 2 title '' with linespoints linestyle 3

eval mpNext
#set format y "%0.2f"
set xlabel '\# threads'
set title '1KB' offset -1.5,0
set key at 100, 35
#unset key
unset ylabel
#set ylabel 'Ops/$\mu$sec'
set yrange [0:35]
#set format x ""
plot \
       'data/sample0/1kb.dat' index 0 title '' with linespoints linestyle 1,\
                     '' index 1 title '\sys' with linespoints linestyle 2,\
                     '' index 2 title '' with linespoints linestyle 3

eval mpNext
#set format y "%0.2f"
set xlabel '\# threads'
set title '4KB' offset -1.5,0
set key at 100, 35
#unset key
#set ylabel 'Ops/$\mu$sec'
set yrange [0:35]
#set format x ""
plot \
       'data/sample0/4kb.dat' index 0 title '' with linespoints linestyle 1,\
                     '' index 1 title '' with linespoints linestyle 2,\
                     '' index 2 title 'PMDK' with linespoints linestyle 3

eval mpNext
#set format y "%0.2f"
set format x "%0.f"
set xlabel '\# threads'
set title '128KB' offset -1.5,0
set key at 80, 0.5
unset key
set ylabel 'Ops/$\mu$sec' offset -1,0
set yrange [0:35]
plot \
       'data/sample0/128kb.dat' index 0 title 'Makalu' with linespoints linestyle 1,\
                     '' index 1 title '\sys' with linespoints linestyle 2,\
                     '' index 2 title 'PMDK' with linespoints linestyle 3

eval mpNext
#set format y "%0.2f"
set format x "%0.f"
set xlabel '\# threads'
set title '256KB' offset -1.5,0
set key at 80, 0.5
unset key
unset ylabel
#set ylabel 'Ops/$\mu$sec'
set yrange [0:35]
plot \
       'data/sample0/256kb.dat' index 0 title 'Makalu' with linespoints linestyle 1,\
                     '' index 1 title '\sys' with linespoints linestyle 2,\
                     '' index 2 title 'PMDK' with linespoints linestyle 3

eval mpNext
#set format y "%0.2f"
set format x "%0.f"
set xlabel '\# threads'
set title '512KB' offset -1.5,0
set key at 80, 0.5
unset key
#set ylabel 'Ops/$\mu$sec'
set yrange [0:35]
plot \
       'data/sample0/512kb.dat' index 0 title 'Makalu' with linespoints linestyle 1,\
                     '' index 1 title '\sys' with linespoints linestyle 2,\
                     '' index 2 title 'PMDK' with linespoints linestyle 3
