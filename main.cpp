#include <iostream>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

ifstream finHighscore;
ofstream foutHighscore;

void updateGame (float time);
void drawGame();
void updateMenu();
void drawMenu();
void init();
void initGame();
void cleanup();
void addArrow(float x, float y, float speed, int dist);
void standardHBx();
void collisions();
void gameOverFct();
void updateArrows(float time);

struct hitbox
{
    int x;
    int y;
    int l;
    int r;
    int t;
    int b;
    int type;
    int index;
    bool isAlive;
};

void testCollisions(hitbox& hitBox1,hitbox& hitBox2);
void procesCollision(hitbox& hitBox1,hitbox& hitBox2);
int addHitbox(int x,int y, float l, float r, float t, float b,int type,int index);

struct arrow
{
    float x,y;
    float speed;
    sf::Sprite sprite;
    int remainingDist;
    int hitboxIdx;
    bool isAlive;
};

struct minion
{
    float x,y;
    sf::Sprite sprite;
    float health;
    int type;
    bool isAlive;
    int index;
    int hitboxIdx;
};

void addMinionCanon (int lane);
void addMinionSoldier (int lane);
void addMinionWizard (int lane);
void spawnMinions();
void insertMinion(minion newMinion);
void newMinionBody (float x, float y);
struct spawner
{
    int target;
    float cooldown;
};

struct minionBody
{
    float x;
    float y;
    bool isAlive;
    sf::Sprite sprite;
    int index;
    int hitboxIdx;
};

spawner addSpawner(int target,float cooldown);

sf::RenderWindow window;
sf::Texture teemoTex;
sf::Sprite teemoSpr;
sf::Texture arrowTex;
sf::Clock aCooldownClock;
sf::Clock mCooldownClock;
sf::Sprite BackgroundSpr;
sf::Texture BackgroundTex;
sf::Texture min1Tex;
sf::Texture min2Tex;
sf::Texture min3Tex;
sf::Texture theEndTex;
sf::Sprite theEndSpr;

sf::Texture menu1NoGlowTex;
sf::Sprite menu1NoGlowSpr;

sf::Sprite menu1GlowSpr;
sf::Texture menu1GlowTex;

sf::Texture menu2GlowTex;
sf::Sprite menu2GlowSpr;

sf::Texture menu2NoGlowTex;
sf::Sprite menu2NoGlowSpr;

sf::Texture menu3NoGlowTex;
sf::Sprite menu3NoGlowSpr;

sf::Texture menu3GlowTex;
sf::Sprite menu3GlowSpr;

sf::Texture flowerTex;
sf::Sprite flowerSpr;

sf::Text kills;
sf::Font roboto;
sf::Text highscore;
/*
sf::Text play;
sf::Text Exit;
*/

int menus=1;
float timeInUpdate;
int teemoSpeed=300;
int resX=1024;
int resY=575;
int tASpeed=600;
int minionSpeed=65;
int minionCount;
bool gameOver;
int ecran=0;
float minionCooldown=1;
float minionBodyScale=0.3;
float teemoX;
float teemoY;
float oldTeemoX;
float oldTeemoY;
float teemoScale=0.08;
float arrowScale=0.5;
float minionScale=0.5;
bool openGame;
bool exitGame;
int nrMenus=2;

vector<arrow> arrows;
vector<hitbox> hitBoxes;
vector<minion> minions;
queue<spawner> spawners;
vector<minionBody> minionBodys;

int main()
{
    sf::Clock clock;

    init();
    clock.restart();    //a trecut deja niste timp
    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (ecran==0)
        {
            openGame=0;
            updateMenu();
            if (openGame==1)
                {initGame();ecran=1;minionCount=0;}
            if (exitGame==1)
                window.close();
            drawMenu();
        }


        if (ecran==1)
        {
            gameOver=0;
            updateGame(clock.restart().asSeconds());
            if (gameOver==1)
                {ecran=0;}
            else
                drawGame();
            //cout << "da";
        }

    }
    //cout << "Felicitri ai omoat " << minionCount << " de minioni!!!";
    return 0;
}


void init()
{
    window.create(sf::VideoMode(resX,resY),"LoL2D");
    window.setFramerateLimit(120);

    kills.setCharacterSize(50);
    kills.setPosition(500,100);
    kills.setFont(roboto);
    kills.setFillColor(sf::Color::Black);

    highscore.setCharacterSize(50);
    highscore.setPosition(500,250);
    highscore.setFont(roboto);
    highscore.setFillColor(sf::Color::Black);

    /*play.setCharacterSize(50);
    play.setPosition(100,100);
    play.setFont(roboto);
    play.setFillColor(sf::Color::)
    */
    roboto.loadFromFile("fonts/Roboto-Black.ttf");

    teemoTex.loadFromFile("img/teemo.png");

    arrowTex.loadFromFile("img/teemoArrow.png");

    BackgroundTex.loadFromFile("img/map_resized.png");

    min1Tex.loadFromFile("img/minion-canon.png");
    min2Tex.loadFromFile("img/minion-soldier.png");
    min3Tex.loadFromFile("img/minion-wizard.png");

    menu1GlowTex.loadFromFile("img/menu1Glow.png");
    menu1NoGlowTex.loadFromFile("img/menu1NoGlow.png");
    menu2GlowTex.loadFromFile("img/menu2Glow.png");
    menu2NoGlowTex.loadFromFile("img/menu2NoGlow.png");

    flowerTex.loadFromFile("img/flower.png");
    flowerSpr.setTexture(flowerTex);

    teemoSpr.setOrigin(320,320);

    BackgroundSpr.setTexture(BackgroundTex);

    teemoSpr.setTexture(teemoTex);
    teemoSpr.setScale(teemoScale,teemoScale);

    menu1GlowSpr.setTexture(menu1GlowTex);
    menu2GlowSpr.setTexture(menu2GlowTex);
    menu1NoGlowSpr.setTexture(menu1NoGlowTex);
    menu2NoGlowSpr.setTexture(menu2NoGlowTex);

    menu1GlowSpr.setPosition(100,100);
    menu2GlowSpr.setPosition(100,300);
    menu1NoGlowSpr.setPosition(100,100);
    menu2NoGlowSpr.setPosition(100,300);
    menu1GlowSpr.setScale(0.5,0.5);
    menu2GlowSpr.setScale(0.5,0.5);
    menu1NoGlowSpr.setScale(0.5,0.5);
    menu2NoGlowSpr.setScale(0.5,0.5);

}
void initGame()
{
    addSpawner(1,minionCooldown);
    addSpawner(2,minionCooldown);
    addSpawner(3,minionCooldown);
    addSpawner(0,3);
    addHitbox(0,0,140*teemoScale,140*teemoScale,100*teemoScale,250*teemoScale,0,0);
    standardHBx();

    BackgroundSpr.setPosition(0,0);


    flowerSpr.scale(minionBodyScale,minionBodyScale);
    teemoX=oldTeemoX=114;
    teemoY=oldTeemoY=280;


}
void updateGame (float time)
{
    if (gameOver==0)
    {

        oldTeemoX=teemoX;
        oldTeemoY=teemoY;
        float arrowCooldown=0.3;


        float teemoDist=time*teemoSpeed;
        float minionDist=time*minionSpeed;

        timeInUpdate=teemoDist;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            teemoX-=teemoDist;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            teemoX+=teemoDist;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            teemoY-=teemoDist;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            teemoY+=teemoDist;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && aCooldownClock.getElapsedTime().asSeconds()>=arrowCooldown)
        {
            //cout << "space";
            addArrow(teemoSpr.getPosition().x+40,teemoSpr.getPosition().y+10,tASpeed,150);
            aCooldownClock.restart();
        }



        updateArrows(time);

        if (teemoX<0)
            teemoX=0;
        if (teemoX>resX)
            teemoX=resX;
        if (teemoY<0)
            teemoY=0;
        if (teemoY>resY)
            teemoY=resY;

        hitBoxes[0].x=teemoX;
        hitBoxes[0].y=teemoY;

        collisions();

        teemoX=hitBoxes[0].x;
        teemoY=hitBoxes[0].y;

        if (spawners.front().cooldown<=0)
        {
            int i;
            if (spawners.front().target==1)
            {
                for (i=1; i<=3; i++)
                {
                    addMinionCanon(i);

                }
                addSpawner(1,minionCooldown);

            }
            if (spawners.front().target==2)
            {
                for (i=1; i<=3; i++)
                {
                    addMinionSoldier(i);

                }
                addSpawner(2,minionCooldown);

            }
            if (spawners.front().target==3)
            {
                for (i=1; i<=3; i++)
                {
                    addMinionWizard(i);

                }
                addSpawner(3,minionCooldown);
                addSpawner(0,3);
            }

            spawners.pop();
        }
        else
        {
            spawners.front().cooldown-=time;
        }

        for (int i=0; i<int(minions.size()); i++)
        {
            if (minions[i].isAlive==1)
            {
                minions[i].x-=minionDist;
                minions[i].sprite.setPosition(minions[i].x, minions[i].y);
                hitBoxes[minions[i].hitboxIdx].x= minions[i].x;
                hitBoxes[minions[i].hitboxIdx].y = minions[i].y;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
        {
            //cout << "hitboxes: ";
            for (int i=0; i<int(hitBoxes.size()); i++)
            {
                //cout << "x:" << hitBoxes[i].x << " y:"<< hitBoxes[i].y << " tipul:" << hitBoxes[i].type << " alive:" << hitBoxes[i].isAlive ;
                if (hitBoxes[i].type==2)
                {
                    //cout << " minion " << minions[hitBoxes[i].index].type;
                }
                //cout << endl;
            }
        }
        for (int i=0; i<int(minions.size()); i++)
        {
            if (hitBoxes[minions[i].hitboxIdx].isAlive==0&&minions[i].isAlive==1)
                //cout << " bug hitbox mort" << endl;
                if (minions[i].isAlive==1)
                {
                    hitbox hitboxV=hitBoxes[minions[i].hitboxIdx];
                    if (hitboxV.index!=i)
                    {
                        //cout << "bug index " << hitboxV.index << " trebuia " << i <<endl;
                    }
                    if (hitboxV.type!=2)
                    {
                        //cout << "bug type " << endl;
                    }
                }
        }
        for (int i=0; i<int(minions.size()); i++)
            if (minions[i].x<50&&minions[i].isAlive)
            {
                gameOverFct();
                gameOver=1;
                minions[i].isAlive=0;
                hitBoxes[minions[i].hitboxIdx].isAlive=0;
            }
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        //{
        //    gameOver=true;
        //}
    }
}

void updateMenu()
{
    std::ostringstream oss;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)==1)
    {
        //cout << "up";
        if (menus>1)
            menus--;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)==1)
    {
        //cout << "down";
        if (menus<nrMenus)
            menus++;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
    {
        //cout << "space";
        if (menus==1)
            {openGame=1;}
        if (menus==2)
            exitGame=1;
    }
    oss << "KILLS: " << minionCount;

    kills.setString(oss.str());

    int highscoreInt=0;
    finHighscore.open("highscore");
    finHighscore >> highscoreInt;
    finHighscore.close();
    //cout <<highscoreInt << " ";
    std::ostringstream os;
    os << "HIGHSCORE: "<<  highscoreInt;
    highscore.setString(os.str());

    //foutHighscore << highscoreScore;
}

void drawMenu()
{
    window.clear();
    window.draw(BackgroundSpr);
    if (menus==1)
        window.draw(menu1GlowSpr);
    else
        window.draw(menu1NoGlowSpr);
    if (menus==2)
        window.draw(menu2GlowSpr);
    else
        window.draw(menu2NoGlowSpr);
    window.draw(kills);
    window.draw(highscore);
    window.display();
}

void updateArrows(float time)
{
    for (int i=0; i<int(arrows.size()); i++)
    {
        if (arrows[i].isAlive==1)
        {
            hitBoxes[arrows[i].hitboxIdx].isAlive=1;
            arrows[i].x+=time*arrows[i].speed;
            hitBoxes[arrows[i].hitboxIdx].x=arrows[i].x;
            arrows[i].remainingDist-=time*arrows[i].speed;

            arrows[i].sprite.setPosition(arrows[i].x,arrows[i].y);
            if (arrows[i].x>resX+100||arrows[i].remainingDist<0)
            {
                hitBoxes[arrows[i].hitboxIdx].isAlive=0;
                arrows[i].isAlive=0;
            }
        }
    }

}

void addArrow (float x, float y, float speed,int dist)
{

    int i=0,ok=0;
    arrow newArrow;
    newArrow.x=x;
    newArrow.y=y;
    newArrow.sprite.setPosition(x,y);
    newArrow.speed=speed;
    newArrow.remainingDist=dist;
    newArrow.hitboxIdx=addHitbox(newArrow.x,newArrow.y,21*arrowScale,4*arrowScale,2*arrowScale,3*arrowScale,1,0);
    //newArrow.hitboxIdx=addHitbox(newArrow.x,newArrow.y,30,30,30,30,1,0);
    newArrow.isAlive=1;
    newArrow.sprite.setTexture(arrowTex);
    newArrow.sprite.setScale(arrowScale,arrowScale);
    newArrow.sprite.setOrigin(89,12);

    for (i=0; i<int(arrows.size()); i++)
        if (arrows[i].isAlive==0)
        {
            arrows[i]=newArrow;
            ok=1;
            hitBoxes[newArrow.hitboxIdx].index=i;
            break;
        }
    if (ok==0)
    {
        arrows.push_back(newArrow);
        hitBoxes[newArrow.hitboxIdx].index=arrows.size()-1;
    }

}



void drawGame()
{
    window.clear();
    window.draw (BackgroundSpr);
    teemoSpr.setPosition(teemoX,teemoY);
    for (int i=0;i<int(minionBodys.size());i++)
    {
        if (minionBodys[i].isAlive==1)
            {window.draw(minionBodys[i].sprite);}
    }
    for (int i=0; i<int(arrows.size()); i++)
    {
        if (arrows[i].isAlive==true)
            window.draw(arrows[i].sprite);

    }
    //for (int i=0;i<int(arrows.size());i++)
    //{cout << arrows[i].x<< " "<< arrows[i].y; cout << endl;}
    for (int i=0; i<int(minions.size()); i++)
        if (minions[i].isAlive==1)
            window.draw(minions[i].sprite);



    window.draw
    (teemoSpr);
    window.display();
}


int addHitbox(int  x,int y, float l, float r, float t, float b, int type,int index)
{
    hitbox newHitbox;
    newHitbox.x=x;
    newHitbox.y=y;
    newHitbox.l=l;
    newHitbox.r=r;
    newHitbox.t=t;
    newHitbox.b=b;

    newHitbox.isAlive=1;
    newHitbox.type=type;
    newHitbox.index=index;

    for (int i=0; i<int(hitBoxes.size()); i++)
    {
        if (hitBoxes[i].isAlive==false )
        {
            hitBoxes[i]=newHitbox;
            return i;
        }
    }
    hitBoxes.push_back(newHitbox);
    return hitBoxes.size()-1;
}

void standardHBx()
{
    //0-teemo 1-arrow 2-monster 3-wall 4-minionBody
    //65,575
    addHitbox(0,0,0,40,0,575,3,0);
    //stanga drumulu lui teemo;

    addHitbox(64,0,0,1000,0,37,3,0);
    //partea de sus a top-ului

    //addHitbox(900,0,0,181,0,575,3,0);
    //laterala dreapta a imaginii

    addHitbox(64,539,0,1000,0,40,3,0);
    //partea de jos a bot-ului

    addHitbox(124,102,0,900,0,140,3,0);
    //intre top si mid

    addHitbox(124,328,0,900,0,140,3,0);
    //intre mid si bot

    //addHitbox(0,0,0,1024,0,575,3,0);
}

void collisions()
{
    int i;
    int j;
    for (i=0; i<int(hitBoxes.size())-1; i++)
    {
        for (j=i+1; j<int(hitBoxes.size()); j++)
        {
            testCollisions(hitBoxes[i],hitBoxes[j]);
        }
    }
}

void testCollisions(hitbox& hitBox1,hitbox& hitBox2)
{
    float l1=hitBox1.x-hitBox1.l,l2=hitBox2.x-hitBox2.l,r1=hitBox1.x+hitBox1.r,r2=hitBox2.x+hitBox2.r;
    float t1=hitBox1.y-hitBox1.t,t2=hitBox2.y-hitBox2.t,b1=hitBox1.y+hitBox1.b,b2=hitBox2.y+hitBox2.b;

    if (hitBox1.type==hitBox2.type) return;//false;

    if (hitBox1.isAlive==false||hitBox2.isAlive==false) return;

    if (hitBox1.type==0&&hitBox2.type==1) return;// false;

    if (hitBox2.type==0&&hitBox1.type==1) return;// false

    if (hitBox1.type==3&&hitBox2.type==2) return;

    if ((((l1>=l2)&&(l1<=r2))||((r1<=r2)&&(r1>=l2))||((l2>=l1)&&(l2<=r1))||((r2<=r1)&&(r2>=l1)))  &&
            (((t1>=t2)&&(t1<=b2))||((b1>=t2)&&(b1<=b2))||((t2>=t1)&&(t2<=b1))||((b2>=t1)&&(b2<=b1))))
    {
        //cout << t1 << " " << t2 << " " << b1 << " "<< b2<< endl;
        procesCollision(hitBox1,hitBox2);
    }
    return;// true;
}

void procesCollision(hitbox& hitBox1,hitbox& hitBox2)
{
    //cout<< "da "<<hitBox1.type<<" " << hitBox2.type <<endl;
    if (hitBox1.type==0&&hitBox2.type==3)
    {
        hitBox1.x=oldTeemoX;
        hitBox1.y=oldTeemoY;

    }
    if (hitBox1.type==1&&hitBox2.type==3)
    {
        //cout << "da "<< endl;
        arrows[hitBox1.index].isAlive=false;
        hitBox1.isAlive=false;
    }
    if (hitBox1.type==3&&hitBox2.type==1)
    {
        //cout << "da "<< endl;
        arrows[hitBox2.index].isAlive=false;
        hitBox2.isAlive=false;
    }
    //cout << "da "<< hitBox1.type << " " << hitBox2.type << " " << hitBox1.x << " " << hitBox1.y << "   " << hitBox2.x << " " << hitBox2.y << endl;
    if (hitBox1.type==1&&hitBox2.type==2)
    {
        minionCount++;
        //arrows[hitBox1.index].isAlive=false;
        //hitBox1.isAlive=false;
        minions[hitBox2.index].isAlive=false;
        hitBox2.isAlive=false;
        newMinionBody(hitBox2.x,hitBox2.y);

        //cout << "da"<< endl;
    }
    if (hitBox1.type==2&&hitBox2.type==1)
    {
        minionCount++;
        minions[hitBox1.index].isAlive=false;
        hitBox1.isAlive=false;
        //arrows[hitBox2.index].isAlive=false;
        //hitBox2.isAlive=false;
        newMinionBody(hitBox1.x,hitBox1.y);
        //cout << "da"<< endl;
    }
    if (hitBox1.type==0&&hitBox2.type==4)
    {
        hitBox2.isAlive=false;
        minionBodys[hitBox2.index].isAlive=false;
        teemoSpeed+=1;
    }
    if (hitBox1.type==4&&hitBox2.type==0)
    {
        hitBox1.isAlive=false;
        minionBodys[hitBox1.index].isAlive=false;
        teemoSpeed+=1;
    }
}

void addMinionCanon (int lane)
{
    minion newMinion;
    newMinion.sprite.setTexture(min1Tex);
    newMinion.x=1024;

    if (lane==1)
        newMinion.y=67;
    if (lane==2)
        newMinion.y=282;
    if (lane==3)
        newMinion.y=500;

    newMinion.sprite.setOrigin(100,100);
    newMinion.isAlive=1;
    newMinion.type=1;
    newMinion.sprite.setPosition(newMinion.x,newMinion.y);
    newMinion.sprite.scale(minionScale,minionScale);
    newMinion.hitboxIdx=addHitbox(newMinion.x,newMinion.y,minionScale*24,minionScale*24,minionScale*54,minionScale*54,2,0);

    insertMinion(newMinion);
}

void addMinionSoldier (int lane)
{
    minion newMinion;
    newMinion.sprite.setTexture(min2Tex);
    newMinion.x=1024;
    if (lane==1)
        newMinion.y=67;
    if (lane==2)
        newMinion.y=282;
    if (lane==3)
        newMinion.y=500;

    newMinion.sprite.setOrigin(100,100);
    newMinion.isAlive=1;
    newMinion.type=2;
    newMinion.sprite.scale(minionScale,minionScale);
    newMinion.sprite.setPosition(newMinion.x,newMinion.y);
    newMinion.hitboxIdx=addHitbox(newMinion.x,newMinion.y,minionScale*24,minionScale*58,minionScale*70,minionScale*35,2,0);

    insertMinion(newMinion);
}

void addMinionWizard (int lane)
{
    minion newMinion;
    newMinion.sprite.setTexture(min3Tex);
    newMinion.x=1024;
    if (lane==1)
        newMinion.y=67;
    if (lane==2)
        newMinion.y=282;
    if (lane==3)
        newMinion.y=500;

    newMinion.sprite.setOrigin(100,100);
    newMinion.isAlive=1;
    newMinion.type=3;
    newMinion.sprite.setPosition(newMinion.x,newMinion.y);
    newMinion.sprite.scale(minionScale,minionScale);

    newMinion.hitboxIdx=addHitbox(newMinion.x,newMinion.y,minionScale*24,minionScale*24,minionScale*35,minionScale*35,2,0);


    insertMinion(newMinion);
}

spawner addSpawner(int target,float cooldown)
{
    spawner newSpawner;
    newSpawner.target=target;
    newSpawner.cooldown=cooldown;
    spawners.push(newSpawner);
    return newSpawner;
}

void insertMinion (minion newMinion)
{
    bool ok=0;
    for (int i=0; i<int(minions.size()); i++)
        if (minions[i].isAlive==0)
        {
            minions[i]=newMinion;
            ok=1;
            hitBoxes[newMinion.hitboxIdx].index=i;
            break;
        }
    if (ok==0)
    {
        minions.push_back(newMinion);
        hitBoxes[newMinion.hitboxIdx].index=minions.size()-1;
    }
}
void gameOverFct()
{
    int highscoreScore;
    minionBodys.clear();
    minions.clear();
    arrows.clear();
    hitBoxes.clear();
    //spawners.empty();
    teemoSpeed=300;
    finHighscore.open("highscore");
    finHighscore>> highscoreScore;
    finHighscore.close();
    if (minionCount>highscoreScore)
        {
            foutHighscore.open("highscore");
            foutHighscore << minionCount;
            foutHighscore.close();
        }
    else

    //minionCount=0;
    init();


}

void newMinionBody (float x, float y)
{
    minionBody newBody;
    newBody.x=x;
    newBody.y=y;
    newBody.isAlive=1;

    newBody.sprite.setTexture(flowerTex);
    newBody.sprite.setPosition(x,y);
    newBody.sprite.setOrigin(50,50);
    newBody.sprite.setScale(minionBodyScale,minionBodyScale);
    newBody.hitboxIdx=addHitbox(x+(100*minionBodyScale)/2,y+(100*minionBodyScale)/2,50*minionBodyScale,50*minionBodyScale,50*minionBodyScale,50*minionBodyScale,4,0);
    for (int i=0;i<int(minionBodys.size());i++)
    {
        if (minionBodys[i].isAlive==0)
        {

            minionBodys[i]=newBody;
            hitBoxes[newBody.hitboxIdx].index=i;
            return;
        }
    }
    minionBodys.push_back(newBody);
    hitBoxes[newBody.hitboxIdx].index=minionBodys.size()-1;
}
