/*INFO_601-TP
	Diletta Maniglia
	Faisal Salhi */

#include <gtk/gtk.h>
#define WINDOW_HEIGHT	500
#define WINDOW_WIDTH	500
#define UI_FILE	"interface.ui"

static GtkWidget *window = NULL;


/* *
 * Fonction qui récupère le chemin d'accès d'une image sur l'ordinateur.
 * Et charge l'image correspondante dans le PixBuf.
 	 We choose the image from the computer.
	 To do that we need to create a file chooser dialog.
  */

void
open_file (	GtkToolButton	*toolbutton,
		gpointer	user_data)
{
	/*
		Variables
	*/
	GtkWidget *dialog; // Dialog is indeed a GtkWidget (Everything that is draw on the screen herit from GtkWidget)
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN; // don't worry about this for now...
	gint res; // This variable is for the result we obtain when we close the window (Either file chosen or closed)

	dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(window),
                                      action,
                                      "_Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "_Open",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

res = gtk_dialog_run (GTK_DIALOG (dialog)); // We're running the dialog now

if (res == GTK_RESPONSE_ACCEPT) // If the user indeed provided us with a file...
  {
		printf("A file has been chosen.");
		//char *filename; // We create a string for the path of the file

		/* We create a GtkFileChooser to get the path of the file.
				(This object is responsible of getting information from the dialog)*/

		//GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

		// We create the file chooser by giving him the dialog we create earlier
		//filename = gtk_file_chooser_get_filename (chooser);

		// What we do there is simply opening the file
		// TODO : Test if the file is indeed a .png file
		//				and do not open it if it's the case.
    //open_file (filename);
    //g_free (filename);
  }
}

/* MAIN : Lance l'application et retourne le status d'execution de
 * l'application à la fermeture. */

int
main (	int 	argc,
	char 	**argv)
{
	/* Variables */
	GtkBuilder *builder; // UI Builder pour construire l'interface à partir du fichier XML UI_FILE
	GObject *window;
	GError *error = NULL;

	gtk_init (	&argc,
			&argv);

	/* Construction de l'interface */

	// Creating the builder to do the interface job
	builder = gtk_builder_new ();

	if( gtk_builder_add_from_file (	builder, /* Building the interface by providing UI FILE */
					UI_FILE,
					&error) == 0)
	{
		g_printerr (	"Error loading file:%s\n",
				error->message);
		g_clear_error (&error);
		return 1;
	}

	/* Connexion des signaux du builder */
	// We tell the builder to connect the signals of the buttons

	// Makes GtkBuilder know that a function name open_file does exists
	gtk_builder_add_callback_symbol (	builder,
																		"open_file",
																		G_CALLBACK(open_file));


	/* Works for MacOs Catalina but not on Ubuntu 19.10 */
		gtk_builder_connect_signals (	builder,
						NULL);


	/* Fermer la fenetre lorsque l'on appuie sur la croix rouge */
	// In this particular case, i do it manually
	window = gtk_builder_get_object (builder, "window"); // i get the window GObject
	// Then i connect the signal to destroy the window at the end of the program.
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	gtk_main ();

	return 0;
}
