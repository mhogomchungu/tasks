

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


Documentation of how to use the library is here[1] and here[2].

A short tutorial on task/async/await as implemented in C# can be viewed from this[8] link.

[1] https://github.com/mhogomchungu/tasks/blob/master/example.cpp

[2] https://github.com/mhogomchungu/tasks/blob/0bd4fd227aa4f13bba25dff74df06719ada6317d/task.h#L598
