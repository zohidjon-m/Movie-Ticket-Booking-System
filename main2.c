#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_MOVIES 200

// Structure for a movie
struct Movie {
    int id;
    char title[100];
    char genre[100];
    int year;
    char country[100];
    int price;
};

// Structure for a user booking
struct User {
    char name[100];
    int numTickets;
    char favAnimal[100];
};

// Function to save movies to the database file
int saveMoviesToFile(int n) {
    FILE *savedMovies = fopen("movies_database.txt", "ab");
    if (savedMovies == NULL) {
        printf("Error opening file for saving movies.\n");
        exit(1);
    }

    struct Movie movies[MAX_MOVIES];
    for (int i = 0; i < n; i++) {
        printf("\nEnter movie details:\n");
        printf("ID of the Movie: ");
        scanf("%d", &movies[i].id);
        printf("Title: ");
        scanf("%s", movies[i].title);
        printf("Genre: ");
        scanf("%s", movies[i].genre);
        printf("Year: ");
        scanf("%d", &movies[i].year);
        printf("Country: ");
        scanf("%s", movies[i].country);
        printf("Price: ");
        scanf("%d", &movies[i].price);
    }
    
    fwrite(movies, sizeof(struct Movie), n, savedMovies);
    fclose(savedMovies);
    return n;
}

// Function to display saved movies from the file
int displaySavedMoviesFromFile() {
    FILE *savedMovies = fopen("movies_database.txt", "rb");
    if (savedMovies == NULL) {
        printf("No previous data found.\n");
        return 0;
    }

    struct Movie movies[MAX_MOVIES];
    int i = 0;
    while (fread(&movies[i], sizeof(struct Movie), 1, savedMovies) != 0 && i < MAX_MOVIES) {
        i++;
    }
    fclose(savedMovies);

    for (int j = 0; j < i; ++j) {
        printf("Id: %d | Title: %s | Genre: %s | Year: %d | Country: %s | Price: %d\n",
               movies[j].id, movies[j].title, movies[j].genre, movies[j].year,
               movies[j].country, movies[j].price);
    }
    return i;
}

// Function to book a ticket for a movie
int bookingTicket(int idMovie) {
    FILE *savedMovies = fopen("movies_database.txt", "rb");
    if (savedMovies == NULL) {
        printf("No previous data found for booking!\n");
        return 0;
    }

    struct Movie movies[MAX_MOVIES];
    int i = 0;
    while (fread(&movies[i], sizeof(struct Movie), 1, savedMovies) != 0 && i < MAX_MOVIES) {
        i++;
    }
    fclose(savedMovies);

    for (int j = 0; j < i; j++) {
        if (movies[j].id == idMovie) {
            printf("Movie Found:\n");
            printf("Id: %d | Title: %s | Genre: %s | Year: %d | Country: %s | Price: %d\n",
                   movies[j].id, movies[j].title, movies[j].genre, movies[j].year,
                   movies[j].country, movies[j].price);

            int bookMovie;
            printf("Enter 1 to book this movie: ");
            scanf("%d", &bookMovie);
            if (bookMovie == 1) {
                FILE *bookedTickets = fopen("booked_tickets.txt", "ab");
                if (bookedTickets == NULL) {
                    printf("Error opening file for booking tickets.\n");
                    exit(1);
                }

                struct User user;
                printf("Please, enter your name: ");
                scanf("%s", user.name);
                printf("How many tickets? ");
                scanf("%d", &user.numTickets);
                printf("Favourite animal: ");
                scanf("%s", user.favAnimal);

                fwrite(&user, sizeof(struct User), 1, bookedTickets);
                fclose(bookedTickets);
                printf("Booking successful!\n");
            } else {
                printf("Booking canceled.\n");
            }
            return 1;
        }
    }
    printf("Movie not found.\n");
    return 0;
}

int main() {
    int first_menu, main_menu;

    do {
        printf("\nWelcome to Zohidjon's Cinema!\n");
        printf("Enter 1 to enter the system: ");
        scanf("%d", &first_menu);
    } while (first_menu != 1);

    printf("\nMain Menu\n1. Admin\n2. User\nEnter: ");
    scanf("%d", &main_menu);

    if (main_menu == 1) {
        // Admin
        char admin_name[20], admin_password[20];
        printf("\nUser Name: ");
        scanf("%s", admin_name);
        printf("\nPassword: ");
        scanf("%s", admin_password);

        if ((strcmp(admin_name, "zohidjon") == 0) && (strcmp(admin_password, "Zoh!dj0n23") == 0)) {
            int admin_menu;
            printf("Admin Menu\n1. Add Movies\n2. Display Booked Tickets\nEnter: ");
            scanf("%d", &admin_menu);

            if (admin_menu == 1) {
                int n;
                printf("How many movies to add? ");
                scanf("%d", &n);
                saveMoviesToFile(n);
                displaySavedMoviesFromFile();
            }
        } else {
            printf("Incorrect credentials!\n");
        }
    } else if (main_menu == 2) {
        // User
        int userMenu;
        printf("User Menu\n1. Display Movies\n3. Book Ticket\nEnter: ");
        scanf("%d", &userMenu);

        if (userMenu == 1) {
            displaySavedMoviesFromFile();
        } else if (userMenu == 3) {
            int idMovie;
            printf("Enter Movie ID to book: ");
            scanf("%d", &idMovie);
            bookingTicket(idMovie);
        }
    } else {
        printf("Invalid choice!\n");
    }
    return 0;
}
