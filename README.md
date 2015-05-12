

Asynchronous programming in Qt/C++ using tasks,continuations and resumable functions.

This project is inspired by this[1] video on channel9.

The project seeks to do async based programming in Qt/C++ using modern C++ with lambdas.

This library wraps a function into a future where the result of the function
can be retrieved through the future's 3 public methods:

1. .get()  runs the wrapped function on the current thread.

2. .then() registers an event to be called when the wrapped function finishes
           and then runs the wrapped function in a different thread.The registered function
           will run in the current thread.

3. .await() suspends the calling function and then runs the wrapped function
            in a separate thread and then unsuspends the calling function when
            the wrapped function finish running.The suspension will be done
            without blocking the current thread leaving free to perform other tasks.


recommending reading up on C#'s await keyword to get a sense of how .await() works.

Example use case for the Task::run().then() API can be found here[0]. Additional example is [2] where an API is
declared and [3] where the declared API is used.

Example use case of the Task::await() API is here[6] where a function call "blocks" waiting for a result without "hanging" the entire GUI application.

A short tutorial on task/async/await as implemented in C# can be viewed from this[8] link.

[0] https://github.com/mhogomchungu/tasks/blob/master/example.cpp

[1] http://channel9.msdn.com/Blogs/Charles/Asynchronous-Programming-for-C-Developers-PPL-Tasks-and-Windows-8

[2] https://github.com/mhogomchungu/zuluCrypt/blob/d0439a4e36521e42fa9392b82dcefd3224d53334/zuluMount-gui/zulumounttask.h#L61

[3] https://github.com/mhogomchungu/zuluCrypt/blob/d0439a4e36521e42fa9392b82dcefd3224d53334/zuluMount-gui/mainwindow.cpp#L812

[4] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3558.pdf

[5] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3564.pdf

[6] https://github.com/mhogomchungu/zuluCrypt/blob/3671e2ea8ae37fb07105de0e2a856b766b6f9fc3/zuluCrypt-gui/createkeyfile.cpp#L179

[7] https://www.youtube.com/watch?v=Y475RshtAHA

[8] http://www.youtube.com/watch?v=DqjIQiZ_ql4
