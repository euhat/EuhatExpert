#include "EuhatApp.h"
#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer data)
{
	EuhatApp *pThis = (EuhatApp *)data;

	pThis->onActivate();
}

EuhatApp::EuhatApp()
{
	app_ = gtk_application_new("com.euhat.expert", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app_, "activate", G_CALLBACK(activate), this);
}

EuhatApp::~EuhatApp()
{
	g_object_unref(app_);
}

void EuhatApp::onActivate()
{
}

int EuhatApp::run(int argc, char **argv)
{
	int status;
	status = g_application_run(G_APPLICATION(app_), argc, argv);
	return 1;
}
