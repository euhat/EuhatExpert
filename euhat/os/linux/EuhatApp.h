#pragma once

struct _GtkApplication;

class EuhatApp
{
public:
	EuhatApp();
	~EuhatApp();

	virtual void onActivate();
	int run(int argc, char **argv);

	struct _GtkApplication *app_;
};
