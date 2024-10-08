#ifdef VERSION0
#include<unistd.h>
#include <gtk/gtk.h>

// Callback function to change the label text
static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  GtkLabel *label = GTK_LABEL(data);

    sleep(2);
    gtk_label_set_text(label, "1");
    sleep(2);
    gtk_label_set_text(label, "1, 2");
    sleep(2);
    gtk_label_set_text(label, "1, 2, 3");

}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *box;
  GtkWidget *label;

  // Create a window
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Hello");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  // Create a box container
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_window_set_child(GTK_WINDOW (window), box);

  // Create a label
  label = gtk_label_new("Press the button");

  // Create a button
  button = gtk_button_new_with_label ("Click Me");

  // Connect button click signal to callback function
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), label);

  // Add the label and button to the box
  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), button);

  // Show the window
  gtk_window_present (GTK_WINDOW (window));
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
#endif

#include <gtk/gtk.h>

// Helper function to force the UI to refresh using g_main_context_pending
static void force_ui_update() {
    GMainContext *context = g_main_context_default();
    while (g_main_context_pending(context)) {
        g_main_context_iteration(context, FALSE);
    }
}

// Callback function to render multiple messages
static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  GtkLabel *label = GTK_LABEL(data);

  // Set first message and force the UI to update
  gtk_label_set_text(label, "Message 1: Hello World!");
  force_ui_update();

  // Simulate some work with a sleep
  g_usleep(1000000);  // 1 second

  // Set second message and force the UI to update
  gtk_label_set_text(label, "Message 2: Still Working...");
  force_ui_update();

  // Simulate more work
  g_usleep(1000000);  // 1 second

  // Set final message and force the UI to update
  gtk_label_set_text(label, "Message 3: Done!");
  force_ui_update();
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *box;
  GtkWidget *label;

  // Create a window
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Hello");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  // Create a box container
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_window_set_child(GTK_WINDOW (window), box);

  // Create a label
  label = gtk_label_new("Press the button");

  // Create a button
  button = gtk_button_new_with_label ("Click Me");

  // Connect button click signal to callback function
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), label);

  // Add the label and button to the box
  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), button);

  // Show the window
  gtk_window_present (GTK_WINDOW (window));
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
