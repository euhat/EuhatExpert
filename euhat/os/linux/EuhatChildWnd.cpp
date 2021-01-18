#include "EuhatChildWnd.h"
#include <gtk/gtk.h>

static gboolean configureEventCb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	EuhatChildWnd *pThis = (EuhatChildWnd *)data;

	if (NULL != pThis->surface_)
		cairo_surface_destroy(pThis->surface_);

	pThis->surface_ = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget));

	pThis->clear();

	return TRUE;
}

static gboolean drawCb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	EuhatChildWnd *pThis = (EuhatChildWnd *)data;

	cairo_set_source_surface(cr, pThis->surface_, 0, 0);
	cairo_paint(cr);

	return FALSE;
}

static gboolean buttonPressEventCb(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	EuhatChildWnd *pThis = (EuhatChildWnd *)data;
	if (NULL == pThis->surface_)
		return FALSE;

	if (event->button == GDK_BUTTON_PRIMARY)
	{
		pThis->xFrom_ = event->x;
		pThis->yFrom_ = event->y;
		pThis->drawBrush(widget, event->x, event->y);
	}
	else if (event->button == GDK_BUTTON_SECONDARY)
	{
		pThis->clear();
		gtk_widget_queue_draw(widget);
	}

	return TRUE;
}

static gboolean motionNotifyEventCb(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	EuhatChildWnd *pThis = (EuhatChildWnd *)data;
	if (NULL == pThis->surface_)
		return FALSE;

	if (event->state & GDK_BUTTON1_MASK)
		pThis->drawBrush(widget, event->x, event->y);

	return TRUE;
}

EuhatChildWnd::EuhatChildWnd(int width, int height)
{
	surface_ = NULL;
	xFrom_ = 0;
	yFrom_ = 0;

	drawingArea_ = gtk_drawing_area_new();

	gtk_widget_set_size_request(drawingArea_, width, height);

	g_signal_connect(drawingArea_, "configure-event", G_CALLBACK(configureEventCb), this);
	g_signal_connect(drawingArea_, "draw", G_CALLBACK(drawCb), this);

	g_signal_connect(drawingArea_, "button-press-event", G_CALLBACK(buttonPressEventCb), this);
	g_signal_connect(drawingArea_, "motion-notify-event", G_CALLBACK(motionNotifyEventCb), this);

	gtk_widget_set_events(drawingArea_, gtk_widget_get_events(drawingArea_) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
}

EuhatChildWnd::~EuhatChildWnd()
{
	if (NULL != surface_)
		cairo_surface_destroy(surface_);
}

void EuhatChildWnd::clear()
{
	cairo_t *cr;

	cr = cairo_create(surface_);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	cairo_destroy(cr);
}

void EuhatChildWnd::drawBrush(GtkWidget *widget, gdouble x, gdouble y)
{
	cairo_t *cr;

	cr = cairo_create(surface_);

//	cairo_rectangle(cr, x - 3, y - 3, 6, 6);
//	cairo_fill(cr);
//	cairo_draw_line(cr, xFrom_, yFrom_, x, y);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);
	cairo_move_to(cr, xFrom_, yFrom_);
	cairo_line_to(cr, x, yFrom_);
	cairo_line_to(cr, x, y);
	cairo_line_to(cr, xFrom_, y);
	cairo_line_to(cr, xFrom_, yFrom_);
	cairo_stroke(cr);

	cairo_destroy(cr);

//	gtk_widget_queue_draw_area(widget, x - 3, y - 3, 6, 6);
	gtk_widget_queue_draw(widget);
}

