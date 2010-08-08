/* */
#include <stdlib.h>
#include <string.h>
#include "jb.h"
#include "ui.h"
#include "st0rm.h"

void already_jb(GtkWidget *widget, gpointer data)
{
	gtk_progress_bar_set_fraction(GTK_WIDGET(pbar), 1.0);

	gtk_main_iteration();

	// Really nothing more to do...
	info("The IPSW is saved as ~/pwned.ipsw.\nRestore to it with your jailbroken recovery mode in iTunes (or use idevicerestore -c).\n\nThank you for using Starst0rm!", "Done!");

	// Quit
	gtk_main_quit();
}

void not_already_jb(GtkWidget *widget, gpointer data)
{
	gtk_progress_bar_set_fraction(GTK_WIDGET(pbar), 1.0);

	gtk_main_iteration();

	extern int pwnable;

	if (pwnable==1)
	{
		// I don't want to implement this yet :P
		// Oh wait, I did :P
		info("Please make sure your device is in DFU mode and securely connected.\nGood instuctions can be found at: http://www.iclarified.com/entry/index.php?enid=1034", "Get ready to PWN!");

		info("WARNING: You do not want your device to disconnect in the middle of this procedure!", "IMPORTANT!");

		char *dfuutil_cmd[512];
		snprintf(dfuutil_cmd, "gksu ./dfu-util ~/pwned.ipsw %s", target);

		gtk_progress_bar_set_text(GTK_WIDGET(pbar), "Using dfu-util to send the exploit");

		int e=system(dfuutil_cmd);

		if (e==0)
		{
			info("Now send your pwned IPSW to your device, either in iTunes or by using idevicerestore -c.", "Starst0rm has jailbroken your device!");
		}

		else {
			info("An error occured when trying to send the exploit...\nYou might try rerunning the application.", "An error occured :(");
		}

		// Quit
		gtk_main_quit();
	}

	else {
		info("Your device must be already jailbroken for Starst0rm to work.\nSorry :(", "Sorry!");

		gtk_main_quit();
	}
}

void simp_pwn(GtkWidget *widget, gpointer data)
{
	// Let things clear up...
	gtk_main_iteration();

	// Let the user know that this can take some time...
	info("Grab yourself some pie and get ready to wait; this can take a while ;)", "This can take a while...");
	gtk_main_iteration();

	// Assemble the command, just './ipsw <input.ipsw> ~/pwned.ipsw bundles/Cydia.tar'
	char *command_str[512];
	snprintf(command_str, 512, "./ipsw %s ~/pwned.ipsw -b res/boot.png -r res/recovery.png bundles/Cydia.tar", ipsw);

	gtk_progress_bar_set_fraction(GTK_WIDGET(pbar), 0.1);
	gtk_progress_bar_set_text(GTK_WIDGET(pbar), "Building the ipsw");

	gtk_main_iteration();

	// And execute the assembled command...
	system(command_str);

	gtk_progress_bar_set_fraction(GTK_WIDGET(pbar), 0.5);

	gtk_main_iteration();

	// Load the ui for the 'Already Jailbroken?' dialog
	GtkBuilder *builder;
	GError* error = NULL;

	// Get a GtkBuilder instance and load the file...
	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, "res/jb_dlg.xml", &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	// Connect all the signals in the UI file...
	gtk_builder_connect_signals (builder, NULL);

	// Get the main dialog
	jb_dlg = GTK_WIDGET (gtk_builder_get_object (builder, "dialog1"));

	// Get the buttons and connect up the callbacks
	GtkWidget *yes_but = GTK_WIDGET (gtk_builder_get_object (builder, "button1"));
        g_signal_connect (G_OBJECT (yes_but), "released",
		      G_CALLBACK (already_jb), NULL);

	GtkWidget *no_but = GTK_WIDGET (gtk_builder_get_object (builder, "button2"));
        g_signal_connect (G_OBJECT (no_but), "released",
		      G_CALLBACK (not_already_jb), NULL);

	// Free up the builder
	g_object_unref (builder);

	// And show the dialog
	gtk_widget_show (jb_dlg);

}

void adv_pwn(GtkWidget *widget, gpointer data)
{
	// Let the thread cool off...
	gtk_main_iteration();

	// Let the user know that this can take some time...
	info("Grab yourself some pie and get ready to wait; this can take a while ;)", "This can take a while...");
	gtk_main_iteration();

	// Get the button states...
	gboolean inst_cydia=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cydia));
	gboolean up_bb=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(bb));
	gboolean cust_bootl=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(bootl));
	gboolean cust_recvl=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(recvl));

	char *command_str[512];

	snprintf(command_str, 512, "./ipsw %s ~/pwned.ipsw", ipsw);

	if (cust_bootl)
	{
		choose_file();

		printf("Going to use a custom boot logo...\n");

		char *bootl_str[512];
		snprintf(bootl_str, 512, " -b %s", ipsw);
		strcat(command_str, bootl_str);

		//free(bootl_str);
	}

	if (cust_recvl)
	{
		choose_file();

		printf("Going to use a custom recovery logo...\n");

		char *recvl_str[512];
		snprintf(recvl_str, 512, " -r %s", ipsw);
		strcat(command_str, recvl_str);

		//free(recvl_str);
	}

	if (up_bb)
	{
		printf("Going to update baseband...\n");
		//snprintf(bb_u, 512, " -bbupdate");
		strcat(command_str, " -bbupdate");
	}

	if (inst_cydia)
	{
		printf("Going to install Cydia...\n");
		strcat(command_str, " bundles/Cydia.tar");
	}

	//free(inst_cydia);
	//free(up_bb);
	//free(cust_bootl);
	//free(cust_recvl);

	gtk_main_iteration();

	gtk_progress_bar_set_fraction(GTK_WIDGET(pbar), 0.1);
	gtk_progress_bar_set_text(GTK_WIDGET(pbar), "Building the ipsw");

	gtk_main_iteration();

	system(command_str);

	gtk_progress_bar_set_fraction(GTK_WIDGET(pbar), 0.5);

	gtk_main_iteration();

	// Load the ui for the 'Already Jailbroken?' dialog
	GtkBuilder *builder;
	GError* error = NULL;

	// Get a GtkBuilder instance and load the file...
	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, "res/jb_dlg.xml", &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	// Connect all the signals in the UI file...
	gtk_builder_connect_signals (builder, NULL);

	// Get the main dialog
	jb_dlg = GTK_WIDGET (gtk_builder_get_object (builder, "dialog1"));

	// Get the buttons and connect up the callbacks
	GtkWidget *yes_but = GTK_WIDGET (gtk_builder_get_object (builder, "button1"));
        g_signal_connect (G_OBJECT (yes_but), "released",
		      G_CALLBACK (already_jb), NULL);

	GtkWidget *no_but = GTK_WIDGET (gtk_builder_get_object (builder, "button2"));
        g_signal_connect (G_OBJECT (no_but), "released",
		      G_CALLBACK (not_already_jb), NULL);

	// Free up the builder
	g_object_unref (builder);

	// And show the dialog
	gtk_widget_show (jb_dlg);
}
