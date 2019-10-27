#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_CARDS 52
#define HAND_SIZE 2

FILE *logFile;
int deck[NUM_OF_CARDS];
int *top, *bottom;
int seed;



void setUpDeck();
void shuffleCards();
void setSeed();
void dealPlayerHands();
void displayDeck();

int p1_hand [HAND_SIZE];
int p2_hand [HAND_SIZE];
int p3_hand [HAND_SIZE];


//main
int main(int argc, char *argv[]){



  setUpDeck();
  shuffleCards();
  setSeed(argv);
  dealPlayerHands();
  displayDeck();

}

void setSeed(char *argv[]){
  seed = atoi(argv[1]);

}

void setUpDeck(){

  int cardNum = 1;
  int deckNum = 0;
  int suitNum = 4;
  int tempCards = NUM_OF_CARDS;

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

void shuffleCards(){
  int tempCards = NUM_OF_CARDS;

  for (int x = 0; x < (tempCards - 1); x++){
    int temp = deck[x];
    int randNum = rand() % (tempCards - x) + x;
    deck[x] = deck[randNum];
    deck[randNum] = temp;
  }
}

void dealPlayerHands(){
  p1_hand[0] = *top;
  top++;
  p2_hand[0] = *top;
  top++;
  p3_hand[0] = *top;
  top++;
}

void displayDeck(){

  logFile = fopen("log.txt", "w");

  fputs("DECK ", logFile);
  printf("DECK ");
  int *deckPtr = top;
  char str[10] = {0};

  //get integer into char to print to file
  while(deckPtr <= bottom){
    sprintf(str, "%d", *deckPtr);
    fputs(str, logFile);
    printf("%d ", *deckPtr);
    deckPtr++;
  }


  //fputs(logFile);
  printf("\n");
}
