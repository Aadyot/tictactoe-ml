#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//graphics libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define see_the_calc 0 
#define playingfordata 0 //if true, comp will play against randomly itself indefinitely
#define trial 0 //if true, comp will play only randommoves
#define number 1.8 //how much i want to punish for losing as compared to reward for winning
#define checkandadd 1 //if true, program will check entire database for redundancy after every game


/*
future plans:
improve error handling
consider rotations and reflections as same.
*/

//filename
//for safety purposes, during testing i will write only to an unimportant file
const char* writefile="gamedata_ttt_c.csv", *readfile="gamedata_ttt_c.csv" ;

// window size
const int h =400, w=400;
//mode button values
const int height=h/4,     width=w/3,    xcoor=w/10,    ycoor=h/5 , gap=h/200;


TTF_Font *small, *medium, *big;
//font sizes
const int big_size=40, medium_size=18, small_size=20;

//colours
SDL_Color white={255,255,255}, black={0,0,0},

    red= {255,0,0}, blue={0,0,255}, green={0,255,0}, yellow={255,255,0},

    mult1={0,255,0}, mult2={0,179,30}, sing1= {0,255,255}, sing2={30,144,255},

    crosscolour= {0,75,255}, zerocolour={0,225,75};


const char *cross_win = "CROSS WINS!!", * zero_win= "ZERO WINS!!", *draw ="DRAW!!",
    *name= "AADYOT", *start= "who will start?",
    *play_again= "PLAY AGAIN?", *ttt="TIC TAC TOE", *mult="MULTIPLAYER", *sing="SINGLEPLAYER",
    *user = "USER", *comp= "COMPUTER", *starts= "STARTS",

    *played_before_text = "I have played this game before :)", 
    *not_played_before_text = "this was a new game!",

    *play_single="PLAY TIC TAC TOE AGAINST COMPUTER", 
    *play_multi = "PLAY TIC TAC TOE AGAINST YOUR FRIENDS",

    *window_fail = "could not create window",
    *file_open_write_fail= "failed to open file while writing",

    *arial="arial.ttf";

struct game_data {
    int win;
    struct play{
        int pos[2];
    }game[9];
    int no_of_moves;
};


//debug functions
void printgrid(int grid[3][3]){
    for(int i=0;i<=2;i++){
        for(int j=0;j<3;j++){
            printf("%d ",grid[i][j]);
        }
        printf("\n");
    }
}

void printgamedata(struct game_data list){
    printf("%d ",list.win);
    for(int i=0;i<9;i++){
        printf("(%d,%d)", list.game[i].pos[0], list.game[i].pos[1]);
    } 
    printf("%d \n",list.no_of_moves);
}

void print_nx2_list(int n,int lis[][2]){
    for(int i=0;i<n;i++){
        printf("(%d,%d)",lis[i][0], lis[i][1]);
    }
    printf("\n");
}

void printlist(int n, int lis[]){
    for(int i=0;i<n;i++){
        printf("%d ",lis[i]);
    }
    printf("\n");
}

//time
void delay(int ms){
    //parameter is in milli seconds
    // Storing start time
    clock_t t = clock();
  
    // looping till required time is not achieved
    while (clock() < t + ms){}
        
}

//text
void put_text(int fontsize, SDL_Renderer *renderer, int X, int Y, const char * text,
             SDL_Color col){

    TTF_Font *font;
    font = TTF_OpenFont(arial, fontsize);

    SDL_Surface * surface = TTF_RenderText_Solid(font, text, col); 
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

    int  text_width, text_height;
    SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);    
    SDL_Rect text_rect= {X,Y,text_width,text_height};

    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}


//graphics
void rectangle(SDL_Renderer * renderer, int x,int y, int w, int h,SDL_Color col){

    //setting the colour
    SDL_SetRenderDrawColor(renderer,col.r,col.g,col.b,SDL_ALPHA_OPAQUE);

    //rectangle of latest colour
    SDL_Rect rectangle1; //define rectangle object
    rectangle1.x=x; //telling details of rectangle object
    rectangle1.y=y;
    rectangle1.w=w;
    rectangle1.h=h;
    SDL_RenderDrawRect( renderer, &rectangle1);
    
}

void line(SDL_Renderer * renderer, int x1,int y1, int x2, int y2, SDL_Color col){

     //setting the colour
    SDL_SetRenderDrawColor(renderer,col.r,col.g,col.b,SDL_ALPHA_OPAQUE);
    
    //line of the latest colour
    SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
}

void matrix(SDL_Renderer * renderer){
    //initialises white grid only on screen     
    line(renderer,width-gap,   0,           width-gap,   h,           white);
    line(renderer,width+gap,   0,           width+gap,   h,           white);
    line(renderer,2*width-gap, 0,           2*width-gap, h,           white);
    line(renderer,2*width+gap, 0,           2*width+gap, h,           white);
    line(renderer,0,           width-gap,   h,           width-gap,   white);
    line(renderer,0,           width+gap,   h,           width+gap,   white);
    line(renderer,0,           2*width-gap, h,           2*width-gap, white);
    line(renderer,0,           2*width+gap, h,           2*width+gap, white);   
    SDL_RenderPresent(renderer);
    
}

void cross(SDL_Renderer * renderer, int grid[3][3], int yx[2], char mode,SDL_Color col){
    //draw a cross and stores it as 1 in matrix
    if (mode=='s'){
        delay(200); //immediate answer by computer feels weird.        
    }
    int x=yx[1],y=yx[0];


    //this library cant take line thickness, so i am emulating that with multiple lines
    int m=20;
    for(float g=0;g<=gap;g+=(float)gap/m){        
        line(renderer,x*h/3+h/20+g,y*h/3+h/20-g,x*h/3+(h/3-h/20)+g,y*h/3+(h/3-h/20)-g,col);
        line(renderer,x*h/3+h/20+g,y*h/3+(h/3-h/20)+g,x*h/3+(h/3-h/20)+g,y*h/3+h/20+g,col);

        line(renderer,x*h/3+h/20-g,y*h/3+h/20+g,x*h/3+(h/3-h/20)-g,y*h/3+(h/3-h/20)+g,col);
        line(renderer,x*h/3+h/20-g,y*h/3+(h/3-h/20)-g,x*h/3+(h/3-h/20)-g,y*h/3+h/20-g,col);
    }

    //to present everything that i have rendered till now.
    //ideally use it once an iteration and not after every rendering.
    //using it many times in loop will be inefficient and will cause flashing
    SDL_RenderPresent(renderer);

    grid[y][x]=1;
}

void zero(SDL_Renderer * renderer, int grid[3][3], int yx[2], SDL_Color col){

    //very interesting algo for drawing circle
    SDL_SetRenderDrawColor(renderer,col.r,col.g,col.b,SDL_ALPHA_OPAQUE);
    
    int X= yx[1], Y = yx[0];
    grid[Y][X]=2;
    X=X*h/3+h/6;
    Y=Y*h/3+h/6;

    int r=h/8;
    int d=2*r;
    
    
    //time_t t1,t2;//this
    int m=10;
    for(float g=r-gap;g<=r+gap;g+=(float)gap/m){
        int x = (g - 1);
        int y = 0;
        int tx = 1;
        int ty = 1;
        int error = (tx - d);
        while (x >= y){        

        //  Each of the following renders an octant of the circle
        
        //start drawing from top, bottom, left and right, moving out in both directions from all.
        //X,Y are centre
        //x,y are deviation from centre.
        //lets just consider 0 to pi/4. rest all is symmetric
        //i start at X + r, Y.
        //now i must take a small step in +y direction and one in -x direction
        //at start, i must take step only in y direction. (circle is vertical at theta=0)
        //

        
        SDL_RenderDrawPoint(renderer, X + x, Y - y);
        SDL_RenderDrawPoint(renderer, X + x, Y + y);
        SDL_RenderDrawPoint(renderer, X - x, Y - y);
        SDL_RenderDrawPoint(renderer, X - x, Y + y);
        SDL_RenderDrawPoint(renderer, X + y, Y - x);
        SDL_RenderDrawPoint(renderer, X + y, Y + x);
        SDL_RenderDrawPoint(renderer, X - y, Y - x);
        SDL_RenderDrawPoint(renderer, X - y, Y + x);


        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - d);
        }
    }
    }
    SDL_RenderPresent(renderer);
    
}

void get_click_coord(SDL_Window *window, int coord[]){
    SDL_Event click_event;
    
    while (1){
        SDL_PumpEvents();
        SDL_PollEvent(&click_event);
        if(click_event.type==SDL_QUIT){
            SDL_DestroyWindow(window);
            SDL_Quit();
            TTF_Quit();
            exit(0);
        }else if(click_event.type==SDL_MOUSEBUTTONDOWN){                
            SDL_GetMouseState(&coord[0],&coord[1]);
            break;
        }
        
    }
}



//game helpers
int decisionmaker(int grid[3][3]){
    //makes decision of win,draw,loss using grid[]
    int total=0;


    //grid value:
    //0 is empty
    //1 is cross
    //2 is zero

    //return value:
    //0 is draw, 1 is cross(comp) win, 2 is zero(user) win 
    //-1 : game is not over   


    //rows and columns
    int rx[3], cx[3], ro[3], co[3], d1x, d2x, d1o, d2o;
    for(int i=0;i<3;i++){
        rx[i]=(grid[i][0]==1 && grid[i][1]==1 && grid[i][2]==1);
        ro[i]=(grid[i][0]==2 && grid[i][1]==2 && grid[i][2]==2);

        cx[i]=(grid[0][i]==1 && grid[1][i]==1 && grid[2][i]==1);
        co[i]=(grid[0][i]==2 && grid[1][i]==2 && grid[2][i]==2);
    }
    //diagonals
    d1x=(grid[0][0]==1 && grid[1][1]==1 && grid[2][2]==1);
    d1o=(grid[0][0]==2 && grid[1][1]==2 && grid[2][2]==2);
    d2x=(grid[2][0]==1 && grid[1][1]==1 && grid[0][2]==1);
    d2o=(grid[2][0]==2 && grid[1][1]==2 && grid[0][2]==2);

    //combined rows and columns
    int arx= (rx[0]|| rx[1]|| rx[2]);
    int acx= (cx[0]|| cx[1]|| cx[2]);
    int aro= (ro[0]|| ro[1]|| ro[2]);
    int aco= (co[0]|| co[1]|| co[2]);


    
    if(arx||acx||d1x||d2x){
        return 1;
    }else if(aro||aco||d1o||d2o){
        return 2;
    }

    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(grid[i][j]!=0){
                total++;
            }
        }
    }
    if (total==9){
        return 0;
    }
    return -1;
}

void refresh(SDL_Renderer * renderer, SDL_Color col){
    //clears the canvas and make it the latest color
    SDL_SetRenderDrawColor(renderer,col.r,col.g,col.b,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void winreporter(int status, SDL_Renderer *renderer, int played_before){
    /*reports the winner
    win=-1 means in progress (function isnt called at that time)
    win=0 means draw
    win=1 means player one (cross) wins
    win=2 means player two(zero) wins
    */
    delay(100);
    
    if (status==1){
        //display on screen
        put_text(big_size, renderer, (int)h/6, h/2-h/10, cross_win, red);
    }
    else if (status==2){
        put_text(big_size, renderer, (int)h/6, h/2-h/10, zero_win, red);
        
    }
    else if (status==0){
        put_text(big_size, renderer, (int)h/4, h/2-h/10, draw, red);
        
    } 

    put_text(small_size, renderer, (played_before? h/12: h/5), 9*h/10, 
                (played_before? played_before_text: not_played_before_text), yellow);
    delay(2000);
}


// lists and structs
int game_compare(struct game_data s1, struct game_data s2){
    if(s1.win==s2.win  && s1.no_of_moves==s2.no_of_moves){
        for(int i=0;i<9;i++){
            if(s1.game[i].pos[0]!=s2.game[i].pos[0]){
                return 0;
            }else if(s1.game[i].pos[1]!=s2.game[i].pos[1]){
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int max_coord(int n, float *lis){
    int max=0;
    for(int i=0;i<n;i++){        
        if(lis[i]>lis[max]){
            max=i;
        }
    }    
    return max;
}

void remaining(struct game_data gamerecord, int currmove, int next_moves[][2]){
    //removes moves which are done (in gamerecord)   from all_moves   and stores in next_moves
    int all_moves[9][2]={{0,0},{0,1},{0,2},{1,0},{1,1},{1,2},{2,0},{2,1},{2,2}};
    int nmi=0;   
    for(int am=0;am<9;am++){ //going through list of all moves
        int nm=1;                
        for(int g=0;g<currmove;g++){ 
            if (all_moves[am][0]==gamerecord.game[g].pos[0] && 
                all_moves[am][1]==gamerecord.game[g].pos[1]){  
                nm=0;
                break;
            }
        }
        if(nm==1){
            next_moves[nmi][0]=all_moves[am][0];
            next_moves[nmi][1]=all_moves[am][1];
            
            nmi++;
        }
    }
}

void initialise_zero(int lis[], int n){
    for(int i=0;i<n;i++){
        lis[i]=0;
    }
}

int find(int c[2], int lis[][2], int n){    
    for(int i=0;i<n;i++){
        if (lis[i][0]==c[0]&& lis[i][1]==c[1]){
            return i;
        }
    }
    return -1;    
}

int weight(int win, int starter){
    if((win==1 && starter==1) || win==2 && starter==2) return 1;
    if(win==0) return 0;
    return -number;
}


//file
void write_game_to_file(const char* filename, struct game_data lis){
    FILE * file1 = fopen(filename, "a");

    if(file1==NULL){
        printf(file_open_write_fail);
        return;
    }

    fprintf(file1, "%d", lis.win);
    for(int i=0;i<9;i++){        
        fprintf(file1, ",(%d,%d)", lis.game[i].pos[0],lis.game[i].pos[1]);
    }
    fprintf(file1, "\n");
    fclose(file1);
}

struct game_data read_next_game_from_file(FILE* readfile){
    struct game_data row;

    int no_of_moves=0;
    fscanf(readfile,"%d,",&row.win);
    for(int i=0;i<9;i++){
        int a,b;
        fscanf(readfile,"(%d,%d),",&a, &b);
        row.game[i].pos[0]=a;
        row.game[i].pos[1]=b;

        if (a>=0){
            no_of_moves++;
        }
    }
    row.no_of_moves=no_of_moves;
    return row;
}

int find_game_in_file(const char* filename, struct game_data lis){
    
    FILE* readfile = fopen(filename, "r" );

    if(readfile==NULL){ //file DNE
        return 0;
    }

    struct game_data row;

    if(!feof(readfile)){
       row= read_next_game_from_file(readfile);
    }
    while(!feof(readfile)){
        if(game_compare(row,lis)){
            fclose(readfile);
            return 1;
        }
        row= read_next_game_from_file(readfile);      
    }
    fclose(readfile);
    return 0;
}

int storegame(struct game_data lis){
    if (checkandadd){    
        //to avoid duplicates, i will check full database before adding anything new
        if(find_game_in_file(readfile, lis)){
            return 1;
        }
    }    
    write_game_to_file(writefile, lis);    
    return 0;                        
}


//move
void random_move(int grid[3][3],int coord[2]){
    while(1){
        coord[0]=rand()%3;
        coord[1]=rand()%3;
        if (grid[coord[0]][coord[1]]==0){
            if(see_the_calc) printf("random move\n");
            return ;
        }
    }
}

void input_move(int grid[3][3], SDL_Window* window, int tttcord[2]){
    /*
    takes coordinates when screen is clicked and updates the argument tttcord to reflect it
    after converting to grid coordinates
    */
    while (1){
        int coord[2];        
        get_click_coord(window, coord);
        
        int y, x;
    
        if (coord[0]<h/3){
            if (coord[1]<h/3)
                y=0, x=0;
            else if( coord[1]<2*h/3)
                y=1,x=0;
            else if (coord[1]<h)
                y=2,x=0;
        }  
        else if(coord[0]<2*h/3){
            if (coord[1]<h/3)
                y=0,x=1;
            else if( coord[1]<2*h/3)
                y=1,x=1;
            else if (coord[1]<h)
                y=2,x=1;
        }
                
        else if (coord[0]<h){
            if (coord[1]<h/3)
                y=0,x=2;
            else if (coord[1]<2*h/3)
                y=1,x=2;
            else if(coord[1]<h)
                y=2,x=2;
        }

        if (grid[y][x]==0){
            tttcord[0]=y;
            tttcord[1]=x;
            return;
        }
    }    
}

void db_move(struct game_data gamerecord,int grid[3][3],int starter,int currplayer, int tttcord[2],
                                                                         int currmove, int win){
    if (( trial || (win==-2  && currplayer==1)) ){        
        random_move(grid, tttcord);
    }
    else{
        int moves_remaining= 9-currmove;
        //finding all possible next moves
        
        int next_moves[moves_remaining][2]; //all possible next moves

        int sum[moves_remaining];  initialise_zero(sum, moves_remaining);
        int frequency[moves_remaining];  initialise_zero(frequency, moves_remaining);
              

        remaining(gamerecord, currmove, next_moves); 
        if(see_the_calc){
            printgamedata(gamerecord);
            print_nx2_list(moves_remaining, next_moves); 
        }

        //----------reading        
        int no_of_useful=0;
        FILE * file = fopen(readfile, "r");      
        while(!feof(file)){ 

            //seeing if a row is useful.
            //prev currmove number of elements must be same.
            int useful=1;

            struct game_data row= read_next_game_from_file(file);  //a row is a full game            
            for(int i=0;i<currmove;i++){
                if(!(row.game[i].pos[0]==gamerecord.game[i].pos[0]  &&  
                                row.game[i].pos[1]==gamerecord.game[i].pos[1])){
                    useful=0;
                    break;
                }
            }

            

            if(useful){  //find weighted sums and frequencies of all useful rows.              
                no_of_useful++;
                int pos=find((row.game[currmove].pos), next_moves, moves_remaining);   
                sum[pos]+= weight(row.win,starter)*(9-row.no_of_moves+1);
                frequency[pos]++;                
            }
        }
        fclose(file);
        if(see_the_calc) printf("file close, useful data collected");

        if(!no_of_useful){ //there are no useful rows
            random_move(grid, tttcord);
        }

        float averages[moves_remaining];

        if(see_the_calc) printf("averages:\n");
        for(int i=0;i<moves_remaining;i++){
            if(frequency[i]>0){
                averages[i]= ((float)sum[i])/frequency[i];
            }else{
                averages[i]=1-number;
            }

            if(see_the_calc) {
                printf("(%d, %d), %f\n", next_moves[i][0], next_moves[i][1], averages[i] );
            }
        }
        

        
        int  k=max_coord(moves_remaining, averages);

        if(see_the_calc){
            printf("%d\n",k);
            print_nx2_list(sizeof(next_moves)/sizeof(next_moves[0]), next_moves);
            printf("best average:(%d, %d), %f\n", next_moves[k][0], next_moves[k][1], averages[k] );
        }

        if(averages[k]>-moves_remaining){
            tttcord[0]=next_moves[k][0];
            tttcord[1]=next_moves[k][1];
        }else{
            random_move(grid,tttcord);
        }
                 
    }
}


//mode
char modesel( int game_no){
    //chooses mode on a separate window     
    
    //DEFINING NEW WINDOW POINTER
    //displaying greeting for first game and "play again" next time
    SDL_Window *mode_window= SDL_CreateWindow((game_no? play_again : ttt), 
             SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w,h,SDL_WINDOW_ALLOW_HIGHDPI);
    
    //IF WINDOW DOESNT OPEN SHOW A NICE MESSAGE
    if(NULL==mode_window){
        printf(window_fail);
        return '\0';
    }
    
    //renderer object
    SDL_Renderer *mode_renderer=NULL;
    mode_renderer = SDL_CreateRenderer(mode_window, -1, SDL_RENDERER_ACCELERATED);


    refresh(mode_renderer, black);
    put_text(small_size, mode_renderer, h-376*h/600,h-h/20,name, white);
    
    //multiplayer button
    rectangle(mode_renderer, xcoor-5,ycoor+30,width+20,height, mult1);
    rectangle(mode_renderer, xcoor+10-5,ycoor+10+30,width,height-20, mult2);
    put_text(medium_size, mode_renderer, (xcoor+width/8)-5,ycoor+height/2.5+30,mult,white);
    

    //singleplayer button
    rectangle(mode_renderer, (h-xcoor-width-5-8),ycoor+30,width+20,height, sing1);
    rectangle(mode_renderer, h-xcoor-width+10-5-8,ycoor+10+30,width,height-20, sing2);
    put_text(medium_size, mode_renderer, (h-xcoor-7*width/8)-22,ycoor+height/2.5+30,sing,white);

    /* not needed i think, can be added in future. but cross button of window suffices
    #quit button
    pygame.draw.rect(choosewin,[255,120,0],(int(h/2-width/2-13),3*ycoor,width+20,height))
    pygame.draw.rect(choosewin,[230,0,0],(int(h/2-width/2+10-13),3*ycoor+10,width,height-20))
    putmedtext("QUIT",[0,0,0],(int(h/2-width/6),int(3*ycoor+height/2.5)),choosewin)*/

    SDL_RenderPresent(mode_renderer);
       
    
    char mode;
    while (1){
        int coord[2];
        get_click_coord(mode_window, coord);
        //click on a mode    
        if ((ycoor) < coord[1] &&  coord[1]<(ycoor + height)){
            if (xcoor < coord[0] && coord[0]<(xcoor+width)){
                mode='m';
                break;
            }
            else if (  (h-xcoor-width) < coord[0]   &&   coord[0]<(h-xcoor)  ){
                mode='s';
                break;
            }
        }
        /*
        #click on exit
        elif (ycoor*3) < coord[1]<(ycoor*3+height) and (h/2-width/2) < coord[0]<(h/2+width/2):
            mode="q"
            a=False    */
    }    
    SDL_DestroyWindow(mode_window);
    SDL_Quit();
    return mode;
}

int whostarts(){
    //DEFINING NEW WINDOW POINTER

    //interface for choosing starter
    SDL_Window *player_choose_window= SDL_CreateWindow(start, 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w,h,SDL_WINDOW_ALLOW_HIGHDPI);
    
    //IF WINDOW DOESNT OPEN SHOW A NICE MESSAGE
    if(NULL==player_choose_window){
        printf(window_fail);
        return -1;
    }
    
    //renderer object
    SDL_Renderer *player_choose_renderer=NULL;
    player_choose_renderer = SDL_CreateRenderer(player_choose_window, -1, SDL_RENDERER_ACCELERATED);

    refresh(player_choose_renderer, black);
    line(player_choose_renderer, w/2,0,w/2,h, blue);

    put_text(medium_size, player_choose_renderer, h/6+8, (int)(4.6*h/10-20), user, sing1);
    put_text(medium_size, player_choose_renderer, h/6-8, (int)(5.4*h/10-20), starts, sing1);
    put_text(medium_size, player_choose_renderer, 2*h/3-35,(int)(4.6*h/10-20), comp, sing1);
    put_text(medium_size, player_choose_renderer, 2*h/3-8, (int)(5.4*h/10-20), starts, sing1);
    
    SDL_RenderPresent(player_choose_renderer);

    int player;
    

    int coord[2];
    get_click_coord(player_choose_window, coord);
    
    //click on a mode    
    if (0 < coord[0] && coord[0]< (h/2)){
        //player starts
        player=2;  
    }          
    else{
        player=1;
    }   
    
    SDL_DestroyWindow(player_choose_window);
    SDL_Quit();
    return player;
}

//play

int game(char mode){  
    //mode ='m'  means multiplayer, 's' means singleplayer

    //in multiplayer first click gets cross. 
    //value of "player" will be 2 for "zero" and 1 for cross
    int player=1;
    if(mode=='s'){
        player=whostarts();
    }

    int player0=player;

    //DEFINING NEW WINDOW POINTER
    SDL_Window *game_window= SDL_CreateWindow((mode=='s'? play_single: play_multi), 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w,h,SDL_WINDOW_ALLOW_HIGHDPI);

    //renderer object
    SDL_Renderer *game_renderer=NULL;
    game_renderer = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED);

    if(game_window==NULL){
        printf(window_fail);
        return 1;
    }

    //clean the screen
    refresh(game_renderer,black);
    matrix(game_renderer);

    //1st element will later be changed to reflect who won. next 9 are to hold coordinates    
    struct game_data gamerecord;
    
    //grid where positions of x, o are stored                                   
    int grid[3][3]={{0,0,0},{0,0,0},{0,0,0}};

    
    //win =0 is draw, 1 is cross(comp) win, 2 is zero(user) win 
    //-1 : game is not over, -2: game has not started yet
    int win=-2;



    int counter=0;
    while (win<=-1){
        int yx[2];
        if (player==1){ 
            if(mode=='s'){
                db_move(gamerecord, grid, player0,player, yx, counter, win);                
            }else{
                input_move(grid,game_window, yx);
            }
            cross(game_renderer, grid, yx, mode, crosscolour);            
            player=2;
        }
        else if(player==2){
            input_move(grid,game_window, yx);
            zero(game_renderer, grid, yx,zerocolour);
            player=1;
        }
        gamerecord.game[counter].pos[0]=yx[0];
        gamerecord.game[counter].pos[1]=yx[1];
        
        counter++;

        win=decisionmaker(grid);
    }

    for(int i=counter;i<9;i++){
        gamerecord.game[i].pos[0]=-1;
        gamerecord.game[i].pos[1]=-1;
    }
    gamerecord.no_of_moves=counter;

    if (win==0 ||mode=='m'){
        gamerecord.win=win;
    }else if(win==player0){
        gamerecord.win=1;
    }else {
        gamerecord.win=2;
    }

    int played_before=storegame(gamerecord);
    winreporter(win, game_renderer, played_before);
    SDL_DestroyWindow(game_window);
    return 0;
}

void noplayer(int game_no){
    //only to be used when collecting data.
    //no graphics 

    
    struct game_data gamerecord;
    int grid[3][3]={{0,0,0},{0,0,0},{0,0,0}};
    int win=-1;
    //win =0 is draw, 1 is cross(comp) win, 2 is zero(user) win 
    //-1 : game is not over 

    int player=1;
    int counter=0;
    while (win==-1){
        int yx[2];
        random_move(grid, yx);        
        int x=yx[1],y=yx[0];
        grid[y][x]=player;
        if (player==1){          
            player=2;
        }
        else if(player==2){
            player=1;
        }
        gamerecord.game[counter].pos[0]=yx[0];
        gamerecord.game[counter].pos[1]=yx[1];
        counter++;
        win=decisionmaker(grid);
    }
    gamerecord.win=win;
    gamerecord.no_of_moves=counter;
    for(int i=counter;i<9;i++){
        gamerecord.game[i].pos[0]=-1;
        gamerecord.game[i].pos[1]=-1;
    }
    storegame(gamerecord);

    //just to make sure something is happening
    if (game_no%50==0)
        printf("%d\n",game_no);
}



int main(int argv, char ** args){
    int game_no=0;

    if(playingfordata){
            while(++game_no) noplayer(game_no);
    }

    //INITIALISING SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
       
    //THE MAIN LOOP. THE CODE CLOSES WHEN THE TOP RIGHT CROSS(SDL_QUIT) IS PRESSED.    
    while(++game_no){        
        //new game
        char mode=modesel(game_no-1);            
        game(mode);
        
    }    
    SDL_Quit();
    TTF_Quit();
    return EXIT_SUCCESS;
}
