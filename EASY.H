// We need to include a few things from OS2.H

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DEV
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_BASE
#define INCL_GPIERRORS


#include <os2.h> // Do not repeat this in your main application

// We need a few things from the C library as well.

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

class Answers
// These are the anwers for the quick Message Dialoges below
{   public :
	enum { yes=MBID_YES, no=MBID_NO, abort=MBID_CANCEL };
};

// For debugging:
void dumplong (long n); // dump a long
void dump (char *s); // dump a string

class Window;
void Warning (char *s, char *title); // Will display s and beep
void Message (char *s, char *title);
int Question (char *s, char *title); // returns Answers::yes or Answers::no
int QuestionAbort (char *s, char *title); // may return Answers::abort
void Warning (char *s, char *title, Window &w);
void Message (char *s, char *title, Window &w);
int Question (char *s, char *title, Window &w);
int QuestionAbort (char *s, char *title, Window &w);
void Beep (int frequency, double seconds);

class Parameter
// Parameter for messages are passed in 32 Bit values.
// Use this class for easy conversion from and to 32 Bit.
{	MPARAM M;
	public :
	Parameter(long m) { M=(MPARAM)m; }
	Parameter(int m1, int m2) { M=MPFROM2SHORT(m1,m2); }
	Parameter(int m) { M=MPFROMSHORT(m); }
	Parameter(void *p) { M=(MPARAM)p; }
	operator MPARAM () { return M; }
	operator LONG () { return (long)M; }
	int lower () { return SHORT1FROMMP(M); }
	int higher () { return SHORT2FROMMP(M); }
};


class Time
// Handle primitive time management.
{	double Seconds;
	public :
	void set (); // ask current time
	Time () { set(); } // initialize with current time
	double seconds () { return Seconds; } // return seconds since UNIX 0
	operator double () { set(); return Seconds; } // return current time in seconds
	void sleep (double t) { DosSleep(1000*t); } // wait for t seconds
};

class String
// Strings are used througout EASY. They are stored on heap.
{	char *P;
	long Size;
	public :
	static long defaultsize; // set String::defaultsize=... to change it
	String (); // initialized with empty default sized string
	String (char *text, long size=defaultsize);
		// initialized with text and size, or defaultsize of shorter
	String (int id); // id from Resource
		// initialized with ressource string (from STRINGTABLE)
	~String (); // free heap storage
	char *text () { return P; } // return content
	long size () { return Size-1; } // return size
	void copy (char *text, long size); // copy (get new heap space)
	void copy (char *text);
	char *filename (); // treat string as path and return file name
	void stripfilename (); // get rid of the name, make it the path
	operator char * () { return P; }
	operator PSZ () { return P; }
	int todouble (double &x); // convert to double (return TRUE on success)
	int tolong (long &n); // same with long
};

class ConvertString : public String
// Additional features: make a String of long and double
{	public :
	ConvertString (long n) : String("",32) { ltoa(n,*this,10); }
	ConvertString (double x) : String("",32)
		{ sprintf(*this,"%-0.10g",x); }
};

class Flag
// Used for various things. Can be fixed, so that changes are
// discarded.
{	int F,Fix;
	public :
	Flag (int f=0) { F=f; Fix=0; }
	// The followint should be self explaining
	void set () { if (Fix) return; F=1; }
	void clear () { if (Fix) return; F=0; }
	void toggle () { if (Fix) return; F=!F; }
	operator int () { return F; }
	void fix () { Fix=1; }
	void unfix () { Fix=0; }
	// Make Flag=1 possible.
	int operator = (int flag) { if (Fix) return flag; return F=flag; }
};

class Rectangle
// A Rectangle can have negative width or height! Used for rubberboxes.
{	LONG X,Y,W,H;
	public :
	Rectangle (LONG x=0, LONG y=0, LONG w=1, LONG h=1)
	{	X=x; Y=y; W=w; H=h;
	}
	LONG x1 ()
	{	if (W<0) return X+W+1;
		else return X;
	}
	LONG x2 ()
	{	if (W>0) return X+W-1;
		else return X;
	}
	LONG y1 ()
	{	if (H<0) return Y+H+1;
		else return Y;
	}
	LONG y2 ()
	{	if (H>0) return Y+H-1;
		else return Y;
	}
	LONG x () { return X; }
	LONG y () { return Y; }
	LONG w () { return W; }
	LONG h () { return H; }
	void resize (LONG w, LONG h)
	{	W=w; H=h;
	}
	void hrescale (double scale);
	void wrescale (double scale);
	// Rescale it to vertical /horizontal relation
	void minsize (LONG wmin, LONG hmin);
};

class Program
// This class must be in every program and the instance must be named program
{	HAB Hab;
	HMQ Hmq;
	QMSG Qmsg;
	public :
	Program();
	~Program();
	int getmessage();
	void dispatch();
	void loop(); // this is the main loop
	void clear_messages(); // dispatch all open messages
	HAB hab() { return Hab; }
	HMQ hmq() { return Hmq; }
};

extern Program program; // declare this in your main module

class PS;

const int FCF_NORMAL=
	FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MINMAX|
	FCF_SHELLPOSITION|FCF_TASKLIST|FCF_ICON;
const int FCF_NORMAL_NOSIZE=
	FCF_TITLEBAR|FCF_SYSMENU|FCF_BORDER|
	FCF_SHELLPOSITION|FCF_TASKLIST|FCF_ICON;

class Menu;

enum clicktype
// type of mouse messages
{ 	button1,button2,button3,
	button1double,button2double,button3double,
	button1up,button1down,
	button2up,button2down,
	button3up,button3down,
	mousemove
};

enum pointertype
// mouse pointer type
{	pointer_wait=SPTR_WAIT,pointer_arrow=SPTR_ARROW,
	pointer_text=SPTR_TEXT,pointer_move=SPTR_MOVE
};

class Window
// Elder class of StandardWindow.
{   protected :
	HWND Handle;
	int Width,Height;
	Flag Visible;
	HPOINTER Old,New;
	public :
	Window ();
	HWND handle () { return Handle; }
	int width () { return Width; }
	int height () { return Height; }
	void update () { WinInvalidateRect(Handle,NULL,TRUE); }
	void size (long w, long h); // call size of StandardWindow
	void pos (long x, long y); // dto.
	void validate () { WinValidateRect(Handle,NULL,TRUE); }
		// make the content valid (sometimes good after redraw)
	int visible () { return Visible; } // yes, if not iconized
	void setpointer (pointertype p); // set pointer for the window
	void resetpointer (); // old pointer
	void showpointer ();
	void hidepointer ();
	void message (int msg, Parameter mp1=0, Parameter mp2=0)
	// send a message to the window
	{	WinPostMsg(Handle,msg,mp1,mp2);
	}
	void capture (int flag=1)
	// capture mouse control, needs to be released (capture(0))
	{	WinSetCapture(HWND_DESKTOP,flag?Handle:NULLHANDLE);
	}
	void pos (long &x, long &y, long &w, long &h);
	// ask window position and size
	void quit () { message(WM_CLOSE); }
	// send quit message, best way to leave a program
};

class Desktop : public Window
{	public :
	Desktop () { Handle=HWND_DESKTOP; }
};

class Scroll
// scroll messages
{	public :
	enum {
		left=SB_LINELEFT,right=SB_LINERIGHT,
		pageleft=SB_PAGELEFT,pageright=SB_PAGERIGHT,
		position=SB_SLIDERPOSITION,
		up=SB_LINEUP,down=SB_LINEDOWN,
		pageup=SB_PAGEUP,pagedown=SB_PAGEDOWN};
};

class Alignment
// text alignments
{	public :
	enum {
		left=TA_LEFT,center=TA_CENTER,right=TA_RIGHT,
		top=TA_TOP,middle=TA_HALF,bottom=TA_BOTTOM};
};

class Keycode
// key flags
{	public :
	enum {
		up=KC_KEYUP,virtualkey=KC_VIRTUALKEY,
		charkey=KC_CHAR };
};

enum {RUBBER_ZERO,RUBBER_START,RUBBER_CANCEL,RUBBER_DONE};

class StandardWindow : public Window
// This is the normal window. No subwindows are declared.
// Just a drawing area.
{	HWND FrameHandle;
	Menu *Windowmenu; // pointer to menu
	int Id; // the resource ID
	int Rubber;
	String Name; // the title
	ULONG Flags;
	public :
	enum {normal=FCF_NORMAL,menu=FCF_MENU,keys=FCF_ACCELTABLE,
		vscrollbar=FCF_VERTSCROLL,hscrollbar=FCF_HORZSCROLL,
		titlebar=FCF_TITLEBAR,sysmemu=FCF_SYSMENU,
		sizeborder=FCF_SIZEBORDER,minmax=FCF_MINMAX,
		min=FCF_MINBUTTON,max=FCF_MAXBUTTON,
		shellposition=FCF_SHELLPOSITION,tasklist=FCF_TASKLIST,
		icon=FCF_ICON,normal_nosize=FCF_NORMAL_NOSIZE};
	// declare flags for additional controls
	StandardWindow (int id,
		char *name="",
		ULONG flags=FCF_NORMAL);
	~StandardWindow ();
	void init ();
	void setmenu (Menu *m) { Windowmenu=m; }
		// automatically called from Menu constructor
	HWND framehandle () { return FrameHandle; }
	void top (); // top the window
	int rubberbox (LONG x, LONG y, clicktype click,
		Rectangle &R, LONG wmin=0, LONG hmin=0,
		double wscale=0, double hscale=0);
		// can be called from clicked. wmin,hmin are minimal sizes.
		// either a width or a height scaling factor are optional.
		// returns RUBBER_DONE etc.
	virtual void redraw (PS &ps);
		// the redraw routine. called by message handler
	virtual void sized () {}
		// called upon resize message
	virtual void clicked (LONG x, LONG y, clicktype click) {}
		// called from message handler upon mouse events
		// clicktype is defined above
	virtual int key (int flags, int code, int scan) { return 0; }
		// called from message handler upon keyboard events
		// flags is of Keycode class
		// code is the ASCII key code, if flag is Keycode::charkey
		// scan is the scan value or the virtual key, if
		// falgs contains Keycode::virtualkey
	void size (LONG w, LONG h);
		// size the windows draw area
	virtual int vscrolled (int scroll, int pos) { return 0; }
		// called from message handler upon scroll messages
	virtual int hscrolled (int scroll, int pos) { return 0; }
	virtual void pres_changed () { update(); }
		// called from message handler, if the windows default
		// values change
	void vscroll (int pos, int min=0, int max=100);
	void hscroll (int pos, int min=0, int max=100);
		// set the scroll bars
	friend MRESULT EXPENTRY MainWindowProc (HWND hwnd,
			ULONG msg,MPARAM mp1, MPARAM mp2);
	void title (char *s); // set window title
	virtual int close () { return 1; }
		// user tries to close the window
		// return 0, if that is not allowed
	void starttimer (double interval, int i=1)
	{	WinStartTimer(program.hab(),Handle,i,interval*1000);
	}
		// start a window timer with the specified interval
		// i is the number of the timer
	void stoptimer (int i=1)
	{	WinStopTimer(program.hab(),Handle,i);
	}
	virtual void timer (int i) {}
		// called from message handler on timer events
};

typedef void Menuproc ();

class Menuentry
// Internal class to represent a menu entry
{   int Command;
	Menuproc *Proc;
	Menuentry *Next;
	public :
	Menuentry (int command, Menuproc *proc,
		Menuentry *next=NULL)
	{	Command=command;
		Proc=proc;
		Next=next;
	}
	Menuentry *next () { return Next; }
	void call (int command) { Proc(); }
	int command () { return Command; }
};

class Menu
// menus appear in a specified window
// all entries must be entered with Menu::add
{   int Command;
	Menuentry *Mp;
	StandardWindow *W;
	public :
	Menu (StandardWindow &window)
	{	Mp=NULL;
		W=&window;
		W->setmenu(this);
	}
	~Menu ();
	void add (ULONG command, Menuproc *proc)
	{   Mp=new Menuentry(command,proc,Mp);
	}
	int call (int command);
	int command () { return Command; }
		// returns the ID of the last menu selection
	void enable (int id, int flag);
		// enable or disable menu entries
	void check (int id, int flag);
		// check or uncheck menu entries
};

class Rgb
// Used to represent colors as ULONG values
{	ULONG Value;
	public :
	Rgb (int red, int green, int blue)
	{	Value=((unsigned long)red<<16)+((unsigned long)green<<8)+blue;
	}
	Rgb (ULONG value=0) { Value=value; }
	operator ULONG () { return Value; }
};

class Colors
// The default 16 color palette
{   public :
	enum
	{	def=CLR_DEFAULT,white=CLR_WHITE,black=CLR_BLACK,
		blue=CLR_BLUE,red=CLR_RED,pink=CLR_PINK,green=CLR_GREEN,
		cyan=CLR_CYAN,yellow=CLR_YELLOW,darkgray=CLR_DARKGRAY,
		darkblue=CLR_DARKBLUE,darkred=CLR_DARKRED,
		darkpink=CLR_DARKPINK,darkgreen=CLR_DARKGREEN,
		darkcyan=CLR_DARKCYAN,brown=CLR_BROWN,palegray=CLR_PALEGRAY,
		gray=CLR_PALEGRAY,neutral=CLR_NEUTRAL
	};
};

class Markers
// Marker types for PS::mark
{	public :
	enum
	{	def=MARKSYM_DEFAULT,cross=MARKSYM_CROSS,plus=MARKSYM_PLUS,
		diamond=MARKSYM_DIAMOND,star=MARKSYM_SIXPOINTSTAR,
		square=MARKSYM_SQUARE,solidsquare=MARKSYM_SOLIDSQUARE,
		soliddiamond=MARKSYM_SOLIDDIAMOND,
		sixpointstar=MARKSYM_SIXPOINTSTAR,
		eightpointstart=MARKSYM_EIGHTPOINTSTAR,
		dot=MARKSYM_DOT,circle=MARKSYM_SMALLCIRCLE,
		blank=MARKSYM_BLANK
	};
};

class Font;
class PS
// The basic interface class for the graphic routines.
// Has WindowPS, RedrawPS, PrinterPS, BitmapPS
// and MetafilePS as children.
{   POINTL P;
	ULONG Color,Alignment;
	protected :
	HPS Handle;
	SIZEL S;
	long X,Y;
	public :
	void setdefaults () // Default Initialisation
	{   X=0; Y=0;
		Handle=NULLHANDLE;
		Color=CLR_DEFAULT; Alignment=TA_LEFT;
	}
	PS ()
	{  	setdefaults();
	}
	PS (HPS hps)
	{	setdefaults(); Handle=hps; GpiQueryPS(Handle,&S);
	}
	void clip (long x, long y, long w, long h); // set clipping
	void offset (long x, long y) { X=x; Y=y; } // set zero offset
	long xoffset () { return X; } // return offset
	long yoffset () { return Y; }
	HPS handle () { return Handle; } // return PS handle
	void erase () { GpiErase(Handle); }
	LONG width () { return S.cx; }
	LONG height () { return S.cy; }
	void color (ULONG color) // set draw color
	{	if (Color!=color)
		{	GpiSetColor(Handle,color);
			Color=color;
		}
	}
	void directcolor (int pure=0);
	void setcolor (int index, Rgb rgb, int pure=0);
		// set color index to value
		// pure=1 avoids dithered colors
	void defaultcolors ();
		// reset default palette
	ULONG backcolor () { return GpiQueryBackColor(Handle); }
	void mix (int mode) { GpiSetMix(Handle,mode); }
		// set background mix mode
	void move (LONG c, LONG r, ULONG color=CLR_DEFAULT);
		// move draw position to (c,r)
	void lineto (LONG c, LONG r, ULONG color=CLR_DEFAULT);
	void linerel (LONG w, LONG h, ULONG color=CLR_DEFAULT);
	void point (LONG w, LONG h, ULONG color=CLR_DEFAULT);
	void text (char *s, ULONG color=CLR_DEFAULT,
		ULONG alignment=TA_LEFT, ULONG valignment=TA_BASE);
	void textbox (char *s, long &width, long &height);
		// compute with and height of text
	double textextend (char *s, double vx, double vy);
	void framedarea (Rectangle &R, int r, ULONG col=CLR_DEFAULT);
	void frame (Rectangle &R, int r=0, ULONG color=CLR_DEFAULT);
	void area (Rectangle &R, int r=0, ULONG color=CLR_DEFAULT);
	void framedarea (LONG w, LONG h, int r, ULONG col=CLR_DEFAULT);
	void frame (LONG w, LONG h, int r=0, ULONG color=CLR_DEFAULT);
	void area (LONG w, LONG h, int r=0, ULONG color=CLR_DEFAULT);
		// These function draw a rectangle (filled or not,
		// frame or not), r ist the corner radius for rounded
		// corners.
	void backarea (LONG w, LONG h);
	void mark (LONG w, LONG h, ULONG type=MARKSYM_DEFAULT,
		ULONG color=CLR_DEFAULT);
		// draw a marker
	void circle (LONG c, LONG r, LONG rad, double factor,
		ULONG col=CLR_DEFAULT);
	void filledcircle (LONG c, LONG r, LONG rad, double factor,
		ULONG col=CLR_DEFAULT, ULONG fillcol=CLR_DEFAULT);
		// draw an ellipse
	void arc (LONG c, LONG r, LONG rad, double factor,
		double phi1, double phi2,
		ULONG col=CLR_DEFAULT);
	void pie (LONG c, LONG r, LONG rad, double factor,
		double phi1, double phi2,
		ULONG col=CLR_DEFAULT);
	void filledpie (LONG c, LONG r, LONG rad, double factor,
		double phi1, double phi2,
		ULONG col=CLR_DEFAULT, ULONG fillcol=CLR_DEFAULT);
		// an arc
	void setfont (Font &font, int id=1);
		// set the drawing font
	void image (long w, long h, unsigned char *data);
		// draw a B/W image
};

inline void StandardWindow::redraw (PS &ps)
{	ps.erase();
}

class WindowPS : public PS
// A PS, which is assigned to a screen window
{   Window *W;
	int getps;
	void set (HPS handle, Window &window)
	{	W=&window;
		Handle=handle;
		S.cx=window.width(); S.cy=window.height();
	}
	public :
	WindowPS (HPS handle, Window &window)
	{	set(handle,window);
		getps=0;
	}
	WindowPS (Window &window)
	{	set(WinGetPS(window.handle()),window);
		getps=1;
	}
	~WindowPS () { if (getps) WinReleasePS(handle()); }
};

class RedrawPS : public WindowPS
// A PS which is generated within a redraw event
// This is only used by the message handler
{	public :
	RedrawPS (HWND hwnd, Window &window) :
		WindowPS(WinBeginPaint(hwnd,NULLHANDLE,NULL),window) {}
	~RedrawPS () { WinEndPaint(handle()); }
};

class MetafilePS : public PS
// A PS accosiciated to a meta file
// The Metafile may be passed to the clipboard
{   HMF Metafilehandle;
	HDC Hdc;
	public :
	MetafilePS (Window &window);
	~MetafilePS ();
	HMF metafilehandle () { return Metafilehandle; }
	void close ();
};

class CopyMode
{  	public :
	enum {
		copy=ROP_SRCCOPY, or=ROP_SRCPAINT, xor=ROP_SRCINVERT,
	};
};

class BitmapPS : public PS
// A PS for a bitmap assigned and sized to window or
// to the screen with own size.
{	HDC DeviceHandle;
	HBITMAP BitmapHandle;
	PBITMAPINFO2 Info;
	int Valid,Planes,Colorbits;
	public :
	BitmapPS (Window &window); // window size
	BitmapPS (long w, long h);
	~BitmapPS ();
	void copy (PS &ps, int mode=CopyMode::copy, long x=0, long y=0);
		// copy the bitmap to a PS
	void save (char *filename);
		// save the bitmap on a file
	HBITMAP bitmaphandle () { return BitmapHandle; }
};

class Queues
// Gets all queues and selects the default one or a named one.
{	ULONG NQueues;
	PRQINFO3 *Queues;
	PRQINFO3 ChosenQueue;
	public :
	Queues (char *name="");
	~Queues () { if (Queues) delete Queues; }
	ULONG number () { return NQueues; }
		// number of all queues
	PRQINFO3 *chosen () { return &ChosenQueue; }
		// the default or named queue
	PRQINFO3 *all ();
		// return all queues (number())
};

class PrinterPS : public PS
// A PS for the printer
{   HDC HandlePrinter;
	DEVOPENSTRUC Dos;
	String Myname;
	PRQINFO3 Queue;
	public :
	Flag Valid,Open;
	PrinterPS (char *name="Print", int op=1);
		// name is the output name, op=0 means: do not open
		// This will take the default queue
	PrinterPS (Queues &q, char *name="Print", int op=1);
		// The prefered way.
		// Declare a qeueu instance and pass it to the PrinterPS
	~PrinterPS () { if (Open) close(); }
	char *queuename () { return Dos.pszLogAddress; }
	char *drivername () { return Dos.pszDriverName; }
	void open (); // open (normally at construction time)
	void close (); // close (called from destructor)
	void newpage (); // start a new print page
};

class Bitmap
// for bitmaps from the ressource
// used by ValueSetItems in dialogs
{	HBITMAP Handle;
	HDC DeviceHandle;
	HPS PsHandle;
	SIZEL S;
	public :
	Bitmap (int id, int width=0, int height=0);
		// id is the ressource ID
	~Bitmap ();
	HBITMAP handle () { return Handle; }
};

class Help
// help interface
// the program should have a *.HLP file associated to it
// you need to declare a HELPTABLE and at least one HELPSUBTABLE
// HELPTABLE ID_Help
//		BEGIN
// 		HELPITEM ID_Window,ID_Helpsubtable,ID_GeneralHelpPage
//      END
// ID_GeneralHelpPage is an ID in your IPF file
// HELPSUBTABLE ID_Helpsubtable
//		BEGIN
//		END
{   HWND Handle;
	Help *H;
	Flag Valid;
	public :
	Help (StandardWindow &window,
		int id, char *filename, char *title="");
		// id is the help tables id
	void Help::general (void) // display general help page
	{	if (Valid) WinSendMsg(Handle,HM_EXT_HELP,NULL,NULL);
	}
	void Help::index (void) // display help index
	{	if (Valid) WinSendMsg(Handle,HM_HELP_INDEX,NULL,NULL);
	}
	void Help::content (void) // display content
	{   if (Valid) WinSendMsg(Handle,HM_HELP_CONTENTS,NULL,NULL);
	}
	void Help::display (int id) // display specific page
	{	if (Valid) WinSendMsg(Handle,HM_DISPLAY_HELP,
			MPFROMSHORT(id),HM_RESOURCEID);
	}
	int valid () { return Valid; } // help file found?
};


class Thread
// a class to start a a thread
// the instance will call a function of type "int f (Parameter)"
// this allows a Parameter to be passed to the function
// the function returns an exit code
// You need to declare a separate instance of Thread for each thread
{	int (*F) (Parameter);
	TID Tid;
	int Started,Expected;
	long Stacksize;
	Parameter P;
	public :
	Thread (int (*f) (Parameter), long stacksize=4096) : P(0)
	{	F=f;
		Started=0; Stacksize=stacksize;
		Expected=0;
	}
	void start (Parameter p=0); // call the function
	void stop (); // stop the thread destructively
	void suspend (); // suspend it
	void resume (); // resume it after suspension
	void wait ();
		// tell the function that you wait for it
		// the function may call expected() to check this
	int expected () { return Expected; }
	Parameter p () { return P; }
	int call () { return F(P); }
	int started () { return Started; }
		// was the thread already started?
	void finished () { Started=0; }
};

class Dialogitem;
class Dialog
// class to display and handle a dialog
// create the dialog in your ressource script
// use the Dialogitem class to implement the various controls
// many controls are already defined here, but you may wish
// to add others
// the dialog knows about the controls added to it and calls
// their handlers on init and exit and change
{	int Result;
	String S;
	Dialogitem *Items;
	Help *H;
	int Hid;
	void init (Window &window, int id);
	Dialog *Next;
	protected :
	int Id;
	HWND Handle;
	Window *W;
	public :	enum { ok=DID_OK,cancel=DID_CANCEL }; // return values
	Dialog (Window &window, int id); // id from ressource
	Dialog (Window &window, int id, Help &h, int hid);
		// add help to a dialog (hid is the help page)
	Dialogitem *entry (Dialogitem *item);
	virtual void carryout (); // display the dialog and start it
	virtual void start () {}
		// you may do additional initializations
	virtual void stop () {}
		// you may add additional exit things
	virtual int handler (int command) { return 0; }
		// you may add a handler, which returns 0 on any
		// command it could handle
	int result () { return Result; }
	char *gettext (int id, char *text, long size=String::defaultsize);
	char *gettext (int id);
	void settext (int id, char *text);
	MRESULT message (int id, int msg,
		Parameter mp1=NULL, Parameter mp2=NULL);
	HWND handle () { return Handle; }
	friend MRESULT EXPENTRY dialogproc (HWND hwnd, ULONG msg,
		MPARAM mp1, MPARAM mp2);
	void finish ()
	{	WinSendMsg(Handle,WM_COMMAND,MPARAM(DID_OK),0);
	}
	virtual int key (int flags, int code, int scan) { return 0; }
	Window *w () { return W; }
	virtual int close () { return 1; }
};

class DialogPanel : public Dialog
// A Dialog, which remains open, until it is finished
{	public :
	DialogPanel (Window &window, int id)
	 : Dialog(window,id) {}
	DialogPanel (Window &window, int id, Help &h, int hid)
	 : Dialog(window,id,h,hid) {}
	virtual void carryout ();
	virtual void show () { carryout(); }
	virtual int close () { return 0; }
};

class Dialogitem
// this is a control item in a dialog
// note: if the dialog is declared globally, then all its items
// must be declared globally too.
{	Dialogitem *Next;
	protected :
	int Id;
	Dialog *D;
	public :
	Dialogitem (int id, Dialog &dialog);
		// id is the ressource ID for the control
	Dialogitem *next () { return Next; }
	int id () { return Id; }
	virtual void init () {} // called on dialog start
	virtual void exit () {} // called on dialog end
	virtual void command (Parameter mp1, Parameter mp2) {}
		// called, if a command refers to the item ID
	virtual void notify () {}
		// some child items call this, if the control
		// changes its value
	void finish () { D->finish(); }
		// aborts the dialog
};

class ButtonItem : public Dialogitem
// A Button, normally handled by a Dialog::handler()
{	Flag Pressed;
	public :
	ButtonItem (int id, Dialog &dialog)
		: Dialogitem(id,dialog),Pressed(0) {}
	virtual void command (Parameter mp1, Parameter mp2)
	{   Pressed.set();
		notify();
	}
	int flag () { return Pressed; }
};

class CheckItem : public Dialogitem
// A check box control item
// You may declare it globally and use reinit(f) to
// set its value
{	int F;
	public :
	CheckItem (int id, Dialog &dialog, int flag=0)
		: Dialogitem(id,dialog),F(flag) {}
		// flag==TRUE checks the item initially
	virtual void init ();
	virtual void exit ();
	virtual void command (Parameter mp1, Parameter mp2);
	void set (int f) { F=f; }
	void reinit (int f) { set(f); init(); }
	int flag () { return F; }
	operator int () { return F; } // return check or not check
};


class RadioItem;
class RadioButton : public Dialogitem
{	int N;
	Flag F;
	RadioItem *R;
	public :
	RadioButton (int id, Dialog &dialog, int flag,
		RadioItem &r, int n) :
			Dialogitem(id,dialog),N(n),R(&r),F(flag) {}
	virtual void command (Parameter mp1, Parameter mp2);
	virtual void init ();
};

class RadioItem
// A radio box item
// it is initialized with the IDs of all its buttons
{	int I,N,*Ids;
	RadioButton **Rb;
	Dialog *D;
	public :
	RadioItem (int *ids, int n, Dialog &dialog, int i=1);
	~RadioItem ();
	operator int () { return I; } // index of selection
	void sel (int sel);
	virtual void notify () {}
};


class StringItem : public Dialogitem
// An input line control box
{   String S;
	int Length;
	Flag Readonly;
	public :
	StringItem (int id, Dialog &dialog, char *string, int length=64)
		: Dialogitem(id,dialog),S(string) { Length=length; }
		// initializw with any string
	virtual void init ();
	virtual void exit ();
	void set (char *text) { S.copy(text); }
	void reinit (char *text) { set(text); init(); }
	void limit (int length); // extend limit
	void readonly (int flag=1) { Readonly=flag; }
		// make the control read only
	operator char * () { return S; }
};

class DoubleItem : public Dialogitem
// a input line which is interpreted as a double value
{   double X;
	String S;
	Flag Readonly;
	public :
	DoubleItem (int id, Dialog &dialog, double x)
		: Dialogitem(id,dialog),S("",64) { X=x; }
	virtual void init ();
	virtual void exit ();
	void set (double x) { X=x; }
	void reinit (double x) { set(x); init(); }
	void readonly (int flag=1) { Readonly=flag; }
	operator double () { return X; }
};


class LongItem : public Dialogitem
// same as double item for long values
{   long N;
	String S;
	Flag Readonly;
	public :
	LongItem (int id, Dialog &dialog, long n)
		: Dialogitem(id,dialog),S("",64) { N=n; }
	virtual void init ();
	virtual void exit ();
	void set (long n) { N=n; }
	void reinit (long n) { set(n); init(); }
	void readonly (int flag=1) { Readonly=flag; }
	operator long () { return N; }
};

class SpinItem : public Dialogitem
// A spin control box
{   long N,Lower,Upper;
	String S;
	public :
	SpinItem (int id, Dialog &dialog, long n,
		long lower, long upper)
		: Dialogitem(id,dialog),S("",64)
	{	N=n; Lower=lower; Upper=upper;
	}
	// you may specify lower and upper limits
	virtual void init ();
	virtual void exit ();
	void set (long n, long lower, long upper)
	{	N=n; Lower=lower; Upper=upper; }
	void reinit (long n, long lower, long upper)
	{	set(n,lower,upper); init();
	}
	void set (long n) { N=n; }
	void reinit (long n) { set(n); init(); }
	operator long () { return N; }
};


class SliderItem : public Dialogitem
// a slider control box
// you probably want to modify init to set ticks and labels
// just subclass it with new virtual init
// call SliderItem::init() from there
{   long N;
	String S;
	public :
	SliderItem (int id, Dialog &dialog, long n)
		: Dialogitem(id,dialog),S("",64)
	{	N=n;
	}
	virtual void init ();
	virtual void exit ();
	void set (long n)
	{	N=n; }
	void reinit (long n)
	{	set(n); init();
	}
	void tick (int n, int size=3);
	void label (int n, char *text);
	operator long () { return N; }
};


class ValuesetItem : public Dialogitem
// a value set control box (for colors or bitmap choices)
// again, you must subclass it to modify init() to enter
// your specific value items
{	int Col,Row;
	public :
	ValuesetItem (int id, Dialog &dialog, int r=1, int c=1)
		: Dialogitem(id,dialog),Col(c),Row(r) {}
	void select (int row, int col);
	void setbitmap (int row, int col, Bitmap &b);
		// set a field to a bitmap
	void setcolor (int row, int col, ULONG color);
		// set a field to a color
	virtual void init () { select(Row,Col); }
	virtual void exit ();
	virtual void command (Parameter mp1, Parameter mp2);
		// messages are passed to this (for reactions to changes)
	int col () { return Col; } // get selected column
	int row () { return Row; } // get selected row
};

class ListItem : public Dialogitem
// list control box
// you must subclass to define an init, which inserts the list entries
{   int Selection;
	public :
	ListItem (int id, Dialog &dialog)
		: Dialogitem(id,dialog) {}
	void select (int sel);
	virtual void init () { select(0); }
	virtual void exit ();
	virtual void command (Parameter mp1, Parameter mp2);
	void insert (char *text); // insert a list entry
	operator int () { return Selection; }
};

class MultilineItem : public Dialogitem
// multiline control box (a small editor)
// no need to subclass this time
{   String S;
	Flag Readonly;
	public :
	MultilineItem (int id, Dialog &dialog, char *string,
			int length=1024)
		: Dialogitem(id,dialog),S(string,length) {}
	virtual void init ();
	virtual void exit ();
	void set (char *text) { S.copy(text); }
	void reinit (char *text) { set(text); init(); }
	void limit (int length);
	void readonly (int flag=1) { Readonly=flag; }
	operator char * () { return S; }
};

class FileSelector
// a file selector
// should be static, so that path information is not lost
{	int Freturn;
	Window *W;
	FILEDLG Fd;
	String Filter,Title,Ok;
	public :
	FileSelector(Window &window,
		char *filter, int saving,
		char *title="", char *ok=0);
		// example ...(window,"*.c",1,"Save C text","Save")
		// saving==FALSE means a load dialog
	char *select ();
		// start the dialog
};

class FontSelector
// a font selector
// after selection, the instance can be used to create a font
// with the Font class
{   FONTDLG Fd;
	String Facename;
	int Codepage,Type,Result;
	double Pointsize;
	public :
	enum { screen, printer };
	FontSelector (int type=screen,
		char *name="", double pointsize=10, int codepage=0)
		: Type(type),Facename(name,66),Pointsize(pointsize),
			Codepage(codepage),Result(DID_CANCEL) {}
	int select (Window &window);
	FATTRS *fat () { return &Fd.fAttrs; }
	FONTDLG *fd () { return &Fd; }
	operator int () { return Result; }
};


class Fonts
// A list of all fonts
{	long Count;
	FONTMETRICS *AllFonts;
	public :
	Fonts (PS &ps);
	~Fonts ();
	long count () { return Count; }
	FONTMETRICS *fonts () { return AllFonts; }
};

class Font
// can be initialized by a font selector
// the window font can be set to any instance of Font
{	FATTRS Fat;
	double NominalPointSize,PointSize;
	public :
	Font (FontSelector &fs);
	FATTRS *fat () { return &Fat; }
	char *name () { return Fat.szFacename; }
	double nominalsize () { return NominalPointSize; }
	double pointsize () { return PointSize; }
};

class Profile
// system or user profiles are used to personalize an application
{	int P;
	String S,H;
	public :
	enum { user=HINI_USERPROFILE,system=HINI_SYSTEMPROFILE };
	Profile (char *prog, int p=user) : P(p),S(prog),H("") {}
		// prog is the programs profile name
	void write (char *k, void *p, unsigned long size)
	// write binary data to the profile (k is the key)
	{	PrfWriteProfileData(P,S,k,p,size);
	}
	void writestring (char *k, char *i);
	void writedouble (char *k, double x);
	void writelong (char *k, long x);
	void writeint (char *k, int x);
		// specific key formats
	int read (char *k, void *p, unsigned long size)
	{	return PrfQueryProfileData(P,S,k,p,&size);
	}
	// read binary data from the profile
	char *readstring (char *k, char *d="",
		long size=String::defaultsize);
	double readdouble (char *k, double x=0);
	long readlong (char *k, long x=0);
	int readint (char *k, int x=0);
};

class Clipboard
// clipboard interface
{	public :
	void copy (MetafilePS &meta);
		// copy a meta file to the clipboard
};

