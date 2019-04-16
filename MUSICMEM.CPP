#include "easy.h"
#include "musicmem.h"

#include <time.h>
#include <math.h>

Program program;

class MainWindow : public StandardWindow
{   char Text[256];
	public :
	Flag Active;
	MainWindow (int id, char *title) :
		StandardWindow(id,title,
			StandardWindow::normal_nosize|StandardWindow::menu|
			StandardWindow::min),
		Active(1)
	{   strcpy(Text,String(5));
		init();
		size(300,350);
		top();
	}
	virtual void redraw (PS &ps);
	virtual void clicked (LONG x, LONG y, clicktype type);
	void drawtext (PS &ps);
	void settext (char *s);
} window(ID_Window,String(1));

void MainWindow::drawtext (PS &ps)
{	ps.move(0,300);
	ps.backarea(300,50);
	ps.move(150,325);
	ps.text(Text,Colors::black,
		Alignment::center,Alignment::middle);
}

void MainWindow::settext (char *s)
{	strcpy(Text,s);
	WindowPS ps(*this);
	drawtext(ps);
}

Menu menu(window);

const int maxsize=512;
double duration=0.5;
int frequency[4]={220,294,330,440};

int arc[4][4]=
{	155,155,0,90,
	145,155,90,180,
	145,145,180,270,
	155,145,270,360
};

LONG darkcol[4]={CLR_DARKRED,CLR_DARKGREEN,
		CLR_DARKBLUE,CLR_BROWN},
	lightcol[4]={CLR_RED,CLR_GREEN,CLR_BLUE,CLR_YELLOW};

void dark (PS &ps, int i)
{	ps.filledpie(arc[i][0],arc[i][1],120,1,
		arc[i][2],arc[i][3],CLR_WHITE,darkcol[i]);
}

void light (PS &ps, int i)
{	ps.filledpie(arc[i][0],arc[i][1],120,1,
		arc[i][2],arc[i][3],CLR_WHITE,lightcol[i]);
}

class Clickcheck // checks for double clicks
{	int Last;
	public :
	Clickcheck () : Last(-1) {}
	void reset () { Last=-1; }
	int check (int i);
} clickcheck;

int Clickcheck::check (int i)
{	if (i==Last) return 0;
	Last=i;
	return 1;
}

class Music
{   int N[512];
	int L,W;
	Flag Mark;
	public :
	Music () : L(0),W(0),Mark(0) {}
	void reset () { L=0; W=0; clickcheck.reset(); }
	void add ();
	void play (int i);
	void play ();
	int wait (int i);
	int done () { return W>=L; }
	int length () { return L; }
	void mark ()
	{	Mark.set();
		window.Active.clear();
		program.clear_messages();
		window.Active.set();
	}
	int marking ();
	int empty () { return L==0; }
} music;

int Music::marking ()
{	if (Mark)
	{	Mark.clear();
		play();
		return 1;
	}
    return 0;
}

int Music::wait (int i)
{	int res=(N[W]==i);
	W++;
	return res;
}

void Music::add ()
{   if (L>=512) return;
	do
	{	N[L]=rand()%4;
	} while (N[L]==N[L-1] && L>0);
	L++;
	W=0;
}

void Music::play (int i)
{	WindowPS ps(window);
	dark(ps,i);
	Beep(frequency[i],duration);
	light(ps,i);
}

void Music::play ()
{	int i;
	WindowPS ps(window);
	for (i=0; i<L; i++)
	{	dark(ps,N[i]);
		Beep(frequency[N[i]],duration);
		light(ps,N[i]);
	}
	W=0;
	char text[256];
	sprintf(text,String(6),L);
	window.settext(text);
}

void MainWindow::redraw (PS &ps)
{	int i;
	ps.erase();
	ps.filledcircle(150,150,140,1,
		CLR_BLACK,CLR_DARKGRAY);
	for (i=0; i<4; i++) light(ps,i);
	drawtext(ps);
}

void MainWindow::clicked (LONG x, LONG y, clicktype type)
{	if (type!=button1up || !window.Active) return;
	if (music.empty())
	{	music.add();
		music.add();
		music.play();
		return;
	}
	if (music.marking()) return;
	x-=150; y-=150;
	if (abs(x)<5 || abs(y)<5) return;
	if (x>0) x-=5;
	else x+=5;
	if (y<0) y-=5;
	else y+=5;
	if (sqrt((double)x*x+(double)y*y)>120) return;
	int i;
	if (x>0)
		if (y>0) i=0; else i=3;
	else
		if (y<0) i=2; else i=1;
	if (!clickcheck.check(i)) return;
	if (music.wait(i))
	{   if (music.done())
		{   music.play(i);
			window.settext(String(7));
			music.add();
			music.mark();
			clickcheck.reset();
		}
		else music.play(i);
	}
	else
	{	char msg[256];
		sprintf(msg,String(2),music.length());
		Message(msg,String(1),window);
		music.play();
		if (Question(String(3),String(1),window)
				==Answers::no)
			return;
		music.reset();
		music.add();
		music.add();
		window.settext(String(5));
		music.mark();
	}
}

void doabort ()
{	window.quit();
}

void donewgame ()
{	music.reset();
	music.add();
	music.add();
	music.play();
}

void dorepeat ()
{	music.play();
}

Help help(window,ID_Help,String(4),String(1));

void dohelp ()
{	help.general();
}

int main (int argc, char *argv[])
{   if (argc==2) duration=atoi(argv[1])/1000;
	time_t t;
	srand(time(&t));
	menu.add(IDM_Help,dohelp);
	menu.add(IDM_Abort,doabort);
	menu.add(IDM_NewGame,donewgame);
	menu.add(IDM_Repeat,dorepeat);
	program.loop();
	return 0;
}

