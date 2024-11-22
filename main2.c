#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MOVIES 200
#define MAX_USERS 100

// Structure for a Movie
struct Movie {
    int id;
    char title[100];
    char genre[50];
    int year;
    char country[50];
    int price;
};

// Structure for a User
struct User {
    char name[100];
    int numTickets;
    char favAnimal[50];
};

// Function Declarations
void adminMenu();
void userMenu();
void addMovies();
void displayMovies();
void bookTicket();
void displayBookedTickets();
void calculateRevenue();
int validateInput(const char *prompt, int min, int max);

// File Names
const char *MOVIE_FILE = "movies_database.txt";
const char *BOOKED_FILE = "booked_tickets.txt";

// Main Function
int main() {
    int choice;
    printf("Welcome to Zohidjon's Cinema!\n");
    printf("1. Admin Login\n");
    printf("2. User Menu\n");
    printf("Enter your choice: ");
    choice = validateInput("Enter your choice (1-2): ", 1, 2);

    switch (choice) {
        case 1:
            adminMenu();
            break;
        case 2:
            userMenu();
            break;
        default:
            printf("Invalid choice. Exiting...\n");
            exit(0);
    }
    return 0;
}

// Admin Menu
void adminMenu() {
    char username[50], password[50];
    printf("\nAdmin Login:\n");
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    if (strcmp(username, "zohidjon") == 0 && strcmp(password, "Zoh!dj0n23") == 0) {
        printf("\nWelcome, Admin!\n");
        int adminChoice;
        do {
            printf("\nAdmin Menu:\n");
            printf("1. Add Movies\n");
            printf("2. Display Movies\n");
            printf("3. Display Booked Tickets\n");
            printf("4. Calculate Revenue\n");
            printf("0. Exit\n");
            adminChoice = validateInput("Enter your choice (0-4): ", 0, 4);

            switch (adminChoice) {
                case 1:
                    addMovies();
                    break;
                case 2:
                    displayMovies();
                    break;
                case 3:
                    displayBookedTickets();
                    break;
                case 4:
                    calculateRevenue();
                    break;
                case 0:
                    printf("Exiting Admin Menu...\n");
                    break;
                default:
                    printf("Invalid choice!\n");
            }
        } while (adminChoice != 0);
    } else {
        printf("Invalid credentials. Returning to main menu...\n");
    }
}

// User Menu
void userMenu() {
    int userChoice;
    do {
        printf("\nUser Menu:\n");
        printf("1. Display Available Movies\n");
        printf("2. Book a Ticket\n");
        printf("3. Exit\n");
        userChoice = validateInput("Enter your choice (1-3): ", 1, 3);

        switch (userChoice) {
            case 1:
                displayMovies();
                break;
            case 2:
                bookTicket();
                break;
            case 3:
                printf("Exiting User Menu...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (userChoice != 3);
}

// Function to add movies
void addMovies() {
    int n;
    printf("How many movies do you want to add? ");
    n = validateInput("Enter the number of movies (1-200): ", 1, MAX_MOVIES);

    FILE *file = fopen(MOVIE_FILE, "ab");
    if (!file) {
        perror("Error opening movies database");
        return;
    }

    struct Movie movie;
    for (int i = 0; i < n; i++) {
        printf("\nEnter details for movie %d:\n", i + 1);
        printf("ID: ");
        movie.id = validateInput("Enter a valid ID: ", 1, 9999);
        printf("Title: ");
        scanf(" %[^\n]", movie.title);
        printf("Genre: ");
        scanf(" %[^\n]", movie.genre);
        printf("Year: ");
        movie.year = validateInput("Enter a valid year: ", 1900, 2100);
        printf("Country: ");
        scanf(" %[^\n]", movie.country);
        printf("Price: ");
        movie.price = validateInput("Enter a valid price: ", 1, 10000);

        fwrite(&movie, sizeof(movie), 1, file);
        printf("Movie added successfully!\n");
    }
    fclose(file);
}

// Function to display all movies
void displayMovies() {
    FILE *file = fopen(MOVIE_FILE, "rb");
    if (!file) {
        printf("No movies found in the database.\n");
        return;
    }

    struct Movie movie;
    printf("\nAvailable Movies:\n");
    while (fread(&movie, sizeof(movie), 1, file)) {
        printf("ID: %d | Title: %s | Genre: %s | Year: %d | Country: %s | Price: %d\n",
               movie.id, movie.title, movie.genre, movie.year, movie.country, movie.price);
    }
    fclose(file);
}

// Function to book a ticket
void bookTicket() {
    int movieID;
    printf("Enter the ID of the movie you want to book: ");
    scanf("%d", &movieID);

    FILE *movieFile = fopen(MOVIE_FILE, "rb");
    FILE *bookedFile = fopen(BOOKED_FILE, "ab");
    if (!movieFile || !bookedFile) {
        perror("Error accessing files");
        return;
    }

    struct Movie movie;
    int found = 0;
    while (fread(&movie, sizeof(movie), 1, movieFile)) {
        if (movie.id == movieID) {
            found = 1;
            printf("You selected:\n");
            printf("ID: %d | Title: %s | Genre: %s | Year: %d | Country: %s | Price: %d\n",
                   movie.id, movie.title, movie.genre, movie.year, movie.country, movie.price);
            break;
        }
    }

    if (!found) {
        printf("Movie not found!\n");
        fclose(movieFile);
        fclose(bookedFile);
        return;
    }

    struct User user;
    printf("Enter your name: ");
    scanf(" %[^\n]", user.name);
    printf("How many tickets do you want? ");
    user.numTickets = validateInput("Enter a valid number of tickets: ", 1, 10);
    printf("What is your favorite animal? ");
    scanf(" %[^\n]", user.favAnimal);

    fwrite(&user, sizeof(user), 1, bookedFile);
    printf("Tickets booked successfully!\n");

    fclose(movieFile);
    fclose(bookedFile);
}

// Function to display booked tickets
void displayBookedTickets() {
    FILE *file = fopen(BOOKED_FILE, "rb");
    if (!file) {
        printf("No tickets have been booked yet.\n");
        return;
    }

    struct User user;
    printf("\nBooked Tickets:\n");
    while (fread(&user, sizeof(user), 1, file)) {
        printf("Name: %s | Tickets: %d | Favorite Animal: %s\n",
               user.name, user.numTickets, user.favAnimal);
    }
    fclose(file);
}

// Function to calculate total revenue
void calculateRevenue() {
    FILE *file = fopen(BOOKED_FILE, "rb");
    if (!file) {
        printf("No tickets have been booked yet.\n");
        return;
    }

    struct User user;
    int totalRevenue = 0;
    while (fread(&user, sizeof(user), 1, file)) {
        totalRevenue += user.numTickets; // Assuming ticket price is fixed or stored separately
    }

    printf("Total revenue from ticket sales: %d\n", totalRevenue);
    fclose(file);
}

// Function to validate input
int validateInput(const char *prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
            return value;
        }
        printf("Invalid input. Try again.\n");
        while (getchar() != '\n'); // Clear input buffer
    }
}
