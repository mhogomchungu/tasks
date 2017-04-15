/*
 * copyright: 2014-2015
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

#include "task.h"
#include "example.h"

#include <QString>
#include <QDebug>
#include <QMetaObject>
#include <QCoreApplication>
#include <iostream>

/*
 * Two examples are shown here,Example 1 and example 2.
 *
 * Explanation of example 1.
 * You can think of Task::run().then() as registering an event for two tasks to be performed in sequence.
 * The first task will be performed and on completion,the second task will be started with the result
 * of the first task.
 *
 * The first task will run on a separate thread and the second one will run on the original thread.
 *
 * The rest of the method will be free to continue independently of the registered tasks.
 *
 * Explanation of example 2.
 *
 * You can think of Task::await() as "suspending" the execution of the method until the result of the task is returned.
 * The suspension is done without blocking the thread and hence the suspension can happen on the GUI thread
 * and the GUI will not hang as a result of it.
 *
 * The net result of example 1 and example 2 are the same.Which one to use depend on your usecase.
 *
 */

static void _testing_task_await() ;
static void _testing_task_future_all() ;

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
	Q_UNUSED( e ) ;
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

	Task::run<QString>( [](){

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
 * 5. resumes "_testing_task_await" method method.
 */
static void _testing_task_await()
{
	std::cout<< "Testing Task::run().then()" << std::endl ;

	QString e = Task::await<QString>( _longRunningTask ) ;

	std::cout << e.toLatin1().constData() << std::endl ;

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
	auto fn1 = [](){ _printThreadID(); } ;
	auto fn2 = [](){ _printThreadID(); } ;
	auto fn3 = [](){ _printThreadID(); } ;

	std::cout<< "Testing Task::run().await() multiple tasks" << std::endl ;

	Task::future<void>& e = Task::run( fn1,fn2,fn3 ) ;

	e.await() ;

	Task::future<void>& f1 = Task::run( fn1 ) ;
	Task::future<void>& f2 = Task::run( fn2 ) ;
	Task::future<void>& f3 = Task::run( fn3 ) ;

	std::cout<< "Testing Task::run().then() multiple tasks" << std::endl ;

	Task::future<void>& s = Task::run( f1,f2,f3 ) ;

	s.then( [](){ QCoreApplication::quit() ;} ) ;
}

void example::run()
{
	_test_run_then() ;
}

