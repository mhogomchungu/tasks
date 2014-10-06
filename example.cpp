/*
 * copyright: 2014
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

static void _printThreadID()
{
	qDebug() << "Thread id: " << QThread::currentThreadId() ;
}

static void _useResult( const QString& e )
{
	Q_UNUSED( e ) ;
}

void example::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

auto _longRunningTask = []()
{
	return "abc" ;
} ;

/*
 * Example 1
 */
#if 1

void example::run()
{
	/*
	 * A sample use where a task is run on separate thread and return a value and the returned
	 * value is used on another task run on the original thread
	 */

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
		 * exit the application since we have reached the end of the demo.
		 */
		QCoreApplication::quit() ;
	} ) ;
}

#else

/*
 * Example 2
 */
void example::run()
{
	/*
	 * Task::await() function below does the following:
	 * 1. suspends the run method.
	 * 2. creates a new thread.
	 * 3. runs the _longRunningTask method in the new thread.
	 * 4. store the result of  _longRunningTask function in r.
	 * 5. resumes the run method.
	 */
	QString r = Task::await<QString>( _longRunningTask ) ;

	qDebug() << r ;

	QCoreApplication::quit() ;
}

#endif
