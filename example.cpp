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

	} ).then( []( const QString& r ){

		/*
		 * use the returned value by the previous task
		 */
		qDebug() << r ;
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
	QString r = Task::await<QString>( _longRunningTask ) ;

	qDebug() << r ;

	QCoreApplication::quit() ;
}

#endif