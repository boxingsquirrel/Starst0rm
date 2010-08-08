/* Test */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "ui.h"
#include "ipsw.h"
#include "jb.h"
#include "st0rm.h"

// Various windows
GtkWidget *window;
GtkWidget *window1;
GtkWidget *jb_dlg;

// Simple/Advanced buttons
GtkWidget *simp_but;
GtkWidget *adv_but;

// The options ticks
GtkWidget* cydia;
GtkWidget* bb;
GtkWidget* bootl;
GtkWidget* recvl;

// Progress bar
GtkWidget *pbar;


// The path to the ipsw...
gchar *ipsw;

// Can this device be jailbroken directly?
int pwnable=0;

// The target, really only used for calling dfu-util...
const char *target="";

// A human-readable target string...
const char *hr_target_str="";

// Flag to see if there actually is a baseband TO update...
int bb_can_up=1;

static void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

void choose_file()
{
	GtkWidget *choose=gtk_file_chooser_dialog_new("Select IPSW...", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

	if (gtk_dialog_run(GTK_DIALOG(choose))==GTK_RESPONSE_ACCEPT)
	{
		ipsw=gtk_file_chooser_get_filename(choose);
		printf("Selected ipsw: %s\n", ipsw);
		gtk_widget_destroy(choose);
		gtk_main_iteration();

		//check_ipsw((const char *)ipsw);
	}

	else {
		info("No IPSW was selected!\nThe program will now exit...", "Fatal Error!");
		exit(-1);
	}
}

void load_main_win()
{
	GtkBuilder *builder;
	GError* error = NULL;

	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, "res/main.xml", &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	gtk_builder_connect_signals (builder, NULL);
	window1 = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));

	GtkWidget *simp_but = GTK_WIDGET (gtk_builder_get_object (builder, "simp_but"));
        g_signal_connect (G_OBJECT (simp_but), "released",
		      G_CALLBACK (simp_pwn), NULL);

	GtkWidget *adv_but = GTK_WIDGET (gtk_builder_get_object (builder, "adv_but"));
        g_signal_connect (G_OBJECT (adv_but), "released",
		      G_CALLBACK (adv_pwn), NULL);

	cydia = GTK_WIDGET (gtk_builder_get_object (builder, "cydia"));
	bb = GTK_WIDGET (gtk_builder_get_object (builder, "bb"));
	bootl = GTK_WIDGET (gtk_builder_get_object (builder, "bootl"));
	recvl = GTK_WIDGET (gtk_builder_get_object (builder, "recvl"));

	pbar = GTK_WIDGET (gtk_builder_get_object (builder, "pbar"));

	gtk_progress_bar_set_text(GTK_WIDGET(pbar), (const gchar *)hr_target_str);

	if (bb_can_up==0)
	{
		gtk_widget_set_sensitive(bb, FALSE);
	}

	//ipsw_choose = GTK_WIDGET (gtk_builder_get_object (builder, "ipsw_choose"));

        //g_signal_connect (G_OBJECT (ipsw_choose), "file-set",
	//	      G_CALLBACK (set_file), NULL);
        g_signal_connect (G_OBJECT (window1), "destroy",
		      G_CALLBACK (destroy), NULL);

	g_object_unref (builder);

	gtk_widget_show (window1);
}

int main(int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	choose_file();

	check_ipsw((const char *)ipsw);

	load_main_win();

	gtk_main ();

	return 0;
}
