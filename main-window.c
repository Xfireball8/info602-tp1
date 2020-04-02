/*INFO_601-TP
	Diletta Maniglia
	Faisal Salhi */

#include <gtk/gtk.h>
#include <math.h>
#define WINDOW_HEIGHT	500
#define WINDOW_WIDTH	500
#define IMAGE_BUFFER_HEIGHT	400
#define IMAGE_BUFFER_WIDTH	400
#define UI_FILE	"interface.ui"

static GtkWidget *window = NULL;
static GtkBuilder *builder = NULL; // UI Builder pour construire l'interface à partir du fichier XML UI_FILE

/**
    Un pixel est une structure de 3 octets (rouge, vert, bleu). On les
    plaque au bon endroit dans un pixbuf pour modifier les couleurs du pixel.
  */
typedef struct {
   guchar rouge;
   guchar vert;
   guchar bleu;
} Pixel;

/**
    Retourne le niveau de gris du pixel.
 */
unsigned char greyLevel( Pixel* data )
{
   return (data->rouge+data->vert+data->bleu)/3;
}

/**
    Met le pixel au niveau de gris \a g.
 */
void setGreyLevel( Pixel* data, unsigned char g )
{
  data->rouge = g;
  data->vert  = g;
  data->bleu  = g;
}
/**
     Va au pixel de coordonnées (x,y) dans le pixbuf.
*/
Pixel* gotoPixel( GdkPixbuf* pixbuf, int x, int y )
{
    int rowstride = gdk_pixbuf_get_rowstride( pixbuf );
    guchar* data  = gdk_pixbuf_get_pixels( pixbuf );
    return (Pixel*)( data + y*rowstride + x*3 );
}

/* Fonction qui permet d'afficher le buffer d'output */
void
make_output_buffer_visible()
{
	GObject *image_output_buffer;
	GObject *image_input_buffer;

	image_input_buffer = gtk_builder_get_object (builder, "input-image-buffer");
	image_output_buffer = gtk_builder_get_object (builder, "output-image-buffer");

	gtk_widget_hide(GTK_WIDGET(image_input_buffer));
	gtk_widget_show(GTK_WIDGET(image_output_buffer));
}

void
make_input_buffer_visible()
{
	GObject *image_output_buffer;
	GObject *image_input_buffer;

	image_input_buffer = gtk_builder_get_object (builder, "input-image-buffer");
	image_output_buffer = gtk_builder_get_object (builder, "output-image-buffer");

	gtk_widget_show(GTK_WIDGET(image_input_buffer));
	gtk_widget_hide(GTK_WIDGET(image_output_buffer));
}

/* Fonction qui crée le tableau de guchar contenant les informations
  de l'image seuillée. */
void
seuillage ()
{
  // Obtenir la valeur du GtkScale
	GObject *input_image = gtk_builder_get_object ( builder, "input-image-buffer");
	GdkPixbuf *input_pixbuf = gtk_image_get_pixbuf (GTK_IMAGE(input_image));
	GObject *output_image = gtk_builder_get_object ( builder, "output-image-buffer");
	GdkPixbuf *output_pixbuf;
	GObject *scale = gtk_builder_get_object (builder, "seuil-bouton");
  int seuil_value = floor((double)gtk_range_get_value (GTK_RANGE(scale)));

  // initialiser et mettre à zéro le tableau de guchar
  guchar *pixel_array =(guchar *)malloc(sizeof(guchar)*IMAGE_BUFFER_HEIGHT*(IMAGE_BUFFER_WIDTH*3));

  // Parcourir chaque pixel du pixbuf et assigner sa
  // valeur en niveau de gris dans le tableau de résultat
  for (int line = 0; line < IMAGE_BUFFER_HEIGHT;line++) {
    for (int column = 0; column < IMAGE_BUFFER_WIDTH;column++) {
      Pixel *current_pixel = gotoPixel(input_pixbuf, column, line);

      if (greyLevel(current_pixel) >= seuil_value) {
        setGreyLevel((Pixel *) pixel_array + (line*IMAGE_BUFFER_HEIGHT) + column*3,
                      255);
      } else {
        setGreyLevel((Pixel *) pixel_array + (line*IMAGE_BUFFER_WIDTH) + column*3,
                      0);
      }

    }
  }

	output_pixbuf = gdk_pixbuf_new_from_data (	pixel_array,
																							GDK_COLORSPACE_RGB,
																							FALSE,
																							8,
																							IMAGE_BUFFER_WIDTH,
																							IMAGE_BUFFER_HEIGHT,
																							IMAGE_BUFFER_WIDTH,
																							NULL,
																							NULL);
  gtk_image_set_from_pixbuf (	GTK_IMAGE(output_image),
															output_pixbuf);
	gtk_widget_queue_draw(GTK_WIDGET(output_image));

}


/* Fonction qui met à jour le pixbuf input avec une image
	chargé depuis le système. */

void
update_image_buffer(char *filename)
{
	// We need to get GtkImage Object (with id: image)
	GObject *image;
	GdkPixbuf *pixbuf;

	image = gtk_builder_get_object (builder, "input-image-buffer");
	pixbuf = gdk_pixbuf_new_from_file_at_size (	filename,
																							IMAGE_BUFFER_WIDTH,
																							IMAGE_BUFFER_HEIGHT,
																							NULL);
	gtk_image_set_from_pixbuf(	GTK_IMAGE(image),
															pixbuf);

	gtk_widget_queue_draw(GTK_WIDGET(image));
}

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
	gint res; // This variable is for the result we obtain when we close the window (Either file chosen or closed)

	dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(window),
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      "_Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "_Open",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

res = gtk_dialog_run (GTK_DIALOG (dialog)); // We're running the dialog now

if (res == GTK_RESPONSE_ACCEPT) // If the user indeed provided us with a file...
  {
		char *filename; // We create a string for the path of the file

		/* We create a GtkFileChooser to get the path of the file.
				(This object is responsible of getting information from the dialog)*/

		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

		// We create the file chooser by giving him the dialog we create earlier
		filename = gtk_file_chooser_get_filename (chooser);

		// What we do there is simply opening the file
		// TODO : Test if the file is indeed a .png file
		//				and do not open it if it's the case.

		// We will need to put the image on the center of our window.

		update_image_buffer(filename);

    g_free (filename);
  }
	gtk_widget_destroy (dialog);
}

/* MAIN : Lance l'application et retourne le status d'execution de
 * l'application à la fermeture. */

int
main (	int 	argc,
	char 	**argv)
{
	/* Variables */
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

	gtk_builder_add_callback_symbol (	builder,
																	  "make_output_buffer_visible",
																		G_CALLBACK(make_output_buffer_visible));
	gtk_builder_add_callback_symbol (	builder,
																		"make_input_buffer_visible",
																		G_CALLBACK(make_input_buffer_visible));
	gtk_builder_add_callback_symbol (	builder,
																		"seuillage",
																		G_CALLBACK(seuillage));


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
