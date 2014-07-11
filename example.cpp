
#include <unistd.h>
#include <sys/syscall.h>

#include "task.h"
#include "example.h"

#include <QString>
#include <QDebug>
#include <QMetaObject>
#include <QCoreApplication>

static void _printThreadID()
{
	qDebug() << "Thread id: " << syscall( SYS_gettid ) ;
}

void example::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

static QString _longRunningProcess()
{
	return "abc" ;
}

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
		_printThreadID();

		/*
		 * Do a time consuming process on a separate thread and return its result
		 */
		return _longRunningProcess() ;

	} ).then( []( const QString& r ){

		/*
		 * use the returned value by the previous task
		 */
		Q_UNUSED( r ) ;
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
