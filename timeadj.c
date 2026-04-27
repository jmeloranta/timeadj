/*
 * System clock adjustment tool. 
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <signal.h>

double cur_offset = 0.0;
GtkWidget *adj, *button_auto;
int ntp_enabled = 1;

void time_down(GtkWidget *win, gpointer data) {

  char buf[128];

  cur_offset -= 0.1;
  sprintf(buf, "%s -0.1", ADJTIMEX);
  system(buf);
  sprintf(buf, "%.1lf s", cur_offset);
  gtk_label_set_text(GTK_LABEL(adj), buf);
}

void time_up(GtkWidget *win, gpointer data) {

  char buf[64];

  cur_offset += 0.1;
  sprintf(buf, "%s 0.1", ADJTIMEX);
  system(buf);
  sprintf(buf, "%.1lf s", cur_offset);
  gtk_label_set_text(GTK_LABEL(adj), buf);
}

void time_auto(GtkWidget *win, gpointer data) {

  char buf[128];

  if(ntp_enabled) {    
    ntp_enabled = 0;
    gtk_button_set_label(GTK_BUTTON(button_auto), "AUTO ON");
    sprintf(buf, "%s f", ADJTIMEX);
    system(buf);
  } else {
    system("timedatectl set-ntp true"); // re-enable automatic time adjustment
    ntp_enabled = 1;
    gtk_button_set_label(GTK_BUTTON(button_auto), "AUTO OFF");
    sprintf(buf, "%s %.2lf", ADJTIMEX, -cur_offset);
    system(buf);
    cur_offset = 0.0;
    sprintf(buf, "%.1lf s", cur_offset);
    gtk_label_set_text(GTK_LABEL(adj), buf);
    sprintf(buf, "%s o", ADJTIMEX);
    system(buf);
  }
}

void cleanup(int x) {

  char buf[128];

  sprintf(buf, "%s %.2lf", ADJTIMEX, -cur_offset); // return to original offset
  system(buf);
  sprintf(buf, "%s o", ADJTIMEX); // enable NTP
  system(buf);
  exit(0);
}  

int main(int argc, char *argv[]) {

  GtkWidget *window, *hbox1, *hbox2, *vbox;
  GtkWidget *adj_label, *button_down, *button_up;
  
  signal(SIGINT, cleanup);
  gtk_init();
  window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window), "Time adjustment");
  gtk_window_set_default_size(GTK_WINDOW(window), 100, 80);

  adj_label = gtk_label_new("Current time offset: ");
  gtk_label_set_xalign(GTK_LABEL(adj_label), 0.0); // 0 = left, 1 = right
  adj = gtk_label_new("0.0 s");
  gtk_label_set_xalign(GTK_LABEL(adj), 0.0); // 0 = left, 1 = right
  
  hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

  button_down = gtk_button_new_with_label("Down 0.1 s");
  button_up = gtk_button_new_with_label("Up   0.1 s");
  button_auto = gtk_button_new_with_label("Auto OFF");

  gtk_box_append(GTK_BOX(hbox1), adj_label);
  gtk_box_append(GTK_BOX(hbox1), adj);
  
  gtk_box_append(GTK_BOX(hbox2), button_down);
  gtk_box_append(GTK_BOX(hbox2), button_up);
  gtk_box_append(GTK_BOX(hbox2), button_auto);

 
  gtk_box_append(GTK_BOX(vbox), hbox1);
  gtk_box_append(GTK_BOX(vbox), hbox2);

  gtk_window_set_child(GTK_WINDOW(window), vbox);

  g_signal_connect(button_down, "clicked", G_CALLBACK(time_down), NULL);
  g_signal_connect(button_up, "clicked", G_CALLBACK(time_up), NULL);
  g_signal_connect(button_auto, "clicked", G_CALLBACK(time_auto), NULL);

  gtk_widget_set_visible(window, TRUE);

  while (g_list_model_get_n_items(gtk_window_get_toplevels()) > 0) // Main iteration loop
    g_main_context_iteration(NULL, TRUE);

  cleanup(1);
}
