##### how to compile
gcc diskbenchmark.c -o diskbenchmark -lpthread
##### config file example
```txt
./t1.txt
./t2.txt
./t3.txt
./t4.txt
```
##### test example
- sequential read , 4 threads block size 4 kb
./diskbenchmark -r -S -b 4 -n 4-f config.txt
- sequential read , 4 threads block size 4 kb, write file size 209996 byte
./diskbenchmark -w -S -b 4 -n 4-f config.txt -s 209996
- random read , 4 threads block size 4 kb
./diskbenchmark -r -R -b 4 -n 4-f config.txt
- random write , 4 threads block size 4 kb, write file size 209996 byte
./diskbenchmark -w -R -b 4 -n 4-f config.txt -s 209996