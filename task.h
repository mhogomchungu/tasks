/*
 * copyright: 2014
 * name : mhogo mchungu
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

#ifndef __TASK_H_INCLUDED__
#define __TASK_H_INCLUDED__

#include <functional>
#include <QThread>
#include <QEventLoop>

/*
 *
 * Examples on how to use the library are at the end of this file.
 *
 */

namespace Task
{
	class Thread : public QThread
	{
		Q_OBJECT
	public:
		Thread()
		{
			connect( this,SIGNAL( finished() ),this,SLOT( deleteLater() ) ) ;
		}
	protected:
		virtual ~Thread()
		{
		}
	private:
		virtual void run( void )
		{
		}
	};

	template< typename T >
	class future
	{
	public:
		future() : m_function( []( const T& t ){ Q_UNUSED( t ) ; } )
		{
		}
		void setActions( std::function< void( void ) > start,
				 std::function< void( void ) > cancel )
		{
			m_start = start ;
			m_cancel= cancel ;
		}
		void then( std::function< void( const T& ) > function )
		{
			m_function = function ;
			m_start() ;
		}
		void start()
		{
			m_start() ;
		}
		void cancel()
		{
			m_cancel() ;
		}
		void run( const T& arg )
		{
			m_function( arg ) ;
		}
	private:
		std::function< void( const T& ) > m_function ;
		std::function< void( void ) > m_start ;
		std::function< void( void ) > m_cancel ;
	};

	template< typename T >
	class ThreadHelper : public Thread
	{
	public:
		ThreadHelper( std::function< T ( void ) > function ) :m_function( function )
		{
		}
		future<T>& taskContinuation( void )
		{
			m_future.setActions( [ this ](){ this->start() ; },
					     [ this ](){ this->deleteLater() ; } ) ;
			return m_future ;
		}
	private:
		~ThreadHelper()
		{
			m_future.run( m_cargo ) ;
		}
		void run( void )
		{
			m_cargo =  m_function() ;
		}
		std::function< T ( void ) > m_function ;
		future<T> m_future ;
		T m_cargo ;
	};

	class future_1
	{
	public:
		future_1() : m_function( [](){} )
		{
		}
		void setActions( std::function< void( void ) > start,
				 std::function< void( void ) > cancel )
		{
			m_start = start ;
			m_cancel= cancel ;
		}
		void then( std::function< void( void ) > function )
		{
			m_function = function ;
			m_start() ;
		}
		void start()
		{
			m_start() ;
		}
		void run()
		{
			m_function() ;
		}
		void cancel()
		{
			m_cancel() ;
		}
	private:
		std::function< void( void ) > m_function ;
		std::function< void( void ) > m_start ;
		std::function< void( void ) > m_cancel ;
	};

	class ThreadHelper_1 : public Thread
	{
	public:
		ThreadHelper_1( std::function< void ( void ) > function ) : m_function( function )
		{
		}
		future_1& taskContinuation( void )
		{
			m_future.setActions( [ this ](){ this->start() ; },
					     [ this ](){ this->deleteLater() ; } ) ;
			return m_future ;
		}
	private:
		~ThreadHelper_1()
		{
			m_future.run() ;
		}
		void run( void )
		{
			m_function() ;
		}
		std::function< void ( void ) > m_function ;
		future_1 m_future ;
	};

	/*
	 * Below APIs runs two tasks,the first one will be run in a different thread and
	 * the second one will be run on the original thread after the completion of the
	 * first one.
	 */

	template< typename T >
	future<T>& run( std::function< T ( void ) > function )
	{
		auto t = new ThreadHelper<T>( function ) ;
		return t->taskContinuation() ;
	}

	static inline future_1& run( std::function< void( void ) > function )
	{
		auto t = new ThreadHelper_1( function ) ;
		return t->taskContinuation() ;
	}

	static inline void exec( std::function< void( void ) > function )
	{
		Task::run( function ).start() ;
	}

	/*
	 * Below APIs implements resumable functions where a function will be "blocked"
	 * waiting for the function to return without "hanging" the current thread.
	 *
	 * recommending reading up on C#'s await keyword to get a sense of what is being
	 * discussed below.
	 */

	static inline void await( Task::future_1& t )
	{
		QEventLoop p ;

		t.then( [ & ](){ p.exit() ; } ) ;

		p.exec() ;
	}

	static inline void await( std::function< void( void ) > function )
	{
		Task::await( Task::run( function ) ) ;
	}

	template< typename T >
	T await( Task::future<T>& t )
	{
		QEventLoop p ;
		T q ;

		t.then( [ & ]( const T& r ){  q = r ; p.exit() ; } ) ;

		p.exec() ;

		return q ;
	}

	template< typename T >
	T await( std::function< T ( void ) > function )
	{
		return Task::await( Task::run( function ) ) ;
	}
}

#if 0

/*
 * templated version that passes a return value of one function to another function
 */
auto _a = [](){
	/*
	 * task _a does what task _a does here.
	 *
	 * This function body will run on a different thread
	 */
	return 0 ;
}

auto _b = []( const int& r ){
	/*
	 * task _b does what task _b does here.
	 *
	 * r is a const reference to a value returned by _a
	 *
	 * This function body will run on the original thread
	 */
}

Task::run<int>( _a ).then( _b ) ;

/*
 * Non templated version that does not pass around return value
 */
auto _c = [](){
	/*
	 * task _a does what task _a does here.
	 *
	 * This function body will run on a different thread
	 */
}

auto _d = [](){
	/*
	 * task _b does what task _b does here.
	 *
	 * r is a const reference to a value returned by _a
	 *
	 * This function body will run on the original thread
	 */
}

Task::run( _c ).then( _d ) ;

/*
 * if no continuation
 */
Task::exec( _c ) ;

/*
 * Task::await() is used to "block" the calling thread until the function returns.
 *
 * Its use case is to do sync programming without hanging the calling thread.
 *
 * example use case for it is to "block" on function in a GUI thread withough blocking the GUI thread
 * hanging the application.
 */

/*
 * await example when the called function return no result
 */
Task::await( _c ) ;

/*
 * await example when the called function return a result
 */
int r = Task::await<int>( _a ) ;

#endif

#endif //__TASK_H_INCLUDED__
