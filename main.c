#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_MOVIES 200
//structure for movie
struct Movie{
   int id;
   char title[100];
   char genre[100];
   int year;
   char country[100];
   int price;
}movies;

//structure for a user, contains Id of a movie, name of a user, number of thickets which a user wants to buy, favourite animal
struct User{
   char name[100];
   int numTickets;
   char favAnimal[100];
}user;

//function which saves movies to movie data base
int saveMoviesToFile(int n){
   FILE *savedMovies;
   savedMovies = fopen("movies_database.txt", "ab");
   if(savedMovies == NULL){
      printf("Error has happened in saving movies, the file is not created.");
      exit(0);
   }

   int i = 0; 
   struct Movie movies[MAX_MOVIES];

    do{
        printf("\nEnter movie details:\n");
        printf("\nID of a Movie: ");
        scanf("%d", &movies[i].id);
       // fgets(movies[i].id, sizeof(movies), stdin);
        printf("\nTitle: ");
        scanf("%s", movies[i].title);
        //fgets(movies[i].title, sizeof(movies), stdin);
        printf("\nGenre: ");
        scanf("%s", movies[i].genre);
        //fgets(movies[i].genre, sizeof(movies), stdin);
        printf("\nYear: ");
        scanf("%d", &movies[i].year);
        //fgets(movies[i].year, sizeof(movies), stdin);
        printf("\nCountry: ");
        scanf("%s", movies[i].country);
        //fgets(movies[i].country, sizeof(movies), stdin);
        printf("\nPrice: ");
        scanf("%d", &movies[i].price);
        i++;
     }while(i<n);
      
     fwrite(movies, sizeof(struct Movie), n, savedMovies);

   fclose(savedMovies);
  }

//to dispaly saved movies from movie data base
int displaySavedMoviesFromFile(){
  FILE *savedMovies = fopen("movies_database.txt", "rb");
  if(savedMovies == NULL){
     printf("No previous data found while dispalying movies!!!\n");
     return 0;
  }

  int i = 0;
  struct Movie movies[MAX_MOVIES];
  while(fread(&movies[i], sizeof(struct Movie), 1, savedMovies) != 0){
     i++;
  }      
  for (int j = 0; j < i ; ++j) {
       printf("Id: %d | Title: %s | Genre: %s | Year: %d | Country: %s | Price: %d \n", movies[j].id, movies[j].title, movies[j].genre, movies[j].year, movies[j].country, movies[j].price);
  }
  fclose(savedMovies);
  return i;
}

int bookingTicket(int idMovie){
   FILE *savedMovies = fopen("movies_database.txt", "rb");
   if(savedMovies == NULL){
      printf("No previous data found in movie data base while booking a ticket!!!\n ");
      return 0;
   } 

   int i = 0, j;
   struct Movie movies[MAX_MOVIES];
   while(fread(&movies[i], sizeof(struct Movie), i, savedMovies) != 0){
     i++;
   } 

   //finding ID of a Movie 
   //finding a structure of a movie by checking whether Id, user entered, is the same as the id of i number of an array or not
   for(j=0; j<i; j++){
      if(movies[j].id == idMovie){
         printf("Id: %d | Title: %s | Genre: %s | Year: %d | Country: %s | Price: %d \n", movies[j].id, movies[j].title, movies[j].genre, movies[j].year, movies[j].country, movies[j].price);
         fclose(savedMovies);
         return j;

         int bookMovie;
         printf("Please, enter 1 to book a movie;");
         printf("\nIs this a movie that you want to book? ");
         scanf("%d", &bookMovie);

         FILE *bookedTickets = fopen("booked_tickets.txt", "a");
         if(bookedTickets == NULL){
         printf("Error has happened while opening a file for booked movies!!!");
         exit(0);
         }

         struct User user[MAX_MOVIES];
         int i=0;
       if(bookMovie == 1){
         printf("Good choice!");
      
         do{
         printf("Please, enter your name: ");
         scanf("%s", user[i].name);
         printf("How many tickets you want to buy? ");
         scanf("%d", &user[i].numTickets);
         printf("What is your favourite animal? ");
         scanf("%s", user[i].favAnimal);
         i++;
      
         }while(i<1);

         fwrite(user, sizeof(struct User), 1, bookedTickets);
      
         //even though bookedTickets file will not be equal to NULL if it is not running at the first time. i added this because i want
         // to check whether this function will work or not 
         if(bookedTickets == NULL){
         printf("Error has happened while writing user information to a file for booked movies!!!");
         exit(0);
         }

         fclose(bookedTickets);

       }else{
         printf("Invalid number is entered!");
         printf("Please, enter 1 to book a movie;");
         }
   

      }else{
         printf("Movie has not found\n");
         fclose(savedMovies);
         return 0;
      }
   }
}
   
   //getting User Data
   //if it is same, then i want to get an information of a user, and store it to the booked_movies.txt file, which I will create now   
int bookMovieTicket(int bookMovie){
   FILE *bookedTickets = fopen("booked_tickets.txt", "a");
    if(bookedTickets == NULL){
      printf("Error has happened while opening a file for booked movies!!!");
      exit(0);
    }

   struct User user[MAX_MOVIES];
   int i=0;
   if(bookMovie == 1){
      printf("Good choice!");
      
      do{
      printf("Please, enter your name: ");
      scanf("%s", user[i].name);
      printf("How many tickets you want to buy? ");
      scanf("%d", &user[i].numTickets);
      printf("What is your favourite animal? ");
      scanf("%s", user[i].favAnimal);
      i++;
      
      }while(i<1);

      fwrite(&user, sizeof(struct User), 1, bookedTickets);
      
      //even though bookedTickets file will not be equal to NULL if it is not running at the first time. i added this because i want
      // to check whether this function will work or not 
      if(bookedTickets == NULL){
      printf("Error has happened while writing user information to a file for booked movies!!!");
      exit(0);
    }

    fclose(bookedTickets);

   }else if(bookMovie == 2){
      //goes to booking Ticket
      int idMovie;
      printf("Please, Enter ID of a Movie that you want to book: ");
      scanf("%d", &idMovie);
      bookingTicket(idMovie);

   }else{
      printf("Invalid number is entered!");
      printf("Please, enter 1 to book a movie; 2 to if it is a wrong movie");
   }

   void displayBookedTickets(){
      FILE *bookedTickets = fopen("booked_tickets.txt", "rb");
      if(bookedTickets == NULL){
      printf("Error has happened while opening a file for booked movies!!!");
      exit(0);

    }
   }
}



int main(){

 int first_menu, main_menu;

 do{
 printf("\n                                    Welcome to Zohidjon's Cinema!\n");
 printf("                 Do you want to enter the system?\n ");
 printf("                If yes, enter 1!\n");
 printf("                 Enter: ");
 scanf("%d", &first_menu);
 
 if (first_menu != 1) {
     printf("\nInvalid input! Please enter 1 to enter the system.\n");
    }
 } while (first_menu != 1);

 printf("\n\n##############################################################################\n\n");
 printf("                                Main Menu        \n ");
 printf("               1. Admin\n");
 printf("                2. User\n\n");
 printf("\n             Enter: ");
 scanf("%d", &main_menu);
 printf("##############################################################################\n");

 if(main_menu == 1){
   
 //Admin checking
 char admin_name[20], admin_password[20];
 int admin_menu;

   printf("\n user_name: ");
   scanf("%s", &admin_name);
   printf("\n password: ");
   scanf("%s", &admin_password);
   
  //Admin menu
   if((strcmp(admin_name, "zohidjon") == 0) && (strcmp(admin_password, "Zoh!dj0n23") == 0)){
      printf("\nWelcome! Admin\n");
      printf("\nWhat do you want to do?\n\n");
      printf("1. Add Movies to Database\n");
      printf("2. Display Booked Tickets\n");
      printf("3. Calculate Total Revenue\n");
      printf("4. Generate Sales Report\n");
      printf("0. Exit the Program\n");
      printf(" Enter: ");
      scanf("%d", &admin_menu);
   
      if(admin_menu == 1){
         //Add movies to database
        int n; 

      printf("How many movies do you want to add? ");
      scanf("%d", &n);
      saveMoviesToFile(n);
      printf("Movies in Database:");
      displaySavedMoviesFromFile();

      }else if(admin_menu == 2){
      //Display booked tickets
      displayBookedTickets();
      }else if(admin_menu == 3){
      //Calculate total revenue

      }else if(admin_menu == 4){
      //Exit program

      }else{
         printf("\n Invalid choice, please enter from 1 to 4 in order to get access!!\n");
      }
           
   }else{
      printf("\nWrong username or password!\n");
   }
   
 }else if(main_menu==2){
   
   //User Menu
   int userMenu;
   printf("Welecome! User\n\n");
   printf("What do you want to do?\n");
   printf("1. Display Available Movies\n");
   printf("2. Search for a Movie\n");
   printf("3. Book Ticket\n");
   printf("4. Cancel Ticket\n");
   printf("0. Exit the Program\n");
   printf("Enter ");
   scanf("%d", &userMenu);
 
    //Display available movies

    if(userMenu == 1){
      displaySavedMoviesFromFile();
      
    }else if(userMenu == 2){
      //Search for a Movie
    }else if(userMenu == 3){
      //Book Ticket
      int idMovie;
      printf("Please, Enter ID of a Movie that you want to book: ");
      scanf("%d", &idMovie);
      bookingTicket(idMovie);
   }else if(userMenu == 4){
      //Cancel Ticket
   
    }else if(userMenu == 5){
      //Exit

    }else{
         printf("\n Invalid choice, please enter from 1 to 4 in order to get access!!\n");
      }
         


 }else{
    printf("\nPlease, choose 1 for Admin or 2 for User!!!\n");
 } 


    return 0;


}
