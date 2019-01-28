# Syncronized_Histogram
Here we create histogram using multiprocessing and multhreading
N: input, B: batch
N = 1 B = 1 0.00449 ms
B = 2 0.00281 ms
B = 3 0.00183 ms
N = 2 B = 1 0.00389 ms
B = 2 0.00227 ms
B = 3 0.00162 ms
N = 3 B = 1 0.00379 ms
B = 2 0.00209 ms
B = 3 0.00099 ms
Our experiment is with different input sizes and for each input, we try with different batch
amounts. When we observe and note the time cost of each batch, we can say that with more batches
we use we execute less long data at the same time.
While using more input means more total data, more batch means lesser smaller pieces of a
data cake. How thin slices are easer to eat than
thicker slices by sharing to more people who start to eat the same time, more batches will
help to handle the multithreading in a shorter
time.
