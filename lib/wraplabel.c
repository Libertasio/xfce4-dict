/*
 *      wraplabel.c
 *
 *      Copyright 2008-2011 Enrico Tröger <enrico(at)xfce(dot)org>
 *      Copyright 2008-2009 Nick Treleaven <nick(dot)treleaven(at)btinternet(dot)com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * A GtkLabel subclass that can wrap to any width, unlike GtkLabel which has a fixed wrap point.
 * (inspired by libview's WrapLabel, http://view.sourceforge.net)
 */


#include <gtk/gtk.h>
#include "wraplabel.h"



#define XFD_WRAP_LABEL_GET_PRIVATE(obj)		(G_TYPE_INSTANCE_GET_PRIVATE((obj),	\
											 XFD_WRAP_LABEL_TYPE, XfdWrapLabelPrivate))

struct _XfdWrapLabelClass
{
	GtkLabelClass parent_class;
};

struct _XfdWrapLabel
{
	GtkLabel parent;
};

typedef struct
{
	gsize wrap_width;
} XfdWrapLabelPrivate;


G_DEFINE_TYPE(XfdWrapLabel, xfd_wrap_label, GTK_TYPE_LABEL);

static void xfd_wrap_label_size_request		(GtkWidget *widget, GtkRequisition *req);
static void xfd_wrap_label_size_allocate	(GtkWidget *widget, GtkAllocation *alloc);
static void xfd_wrap_label_set_wrap_width	(GtkWidget *widget, gsize width);
static void xfd_wrap_label_get_preferred_width	(GtkWidget *widget, gint *minimal_width, gint *natural_width);
static void xfd_wrap_label_get_preferred_height	(GtkWidget *widget, gint *minimal_height, gint *natural_height);


static void xfd_wrap_label_class_init(XfdWrapLabelClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	widget_class->get_preferred_width = xfd_wrap_label_get_preferred_width;
	widget_class->get_preferred_height = xfd_wrap_label_get_preferred_height;
	widget_class->size_allocate = xfd_wrap_label_size_allocate;

	g_type_class_add_private(klass, sizeof (XfdWrapLabelPrivate));
}


static void xfd_wrap_label_init(XfdWrapLabel *self)
{
	XfdWrapLabelPrivate *priv = XFD_WRAP_LABEL_GET_PRIVATE(self);

	priv->wrap_width = 0;
}


/* Sets the point at which the text should wrap. */
static void xfd_wrap_label_set_wrap_width(GtkWidget *widget, gsize width)
{
	XfdWrapLabelPrivate *priv;

	if (width == 0)
		return;

	/*
	* We may need to reset the wrap width, so do this regardless of whether
	* or not we've changed the width.
	*/
	pango_layout_set_width(gtk_label_get_layout(GTK_LABEL(widget)), width * PANGO_SCALE);

	priv = XFD_WRAP_LABEL_GET_PRIVATE(widget);
	if (priv->wrap_width != width)
	{
		priv->wrap_width = width;
		gtk_widget_queue_resize(widget);
	}
}


/* Forces the height to be the size necessary for the Pango layout, while allowing the
 * width to be flexible. */
static void xfd_wrap_label_size_request(GtkWidget *widget, GtkRequisition *req)
{
	gint height;

	pango_layout_get_pixel_size(gtk_label_get_layout(GTK_LABEL(widget)), NULL, &height);

	req->width  = 0;
	req->height = height;
}


/* Sets the wrap width to the width allocated to us. */
static void xfd_wrap_label_size_allocate(GtkWidget *widget, GtkAllocation *alloc)
{
	(* GTK_WIDGET_CLASS(xfd_wrap_label_parent_class)->size_allocate)(widget, alloc);

	xfd_wrap_label_set_wrap_width(widget, alloc->width);
}


static void
xfd_wrap_label_get_preferred_width (GtkWidget *widget, gint *minimal_width, gint *natural_width)
{
  GtkRequisition requisition;

  xfd_wrap_label_size_request (widget, &requisition);
  *minimal_width = *natural_width = requisition.width;
}

static void
xfd_wrap_label_get_preferred_height (GtkWidget *widget, gint *minimal_height, gint *natural_height)
{
  GtkRequisition requisition;

  xfd_wrap_label_size_request (widget, &requisition);
  *minimal_height = *natural_height = requisition.height;
}


void xfd_wrap_label_set_text(GtkLabel *label, const gchar *text)
{
	XfdWrapLabelPrivate *priv = XFD_WRAP_LABEL_GET_PRIVATE(label);

	gtk_label_set_text(label, text);
	xfd_wrap_label_set_wrap_width(GTK_WIDGET(label), priv->wrap_width);
}


GtkWidget *xfd_wrap_label_new(const gchar *text)
{
	GtkWidget *l = g_object_new(XFD_WRAP_LABEL_TYPE, NULL);

	if (text != NULL && text[0] != '\0')
		gtk_label_set_text(GTK_LABEL(l), text);

	pango_layout_set_wrap(gtk_label_get_layout(GTK_LABEL(l)), PANGO_WRAP_WORD_CHAR);

	return l;
}
