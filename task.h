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

#include <functional>
#include <QThread>

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
		void setStartFunction( std::function< void( void ) > function )
		{
			m_start = function ;
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
		void run( const T& arg )
		{
			m_function( arg ) ;
		}
	private:
		std::function< void( const T& ) > m_function ;
		std::function< void( void ) > m_start ;
	};

	template< typename T >
	class ThreadHelper : public Thread
	{
	public:
		ThreadHelper( std::function< T ( void ) > function ) : m_function( function )
		{
		}
		future<T>& taskContinuation( void )
		{
			m_future.setStartFunction( [&](){ this->start() ; } ) ;
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
		void setStartFunction( std::function< void( void ) > function )
		{
			m_start = function ;
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
	private:
		std::function< void( void ) > m_function ;
		std::function< void( void ) > m_start ;
	};

	class ThreadHelper_1 : public Thread
	{
	public:
		ThreadHelper_1( std::function< void ( void ) > function ) : m_function( function )
		{
		}
		future_1& taskContinuation( void )
		{
			m_future.setStartFunction( [&](){ this->start() ; } ) ;
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
	 * This API runs two tasks,the first one will be run in a different thread and
	 * the second one will be run on the original thread after the completion of the
	 * first one.
	 *
	 * See example at the end of this header file for a sample use case
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
	 *
	 * Note that this function returns an argument.
	 */
	return 0 ;
}

auto _b = []( const int& r ){
	/*
	 * task _b does what task _b does here.
	 *
	 * Note that this function requires an argument that matches the returned argument type of _a
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
auto _a = [](){
	/*
	 * task _a does what task _a does here.
	 *
	 * This function body will run on a different thread
	 *
	 * Note that this function returns no argument
	 */
}

auto _b = [](){
	/*
	 * task _b does what task _b does here.
	 *
	 * Note that this function requires no argument.
	 * This function body will run on the original thread
	 */
}

Task::run( _a ).then( _b ) ;

/*
 * if no continuation
 */
Task::exec( _a ) ;

#endif
