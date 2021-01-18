#pragma once

struct _GtkWidget;

class EuhatApp;
class EuhatChildWnd;

class EuhatWindow
{
public:
	EuhatWindow(EuhatApp *app);
	~EuhatWindow();

	void setTitle(const char *title);
	void addChildWnd(EuhatChildWnd *childWnd);
	void showAll();

	struct _GtkWidget *window_;
//	struct _GtkWidget *frame_;
	struct _GtkWidget *box_;
};
