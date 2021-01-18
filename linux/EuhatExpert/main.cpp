#include <EuhatApp.h>
#include <EuhatWindow.h>
#include <EuhatChildWnd.h>
#include <list>
#include <vector>
#include <string>
#include <memory>

using namespace std;

class TstApp : public EuhatApp
{
public:
	void onActivate();

	unique_ptr<EuhatWindow> mainWin_;
	unique_ptr<EuhatChildWnd> childWnd_;
	unique_ptr<EuhatChildWnd> childWnd2_;
};

void TstApp::onActivate()
{
	mainWin_.reset(new EuhatWindow(this));
	childWnd_.reset(new EuhatChildWnd(400, 400));
	childWnd2_.reset(new EuhatChildWnd(400, 400));
	mainWin_->addChildWnd(childWnd_.get());
	mainWin_->addChildWnd(childWnd2_.get());
	mainWin_->showAll();
}

int main(int argc, char **argv)
{
	TstApp app;
	return app.run(argc, argv);
}
