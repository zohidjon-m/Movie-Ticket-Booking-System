#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_MOVIES 100 //maximum number of movies
#define MAX_STRING_LENGTH 50 

//Structre for Movie: title, year, genre, country 
struct Movie{
    char title[50];
    int year;
    char genre[20];
    char country[40];
};

//Function to Save Movies into a File using File Handling
void saveMoviesToFile(struct Movie *movies, int movieCount){
   FILE *file =  fopen("movies.txt", "w");
   if(file == NULL){
      printf("Error happened in saveMoviesToFile!!!\n");
    return; 
   }
   for(int i=0; i<movieCount; ++i){
      fprintf(file,"%s %d %s %s\n", movies[i].title, movies[i].year, movies[i].genre, movies[i].country);
   } 
   fclose(file);
}

//Function to Load Movies From A File created
int loadMoviesFromFile(struct Movie *movies){
      FILE *file = fopen("movies.txt", "r");
      if(file == NULL){
         printf("No previous data found (loadMoviesFromFile)!!!\n");
         return 0;
      }
   int i = 0;
   while(fscanf(file, "%[^;]; %d; %[^;]; %[^\n]\n", movies[i].title, &movies[i].year, movies[i].genre, movies[i].country) != EOF){
      i++;
   }

   fclose(file);
   return i;
}

int main(){

 int main_menu, first_menu;
do{
 printf("\n                                    Welcome to Zohidjon's Cinema!\n");
 printf("                 Do you want to enter the system?\n ");
 printf("                 If yes, enter 1!\n");
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
 printf("\n             Enter: \n\n");
 scanf("%d", &main_menu);
 printf("##############################################################################\n");
 

 if(main_menu == 1){
   // printf("Welecome! Admin\n");

   //Admin checking
char admin_name[20], admin_password[20];
int admin_menu;

   printf("\n user_name: ");
   scanf("%s", &admin_name);
   //printf("\n password: ");
   //scanf("%s", &admin_password);
//Admin menu
struct Movie *movies = NULL; // Declare movies pointer outside the switch
char addMovieChoice;//in the section of adding movies
   if(strcmp(admin_name, "zohidjon") == 0){ //&& strcmp(admin_password, "Zoh!dj0n23") == 0){
            printf("\nWelcome! Admin\n");
            printf("\nWhat do you want to do?\n\n");
            printf("1. Add Movies to Database\n");
            printf("2. Display Booked Tickets\n");
            printf("3. Calculate Total Revenue\n");
            printf("4. Exit the Program\n");
            printf(" Enter: ");
            scanf("%d", &admin_menu);

            switch(admin_menu){
               case 1:
              //Add movies to database

              //Allocate Memory for Movies
               movies = malloc(MAX_MOVIES * sizeof(struct Movie));
               if(movies == NULL){
               printf("Memory allocation failed!!!\n");
               return 1;
               }

               int movieCount = loadMoviesFromFile(movies);            
            
               do{
                  printf("\nEnter movie details: ");
                  printf("\nTitle: ");            
                  fgets(movies[movieCount].title, MAX_STRING_LENGTH, stdin);
                  getchar(); // Clear the newline character left in the input buffer
                  printf("\nYear: ");
                  scanf("%d", &movies[movieCount].year);                  
                  printf("\nGenre: ");               
                  fgets(movies[movieCount].genre, MAX_STRING_LENGTH, stdin);
                  getchar(); // Clear the newline character left in the input buffer
                  printf("\nCountry: ");                  
                  fgets(movies[movieCount].country, MAX_STRING_LENGTH, stdin);
                  getchar(); // Clear the newline character left in the input buffer
                  movieCount++;

                  printf("\nDo you want to add another movie? y/n: ");
                  scanf(" %c", &addMovieChoice);
                  getchar(); // Clear the newline character left in the input buffer
               } while((addMovieChoice == 'y' || addMovieChoice == 'Y') && movieCount < MAX_MOVIES);

               saveMoviesToFile(movies, movieCount);

               // Display all movies (existing and newly added)
               printf("\nMovies in the database:\n");
               for (int i = 0; i < movieCount; i++) {
               printf("Title: %s | Year: %d | Genre: %s | Country: %s\n",
               movies[i].title, movies[i].year, movies[i].genre, movies[i].country);
                }

                free(movies); // Free allocated memory

             break;
               case 2:
            //Display booked tickets
             break;
               case 3:
            //Calculate total revenue
             break;
               case 4:
            //Exit program
             break;
              default:
            printf("\ninvalid choice\n");
            break;
            }
      
   }else{
      printf("\nWrong username or password!\n");
   }
   
 }else if(main_menu==2){
    printf("Welecome! User\n");
 }else{
    printf("\nPlease, choose 1 for Admin or 2 for User!!!\n");
 } 


    return 0;
}
