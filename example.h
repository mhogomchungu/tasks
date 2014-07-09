
#include <QObject>

class example : public QObject
{
	Q_OBJECT
public:
	void start(void);
private slots:
	void run();

};
