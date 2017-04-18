

Asynchronous programming in Qt/C++ using tasks,continuations and resumable functions.
========

The project seeks to do async based programming in Qt/C++ using modern C++ with lambdas.

This library wraps a function into a future where the result of the wrapped function
can be retrieved through the future's 3 public methods:

1. .get()  This method runs the wrapped function on the current thread
           and could block the thread and hang GUI. This API is useful when you are already
           in a background thread.

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

4. .cancel() This is an additional API that can be used to cancel a future. It is important to know that
             this method does not terminate a running thread that is powering a future, it just releases
	     memory used by a future and this method should be used if a future is to be discarded after it
	     it is accured but never used.

5. .thread() This is an additional API and it returns a pointer to a thread that is powering a future. This pointer
             coud be a nullptr and it is owned by the future object and should NOT be deleted by users of the API.

Examples of using a future.
========

**1. Example use of .get() method of a future.**

```c++

Task::future<int>& foo = bar() ;

int r = foo.get() ;

std::cout << r << "\n" ;

```

**2. Example use of .await() method of a future.**

```c++

Task::future<int>& foo = bar() ;

int r = foo.await() ;

std::cout << r << "\n" ;

```
**2. Example use an alternative use of .await() method of a future.**

```c++

int foo() ; //function prototype

int r = Task::await<int>( foo ) ;

std::cout << r << "\n" ;

```

**3. Example use of .then() method of a future.**

```c++

void meaw( int ) ; //function prototype

Task::future<int>& foo = bar() ;

foo.then( meaw ) ;

```

Examples of creating a future.
========

**1. Creating a future that has no result.**
```c++

void bar() ; //function prototype

Task::future<void>& foo = Task::run( bar ) ;

```

**2. Creating a future that has result.**
```c++

int foo() ; //function prototype

Task::future<int>& foo = Task::run<int>( foo ) ;

```

**3. Creating a future that combines multiple functions. Each function will run in its own thread concurrently. The result of the future is undefined and a function that takes no argument should be used if .await() method of the future is called.**

```c++

int foo() ; //function prototype
int bar() ; //function prototype

Task::future<int>& foo = Task::run<int>( foo,bar ) ;

```

**3. Creating a future that combines multiple futures. Each future will run in its own thread concurrently. The result of the future is undefined and a function that takes no argument should be used if .await() method of the future is called.**

```c++

Task::future<int>& foo() ; //function prototype
Task::future<int>& bar() ; //function prototype

Task::future<int>& foo = Task::run<int>( foo,bar ) ;

```

**4. Creating a future that combines multiple tasks and their continuations that take no argument. Each task will run in its own thread concurrently.**
```c++

void foo() ; //function prototype
void bar() ; //function prototype

void foo_continuation() ; //function prototype
void bar_continuation() ; //function prototype

Task::future<void>& e = Task::run( Task::void_pair{ foo,foo_continuation },
				   Task::void_pair{ bar,bar_continuation } ) ;

```

**5. Creating a future that combines multiple tasks and their continuations that takes an argument. Each task will run in its own thread concurrently. The result of the future is undefined and a function that takes no argument should be used if .await() method of the future is called.**
```c++

int foo() ; //function prototype
int bar() ; //function prototype

void foo_continuation( int ) ; //function prototype
void bar_continuation( int ) ; //function prototype

Task::future<int>& e = Task::run( Task::pair<int>{ foo,foo_continuation },
				  Task::pair<int>{ bar,bar_continuation } ) ;
```

Further documentation of how to use the library is here[1] and here[2].

A short tutorial on task/async/await as implemented in C# can be viewed from this[8] link.

[1] https://github.com/mhogomchungu/tasks/blob/master/example.cpp

[2] https://github.com/mhogomchungu/tasks/blob/0bd4fd227aa4f13bba25dff74df06719ada6317d/task.h#L598
