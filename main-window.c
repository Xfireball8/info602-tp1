/*INFO_601-TP
	Diletta Maniglia
	Faisal Salhi */

#include <gtk/gtk.h>
#include <math.h>
#include <assert.h>
#define WINDOW_HEIGHT	500
#define WINDOW_WIDTH	500
#define IMAGE_BUFFER_HEIGHT	400
#define IMAGE_BUFFER_WIDTH	400
#define RGB_MIN 0
#define RGB_MAX 255
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
    Utile pour vérifier que le GdkPixbuf a un formal usuel: 3 canaux RGB, 24 bits par pixel,
*/
void analyzePixbuf( GdkPixbuf* pixbuf )
{
   int n_channels      = gdk_pixbuf_get_n_channels( pixbuf );      // Nb de canaux (Rouge, Vert, Bleu, potentiellement Alpha)
  int has_alpha       = gdk_pixbuf_get_has_alpha( pixbuf );       // Dit s'il y a un canal Alpha (transparence).
  int bits_per_sample = gdk_pixbuf_get_bits_per_sample( pixbuf ); // Donne le nombre de bits par échantillon (8 bits souvent).
  guchar* data        = gdk_pixbuf_get_pixels( pixbuf );          // Pointeur vers le tampon de données
  int width           = gdk_pixbuf_get_width( pixbuf );           // Largeur de l'image en pixels
  int height          = gdk_pixbuf_get_height( pixbuf );          // Hauteur de l'image en pixels
  int rowstride       = gdk_pixbuf_get_rowstride( pixbuf );       // Nombre d'octets entre chaque ligne dans le tampon de données
  printf( "n_channels = %d\n", n_channels );
  printf( "has_alpha  = %d\n", has_alpha );
  printf( "bits_per_sa= %d\n", bits_per_sample );
  printf( "width      = %d\n", width );
  printf( "height     = %d\n", height );
  printf( "data       = %p\n", data );
   printf( "rowstride  = %d\n", rowstride );
  Pixel*  pixel = (Pixel*) data;
   printf( "sizeof(Pixel)=%ld\n", sizeof(Pixel) );
   size_t diff = ((guchar*) (pixel+1)) - (guchar*) pixel;
   printf( "(pixel+1) - pixel=%ld\n", diff );
  assert( n_channels == 3 );
  assert( has_alpha == FALSE );
  assert( bits_per_sample == 8 );
   assert( sizeof(Pixel) == 3 );
  assert( diff == 3 );
}

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


/**
 */
void setColor( Pixel* data, unsigned char r, unsigned char g, unsigned char b )
{
  data->rouge = r;
  data->vert  = g;
  data->bleu  = b;
}

/****************** Structure Union-Find ************/

/// Un Objet stocke donc un pointeur vers son pixel, son rang et un pointeur vers l'objet père.
typedef struct SObjet {
  Pixel* pixel; // adresse du pixel dans le pixbuf
  int rang;
  struct SObjet* pere;
} Objet;


Objet*
CreerEnsembles(GdkPixbuf *pixbuf)
{
	int height = gdk_pixbuf_get_height(pixbuf);
	int width = gdk_pixbuf_get_width(pixbuf);
	int size = height*width;
	Objet *ensemble = (Objet *)malloc(sizeof(Objet)*size);

	for (int line = 0; line < height; line++){
		for (int column = 0; column < width; column++){
			ensemble[(line*width)+column].pixel = gotoPixel(	pixbuf,
																												column,
																												line);
			ensemble[(line*width)+column].rang = 1;
			ensemble[(line*width)+column].pere = &ensemble[(line*height)+column];
		}
	}
	return ensemble;
}

// Retourne le représentant de l'objet obj
Objet*
TrouverEnsemble( Objet* obj )
{
	if (obj->pere != obj){
		obj->pere = TrouverEnsemble(obj->pere);
		return obj->pere;
	} else {
		return obj->pere;
	}
}

// Si obj1 et obj2 n'ont pas les mêmes représentants, appelle Lier sur leurs représentants
void
Lier( Objet* obj1, Objet* obj2 )
{

	Objet *rep1 = TrouverEnsemble(obj1);
	Objet *rep2 = TrouverEnsemble(obj2);

	if (rep1 != rep2){
		if (obj1->rang > obj2->rang){
			obj2->pere = obj1;
		} else {
			obj1->pere = obj2;
			if (obj1->rang == obj2->rang){
				obj2->rang +=1;
			}
		}
	}
}

// Si obj1 et obj2 sont tous deux des racines, et sont distincts, alors réalise l'union des deux arbres.
void
Union( Objet* obj1, Objet* obj2 )
{
	if (obj1->pere == obj1 && obj2->pere == obj2 && obj1 != obj2){
		Lier(obj1,obj2);
	}
}

/****************************************************/



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
	int height = gdk_pixbuf_get_height(input_pixbuf);
	int width = gdk_pixbuf_get_width(input_pixbuf);
	int rowstride =  gdk_pixbuf_get_rowstride( input_pixbuf );
	GObject *output_image = gtk_builder_get_object ( builder, "output-image-buffer");
	GdkPixbuf *output_pixbuf;
	GObject *scale = gtk_builder_get_object (builder, "scale");
  int seuil_value = (int)gtk_range_get_value (GTK_RANGE(scale));

  // initialiser et mettre à zéro le tableau de guchar
  guchar *pixel_array =(guchar *)malloc(sizeof(guchar)*IMAGE_BUFFER_HEIGHT*(IMAGE_BUFFER_WIDTH*3));

  // Parcourir chaque pixel du pixbuf et assigner sa
  // valeur en niveau de gris dans le tableau de résultat
  for (int line = 0; line < height;line++) {
    for (int column = 0; column < width;column++) {
      Pixel *current_pixel = gotoPixel(input_pixbuf, column, line);

      if (greyLevel(current_pixel) >= seuil_value) {
        setGreyLevel((Pixel *) (pixel_array + (line*rowstride) + column*3),
                      255);
      } else {
        setGreyLevel((Pixel *) (pixel_array + (line*rowstride) + column*3),
                      0);
      }

    }
  }

	output_pixbuf = gdk_pixbuf_new_from_data (	pixel_array,
																							GDK_COLORSPACE_RGB,
																							FALSE,
																							8,
																							width,
																							height,
																							rowstride,
																							NULL,
																							NULL);

  gtk_image_set_from_pixbuf (	GTK_IMAGE(output_image),
															output_pixbuf);
	gtk_widget_queue_draw(GTK_WIDGET(output_image));

}

void
composante_connexe() {
	seuillage();
	GObject *image = gtk_builder_get_object(builder, "output-image-buffer");
	GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
	int height = gdk_pixbuf_get_height(pixbuf);
	int width = gdk_pixbuf_get_width(pixbuf);
	int size = height*width;
	Objet *ensemble = CreerEnsembles(pixbuf);

	for (int i = 0; i < (size-1); i++){
		if (greyLevel(ensemble[i].pixel) == greyLevel(ensemble[i+1].pixel)){
			Union(&ensemble[i],&ensemble[i+1]);
		}

		if (((-i-width)+((size)-1)) > 0 &&
				greyLevel(ensemble[i].pixel) == greyLevel(ensemble[i+width].pixel))
				{
					Union(&ensemble[i],&ensemble[i+width]);
				}

	}


	for (int i = 0; i < height*width; i++){
		if (ensemble[i].pere == &ensemble[i]){
			 int r = RGB_MIN + (rand() % (int)(RGB_MAX - RGB_MIN + 1));
			 int g = RGB_MIN + (rand() % (int)(RGB_MAX - RGB_MIN + 1));
			 int b = RGB_MIN + (rand() % (int)(RGB_MAX - RGB_MIN + 1));
			 setColor(ensemble[i].pixel, r,g,b);
		}
	}


	for (int i = 0; i < height*width; i++){
		if (ensemble[i].pere != &ensemble[i]){
			 Pixel *pere = ensemble[i].pere->pixel;
			 setColor(ensemble[i].pixel, pere->rouge,pere->vert,pere->bleu);
		}
	}

	gtk_widget_queue_draw(GTK_WIDGET(image));

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
	gtk_builder_add_callback_symbol (	builder,
																		"composante_connexe",
																		G_CALLBACK(composante_connexe));
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
