#include "EuhatWindow.h"
#include "EuhatApp.h"
#include "EuhatChildWnd.h"
#include <gtk/gtk.h>

static void closeWindow(gpointer data)
{
}

EuhatWindow::EuhatWindow(EuhatApp *app)
{
	window_ = gtk_application_window_new(app->app_);

	g_signal_connect(window_, "destroy", G_CALLBACK(closeWindow), this);

	gtk_container_set_border_width(GTK_CONTAINER(window_), 8);

/*	frame_ = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame_), GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(window_), frame_);
*/
	box_ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_container_add(GTK_CONTAINER(window_), box_);
}

EuhatWindow::~EuhatWindow()
{
}

void EuhatWindow::setTitle(const char *title)
{
	gtk_window_set_title(GTK_WINDOW(window_), title);
}

void EuhatWindow::addChildWnd(EuhatChildWnd *childWnd)
{
//	gtk_container_add(GTK_CONTAINER(frame_), childWnd->drawingArea_);
	gtk_container_add(GTK_CONTAINER(box_), childWnd->drawingArea_);
}

void EuhatWindow::showAll()
{
	gtk_widget_show_all(window_);
}
