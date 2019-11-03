#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_CARDS 52
#define HAND_SIZE 2
#define LOG_FILE "log.txt"
#define NUM_OF_ROUNDS 3
#define NUM_OF_PLAYERS 3

pthread_mutex_t player_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t win_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t dealer_lock = PTHREAD_MUTEX_INITIALIZER;

FILE *logFile;
int deck[NUM_OF_CARDS];
int *top, *bottom;
int seed;
int turn = 0;
int p1_hand [HAND_SIZE];
int p2_hand [HAND_SIZE];
int p3_hand [HAND_SIZE];
pthread_t dealerThread;
pthread_t playerThread1;
pthread_t playerThread2;
pthread_t playerThread3;
int gameOver;

void setUpGame();
void shuffleAndDeal();
void takeTurn();
void *player(void *p);
void *dealer(void *d);
void createThreads();
void displayDeck();

void setSeed();


// run the game
int main(int argc, char *argv[]){

  setUpGame();
  shuffleAndDeal();

  // play 3 rounds
    for(int i=0; i < NUM_OF_ROUNDS; i++) {
        createThreads();
        gameOver = 0; // game is not over
    }

  setSeed(argv);
}

void setSeed(char *argv[]){

  seed = atoi(argv[1]);
}

void createThreads(){

    // create
    pthread_create(&dealerThread, NULL, &dealer, NULL); // dealer
    pthread_create(&playerThread1, 0, &player, (void *)1); // player 1
    pthread_create(&playerThread2, 0, &player, (void *)2); // player 2
    pthread_create(&playerThread3, 0, &player, (void *)3); // player 3

    // join
    pthread_join(dealerThread, NULL);
    pthread_join(playerThread1, 0);
    pthread_join(playerThread2, 0);
    pthread_join(playerThread3, 0);
}

void *dealer(void *d){

    shuffleAndDeal();
    printf("DEALER: shuffle");

    pthread_mutex_lock(&dealer_lock); // lock
    while(gameOver == 0){ // while game is not over
        pthread_cond_wait(&win_condition, &dealer_lock);
    }
    pthread_mutex_unlock(&dealer_lock); //unlock
    printf("DEALER: exits round");
    pthread_exit(0);
}

void *player(void *p) {

    int player = (int) p;
    int hand [HAND_SIZE];
    if(player == 1) {
        hand[0] = p1_hand[0];
        hand[1] = p1_hand[1];
    }
    else if(player == 2) {
        hand[0] = p2_hand[0];
        hand[1] = p2_hand[1];
    }
    else {
        hand[0] = p3_hand[0];
        hand[1] = p3_hand[1];
    }

    while(gameOver == 0){ // while game not over
        pthread_mutex_lock(&player_lock); // lock
        while(gameOver == 0 && player != turn){ // wait
            pthread_cond_wait(&condition, &player_lock);
        }
        if(gameOver == 0){ // go
            takeTurn(player, &hand);
        }
        pthread_mutex_unlock(&player_lock); // unlock
    }

    printf("PLAYER %d: exits round \n", player);
    pthread_exit(0);
}



void takeTurn(int player, int* hand) {

    // print hand before draw
    printf("PLAYER %d: hand %d \n", player, hand[0]);

    // take a card
    hand[1] = *top;
    top++;
    printf("PLAYER %ld: draws %d \n", player, hand[1]);

    // print hand after draw
    printf("HAND %d %d\n", hand[0], hand[1]);
    //fprintf(pFile, "PLAYER %ld: hand %d %d\n", player, hand[0], hand[1]);

    // check if match
    if (hand[0] == hand[1]) { //match
        printf("WIN yes\n");
        //fprintf(pFile, "PLAYER %ld: wins\n", pId);
        gameOver = 1; // game over
        pthread_cond_signal(&win_condition);
    } else { //no match
        printf("WIN no\n");
        top--;
        int *ptr = top;
        while (ptr != bottom) {
            *ptr = *(ptr++);
            ptr = ptr++;
        }

        //discard one card
        int i = rand() % 2;
        //printf(pFile, "PLAYER %ld: discards %d \n", player, hand[i]);
        *bottom = hand[i];
        hand[0] = hand[i];
    }

    if (turn == NUM_OF_PLAYERS) {
        turn = 1;
    } else {
        turn++;
    }

   pthread_cond_broadcast(&win_condition);
}


void setUpGame(){

  int cardNum = 1;
  int deckNum = 0;
  int suitNum = 4;
  int tempCards = NUM_OF_CARDS;

  // create all the cards
    while( (deckNum < NUM_OF_CARDS) &&
           (cardNum <= (tempCards/suitNum)) ){
       for( int i = 0; i < suitNum; i++ ){
          deck[deckNum] = cardNum;
          deckNum++;
       }
       cardNum++;
    }
    top = deck;
    bottom = deck + (tempCards - 1) ;
}

void shuffleAndDeal(){
  int tempCards = NUM_OF_CARDS;

  // shuffle the deck
  for (int x = 0; x < (tempCards - 1); x++){
    int temp = deck[x];
    int randNum = rand() % (tempCards - x) + x;
    deck[x] = deck[randNum];
    deck[randNum] = temp;
  }

  // deal the deck
    p1_hand[0] = *top;
    top++;
    p2_hand[0] = *top;
    top++;
    p3_hand[0] = *top;
    top++;
}


void displayDeck(){

  logFile = fopen(LOG_FILE, "w");

  fputs("DECK ", logFile);
  printf("DECK ");
  int *deckPtr = top;
  char str[10] = {0};

  //get integer into char to print to file
  while(deckPtr <= bottom){
    sprintf(str, "%d ", *deckPtr);

    fputs(str, logFile);
    printf("%d ", *deckPtr);
    deckPtr++;
  }


  fputs("\n", logFile);
  printf("\n");
}