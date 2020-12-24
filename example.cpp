/*
 * copyright: 2014-2017
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "task.hpp"
#include "example.h"

#include <QString>
#include <QMetaObject>
#include <QCoreApplication>
#include <iostream>

static void _testing_task_await() ;
static void _testing_task_future_all() ;
static void _testing_multiple_tasks() ;
static void _testing_multiple_tasks_with_start() ;
static void _testing_queue_with_no_results() ;
static void _testing_queue_with_results() ;
static void _testing_checking_multiple_futures() ;

template< typename T >
static void _print( const T& e )
{
	std::cout << e << std::endl ;
}

struct wait{

	void task_finished( const char * s )
	{
		QMutexLocker m( &mutex ) ;

		counter++ ;

		std::cout << s << std::endl ;

		if( counter == max ){

			loop.exit() ;
		}
	}
	int max = 3 ;
	int counter = 0 ;
	QMutex mutex ;
	QEventLoop loop ;
};

void example::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

QString _longRunningTask()
{
	return "abc" ;
}

static void _printThreadID()
{
	std::cout << "Thread id: " << QThread::currentThreadId() << std::endl ;
}

static void _useResult( const QString& e )
{
	Q_UNUSED( e )
}

/*
 * A sample use where a task is run on separate thread and return a value and the returned
 * value is used on another task run on the original thread
 */
static void _test_run_then()
{
	std::cout<< "Testing Task::run().then()" << std::endl ;

	/*
	 * print the thread id to know we are on what thread.
	 * We are on the original thread here.
	 */
	_printThreadID() ;

	Task::run( [](){

		/*
		 * print the thread id to know we are on what thread.
		 * We are on a separate thread here
		 */
		_printThreadID() ;

		/*
		 * Do a time consuming process on a separate thread and return its result
		 */
		return _longRunningTask() ;

	} ).then( []( QString r ){

		/*
		 * use the returned value by the previous task
		 */
		_useResult( r ) ;
		/*
		 * print the thread id to know we are on what thread.
		 * We are back on the original thread and we will get here as a continuation of the task
		 * that completed above
		 */
		_printThreadID() ;

		/*
		 * moving on to the next test.
		 */
		_testing_task_await() ;
	} ) ;
}

/*
 * Task::await() function below does the following:
 * 1. suspends the "_testing_task_await" method at a point where Task::await() method is called.
 * 2. creates a new thread.
 * 3. runs the _longRunningTask method in the new thread.
 * 4. store the result of  _longRunningTask function in r.
 * 5. resumes "_testing_task_await" method.
 */
static void _testing_task_await()
{
	_print( "Testing Task::await()" ) ;

	QString e = Task::await( _longRunningTask ) ;

	_print( e.toLatin1().constData() ) ;

	/*
	 * moving on to the next test.
	 */
	_testing_task_future_all() ;
}

/*
 * Task::run() function below does the following:
 * 1. Collects a bunch of tasks.
 * 2. Runs each task on its own thread.
 * 3. Returns a future that holds all above tasks.
 * 4. .await() can be called on the future to suspend the current thread at a point
 *    where this medhod is called to wait for all tasks to finish.
 * 5. .then() can be called on the future to register an event to be called when all
 *    tasks finish running.
 */
static void _testing_task_future_all()
{
	auto fn1 = [](){ _printThreadID() ; } ;
	auto fn2 = [](){ _printThreadID() ; } ;
	auto fn3 = [](){ _printThreadID() ; } ;

	_print( "Testing Task::run().await() multiple tasks" ) ;

	Task::future<void>& e = Task::run_tasks( fn1,fn2,fn3 ) ;

	e.await() ;

	_print( "Testing Task::run().then() multiple tasks" ) ;

	Task::future<void>& f1 = Task::run( fn1 ) ;
	Task::future<void>& f2 = Task::run( fn2 ) ;
	Task::future<void>& f3 = Task::run( fn3 ) ;

	Task::future<void>& s = Task::run_tasks( f1,f2,f3 ) ;

	s.then( [](){

		/*
		 * moving on to the next test.
		 */
		_testing_multiple_tasks() ;
	} ) ;
}

/*
 * Task::run() function below does the following:
 * 1. Collects a bunch of tasks and their continuations.
 * 2. Runs each task on its own thread.
 * 3. On completion of each task,run its continuation on the current thread.
 * 4. Returns a future that holds all above tasks.
 * 5. .await() can be called on the future to suspend the current thread at a point
 *    where this medhod is called to wait for all tasks and their continuations to finish.
 * 6. .then() can be called on the future to register an event to be called when all
 *    tasks and their continuations finish.
 */
static void _testing_multiple_tasks_non_movable()
{
	_print( "Testing multiple tasks without continuation arguments" ) ;

	auto fna1 = [ a = std::unique_ptr<int>() ](){ _printThreadID(); } ;
	auto fna2 = [ a = std::unique_ptr<int>() ](){ _printThreadID(); } ;
	auto fna3 = [ a = std::unique_ptr<int>() ](){ _printThreadID(); } ;

	auto ra1 = [ a = std::unique_ptr<int>() ](){ _print( "r1" ) ; } ;
	auto ra2 = [ a = std::unique_ptr<int>() ](){ _print( "r2" ) ; } ;
	auto ra3 = [ a = std::unique_ptr<int>() ](){ _print( "r3" ) ; } ;

	Task::future<void>& e = Task::run( Task::make_pair( std::move( fna1 ),std::move( ra1 ) ),
					   Task::make_pair( std::move( fna2 ),std::move( ra2 ) ),
					   Task::make_pair( std::move( fna3 ),std::move( ra3 ) ) ) ;

	e.await() ;

	_print( "Testing multiple tasks with continuation arguments" ) ;

	auto fn1 = [ a = std::unique_ptr<int>() ](){ _printThreadID() ; return 0 ; } ;
	auto fn2 = [ a = std::unique_ptr<int>() ](){ _printThreadID() ; return 0 ; } ;
	auto fn3 = [ a = std::unique_ptr<int>() ](){ _printThreadID() ; return 0 ; } ;

	auto r1 = [ a = std::unique_ptr<int>() ]( int ){ _print( "r1" ) ; } ;
	auto r2 = [ a = std::unique_ptr<int>() ]( int ){ _print( "r2" ) ; } ;
	auto r3 = [ a = std::unique_ptr<int>() ]( int ){ _print( "r3" ) ; } ;

	Task::future<int>& s = Task::run( Task::make_pair( std::move( fn1 ),std::move( r1 ) ),
					  Task::make_pair( std::move( fn2 ),std::move( r2 ) ),
					  Task::make_pair( std::move( fn3 ),std::move( r3 ) ) ) ;

	s.then( _testing_multiple_tasks_with_start ) ;
}

static void _testing_multiple_tasks()
{
	_print( "Testing multiple tasks without continuation arguments" ) ;

	auto fna1 = [](){ _printThreadID(); } ;
	auto fna2 = [](){ _printThreadID(); } ;
	auto fna3 = [](){ _printThreadID(); } ;

	auto ra1 = [](){ _print( "r1" ) ; } ;
	auto ra2 = [](){ _print( "r2" ) ; } ;
	auto ra3 = [](){ _print( "r3" ) ; } ;

	Task::future<void>& e = Task::run( Task::make_pair( fna1,ra1 ),
					   Task::make_pair( fna2,ra2 ),
					   Task::make_pair( fna3,ra3 ) ) ;

	e.await() ;

	_print( "Testing multiple tasks with continuation arguments" ) ;

	auto fn1 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn2 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn3 = [](){ _printThreadID() ; return 0 ; } ;

	auto r1 = []( int ){ _print( "r1" ) ; } ;
	auto r2 = []( int ){ _print( "r2" ) ; } ;
	auto r3 = []( int ){ _print( "r3" ) ; } ;

	Task::future<int>& s = Task::run( Task::make_pair( fn1,r1 ),
					  Task::make_pair( fn2,r2 ),
					  Task::make_pair( fn3,r3 ) ) ;

	s.then( _testing_multiple_tasks_non_movable ) ;
}

static void _testing_multiple_tasks_with_start()
{
	std::cout<< "Testing multiple tasks with continuation arguments using start" << std::endl ;

	wait w ;

	auto fn1 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn2 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn3 = [](){ _printThreadID() ; return 0 ; } ;

	auto r1 = [ & ]( int ){ w.task_finished( "r1" ) ; } ;
	auto r2 = [ & ]( int ){ w.task_finished( "r2" ) ; } ;
	auto r3 = [ & ]( int ){ w.task_finished( "r3" ) ; } ;

	Task::future<int>& s = Task::run( Task::make_pair( fn1,r1 ),
					  Task::make_pair( fn2,r2 ),
					  Task::make_pair( fn3,r3 ) ) ;

	s.start() ;

	w.loop.exec() ;

	QCoreApplication::quit() ;
}

static void _testing_queue_with_no_results()
{
	std::cout<< "Testing queue with no result" << std::endl ;

	auto fna1 = [](){ _printThreadID(); } ;
	auto fna2 = [](){ _printThreadID(); } ;
	auto fna3 = [](){ _printThreadID(); } ;

	auto ra1 = [](){ _print( "r1" ) ; } ;
	auto ra2 = [](){ _print( "r2" ) ; } ;
	auto ra3 = [](){ _print( "r3" ) ; } ;

	Task::future<void>& e = Task::run( Task::make_pair( fna1,ra1 ),
					   Task::make_pair( fna2,ra2 ),
					   Task::make_pair( fna3,ra3 ) ) ;

	e.queue( _testing_queue_with_results ) ;
}

static void _testing_queue_with_results()
{
	std::cout<< "Testing queue with result" << std::endl ;

	auto fn1 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn2 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn3 = [](){ _printThreadID() ; return 0 ; } ;

	auto r1 = [ = ]( int ){ _print( "r1" ) ; } ;
	auto r2 = [ = ]( int ){ _print( "r2" ) ; } ;
	auto r3 = [ = ]( int ){ _print( "r3" ) ; } ;

	Task::future<int>& s = Task::run( Task::make_pair( fn1,r1 ),
					  Task::make_pair( fn2,r2 ),
					  Task::make_pair( fn3,r3 ) ) ;
	s.queue( _test_run_then ) ;
}

static void _testing_checking_multiple_futures()
{
	auto fn1 = [](){} ;
	auto fn2 = [](){} ;
	auto fn3 = [](){} ;

	_print( "Testing finding out if a future manages multiple futures" ) ;

	Task::future<void>& e = Task::run_tasks( fn1,fn2,fn3 ) ;

	const auto& z = e.all_threads() ;

	std::string s = e.manages_multiple_futures() ? "true" : "false" ;

	_print( "A future managed multiple futures: " + s ) ;
	_print( "Number of future managed: " + QString::number( z.size() ).toStdString() ) ;
}

static void _test_when_any1()
{
	_print( "Testing when_any" ) ;

	wait w ;

	auto ll1 = [ & ](){

		QThread::currentThread()->sleep( 5 ) ;
		w.task_finished( "aaa" ) ;
	} ;

	auto ll2 = [ & ](){

		QThread::currentThread()->sleep( 2 ) ;
		w.task_finished( "bbb" ) ;
	} ;

	auto ll3 = [ & ](){

		QThread::currentThread()->sleep( 3 ) ;
		w.task_finished( "ccc" ) ;
	} ;

	Task::run_tasks( ll1,ll2,ll3 ).when_any( [](){

		_print( "when_any called" ) ;
	} ) ;

	w.loop.exec() ;

	_print( "Done testing when_any" ) ;
}

static void _test_when_any2()
{
	_print( "Testing when_any with result" ) ;

	wait w ;

	auto fn1 = [ & ](){

		QThread::currentThread()->sleep( 5 ) ;
		w.task_finished( "aaa" ) ;
		return 0 ;
	} ;

	auto fn2 = [ & ](){

		QThread::currentThread()->sleep( 2 ) ;
		w.task_finished( "bbb" ) ;
		return 0 ;
	} ;

	auto fn3 = [ & ](){

		QThread::currentThread()->sleep( 3 ) ;
		w.task_finished( "ccc" ) ;
		return 0 ;
	} ;

	auto ff = [](int){};

	Task::future<int>& s = Task::run( Task::make_pair( fn1,ff ),
					  Task::make_pair( fn2,ff ),
					  Task::make_pair( fn3,ff ) ) ;

	s.when_any( [](){

		_print( "when_any called" ) ;
	} ) ;

	w.loop.exec() ;

	_print( "Done testing when_any with result" ) ;
}

static void _test_move_only_callables()
{
	Task::run( [ a = std::unique_ptr<int>() ]( int x ){ return x ; },4 ).get() ;
	Task::run( [ a = std::unique_ptr<int>() ](int){},4 ).get() ;

	Task::run( [ a = std::unique_ptr<int>() ](){ return 6 ; } ).get() ;
	Task::run( [ a = std::unique_ptr<int>() ](){} ).get() ;

	Task::await( [ a = std::unique_ptr<int>() ](){ return 6 ; } ) ;

	auto& tt = Task::run( [ a = std::unique_ptr<int>() ](){ return 6 ; } ) ;

	Task::await( tt ) ;
	Task::await( Task::run( [ a = std::unique_ptr<int>() ](){} ) ) ;

	Task::await( [ a = std::unique_ptr<int>() ]( int x ){ return x ; },4 ) ;

	Task::await( [ a = std::unique_ptr<int>() ](int){},4 ) ;

	Task::await( [ a = std::unique_ptr<int>() ](){ return 6 ; } ) ;

	Task::await( [ a = std::unique_ptr<int>() ](){} ) ;

	auto& zz = Task::run( [ a = std::unique_ptr<int>() ](){ return 6 ; } ) ;

	Task::await( zz ) ;
	Task::await( Task::run( [ a = std::unique_ptr<int>() ](){} ) ) ;

	Task::exec( [ a = std::unique_ptr<int>() ]( int x ){ return x ; },4 ) ;
	Task::exec( [ a = std::unique_ptr<int>() ](int){},4 ) ;

	Task::exec( [ a = std::unique_ptr<int>() ](){ return 6 ; } ) ;
	Task::exec( [ a = std::unique_ptr<int>() ](){} ) ;

	Task::run_tasks( [ a = std::unique_ptr<int>() ](){ return 6 ; },
			 [ a = std::unique_ptr<int>() ](){ return 6 ; },
			 [ a = std::unique_ptr<int>() ](){ return 6 ; },
			 [ a = std::unique_ptr<int>() ](){ return 6 ; } ).get() ;

	Task::run_tasks( [](){ return 6 ; },
			 [](){ return 6 ; },
			 [](){ return 6 ; },
			 [](){ return 6 ; } ).get() ;

	Task::run_tasks( Task::run( [ a = std::unique_ptr<int>() ](){} ),
			 Task::run( [ a = std::unique_ptr<int>() ](){} ),
			 Task::run( [ a = std::unique_ptr<int>() ](){} ) ).get() ;
}

static void _test_copyable_callables()
{
	auto aa = []( int x ){ return x ; } ;

	auto bb = [](){	return 6 ; } ;

	auto cc = [](){} ;

	auto dd = [](int){} ;

	Task::run( aa,4 ).get() ;
	Task::run( dd,4 ).get() ;

	Task::run( bb ).get() ;
	Task::run( cc ).get() ;

	Task::await( bb ) ;

	auto& tt = Task::run( bb ) ;

	Task::await( tt ) ;
	Task::await( Task::run( cc ) ) ;

	Task::await( aa,4 ) ;

	Task::await( dd,4 ) ;

	Task::await( bb ) ;

	Task::await( cc ) ;

	auto& zz = Task::run( bb ) ;

	Task::await( zz ) ;
	Task::await( Task::run( cc ) ) ;

	Task::exec( aa,4 ) ;
	Task::exec( dd,4 ) ;

	Task::exec( bb ) ;
	Task::exec( cc ) ;

	Task::run_tasks( bb,bb,cc ).get() ;

	Task::run_tasks( Task::run( cc ),Task::run( cc ) ).get() ;
}

struct foo
{
	foo()
	{
	}
	foo(int a)
	{
		_print(a);
	}
	std::unique_ptr<int> m ;
};

struct www
{
	void operator()()
	{

	}
	www( const www& )
	{
		_print( "const www&" ) ;
	}
	www( www&& )
	{
		_print( "www&&" ) ;
	}
	www()
	{
		_print( "www()" ) ;
	}
	www& operator=( const www& )
	{
		_print( "www& operator=( const www& )" ) ;

		return *this ;
	}
	www& operator=( www&& )
	{
		_print( "www& operator=( www&& )" ) ;

		return *this ;
	}

	std::unique_ptr<int> m ;
};

void example::run()
{
#if __cplusplus >= 201703L

	Task::await([](foo,foo,foo){},foo(7),foo(7),foo(7));
#endif
	_test_copyable_callables() ;

	_test_move_only_callables() ;

	_test_when_any1() ;

	_test_when_any2() ;

	auto run_main = []( QVariant x ){

		std::cout << x.value<int>() << ": mm Thread id: " << QThread::currentThreadId() << std::endl ;
	} ;

	auto run_bg = []( const Task::progress& pp ){

		for( int i = 0 ; i < 2 ;i++ ){
			std::cout << i << ": bg Thread id: " << QThread::currentThreadId() << std::endl ;
			pp.update( i ) ;
			QThread::currentThread()->sleep(1);
		}
	} ;

	std::cout << "main Thread: " << QThread::currentThreadId() << std::endl ;

	Task::future<void>& abc = Task::run( this,run_bg,run_main ) ;

	abc.await() ;

	_testing_checking_multiple_futures() ;

	_testing_queue_with_no_results() ;
}
