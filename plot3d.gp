set terminal png size 800,600 enhanced font "Helvetica,11"
set output filename.".png"
set xrange [`echo $BH_XRANGE`]
set yrange [`echo $BH_YRANGE`]
set zrange [`echo $BH_ZRANGE`]
set cbrange [`echo $BH_ZRANGE`]
set ticslevel 0
set border 4095 ls 1 lc rgb '#696969'
unset colorbox
splot filename u 1:2:3 w p pt 7 palette notitle