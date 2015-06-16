#!/bin/bash

read -a lims < $1/plot.cfg
if [ -z $lims ]
then
	echo "Please provide a valid out_* directory as an argument."
	exit
fi

if [ -z $2 ]
then
	echo "Using zoom = x1"
else
	echo "Using zoom = x$2"
	lims[0]=`bc -l <<< "${lims[0]}/$2"`
	lims[1]=`bc -l <<< "${lims[1]}/$2"`
	lims[2]=`bc -l <<< "${lims[2]}/$2"`
	lims[3]=`bc -l <<< "${lims[3]}/$2"`
	lims[4]=`bc -l <<< "${lims[4]}/$2"`
	lims[5]=`bc -l <<< "${lims[5]}/$2"`
fi

export BH_XRANGE=`echo "${lims[0]}:${lims[1]}"`
export BH_YRANGE=`echo "${lims[2]}:${lims[3]}"`
export BH_ZRANGE=`echo "${lims[4]}:${lims[5]}"`

echo "Generating png files... "
for f in $1/*.out; do
    gnuplot -e "filename='$f'" plot3d.gp
done

echo "Generating animated gif..."
rm -f $1.gif
if [ -z $3 ]
then
	convert -dispose none -delay 1.5 $1/*.png gif_$1.gif
else
	convert -dispose none -delay 3 $1/*.png gif_$1.gif
fi