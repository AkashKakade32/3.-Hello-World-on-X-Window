//Standard Header Files
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>

#define WINWIDTH 800
#define WINHEIGHT 600

Display* display = NULL;
Colormap colormap;
Window window;
XVisualInfo visualInfo;
Bool bfullScreen = False; 


int main()
{
	void uninitialize();
	void toggleFullScreen();

	int defaultDepth;
	Status status;
	XSetWindowAttributes windowAttribute;
	int styleMask;
	Atom windowManagerDelete;
	XEvent event;
	KeySym keySym;
	int screenWidth, screenHeight;
	char keys[26];

	static XFontStruct* pFontStruct = NULL;
	static int winWidth, winHeight;
	static GC gc;
	XColor greenColor;
	XGCValues gcValues;
	int stringLenght;
	int stringWidth;
	int fontHeight;
	char str[] = "Hello World !!!";

	//code
	display = XOpenDisplay(NULL);

	if (display == NULL)
	{
		printf("XOpenDisplay is failed\n");
		uninitialize();
		exit(1);
	}

	//Get Screen from above display
	int defaultScreen;

	defaultScreen = XDefaultScreen(display);

	defaultDepth = XDefaultDepth(display, defaultScreen);

	memset((void*)&visualInfo, 0, sizeof(XVisualInfo));

	status = XMatchVisualInfo(display, defaultScreen, defaultDepth, TrueColor, &visualInfo);

	if (status == 0)
	{
		printf("XMatchVisualInfo() is failed \n");

		uninitialize();

		exit(1);
	}

	//Take window attributes/properties

	memset((void*)&windowAttribute, 0, sizeof(XSetWindowAttributes));

	windowAttribute.border_pixel = 0;
	windowAttribute.background_pixel = XBlackPixel(display, visualInfo.screen);
	windowAttribute.background_pixmap = 0;
	windowAttribute.colormap = XCreateColormap(
		display,
		XRootWindow(display, visualInfo.screen),
		visualInfo.visual,
		AllocNone);

	//Assigning this colormap to global colormap
	colormap = windowAttribute.colormap;

	//Set the style of window
	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

	//Create Windwow

	window = XCreateWindow(display,
		XRootWindow(display, visualInfo.screen),
		0,
		0,
		WINWIDTH,
		WINHEIGHT,
		0,
		visualInfo.depth,
		InputOutput,
		visualInfo.visual,
		styleMask,
		&windowAttribute);

	if (!window)
	{
		printf("XCreateWindow() failed\n");
		uninitialize();
		exit(1);
	}

	//Specify to which evnts should windows response
	XSelectInput(display, window, 
		ExposureMask | VisibilityChangeMask | StructureNotifyMask | KeyPressMask | ButtonPressMask | PointerMotionMask | FocusChangeMask);

	//Specify window manager delete atom
	XInternAtom(display, "WM_DELETE_WINDOW", True);

	XSetWMProtocols(display, window, &windowManagerDelete, 1);

	//Give the caption to the window
	XStoreName(display, window, "AK : XWindow");

	//Show Mapped 
	XMapWindow(display, window);

	//Center the window
	screenWidth = XWidthOfScreen(XScreenOfDisplay(display, visualInfo.screen));
	screenHeight = XHeightOfScreen(XScreenOfDisplay(display, visualInfo.screen));
	XMoveWindow(display, window, (screenWidth - WINWIDTH)/2, (screenHeight - WINHEIGHT)/2);

	//event loop
	while (1)
	{
		XNextEvent(display, &event);

		switch (event.type)
		{

		case MapNotify:
			//printf("MapNotify Event is received\n");
			pFontStruct = XLoadQueryFont(display, "fixed");
			break;

		case FocusIn:
			//printf("FoucusIn event is received\n");
			break;

		case FocusOut:
			//printf("FocusOut event is received\n");
			break;

		case ConfigureNotify:
			//printf("ConfigureNotify event is received\n");
			winWidth = event.xconfigure.width;
			winHeight = event.xconfigure.height;
			break;

		case Expose:
			gc = XCreateGC(display, window, 0, &gcValues);
			XSetFont(display, gc, pFontStruct->fid);
			XAllocNamedColor(display, colormap, "green", &greenColor, &greenColor);
			XSetForeground(display, gc, greenColor.pixel);

			stringLenght = strlen(str);
			stringWidth = XTextWidth(pFontStruct, str, stringLenght);
			fontHeight = pFontStruct->ascent + pFontStruct->descent;

			XDrawString(display, window, gc, (winWidth - stringWidth)/2, (winHeight - fontHeight)/2, str, stringLenght);

			break;

		case ButtonPress:
			switch(event.xbutton.button)
			{
				case 1:
				//printf("Left Mouse Button is Clicked\n");
				break;

				case 2:
				//printf("Middle Mouse Button is Clicked\n");
				break;

				case 3:
				//printf("Right Mouse Button is Clicked\n");

				default:
				break;
			}
			break;

			

		case KeyPress:
				keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
				switch (keySym)
				{
				case XK_Escape:
						XUnloadFont(display, pFontStruct->fid);
						XFreeGC(display, gc);
						uninitialize();
						exit(0);
						break;

				default:
					break;
				}
				XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
				switch(keys[0])
				{
					case 'F':
					case 'f':
					if(bfullScreen == False)
					{
						toggleFullScreen();
						bfullScreen = True;
					}
					else
					{
						toggleFullScreen();
						bfullScreen = False;
					}
					break;

					default:
					break;
				}
				break;

		case 33:
			XUnloadFont(display, pFontStruct->fid);
			XFreeGC(display, gc);
			uninitialize();
			exit(0);
			break;

		default:
			break;
		}
	}

	
	uninitialize();

	return (0);
}

void toggleFullScreen(void)
{
	//local variable declarations
	Atom windowManagerStateNormal;
	Atom windowManagerStateFullScreen;
	XEvent event;
	

	//code
	windowManagerStateNormal = XInternAtom(display, "_NET_WM_STATE", False);

	windowManagerStateFullScreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	memset((void *)&event, 0, sizeof(XEvent));

	event.type = ClientMessage;
	event.xclient.window = window;
	event.xclient.message_type = windowManagerStateNormal;
	event.xclient.format = 32;
	event.xclient.data.l[0] = bfullScreen ? 0 : 1;
	event.xclient.data.l[1] = windowManagerStateFullScreen;

	XSendEvent(display, 
	XRootWindow(display, visualInfo.screen),
	False,
	SubstructureNotifyMask,
	&event);
}

void uninitialize()
{
	//code

	if (window)
	{
		XDestroyWindow(display, window);
	}

	if (colormap)
	{
		XFreeColormap(display, colormap);
	}

	if (display)
	{
		XCloseDisplay(display);
		display = NULL;
	}

	//Christopher Tronche
	//Kenton Lee
}
