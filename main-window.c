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
 * */

void
open_file (	GtkToolButton	*toolbutton,
		gpointer	user_data)
{
	printf("Ok.");
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

	builder = gtk_builder_new ();

	if( gtk_builder_add_from_file (	builder,
					UI_FILE,
					&error) == 0)
	{
		g_printerr (	"Error loading file:%s\n",
				error->message);
		g_clear_error (&error);
		return 1;
	}

	/* Connexion des signaux du builder */

	gtk_builder_connect_signals (	builder,
					NULL);

	/* Fermer la fenetre lorsque l'on appuie sur la croix rouge */
	window = gtk_builder_get_object (builder, "window");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	gtk_main ();

	return 0;
}
