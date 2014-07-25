

This project is inspired by this[1] video on channel9

The project seeks to do async based programming in Qt/C++ using modern C++ with lambdas.

It provides two sets of APIs.

1. Task::run().then() API that allows async programming with continuation[4]

2. Task::await() API thats gives sync programming without "hanging" the current thread[5]

Another example for sample use cause is this API[2] followed by usage of the API here[3]

[1] http://channel9.msdn.com/Blogs/Charles/Asynchronous-Programming-for-C-Developers-PPL-Tasks-and-Windows-8

[2] https://github.com/mhogomchungu/zuluCrypt/blob/d0439a4e36521e42fa9392b82dcefd3224d53334/zuluMount-gui/zulumounttask.h#L61

[3] https://github.com/mhogomchungu/zuluCrypt/blob/d0439a4e36521e42fa9392b82dcefd3224d53334/zuluMount-gui/mainwindow.cpp#L812

[4] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3558.pdf

[5] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3564.pdf
