
#include <QCoreApplication>
#include "example.h"

int main( int argc,char *argv[] )
{
	QCoreApplication app( argc,argv ) ;

	example e ;
	e.start() ;

	return app.exec() ;
}
