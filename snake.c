#include<graph.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

typedef struct Coord Coord;
typedef struct Ecran Ecran;
typedef struct GameInfo GameInfo;

struct Ecran {
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    int GAME_WIDTH;
    int GAME_HEIGHT;
    int GRID_WIDTH;
    int GRID_HEIGHT;
    int GRID_SIZE;
};

struct Coord {
   int x;
   int y;
   int coche;
};

struct GameInfo {
    Ecran screen;
    int taille_serpent;
    int taille_depart_serpent;
    int nb_pastille;
    int nb_depart_pastille;
    int nb_obstacle;
    int nb_depart_obstacle;
    int niveau;
    int score;
    int xinit;
    int yinit;
    int vitesse;
};

void fermer_jeux();
int aggrandissement_taille(Coord* tab[], int taille, int nb_plus);
int regle(Coord pastille[], Coord serpent[], Coord obstacle[], int nb_pastille, int nb_obstacle, Ecran screen, int taille);
void generation(Coord serpent[], Coord pastille[], Coord obstacle[],int taille, int nb_pastille, int nb_obstacle, int xinit, int yinit, Ecran screen);
void changecoord(Coord tab[], int taille, int deplace);
int deplacement(KeySym touche, int lastdeplace);
void remplisssage(Coord tab[], int taille, int size, couleur c);
void initial(Coord tab[], int taille);
GameInfo new_level(Coord* pastille, Coord* obstacle, Coord* serpent, GameInfo gameinfo);
void game_over(GameInfo gameinfo, Coord* serpent[], Coord* pastille[], Coord* obstacles[]);
void game(GameInfo gameinfo);
void menu(struct GameInfo gameinfo);


void fermer_jeux() {
    FermerGraphique();
}

int aggrandissement_taille (Coord* tab[], int taille, int nb_plus) {
    *tab = realloc(*tab,sizeof(Coord) * (taille+nb_plus));
    taille ++;
    return taille;
}

int regle(Coord pastille[], Coord serpent[], Coord obstacle[], int nb_pastille, int nb_obstacle, Ecran screen, int taille) {
    //Si le serpent mange une pastille
    for(int i = 0; i < nb_pastille; ++i)
        if(pastille[i].x == serpent[0].x && pastille[i].y == serpent[0].y) {
            pastille[i].coche = 0;
            return 2;
        }
    //Si le serpent se prend un obstacle
    for(int i = 0; i < nb_obstacle; ++i)
        if(obstacle[i].x == serpent[0].x && obstacle[i].y == serpent[0].y && obstacle[i].coche == 1)
            return 1;
    //Si le serpent se mange sois meme
    for(int i = 1; i < taille; ++i)
        if(serpent[i].x == serpent[0].x && serpent[i].y == serpent[0].y)
            return 1;
    //Si le serpent depasse les limites de la zone de jeu
    if(serpent[0].x > screen.GRID_WIDTH || serpent[0].y > screen.GRID_HEIGHT || serpent[0].x <=0 || serpent[0].y <=0)
        return 1;

    //Si le serpent a mangé tout les pastilles
    int count = 0;
    for(int i = 0; i < nb_pastille; ++i)
        if(pastille[i].coche == 0)
            count++;

    if(count == nb_pastille)
        return 3;
    return 0;
}

void generation (Coord serpent[], Coord pastille[], Coord obstacle[],int taille, int nb_pastille, int nb_obstacle, int xinit, int yinit, Ecran screen) {
    srand(time(NULL) );
    //Placement serpent centre terrain
    for(int i = 0; i < taille; ++i, ++yinit) {
        serpent[i].x = xinit;
        serpent[i].y = yinit;
    }
    //Placement pastille
    for(int i = 0; i < nb_pastille; ++i) {
        while(pastille[i].x <= 1 || pastille[i].y <= 1 || pastille[i].x >= screen.GRID_WIDTH || pastille[i].y >= screen.GRID_HEIGHT) {
            pastille[i].x=rand()%screen.GRID_WIDTH;
            pastille[i].y=rand()%screen.GRID_HEIGHT;
        }
        for(int j = 0; j < taille; ++j)
            while (pastille[i].x == serpent[j].x && pastille[i].y == serpent[j].y) {
                pastille[i].x=rand()%screen.GRID_WIDTH;
                pastille[i].y=rand()%screen.GRID_HEIGHT;
            }
    }
    //Placement obstacle
    for(int i = 0; i < nb_obstacle; ++i) {
        while(obstacle[i].x <= 0 || obstacle[i].y <= 0 || obstacle[i].x >= screen.GRID_WIDTH || obstacle[i].y >= screen.GRID_HEIGHT) {
            obstacle[i].x=rand()%screen.GRID_WIDTH;
            obstacle[i].y=rand()%screen.GRID_HEIGHT;
        }
        for(int j = 0; j < taille; ++j)
            while(obstacle[i].x == serpent[j].x && obstacle[i].y == serpent[j].y) {
                obstacle[i].x=rand()%screen.GRID_WIDTH;
                obstacle[i].y=rand()%screen.GRID_HEIGHT;
            }
    }
}

void changecoord(Coord tab[], int taille, int deplace) {
    for (int i = taille-1; i > 0; --i)
        tab[i]=tab[i-1];

    if (deplace == 1)
        tab[0].y -= 1;

    else if (deplace == 2)
        tab[0].x -= 1;

    else if (deplace == -1)
        tab[0].y += 1;

    else if (deplace == -2)
        tab[0].x += 1;
}

int deplacement(KeySym touche, int lastdeplace) {
    int deplace;

    if (touche == XK_Up)
        deplace = 1;

    else if (touche == XK_Left)
        deplace = 2;

    else if (touche == XK_Down)
        deplace = -1;

    else if (touche == XK_Right)
        deplace = -2;

    if (lastdeplace == -deplace)
        return lastdeplace;

    return deplace;
}

void remplisssage(Coord tab[], int taille, int size, couleur c) {
    ChoisirCouleurDessin(c);

    for (int i = 0; i < taille; ++i)
        if (tab[i].coche == 1)
            RemplirRectangle(tab[i].x*size,tab[i].y*size,size,size);
}

void initial(Coord tab[], int taille) {
    //Rendre les cases dessinable
    for (int i = 0; i < taille ; ++i)
        tab[i].coche = 1;
}

GameInfo new_level(Coord* pastille, Coord* obstacle, Coord* serpent, GameInfo gameinfo) {
    initial(serpent, gameinfo.taille_serpent);
    initial(pastille, gameinfo.nb_pastille);
    initial(obstacle, gameinfo.nb_obstacle);
    gameinfo.niveau++;
    generation(serpent, pastille, obstacle, gameinfo.taille_serpent, gameinfo.nb_pastille, gameinfo.nb_obstacle, gameinfo.xinit, gameinfo.yinit, gameinfo.screen);
    return gameinfo;
}

void game_over (GameInfo gameinfo, Coord* serpent[], Coord* pastille[], Coord* obstacles[]) {
    free(*pastille);
    free(*serpent);
    free(*obstacles);

    couleur b = CouleurParNom("black");
    EffacerEcran(b);
    couleur m=CouleurParNom("red");
    ChoisirCouleurDessin(m);
    EcrireTexte(gameinfo.screen.SCREEN_WIDTH/2, 90, "GAME OVER", 2);
    m=CouleurParNom("white");
    ChoisirCouleurDessin(m);
    char scorec[9999];
    char sniveau[9999];

    snprintf(scorec, 100, "Score : %2d", gameinfo.score);
    snprintf(sniveau, 100, "Niveau : %2d", gameinfo.niveau);
    EcrireTexte(gameinfo.screen.SCREEN_WIDTH/2, 190, scorec, 2);
    EcrireTexte(gameinfo.screen.SCREEN_WIDTH/2, 220, sniveau, 2);
    EcrireTexte(gameinfo.screen.SCREEN_WIDTH/2, 330, "Press enter to restart a game", 1);
    EcrireTexte(gameinfo.screen.SCREEN_WIDTH/2, 400, "Press Esc to quit the game", 1);
    KeySym touche = XK_A;
    CopierZone(1, 0, 0, 0, gameinfo.screen.SCREEN_WIDTH, gameinfo.screen.SCREEN_HEIGHT, 0, 0);

    while(1) {
        if(ToucheEnAttente()) {
            touche = Touche();
        }
        if (touche == XK_Return) {
            game(gameinfo);
            break;
        }
        else if(touche == XK_Escape) {
            fermer_jeux();
            break;
        }
    }
}

void game (GameInfo gameinfo) {
    gameinfo.taille_serpent = gameinfo.taille_depart_serpent;
    gameinfo.nb_pastille = gameinfo.nb_depart_pastille;
    gameinfo.nb_obstacle = gameinfo.nb_depart_obstacle;
    gameinfo.niveau = 1;
    gameinfo.score = 0;
    gameinfo.vitesse = 100000;

    int deplace = 1, suivant= Microsecondes(), test = 0;

    char scorec[9999];
    char sniveau[9999];

    gameinfo.xinit = gameinfo.screen.GRID_WIDTH/2;
    gameinfo.yinit = gameinfo.screen.GRID_HEIGHT/2;

    //generation serpent
    Coord* serpent = malloc(sizeof(Coord) * gameinfo.taille_serpent);

    //generation pastilles
    Coord* pastille = malloc(sizeof(Coord)* gameinfo.nb_pastille);

    //generation obstacles
    Coord* obstacles = malloc(sizeof(Coord) * gameinfo.nb_obstacle);

    generation(serpent, pastille, obstacles, gameinfo.taille_serpent, gameinfo.nb_pastille, gameinfo.nb_obstacle, gameinfo.xinit, gameinfo.yinit, gameinfo.screen);

    initial(pastille, gameinfo.nb_pastille);
    initial(serpent, gameinfo.taille_serpent);
    initial(obstacles, gameinfo.nb_obstacle);

    couleur c;
    couleur b = CouleurParNom("black");

    KeySym touche = XK_Up;

    while (1) {
        int suivant1 = Microsecondes();
        if(suivant1-suivant< gameinfo.vitesse) {
            continue;
        }
        suivant= Microsecondes();
        EffacerEcran(b);
        ChoisirCouleurDessin(CouleurParComposante(0, 219, 47));
        RemplirRectangle(gameinfo.screen.GRID_SIZE, gameinfo.screen.GRID_SIZE, gameinfo.screen.GAME_WIDTH, gameinfo.screen.GAME_HEIGHT);
        c = CouleurParNom("white");
        ChoisirCouleurDessin(c);

        EcrireTexte(10, gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, "Score: ", 1 );
        sprintf(scorec, "%04d", gameinfo.score);
        EcrireTexte(10 + TailleChaineEcran("Score: ", 1), gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, scorec, 1);
        EcrireTexte(gameinfo.screen.SCREEN_WIDTH - 130, gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, "Niveau: ", 1);
        sprintf(sniveau, "%04d", gameinfo.niveau);
        EcrireTexte(gameinfo.screen.SCREEN_WIDTH - 130 + TailleChaineEcran("Niveau: ", 1), gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, sniveau, 1);

        c = CouleurParNom("yellow");
        remplisssage(serpent, gameinfo.taille_serpent, gameinfo.screen.GRID_SIZE, c);
        c = CouleurParNom("red");
        remplisssage(pastille, gameinfo.nb_pastille, gameinfo.screen.GRID_SIZE, c);
        c = CouleurParNom("black");
        remplisssage(obstacles, gameinfo.nb_obstacle, gameinfo.screen.GRID_SIZE, c);

        if(ToucheEnAttente())
            touche=Touche();

        if(touche != XK_space)
            test = regle(pastille, serpent, obstacles, gameinfo.nb_pastille, gameinfo.nb_obstacle, gameinfo.screen, gameinfo.taille_serpent);
        else {
            touche= XK_Return;
            while(touche != XK_space) {
                if(ToucheEnAttente())
                    touche = Touche();
            }
            if(deplace == 1) {
                touche = XK_Up;
            }
            else if(deplace == 2) {
                touche = XK_Right;
            }
            else if(deplace == -1) {
                touche = XK_Down;
            }
            else if(deplace == -2) {
                touche = XK_Left;
            }
        }

        deplace = deplacement(touche, deplace);
        changecoord(serpent, gameinfo.taille_serpent, deplace);

        //Si le serpent a mangé une pastille
        if(test == 2) {
            gameinfo.taille_serpent = aggrandissement_taille(&serpent, gameinfo.taille_serpent, 1);
            gameinfo.score += 5;
        }
        //Si le serpent a fini le niveau
        if(test == 3) {
            gameinfo.taille_serpent = gameinfo.taille_depart_serpent;
            serpent = realloc(serpent, sizeof(Coord) * gameinfo.taille_serpent);

            gameinfo.nb_pastille = aggrandissement_taille(&pastille, gameinfo.nb_pastille, 1);
            gameinfo.nb_obstacle = aggrandissement_taille(&obstacles, gameinfo.nb_obstacle, 1);
            gameinfo = new_level(pastille, obstacles, serpent, gameinfo);

            EffacerEcran(b);
            ChoisirCouleurDessin(CouleurParComposante(0, 219, 47));
            RemplirRectangle(gameinfo.screen.GRID_SIZE, gameinfo.screen.GRID_SIZE, gameinfo.screen.GAME_WIDTH, gameinfo.screen.GAME_HEIGHT);
            c = CouleurParNom("white");
            ChoisirCouleurDessin(c);

            EcrireTexte(10, gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, "Score: ", 1);
            sprintf(scorec, "%04d", gameinfo.score);
            EcrireTexte(10 + TailleChaineEcran("Score: ", 1), gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, scorec, 1);
            EcrireTexte(gameinfo.screen.SCREEN_WIDTH - 130, gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, "Niveau: ", 1);
            sprintf(sniveau, "%04d", gameinfo.niveau);
            EcrireTexte(gameinfo.screen.SCREEN_WIDTH - 130 + TailleChaineEcran("Niveau: ", 1), gameinfo.screen.SCREEN_HEIGHT - gameinfo.screen.GRID_SIZE, sniveau, 1);

            c = CouleurParNom("yellow");
            remplisssage(serpent, gameinfo.taille_serpent, gameinfo.screen.GRID_SIZE, c);
            c = CouleurParNom("red");
            remplisssage(pastille, gameinfo.nb_pastille, gameinfo.screen.GRID_SIZE, c);
            c = CouleurParNom("black");
            remplisssage(obstacles, gameinfo.nb_obstacle, gameinfo.screen.GRID_SIZE, c);

            CopierZone(1, 0, 0, 0, gameinfo.screen.SCREEN_WIDTH, gameinfo.screen.SCREEN_HEIGHT, 0, 0);

            deplace = 1;

            touche = XK_Up;

            while(touche != XK_space) {
                if(ToucheEnAttente())
                    touche = Touche();
            }
            gameinfo.vitesse -= 5000;
            test = 0;
        }

        //Si le serpent à perdu
        if(test == 1) {
            game_over(gameinfo, &serpent, &pastille, &obstacles);
            break;
        }

        //Quitter la partie
        if(touche == XK_Escape) {
            menu(gameinfo);
            break;
        }

        CopierZone(1, 0, 0, 0, gameinfo.screen.SCREEN_WIDTH, gameinfo.screen.SCREEN_HEIGHT, 0, 0);
    }
}

void menu(struct GameInfo gameinfo) {
    couleur c = CouleurParNom("black");
    EffacerEcran(c);

    c = CouleurParNom("white");
    ChoisirCouleurDessin(c);
    EcrireTexte(gameinfo.screen.SCREEN_WIDTH / 2, 18 * gameinfo.screen.GRID_SIZE, "SNAKE", 2);
    EcrireTexte(gameinfo.screen.SCREEN_HEIGHT / 3, 21 * gameinfo.screen.GRID_SIZE, "Press enter to start the game", 2);
    EcrireTexte(gameinfo.screen.SCREEN_HEIGHT / 3, 24 * gameinfo.screen.GRID_SIZE,  "Press Esc to quit the game", 2);
    EcrireTexte(gameinfo.screen.SCREEN_HEIGHT / 3, 27 * gameinfo.screen.GRID_SIZE,  "Press P to change settings", 2);
    KeySym touche = XK_A;
    CopierZone(1, 0, 0, 0, gameinfo.screen.SCREEN_WIDTH, gameinfo.screen.SCREEN_HEIGHT, 0, 0);


    while(1) {
        if(ToucheEnAttente())
            touche = Touche();
        if(touche == XK_Return)
            break;
        if(touche == XK_Escape)
            return;
        if(touche == XK_p){
            //TODO parametres
        }
    }
    game(gameinfo);
}

int main(int argc, char const *argv[]) {
    ChoisirEcran(1);

    GameInfo gameinfo;
    gameinfo.taille_depart_serpent = 10;
    gameinfo.nb_depart_obstacle = 0;
    gameinfo.nb_depart_pastille = 5;
    gameinfo.niveau = 1;
    gameinfo.score = 0;
    gameinfo.screen.SCREEN_WIDTH = 600;
    gameinfo.screen.SCREEN_HEIGHT = 400;
    gameinfo.screen.GRID_SIZE = 10;
    gameinfo.screen.GAME_WIDTH = gameinfo.screen.SCREEN_WIDTH - 2 * gameinfo.screen.GRID_SIZE;
    gameinfo.screen.GAME_HEIGHT = gameinfo.screen.SCREEN_HEIGHT - 5 * gameinfo.screen.GRID_SIZE;
    gameinfo.screen.GRID_WIDTH = gameinfo.screen.GAME_WIDTH / gameinfo.screen.GRID_SIZE;
    gameinfo.screen.GRID_HEIGHT = gameinfo.screen.GAME_HEIGHT / gameinfo.screen.GRID_SIZE;

    InitialiserGraphique();
    CreerFenetre((Maxx() - gameinfo.screen.SCREEN_WIDTH) / 2, (Maxy() - gameinfo.screen.SCREEN_HEIGHT) / 2, gameinfo.screen.SCREEN_WIDTH, gameinfo.screen.SCREEN_HEIGHT);
    menu(gameinfo);

    return 0;
}
