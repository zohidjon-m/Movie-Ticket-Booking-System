#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 5
#define COLS 8
#define SLOTS 3
#define MAX_TICKETS 5000

struct Movie {
    int id;
    char title[60];
    char genre[30];
    int year;
    char country[30];
    int price;
};

// every ticket is one seat, so cancelling a seat just removes one ticket
struct Ticket {
    int id;
    char name[40];
    int movieId;
    int slot;
    int row;
    int col;
    int price;
};

char *slotTimes[SLOTS] = {"10:00", "14:00", "19:00"};

struct Movie *movies = NULL;
int movieCount = 0;

struct Ticket tickets[MAX_TICKETS];
int ticketCount = 0;
int nextTicketId = 1;

char seats[ROWS][COLS];


// ---------- reading input ----------

// throws away the rest of a line that was too long for the buffer
void flushLine() {
    int c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

// we read whole lines with fgets, so wrong input can't get stuck in the buffer
void getText(char *prompt, char *text, int size) {
    char line[200];
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nInput ended. Goodbye!\n");
            exit(0);
        }

        int len = strlen(line);
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
        } else {
            flushLine();
        }

        if (line[0] == '\0') {
            printf("This cannot be empty.\n");
            continue;
        }

        strncpy(text, line, size - 1);
        text[size - 1] = '\0';

        // '|' separates the fields in our files, so we can't allow it in text
        for (int i = 0; text[i] != '\0'; i++) {
            if (text[i] == '|') {
                text[i] = '/';
            }
        }
        return;
    }
}

int getNumber(char *prompt, int min, int max) {
    char text[50];
    while (1) {
        getText(prompt, text, sizeof(text));

        int onlyDigits = 1;
        for (int i = 0; text[i] != '\0'; i++) {
            if (text[i] < '0' || text[i] > '9') {
                onlyDigits = 0;
            }
        }

        // max 7 digits so atoi can't overflow
        if (onlyDigits && strlen(text) <= 7) {
            int n = atoi(text);
            if (n >= min && n <= max) {
                return n;
            }
        }
        printf("Please enter a number from %d to %d.\n", min, max);
    }
}

void makeLower(char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = s[i] + 32;
        }
    }
}

// search should work for "parasite" and "PARASITE", so compare lowercase copies
int contains(char *text, char *word) {
    char a[100], b[100];
    strcpy(a, text);
    strcpy(b, word);
    makeLower(a);
    makeLower(b);
    return strstr(a, b) != NULL;
}

int sameName(char *name1, char *name2) {
    char a[100], b[100];
    strcpy(a, name1);
    strcpy(b, name2);
    makeLower(a);
    makeLower(b);
    return strcmp(a, b) == 0;
}


// ---------- files ----------

void addToList(struct Movie m) {
    movies = realloc(movies, (movieCount + 1) * sizeof(struct Movie));
    if (movies == NULL) {
        printf("Out of memory.\n");
        exit(1);
    }
    movies[movieCount] = m;
    movieCount++;
}

void loadMovies() {
    FILE *f = fopen("movies.txt", "r");
    if (f == NULL) {
        return; // first run, no file yet
    }
    char line[300];
    struct Movie m;
    while (fgets(line, sizeof(line), f) != NULL) {
        int n = sscanf(line, "%d|%59[^|]|%29[^|]|%d|%29[^|]|%d",
                       &m.id, m.title, m.genre, &m.year, m.country, &m.price);
        if (n == 6) {
            addToList(m);
        }
    }
    fclose(f);
}

void saveMovies() {
    FILE *f = fopen("movies.txt", "w");
    if (f == NULL) {
        printf("Could not save movies.\n");
        return;
    }
    for (int i = 0; i < movieCount; i++) {
        struct Movie m = movies[i];
        fprintf(f, "%d|%s|%s|%d|%s|%d\n", m.id, m.title, m.genre, m.year, m.country, m.price);
    }
    fclose(f);
}

void loadTickets() {
    FILE *f = fopen("tickets.txt", "r");
    if (f == NULL) {
        return;
    }
    char line[300];
    struct Ticket t;
    while (fgets(line, sizeof(line), f) != NULL && ticketCount < MAX_TICKETS) {
        int n = sscanf(line, "%d|%39[^|]|%d|%d|%d|%d|%d",
                       &t.id, t.name, &t.movieId, &t.slot, &t.row, &t.col, &t.price);
        if (n != 7) {
            continue;
        }
        // skip bad lines so a broken file can't crash the seat map
        if (t.slot < 0 || t.slot >= SLOTS || t.row < 0 || t.row >= ROWS || t.col < 0 || t.col >= COLS) {
            continue;
        }
        tickets[ticketCount] = t;
        ticketCount++;
        if (t.id >= nextTicketId) {
            nextTicketId = t.id + 1;
        }
    }
    fclose(f);
}

void saveTickets() {
    FILE *f = fopen("tickets.txt", "w");
    if (f == NULL) {
        printf("Could not save tickets.\n");
        return;
    }
    for (int i = 0; i < ticketCount; i++) {
        struct Ticket t = tickets[i];
        fprintf(f, "%d|%s|%d|%d|%d|%d|%d\n", t.id, t.name, t.movieId, t.slot, t.row, t.col, t.price);
    }
    fclose(f);
}


// ---------- movies ----------

struct Movie *findMovie(int id) {
    for (int i = 0; i < movieCount; i++) {
        if (movies[i].id == id) {
            return &movies[i];
        }
    }
    return NULL;
}

void showHeader() {
    printf("\n%-4s %-30s %-18s %-5s %-12s %s\n", "ID", "Title", "Genre", "Year", "Country", "Price");
}

void showMovie(struct Movie *m) {
    printf("%-4d %-30s %-18s %-5d %-12s %d KRW\n", m->id, m->title, m->genre, m->year, m->country, m->price);
}

void showMovies() {
    if (movieCount == 0) {
        printf("No movies yet.\n");
        return;
    }
    showHeader();
    for (int i = 0; i < movieCount; i++) {
        showMovie(&movies[i]);
    }
}

void addMovies() {
    int n = getNumber("How many movies do you want to add (1-20)? ", 1, 20);
    for (int i = 0; i < n; i++) {
        struct Movie m;
        printf("\nMovie %d of %d\n", i + 1, n);

        // new id = biggest id so far + 1, so two movies never share an id
        m.id = 1;
        for (int j = 0; j < movieCount; j++) {
            if (movies[j].id >= m.id) {
                m.id = movies[j].id + 1;
            }
        }

        getText("Title: ", m.title, sizeof(m.title));
        getText("Genre: ", m.genre, sizeof(m.genre));
        m.year = getNumber("Year: ", 1888, 2100);
        getText("Country: ", m.country, sizeof(m.country));
        m.price = getNumber("Price in KRW: ", 1, 100000);

        addToList(m);
        saveMovies(); // saving after each movie so nothing is lost if we stop halfway
        printf("Movie added with ID %d.\n", m.id);
    }
}

void searchMovies() {
    char word[60];
    getText("Search by title or genre: ", word, sizeof(word));

    int found = 0;
    for (int i = 0; i < movieCount; i++) {
        struct Movie *m = &movies[i];
        if (contains(m->title, word) || contains(m->genre, word)) {
            if (found == 0) {
                showHeader();
            }
            showMovie(m);
            found++;
        }
    }
    if (found == 0) {
        printf("No movie found for \"%s\".\n", word);
    }
}


// ---------- seats and tickets ----------

// the seat map is rebuilt from the ticket list each time, so it always matches the file
void fillSeats(int movieId, int slot) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c] = '.';
        }
    }
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].movieId == movieId && tickets[i].slot == slot) {
            seats[tickets[i].row][tickets[i].col] = 'X';
        }
    }
}

int countFreeSeats() {
    int count = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (seats[r][c] == '.') {
                count++;
            }
        }
    }
    return count;
}

void showSeats() {
    printf("\n      SCREEN\n   ");
    for (int c = 0; c < COLS; c++) {
        printf("%d ", c + 1);
    }
    printf("\n");
    for (int r = 0; r < ROWS; r++) {
        printf(" %c ", 'A' + r);
        for (int c = 0; c < COLS; c++) {
            printf("%c ", seats[r][c]);
        }
        printf("\n");
    }
    printf("(. = free, X = taken)\n");
}

// returns row * COLS + col so we only have to give back one number
int askSeat() {
    char text[20];
    while (1) {
        getText("Seat (for example B3): ", text, sizeof(text));

        char letter = text[0];
        if (letter >= 'a' && letter <= 'z') {
            letter = letter - 32;
        }
        int row = letter - 'A';
        int col = atoi(text + 1) - 1;

        if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
            printf("Rows are A-%c and seats are 1-%d.\n", 'A' + ROWS - 1, COLS);
        } else if (seats[row][col] == 'X') {
            printf("That seat is already taken.\n");
        } else {
            return row * COLS + col;
        }
    }
}

void bookTickets() {
    if (movieCount == 0) {
        printf("No movies yet.\n");
        return;
    }

    showMovies();
    int id = getNumber("\nMovie ID: ", 1, 999999);
    struct Movie *m = findMovie(id);
    if (m == NULL) {
        printf("Movie not found.\n");
        return;
    }

    for (int s = 0; s < SLOTS; s++) {
        printf("%d. %s\n", s + 1, slotTimes[s]);
    }
    int slot = getNumber("Time slot: ", 1, SLOTS) - 1;

    fillSeats(m->id, slot);
    int freeCount = countFreeSeats();
    if (freeCount == 0) {
        printf("Sorry, this show is sold out.\n");
        return;
    }
    showSeats();

    char name[40];
    getText("Your name: ", name, sizeof(name));

    int max = 5;
    if (freeCount < max) {
        max = freeCount;
    }
    printf("You can book up to %d tickets.\n", max);
    int n = getNumber("How many tickets? ", 1, max);
    if (ticketCount + n > MAX_TICKETS) {
        printf("Ticket storage is full.\n");
        return;
    }

    int total = 0;
    for (int i = 0; i < n; i++) {
        int seat = askSeat();
        int row = seat / COLS;
        int col = seat % COLS;
        seats[row][col] = 'X'; // so the next seat in this booking can't be the same one

        struct Ticket *t = &tickets[ticketCount];
        t->id = nextTicketId;
        nextTicketId++;
        strcpy(t->name, name);
        t->movieId = m->id;
        t->slot = slot;
        t->row = row;
        t->col = col;
        t->price = m->price;
        ticketCount++;

        total = total + m->price;
        printf("Ticket #%d - seat %c%d\n", t->id, 'A' + row, col + 1);
    }
    saveTickets();
    printf("Booked %d ticket(s) for %s at %s. Total: %d KRW\n", n, m->title, slotTimes[slot], total);
}

void cancelTicket() {
    int id = getNumber("Ticket ID to cancel: ", 1, 999999);
    char name[40];
    // asking for the name stops people from cancelling someone else's ticket
    getText("Name on the ticket: ", name, sizeof(name));

    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].id == id) {
            if (!sameName(tickets[i].name, name)) {
                printf("That name does not match this ticket.\n");
                return;
            }
            int price = tickets[i].price;
            for (int j = i; j < ticketCount - 1; j++) {
                tickets[j] = tickets[j + 1];
            }
            ticketCount--;
            saveTickets();
            printf("Ticket #%d cancelled. Refund: %d KRW\n", id, price);
            return;
        }
    }
    printf("Ticket not found.\n");
}

void showTickets() {
    if (ticketCount == 0) {
        printf("No tickets booked yet.\n");
        return;
    }
    printf("\n%-5s %-20s %-30s %-6s %-5s %s\n", "ID", "Name", "Movie", "Time", "Seat", "Price");
    for (int i = 0; i < ticketCount; i++) {
        struct Ticket t = tickets[i];
        struct Movie *m = findMovie(t.movieId);
        char *title = "?";
        if (m != NULL) {
            title = m->title;
        }
        printf("%-5d %-20s %-30s %-6s %c%-4d %d KRW\n",
               t.id, t.name, title, slotTimes[t.slot], 'A' + t.row, t.col + 1, t.price);
    }
}


// ---------- revenue and report ----------

int totalRevenue() {
    int sum = 0;
    for (int i = 0; i < ticketCount; i++) {
        sum = sum + tickets[i].price;
    }
    return sum;
}

// takes a FILE* so the same code can print to the screen and write the report file
void printReport(FILE *out) {
    fprintf(out, "=== Sales Report ===\n");
    fprintf(out, "Tickets sold: %d\n", ticketCount);
    fprintf(out, "Total revenue: %d KRW\n\n", totalRevenue());

    fprintf(out, "%-30s %8s %12s\n", "Movie", "Tickets", "Revenue");
    for (int i = 0; i < movieCount; i++) {
        int count = 0;
        int sum = 0;
        for (int j = 0; j < ticketCount; j++) {
            if (tickets[j].movieId == movies[i].id) {
                count++;
                sum = sum + tickets[j].price;
            }
        }
        fprintf(out, "%-30s %8d %12d\n", movies[i].title, count, sum);
    }

    fprintf(out, "\n%-8s %8s\n", "Time", "Tickets");
    for (int s = 0; s < SLOTS; s++) {
        int count = 0;
        for (int j = 0; j < ticketCount; j++) {
            if (tickets[j].slot == s) {
                count++;
            }
        }
        fprintf(out, "%-8s %8d\n", slotTimes[s], count);
    }
}

void generateReport() {
    printf("\n");
    printReport(stdout);

    FILE *f = fopen("sales_report.txt", "w");
    if (f == NULL) {
        printf("Could not write the report file.\n");
        return;
    }
    printReport(f);
    fclose(f);
    printf("\nReport saved to sales_report.txt\n");
}


// ---------- menus ----------

void adminMenu() {
    char username[30], password[30];
    getText("Username: ", username, sizeof(username));
    getText("Password: ", password, sizeof(password));
    if (strcmp(username, "admin") != 0 || strcmp(password, "admin123") != 0) {
        printf("Wrong username or password.\n");
        return;
    }

    int choice = -1;
    while (choice != 0) {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add movies\n");
        printf("2. Display movies\n");
        printf("3. Display booked tickets\n");
        printf("4. Calculate total revenue\n");
        printf("5. Generate sales report\n");
        printf("0. Back\n");
        choice = getNumber("Choice: ", 0, 5);

        switch (choice) {
            case 1: addMovies(); break;
            case 2: showMovies(); break;
            case 3: showTickets(); break;
            case 4: printf("Total revenue: %d KRW\n", totalRevenue()); break;
            case 5: generateReport(); break;
        }
    }
}

void userMenu() {
    int choice = -1;
    while (choice != 0) {
        printf("\n--- User Menu ---\n");
        printf("1. Display available movies\n");
        printf("2. Search for a movie\n");
        printf("3. Book tickets\n");
        printf("4. Cancel a ticket\n");
        printf("0. Back\n");
        choice = getNumber("Choice: ", 0, 4);

        switch (choice) {
            case 1: showMovies(); break;
            case 2: searchMovies(); break;
            case 3: bookTickets(); break;
            case 4: cancelTicket(); break;
        }
    }
}

int main() {
    loadMovies();
    loadTickets();

    int choice = -1;
    while (choice != 0) {
        printf("\n=== Movie Ticket Booking System ===\n");
        printf("1. Admin\n");
        printf("2. User\n");
        printf("0. Exit\n");
        choice = getNumber("Choice: ", 0, 2);

        if (choice == 1) {
            adminMenu();
        } else if (choice == 2) {
            userMenu();
        }
    }

    printf("Goodbye!\n");
    free(movies);
    return 0;
}
