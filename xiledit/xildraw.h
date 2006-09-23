/*
 * (C) Copyright 2006 Jean-Baptiste Note <jean-baptiste.note@m4x.org>
 * All rights reserved.
 */

/**
 * xildraw.h
 *
 * A GTK+ widget that implements a xilinx bitstream display
 *
 */

#ifndef __EGG_XILDRAW_FACE_H__
#define __EGG_XILDRAW_FACE_H__

#include <gtk/gtk.h>

#include "analysis.h"
#include "bitdraw.h"

G_BEGIN_DECLS

#define EGG_TYPE_XILDRAW_FACE          (egg_xildraw_face_get_type ())
#define EGG_XILDRAW_FACE(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), EGG_TYPE_XILDRAW_FACE, EggXildrawFace))
#define EGG_XILDRAW_FACE_CLASS(obj)    (G_TYPE_CHECK_CLASS_CAST ((obj), EGG_XILDRAW_FACE, EggXildrawFaceClass))
#define EGG_IS_XILDRAW_FACE(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EGG_TYPE_XILDRAW_FACE))
#define EGG_IS_XILDRAW_FACE_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((obj), EFF_TYPE_XILDRAW_FACE))
#define EGG_XILDRAW_FACE_GET_CLASS     (G_TYPE_INSTANCE_GET_CLASS ((obj), EGG_TYPE_XILDRAW_FACE, EggXildrawFaceClass))

typedef struct _EggXildrawFace      EggXildrawFace;
typedef struct _EggXildrawFaceClass EggXildrawFaceClass;

struct _EggXildrawFace
{
  GtkDrawingArea parent;

  /* offscreen buffer state */
  GdkPixmap *pixmap;
  drawing_context_t *ctx;
  bitstream_analyzed_t *nlz;

  /* Adjustments for position and zoom */
  GtkAdjustment *vadjust;
  GtkAdjustment *hadjust;
  GtkAdjustment *zoomadjust;

  /* state variables for dragging */
  gboolean drag;
  double refx;
  double refy;

  /* context menu */
  GtkMenu *menu;
};

struct _EggXildrawFaceClass
{
  GtkDrawingAreaClass parent_class;
};

GtkType egg_xildraw_face_get_type (void);
GtkWidget *egg_xildraw_face_new (bitstream_analyzed_t *);
void egg_xildraw_adapt_window(EggXildrawFace *xildraw, GtkWindow *window);

/* Quirk ! */
void egg_xildraw_fullscreen(EggXildrawFace *self);
void egg_xildraw_unfullscreen(EggXildrawFace *self);
void egg_xildraw_site_names(EggXildrawFace *self, gboolean val);

G_END_DECLS

#endif
