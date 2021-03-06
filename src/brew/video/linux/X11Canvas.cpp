/**
 *
 *  |_  _ _
 *  |_)| (/_VV
 *
 *  Copyright 2015-2018 Marcus v. Keil
 *
 *  Created on: Aug 21, 2016
 *
 */

#include <brew/video/linux/X11Canvas.h>

#include <X11/XKBlib.h>

namespace brew {

X11Canvas::X11Canvas(VideoContext& ctx) :
		VideoCanvas(ctx),
		display(nullptr),
		parent(0),
		cmap(0),
		visual(nullptr),
		win(0),
		wmDeleteMessage(0),
        cursorEnabled(true),
        cursorUpdated(false) {

}

void X11Canvas::onInit() {
	// Open a connection to the local X server. We need this to set some variables in the constructor.
	if (display == nullptr)
		display = XOpenDisplay(nullptr);

	if (display == nullptr) {
		throw RuntimeException("Cannot connect to X server.");
	}

	// Set the default parent to the root window
	if (!parent)
		parent = DefaultRootWindow(display);

	// Retrieve our visual
	visual = chooseVisual();

	if (visual == nullptr) {
		RuntimeException("No appropriate visual found.");
	}

	// Create the colormap based on visual info
	cmap = XCreateColormap(display, parent, visual->visual, AllocNone);

	XSetWindowAttributes swa = {};
	swa.colormap = cmap;
	// Set the even masks
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask /* resize & destroy */;

	u32 x = 0, y = 0, w = static_cast<u32>(getWidth()), h = static_cast<u32>(getHeight());
	win = XCreateWindow(display, parent, x, y, w, h, 0, visual->depth,
			InputOutput, visual->visual, CWColormap | CWEventMask, &swa);

	// Show the window
	XMapWindow(display, win);

	// Set the title
	//XStoreName(display, win, getWindowTitle().c_str());

	// Disable annoying autorepeat of key strokes
	XkbSetDetectableAutoRepeat(display, 1, nullptr);

	// Make the window react to a user close event
	wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, win, &wmDeleteMessage, 1);

    getAppContext().log->stream("X11Canvas") << "Window created.";
}

void X11Canvas::onDrop() {

	if (visual != nullptr) {
		// TODO: How to gracefully delete visuals? Is this even required?
	}
	visual = nullptr;

	if (win) {
		XDestroyWindow(display, win);
	}
	win = 0;

	if (cmap) {
		XFreeColormap(display, cmap);
	}

	cmap = 0;

	if (display != nullptr) {
		XCloseDisplay(display);
	}

	display = nullptr;

    getAppContext().log->stream("X11Canvas") << "Window destroyed.";

}

void X11Canvas::onPumpMessages() {

    // Update the cursor
    updateCursor();

	XEvent xev = {};
	while (XPending(display) > 0) {

		XNextEvent(display, &xev);

		if(xev.type == ConfigureNotify) {
			// Resized
			RenderEvent e(*this, RenderEvent::Type::Resize);
			e.dimensions.set(0,0,xev.xconfigure.width,xev.xconfigure.height);
			this->width = static_cast<SizeT>(e.dimensions.getWidth());
			this->height = static_cast<SizeT>(e.dimensions.getHeight());
			fireRenderEvent(e);
		}

		else if(xev.type == ClientMessage) {
			if (xev.xclient.data.l[0] == wmDeleteMessage) {
				drop();
			}
			break;
		}

		else if (xev.type == Expose) {

		}

		else {
			fireXEvent(xev);
		}
	}

}

void X11Canvas::setCursorVisible(bool enabled) {
	if(cursorEnabled == enabled) {
		return;
	}

	cursorEnabled = enabled;
    cursorUpdated = true;
}

void X11Canvas::updateCursor() {
    if(!cursorUpdated) {
        return;
    }

    cursorUpdated = false;

    if(!cursorEnabled) {
        // Hide the cursor
        ::Pixmap bm_no;
        Colormap cmap;
        Cursor no_ptr;
        XColor black, dummy;
        static char bm_no_data[] = {0, 0, 0, 0, 0, 0, 0, 0};

        cmap = DefaultColormap(display, DefaultScreen(display));
        XAllocNamedColor(display, cmap, "black", &black, &dummy);
        bm_no = XCreateBitmapFromData(display, win, bm_no_data, 8, 8);
        no_ptr = XCreatePixmapCursor(display, bm_no, bm_no, &black, &black, 0, 0);

        XDefineCursor(display, win, no_ptr);
        XFreeCursor(display, no_ptr);
        if (bm_no != None)
            XFreePixmap(display, bm_no);
        XFreeColors(display, cmap, &black.pixel, 1, 0);
    } else {
        XUndefineCursor(display, win);
    }
}

} /* namespace brew */
