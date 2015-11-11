

Asynchronous programming in Qt/C++ using tasks,continuations and resumable functions.

The project seeks to do async based programming in Qt/C++ using modern C++ with lambdas.

This library wraps a function into a future where the result of the wrapped function
can be retrieved through the future's 3 public methods:

1. .get()  This method runs the wrapped function on the current thread
           and could block the thread and hang GUI.

2. .then() This method does three things:

            1. Registers a method to be called when a wrapped function finish running.

            2. Runs the wrapped function on a background thread.

            3. Runs the registered method on the current thread when the wrapped
               function finish running.

3. .await() This method does three things:

            1. Suspends the current thread at a point where this method is called.

            2. Creates a background thread and then runs the wrapped function in the
               background thread.

            3. Unsuspends the current thread when the wrapped function finish and let
               the thread current continue normally.

            The suspension at step 1 is done without blocking the thread and hence the
            suspension can be done in the GUI thread and the GUI will remain responsive.

            recommending reading up on C#'s await keyword to get a sense of how
            .await() works.


Example use case for the Task::run().then() API can be found here[0]. Additional example is [2] where an API is
declared and [3] where the declared API is used.

Example use case of the Task::await() API is here[6] where a function call "blocks" waiting for a result without "hanging" the entire GUI application.

A short tutorial on task/async/await as implemented in C# can be viewed from this[8] link.

[0] https://github.com/mhogomchungu/tasks/blob/master/example.cpp

[2] https://github.com/mhogomchungu/zuluCrypt/blob/529582af891182a849756d5283d92293c991ec8e/zuluCrypt-gui/utility.h#L235

[3] https://github.com/mhogomchungu/zuluCrypt/blob/529582af891182a849756d5283d92293c991ec8e/zuluCrypt-gui/luksaddkey.cpp#L392

[4] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3558.pdf

[5] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3564.pdf

[6] https://github.com/mhogomchungu/zuluCrypt/blob/3671e2ea8ae37fb07105de0e2a856b766b6f9fc3/zuluCrypt-gui/createkeyfile.cpp#L179

[7] https://www.youtube.com/watch?v=Y475RshtAHA

[8] http://www.youtube.com/watch?v=DqjIQiZ_ql4
