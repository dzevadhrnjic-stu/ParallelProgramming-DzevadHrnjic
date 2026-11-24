default:	PartialSum
all:	PartialSum

%.o: %.c
	mpicc -c $^

PartialSum: PartialSum.o timer.o
	mpicc $^ -o PartialSum

run:
	mpirun -n 4 ./PartialSum

run2:
	mpirun -n 2 ./PartialSum

run4:
	mpirun -n 4 ./PartialSum

run8:
	mpirun --oversubscribe -n 8 ./PartialSum

clean:
	rm -f PartialSum PartialSum.o timer.o