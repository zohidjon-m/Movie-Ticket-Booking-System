#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ROWS 5
#define COLS 8
#define SLOTS 3
#define MAX_PER_BOOKING 5
#define ADMIN_USER "admin"
#define ADMIN_PASS "admin123"
#define MOVIES_FILE "movies.txt"
#define TICKETS_FILE "tickets.txt"
#define REPORT_FILE "sales_report.txt"

typedef struct {
    int id;
    char title[64];
    char genre[32];
    int year;
    char country[32];
    int price;
} Movie;

// One ticket = one seat, so cancelling a seat is just removing a record
typedef struct {
    int id;
    char name[48];
    int movieId;
    int slot;
    int row;
    int col;
    int price;
} Ticket;

static const char *SLOT_TIMES[SLOTS] = {"10:00", "14:00", "19:00"};

static Movie *movies;
static int movieCount, nextMovieId = 1;
static Ticket *tickets;
static int ticketCount, nextTicketId = 1;

/* ---------- input helpers ---------- */

// fgets instead of scanf: a bad line can never leave junk in the buffer
static void readLine(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (!fgets(buf, (int)size, stdin)) {
        printf("\nInput closed. Goodbye!\n");
        exit(0);
    }
    size_t len = strcspn(buf, "\n");
    if (buf[len] == '\0') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {} // drop the overlong rest
    }
    buf[len] = '\0';
}

// '|' is the file separator, so it can't be stored in text
static void readText(const char *prompt, char *buf, size_t size) {
    for (;;) {
        readLine(prompt, buf, size);
        if (buf[0] != '\0') break;
        printf("Cannot be empty.\n");
    }
    for (char *p = buf; *p; p++)
        if (*p == '|') *p = '/';
}

static int readInt(const char *prompt, int min, int max) {
    char line[32];
    for (;;) {
        readLine(prompt, line, sizeof line);
        char *end;
        long v = strtol(line, &end, 10);
        if (end != line && *end == '\0' && v >= min && v <= max) return (int)v;
        printf("Enter a number from %d to %d.\n", min, max);
    }
}

static void *grow(void *p, size_t elemSize, int count) {
    void *q = realloc(p, elemSize * (size_t)count);
    if (!q) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return q;
}

// Walks both strings with pointers, case-insensitive substring match
static int containsIgnoreCase(const char *text, const char *key) {
    for (; *text; text++) {
        const char *t = text, *k = key;
        while (*k && tolower((unsigned char)*t) == tolower((unsigned char)*k)) {
            t++;
            k++;
        }
        if (!*k) return 1;
    }
    return 0;
}

static int sameText(const char *a, const char *b) {
    return strlen(a) == strlen(b) && containsIgnoreCase(a, b);
}

/* ---------- file storage ---------- */

static void saveMovies(void) {
    FILE *f = fopen(MOVIES_FILE, "w");
    if (!f) {
        perror("Cannot save movies");
        return;
    }
    for (int i = 0; i < movieCount; i++) {
        Movie *m = &movies[i];
        fprintf(f, "%d|%s|%s|%d|%s|%d\n", m->id, m->title, m->genre, m->year, m->country, m->price);
    }
    fclose(f);
}

static void saveTickets(void) {
    FILE *f = fopen(TICKETS_FILE, "w");
    if (!f) {
        perror("Cannot save tickets");
        return;
    }
    for (int i = 0; i < ticketCount; i++) {
        Ticket *t = &tickets[i];
        fprintf(f, "%d|%s|%d|%d|%d|%d|%d\n", t->id, t->name, t->movieId, t->slot, t->row, t->col, t->price);
    }
    fclose(f);
}

static void loadMovies(void) {
    FILE *f = fopen(MOVIES_FILE, "r");
    if (!f) return; // first run: no file yet
    char line[256];
    Movie m;
    while (fgets(line, sizeof line, f)) {
        if (sscanf(line, "%d|%63[^|]|%31[^|]|%d|%31[^|]|%d",
                   &m.id, m.title, m.genre, &m.year, m.country, &m.price) != 6)
            continue;
        movies = grow(movies, sizeof *movies, movieCount + 1);
        movies[movieCount++] = m;
        if (m.id >= nextMovieId) nextMovieId = m.id + 1;
    }
    fclose(f);
}

static void loadTickets(void) {
    FILE *f = fopen(TICKETS_FILE, "r");
    if (!f) return;
    char line[256];
    Ticket t;
    while (fgets(line, sizeof line, f)) {
        if (sscanf(line, "%d|%47[^|]|%d|%d|%d|%d|%d",
                   &t.id, t.name, &t.movieId, &t.slot, &t.row, &t.col, &t.price) != 7)
            continue;
        // Bounds are checked here so a hand-edited file can't index outside the seat map
        if (t.slot < 0 || t.slot >= SLOTS || t.row < 0 || t.row >= ROWS || t.col < 0 || t.col >= COLS)
            continue;
        tickets = grow(tickets, sizeof *tickets, ticketCount + 1);
        tickets[ticketCount++] = t;
        if (t.id >= nextTicketId) nextTicketId = t.id + 1;
    }
    fclose(f);
}

/* ---------- movies ---------- */

static Movie *findMovie(int id) {
    for (int i = 0; i < movieCount; i++)
        if (movies[i].id == id) return &movies[i];
    return NULL;
}

static void printMovieHeader(void) {
    printf("\n%-4s %-30s %-18s %-5s %-12s %s\n", "ID", "Title", "Genre", "Year", "Country", "Price");
}

static void printMovie(const Movie *m) {
    printf("%-4d %-30s %-18s %-5d %-12s %d KRW\n", m->id, m->title, m->genre, m->year, m->country, m->price);
}

static void displayMovies(void) {
    if (movieCount == 0) {
        printf("No movies available yet.\n");
        return;
    }
    printMovieHeader();
    for (int i = 0; i < movieCount; i++) printMovie(&movies[i]);
}

static void addMovies(void) {
    int n = readInt("How many movies to add (1-20)? ", 1, 20);
    for (int i = 0; i < n; i++) {
        Movie m;
        printf("\nMovie %d of %d\n", i + 1, n);
        m.id = nextMovieId++; // auto IDs rule out duplicates
        readText("Title: ", m.title, sizeof m.title);
        readText("Genre: ", m.genre, sizeof m.genre);
        m.year = readInt("Year: ", 1888, 2100);
        readText("Country: ", m.country, sizeof m.country);
        m.price = readInt("Price (KRW): ", 1, 1000000);

        movies = grow(movies, sizeof *movies, movieCount + 1);
        movies[movieCount++] = m;
        saveMovies(); // saved per movie so nothing is lost if input stops midway
        printf("Added with ID %d.\n", m.id);
    }
}

static void searchMovies(void) {
    char key[64];
    readText("Search by title or genre: ", key, sizeof key);
    int found = 0;
    for (const Movie *m = movies; m < movies + movieCount; m++) {
        if (containsIgnoreCase(m->title, key) || containsIgnoreCase(m->genre, key)) {
            if (!found) printMovieHeader();
            printMovie(m);
            found++;
        }
    }
    if (!found) printf("No movie matches \"%s\".\n", key);
}

/* ---------- seats and tickets ---------- */

// The seat map is rebuilt from the ticket list, so it can never disagree with the file
static void buildSeatMap(int movieId, int slot, char map[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) map[r][c] = '.';
    for (int i = 0; i < ticketCount; i++)
        if (tickets[i].movieId == movieId && tickets[i].slot == slot)
            map[tickets[i].row][tickets[i].col] = 'X';
}

static int countFreeSeats(char map[ROWS][COLS]) {
    int free = 0;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (map[r][c] == '.') free++;
    return free;
}

static void printSeatMap(char map[ROWS][COLS]) {
    printf("\n     SCREEN\n   ");
    for (int c = 0; c < COLS; c++) printf("%d ", c + 1);
    printf("\n");
    for (int r = 0; r < ROWS; r++) {
        printf(" %c ", 'A' + r);
        for (int c = 0; c < COLS; c++) printf("%c ", map[r][c]);
        printf("\n");
    }
    printf("(. free, X taken)\n");
}

static void readSeat(char map[ROWS][COLS], int *row, int *col) {
    char line[16], letter;
    int num;
    for (;;) {
        readLine("Seat (e.g. B3): ", line, sizeof line);
        if (sscanf(line, " %c%d", &letter, &num) == 2) {
            int r = toupper((unsigned char)letter) - 'A', c = num - 1;
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
                if (map[r][c] == '.') {
                    *row = r;
                    *col = c;
                    return;
                }
                printf("That seat is taken.\n");
                continue;
            }
        }
        printf("Use a row A-%c and seat 1-%d.\n", 'A' + ROWS - 1, COLS);
    }
}

static void bookTickets(void) {
    if (movieCount == 0) {
        printf("No movies available yet.\n");
        return;
    }
    displayMovies();
    Movie *m = findMovie(readInt("\nMovie ID: ", 1, 999999));
    if (!m) {
        printf("Movie not found.\n");
        return;
    }

    for (int s = 0; s < SLOTS; s++) printf("%d. %s\n", s + 1, SLOT_TIMES[s]);
    int slot = readInt("Time slot: ", 1, SLOTS) - 1;

    char map[ROWS][COLS];
    buildSeatMap(m->id, slot, map);
    int free = countFreeSeats(map);
    if (free == 0) {
        printf("Sold out for this time slot.\n");
        return;
    }
    printSeatMap(map);

    char name[48], prompt[48];
    readText("Your name: ", name, sizeof name);
    int max = free < MAX_PER_BOOKING ? free : MAX_PER_BOOKING;
    snprintf(prompt, sizeof prompt, "Number of tickets (1-%d): ", max);
    int n = readInt(prompt, 1, max);

    int total = 0;
    for (int i = 0; i < n; i++) {
        int r, c;
        readSeat(map, &r, &c);
        map[r][c] = 'X'; // marks it taken for the next pick in this same booking

        Ticket t = {nextTicketId++, "", m->id, slot, r, c, m->price};
        strcpy(t.name, name);
        tickets = grow(tickets, sizeof *tickets, ticketCount + 1);
        tickets[ticketCount++] = t;
        total += m->price;
        printf("Ticket #%d: seat %c%d\n", t.id, 'A' + r, c + 1);
    }
    saveTickets();
    printf("Booked %d ticket(s) for \"%s\" at %s. Total: %d KRW\n", n, m->title, SLOT_TIMES[slot], total);
}

static void cancelTicket(void) {
    int id = readInt("Ticket ID to cancel: ", 1, 999999);
    char name[48];
    readText("Name on the ticket: ", name, sizeof name); // stops people cancelling each other's seats
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].id != id) continue;
        if (!sameText(tickets[i].name, name)) {
            printf("Name does not match this ticket.\n");
            return;
        }
        int refund = tickets[i].price;
        memmove(&tickets[i], &tickets[i + 1], (size_t)(ticketCount - i - 1) * sizeof *tickets);
        ticketCount--;
        saveTickets();
        printf("Ticket #%d cancelled. Refund: %d KRW\n", id, refund);
        return;
    }
    printf("Ticket not found.\n");
}

static void displayTickets(void) {
    if (ticketCount == 0) {
        printf("No tickets booked yet.\n");
        return;
    }
    printf("\n%-5s %-20s %-30s %-6s %-5s %s\n", "ID", "Name", "Movie", "Time", "Seat", "Price");
    for (int i = 0; i < ticketCount; i++) {
        const Ticket *t = &tickets[i];
        const Movie *m = findMovie(t->movieId);
        printf("%-5d %-20s %-30s %-6s %c%-4d %d KRW\n", t->id, t->name, m ? m->title : "?",
               SLOT_TIMES[t->slot], 'A' + t->row, t->col + 1, t->price);
    }
}

/* ---------- revenue and report ---------- */

static int totalRevenue(void) {
    int sum = 0;
    for (int i = 0; i < ticketCount; i++) sum += tickets[i].price;
    return sum;
}

static void printReport(FILE *out) {
    fprintf(out, "=== Sales Report ===\n");
    fprintf(out, "Tickets sold: %d\nTotal revenue: %d KRW\n\n", ticketCount, totalRevenue());

    fprintf(out, "%-30s %8s %12s\n", "Movie", "Tickets", "Revenue");
    for (int i = 0; i < movieCount; i++) {
        int count = 0, sum = 0;
        for (int j = 0; j < ticketCount; j++) {
            if (tickets[j].movieId == movies[i].id) {
                count++;
                sum += tickets[j].price;
            }
        }
        fprintf(out, "%-30s %8d %12d\n", movies[i].title, count, sum);
    }

    fprintf(out, "\n%-8s %8s\n", "Time", "Tickets");
    for (int s = 0; s < SLOTS; s++) {
        int count = 0;
        for (int j = 0; j < ticketCount; j++)
            if (tickets[j].slot == s) count++;
        fprintf(out, "%-8s %8d\n", SLOT_TIMES[s], count);
    }
}

static void generateReport(void) {
    printf("\n");
    printReport(stdout);
    FILE *f = fopen(REPORT_FILE, "w");
    if (!f) {
        perror("Cannot write report file");
        return;
    }
    printReport(f);
    fclose(f);
    printf("\nSaved to %s\n", REPORT_FILE);
}

/* ---------- menus ---------- */

static void adminMenu(void) {
    char user[32], pass[32];
    readLine("Username: ", user, sizeof user);
    readLine("Password: ", pass, sizeof pass);
    if (strcmp(user, ADMIN_USER) != 0 || strcmp(pass, ADMIN_PASS) != 0) {
        printf("Wrong username or password.\n");
        return;
    }

    int choice;
    do {
        printf("\n--- Admin Menu ---\n"
               "1. Add movies\n2. Display movies\n3. Display booked tickets\n"
               "4. Calculate total revenue\n5. Generate sales report\n0. Back\n");
        choice = readInt("Choice: ", 0, 5);
        switch (choice) {
            case 1: addMovies(); break;
            case 2: displayMovies(); break;
            case 3: displayTickets(); break;
            case 4: printf("Total revenue: %d KRW\n", totalRevenue()); break;
            case 5: generateReport(); break;
        }
    } while (choice != 0);
}

static void userMenu(void) {
    int choice;
    do {
        printf("\n--- User Menu ---\n"
               "1. Display available movies\n2. Search for a movie\n3. Book tickets\n"
               "4. Cancel a ticket\n0. Back\n");
        choice = readInt("Choice: ", 0, 4);
        switch (choice) {
            case 1: displayMovies(); break;
            case 2: searchMovies(); break;
            case 3: bookTickets(); break;
            case 4: cancelTicket(); break;
        }
    } while (choice != 0);
}

int main(void) {
    loadMovies();
    loadTickets();

    int choice;
    do {
        printf("\n=== Movie Ticket Booking System ===\n1. Admin\n2. User\n0. Exit\n");
        choice = readInt("Choice: ", 0, 2);
        if (choice == 1) adminMenu();
        else if (choice == 2) userMenu();
    } while (choice != 0);

    printf("Goodbye!\n");
    free(movies);
    free(tickets);
    return 0;
}
