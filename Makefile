obj = main.o mkfs.o mkfs_dir.o mkfs_file.o mkfs_net.o utils.o

mkfs : $(obj)
	g++ -o mkfs $(obj)

main.o : 
	g++ -c main.cpp
mkfs.o : mkfs.h
mkfs_dir.o : mkfs_dir.h
mkfs_file.o : mkfs_file.h
mkfs_net.o : mkfs_net.h
utils.o: utils.h

.PHONY : clean
clean:
	rm mkfs $(obj)
