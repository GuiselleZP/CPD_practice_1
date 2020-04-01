#g++ `pkg-config --cflags opencv` blur-effect-sec.cpp `pkg-config --libs opencv` -o blurSecuencial.out -lpthread
#g++ `pkg-config --cflags opencv` blur-effect-par.cpp `pkg-config --libs opencv` -o blurParalelo.out -lpthread
#! /bin/bash


#! /bin/bash

gcc -Wall -pedantic blur_effect_Sec.c -o blurSec -lm
gcc -Wall -pedantic blur_effect_Par.c -o blurPar -lm


echo "" > resultados.txt

#Mediciones del programa secuencial para kernel entre 3 y 15
echo "**********************************" >> resultados.txt
echo "----------------------------------" >> resultados.txt
echo "       Ejecucion secuencial       " >> resultados.txt
echo "----------------------------------" >> resultados.txt
echo "**********************************" >> resultados.txt

kernel=3

echo "----------------------------------" >> resultados.txt
echo "         imagen 720p              " >> resultados.txt
echo "----------------------------------" >> resultados.txt
while [ $kernel -lt 16 ];
    do
    echo "Con un kernel de" $kernel >> resultados.txt
    { time ./blurSec image1.jpg $kernel >/dev/null 2>&1; } 2>> resultados.txt
    echo "" >> resultados.txt
    kernel=$[$kernel+2]
done

kernel=3
echo " "
echo "----------------------------------" >> resultados.txt
echo "          imagen 1080p            " >> resultados.txt
echo "----------------------------------" >> resultados.txt
while [ $kernel -lt 16 ];
    do
    echo "Con un kernel de" $kernel >> resultados.txt
    { time ./blurSec image2.jpg $kernel >/dev/null 2>&1; } 2>> resultados.txt
    echo "" >> resultados.txt
    kernel=$[$kernel+2]
done

kernel=3
echo " "
echo "----------------------------------" >> resultados.txt
echo "          imagen 4k               " >> resultados.txt
echo "----------------------------------" >> resultados.txt
while [ $kernel -lt 16 ];
    do
    echo "Con un kernel de" $kernel >> resultados.txt
    { time ./blurSec image3.jpg $kernel >/dev/null 2>&1; } 2>> resultados.txt
    echo "" >> resultados.txt
    kernel=$[$kernel+2]
done


##################################################################################################
##################################################################################################
########################################Paralelo##################################################
##################################################################################################
##################################################################################################


#Resultados del programa paralelizado para kernel entre 3 y 15
echo "" >> resultados.txt
echo "**********************************" >> resultados.txt
echo "----------------------------------" >> resultados.txt
echo "      Ejecicion en paralelo       " >> resultados.txt
echo "----------------------------------" >> resultados.txt
echo "**********************************" >> resultados.txt

kernel=3
threads=2
echo "----------------------------------" >> resultados.txt
echo "            imagen 720p           " >> resultados.txt
echo "----------------------------------" >> resultados.txt

while [ $kernel -lt 16 ];
    do
	while [ $threads -lt 17 ];
        do
        echo "Con un numero de hilos de" $threads >> resultados.txt
        echo "Con un kernel de" $kernel >> resultados.txt
        { time ./blurPar image1.jpg $kernel $threads >/dev/null 2>&1; } 2>> resultados.txt
	echo "" >> resultados.txt
	echo "" >> resultados.txt
	threads=$[$threads*2]
    done
    kernel=$[$kernel+2]
    threads=2
done

kernel=3
threads=2
echo "----------------------------------" >> resultados.txt
echo "          imagen 1080p            " >> resultados.txt
echo "----------------------------------" >> resultados.txt

while [ $kernel -lt 16 ];
    do
	while [ $threads -lt 17 ];
        do
        echo "Con un numero de hilos de" $threads >> resultados.txt
        echo "Con un kernel de" $kernel >> resultados.txt
        { time ./blurPar image2.jpg $kernel $threads >/dev/null 2>&1; } 2>> resultados.txt
	echo "" >> resultados.txt
	echo "" >> resultados.txt
	threads=$[$threads*2]
    done
    kernel=$[$kernel+2]
    threads=2
done

kernel=3
threads=2
echo "----------------------------------" >> resultados.txt
echo "             imagen 4k            " >> resultados.txt
echo "----------------------------------" >> resultados.txt

while [ $kernel -lt 16 ];
    do
	while [ $threads -lt 17 ];
        do
        echo "Con un numero de hilos de" $threads >> resultados.txt
        echo "Con un kernel de" $kernel >> resultados.txt
        { time ./blurPar image3.jpg $kernel $threads >/dev/null 2>&1; } 2>> resultados.txt
	echo "" >> resultados.txt
	echo "" >> resultados.txt
	threads=$[$threads*2]
    done
    kernel=$[$kernel+2]
    threads=2
done
