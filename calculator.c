#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gtk/gtk.h>

GtkWidget *label;
char buffer[50];

double evaluate_expression(const char *expression) {
    double stack[100];
    int sp = 0;
    char op_stack[100];
    int op_sp = 0;
    const char *p = expression;
    double num = 0;
    

    while (*p != '\0') {
        
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (isdigit(*p) || *p == '.') {
            num = strtod(p, (char **)&p);
            stack[sp++] = num;
        } else if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
            while (op_sp > 0 && (op_stack[op_sp - 1] == '*' || op_stack[op_sp - 1] == '/')) {
                char op = op_stack[--op_sp];
                double b = stack[--sp];
                double a = stack[--sp];
                double result;
                switch(op) {
                    case '+':
                        result = a + b;
                        break;
                    case '-':
                        result = a - b;
                        break;
                    case '*':
                        result = a * b;
                        break;
                    case '/':
                        if (b == 0) {
                            return NAN;
                        }
                        result = a / b;
                        break;
                }
                stack[sp++] = result;
            }
            op_stack[op_sp++] = *p;
            p++;
        } else {
            return NAN;
        }
    }
    while (op_sp > 0) {
        char op = op_stack[--op_sp];
        double b = stack[--sp];
        double a = stack[--sp];
        double result;
        switch(op) {
            case '+':
                result = a + b;
                break;
            case '-':
                result = a - b;
                break;
            case '*':
                result = a * b;
                break;
            case '/':
                if (b == 0) {
                    return NAN;
                }
                result = a / b;
                break;
        }
        stack[sp++] = result;
    }

    return stack[--sp];
}

void on_button_clicked(GtkWidget *widget, gpointer data) {
    const char *text = gtk_button_get_label(GTK_BUTTON(widget));
    if (strcmp(text, "C") == 0) {
        buffer[0] = '\0';
    } else if (strcmp(text, "=") == 0) {
        double result = evaluate_expression(buffer);
        if (isnan(result)) {
            sprintf(buffer, "Error");
        } else {
            sprintf(buffer, "%g", result);
        }
    } else {
        strcat(buffer, text);
    }
    gtk_label_set_text(GTK_LABEL(label), buffer);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    label = gtk_label_new("0");
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_vexpand(label, TRUE);
    GtkCssProvider *provider = gtk_css_provider_new();
    GtkStyleContext *context = gtk_widget_get_style_context(label);
    gtk_css_provider_load_from_data(provider, "label { font-family: sans-serif; font-size: 20px; }", -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 4, 1);

    const char *buttons[] = {"7", "8", "9", "/", "4", "5", "6", "*", "1", "2", "3", "-", "0", "C", "=", "+"};
    int row = 1, col = 0;

    for (int i = 0; i < 16; i++) {
        button = gtk_button_new_with_label(buttons[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
        gtk_widget_set_hexpand(button, TRUE);
        gtk_widget_set_vexpand(button, TRUE);
        col++;
        if (col > 3) {
            col = 0;
            row++;
        }
    }

    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}