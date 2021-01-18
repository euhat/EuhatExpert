#pragma once

struct _cairo_surface;
struct _GtkWidget;

class EuhatChildWnd
{
public:
	EuhatChildWnd(int width, int height);
	~EuhatChildWnd();

	void clear();
	void drawBrush(struct _GtkWidget *widget, double x, double y);
	
	struct _GtkWidget *drawingArea_;
	struct _cairo_surface *surface_;

	double xFrom_;
	double yFrom_;
};
