#include <gtk/gtk.h>

// Various windows
extern GtkWidget *window;
extern GtkWidget *window1;
extern GtkWidget *jb_dlg;

// Simple/Advanced buttons
extern GtkWidget *simp_but;
extern GtkWidget *adv_but;

// The options ticks
extern GtkWidget* cydia;
extern GtkWidget* bb;
extern GtkWidget* bootl;
extern GtkWidget* recvl;

// Progress bar
extern GtkWidget *pbar;

extern void set_button_label(const gchar *label);
extern void set_progress(const gchar *label);
extern void info(const char *text, const char *title);
extern int confirm(const char *q, const char *title);
extern void null_dlg(const char *q, const char *title);
