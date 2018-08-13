#!/bin/sh -xv
# this shell runs the main program 
# several times to see the effect of numP
rm KL.txt top1.txt kNNresult.txt knnFinal.txt KLFinal.txt top1Final.txt>/dev/null #suppress error message
numP=50

numR=2
numObjSm=200
ratio=0.02
k=3
range=2
: '
range=0.5
dumb=1
while [ $dumb -le 5 ]
do
    timeshot=200

    while [ $timeshot -le 800 ]
    do
        ./main $numP $timeshot $numR $numObjSm $ratio $k $range
        timeshot=$(($timeshot+40))
    done
    ./KLAvg.o
    rm KL.txt
    ./top1Avg.o
    rm top1.txt
    ./knnAvg.o $k
    rm kNNresult.txt
    dumb=$(($dumb+1))
    range=`echo "$range+0.5"|bc`

done
cp KLFinal.txt KL_range.txt
cp knnFinal.txt knn_range.txt
cp top1Final.txt top1_range.txt
range=2
'


numP=2
while [ $numP -le 512 ]
do
    timeshot=200

    while [ $timeshot -le 800 ]
    do
        ./main $numP $timeshot $numR $numObjSm $ratio $k $range
        timeshot=$(($timeshot+40))
    done
    ./KLAvg.o
    rm KL.txt
    ./top1Avg.o
    rm top1.txt
    ./knnAvg.o $k
    rm kNNresult.txt
    numP=$(($numP*2))
done
cp KLFinal.txt KL_numP.txt
cp knnFinal.txt knn_numP.txt
cp top1Final.txt top1_numP.txt
numP=50

: '
numObjSm=200
while [ $numObjSm -le 1000 ]
do
    timeshot=200

    while [ $timeshot -le 800 ]
    do
        ./main $numP $timeshot $numR $numObjSm $ratio $k $range
        timeshot=$(($timeshot+40))
    done
    ./KLAvg.o
    rm KL.txt
    ./top1Avg.o
    rm top1.txt
    ./knnAvg.o $k
    rm kNNresult.txt
    numObjSm=$(($numObjSm+200))
done
cp KLFinal.txt KL_obj.txt
cp knnFinal.txt knn_obj.txt
cp top1Final.txt top1_obj.txt
numObjSm=200
'
: '
numR=1
timeshot=500
while [ $numR -le 5 ]
do
    ./main $numP $timeshot $numR $numObjSm $ratio $k
    
    numR=$(($numR+1))
done
numR=2
'
: '
dumb=1
while [ $dumb -le 5 ]
do
    timeshot=200
    while [ $timeshot -le 800 ]
    do
        ./main $numP $timeshot $numR $numObjSm $ratio $k $range
        timeshot=$(($timeshot+40))
    done
    ./KLAvg.o
    rm KL.txt
    ratio=`echo "$ratio+0.01"|bc`
    dumb=$(($dumb+1))
done
cp KLFinal.txt KL_queryW.txt
ratio=0.02
'
: '
k=2
while [ $k -le 9 ]
do
    timeshot=200
    while [ $timeshot -le 800 ]
    do
        ./main $numP $timeshot $numR $numObjSm $ratio $k $range
        timeshot=$(($timeshot+40))
    done
    ./knnAvg.o $k
    rm kNNresult.txt
    k=$(($k+1))
done
cp knnFinal.txt knn_k.txt
k=3
'


exit 0
