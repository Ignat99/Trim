// #include <g++-2/stl.h>
#include <string> //.h
#include <math.h>
//#include <g++-2/stdiostream.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <map>
#include <X11/Xlib.h>
//#include <g++-2/iostream>
#include "calculator.h"
#include "node.h"

#define MAX_Q 5
/*	Create checkerboard texture	*/
#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

#define checkImageWidth1 8
#define checkImageHeight1 16
#define checkImageLetter1 27
static GLubyte checkImage1[checkImageLetter1][checkImageHeight1][checkImageWidth1][4];
static GLdouble Xpos, Ypos, Zpos, Xangle, Yangle/*, XCpos, YCpos = 10.0, ZCpos */;


//using namespace std;

static int snglBuf[] = { GLX_RGBA, GLX_DEPTH_SIZE, 16, None };
//static int dblBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
static int onPult = 0;

/*static String fallbackResources[] = {
//	"*title: Control: trim_lamanga",
//	"*glxarea*width: 500", "*glxarea*height: 500", NULL
//};
*/
String fallback_resources[] = {
	"*title: Pult: Trim01",
	"*glxarea*width: 500", "*glxarea*height: 100",
	"*input:		True",
	"*showGrip:		off",
	"*Text*preferredPaneSize: 22",
	"*Text*editType:	edit",
	"*Text*scrollVertical:	whenNeeded",
	"*Text*scrollHorizontal: whenNeeded",
	"*Text*autoFill:	on",
	"*drop*label:		DROP this window.",
	"*clear*label:		CLEAR the text in window   ",
	"*print*label:		UPDATE the name of node.   ",
	"*print1*label:		UPDATE the data.a1 of node.",
	"*print2*label:		UPDATE the data.a2 of node.",
	"*print3*label:		UPDATE the data.a3 of node.",
	"*print4*label:		UPDATE the data.a4 of node.",
	"*print5*label:		UPDATE the data.a5 of node.",
	"*print6*label:		UPDATE the data.a6 of node.",
	"*print7*label:		UPDATE the data.a7 of node.",
	NULL,
};

Display * dpy;
GLboolean	doubleBuffer = GL_TRUE;
XtAppContext app;
XtWorkProcId workId = 0;
Widget  top_level , calc_form, calc_view, dialog_win,  display_win , off_key, vbox[BUTTONS/4],
	paned, clear, print, text, drop, text1, print1, text2, print2, text3, print3,
	text4, print4, text5, print5, text6, print6, text7, print7;
char data_a1[255] , data_a2[15], data_a3[15], data_a4[15], data_a5[15], data_a6[15], data_a7[15]; 
GLXContext cx;
XVisualInfo *vi;
Colormap cmap;
Window root_window, win;
GLfloat         xAngle = 42.0, yAngle = 82.0, zAngle = 112.0;
//ggi_visual_t vis,vis_screen;
//GC gr_context1, gr_context2;
GLfloat white[3] = { 1.0, 1.0, 1.0 };
GLuint fontOffset;
//  surface variabe
GLfloat ctlpoints[4][4][3];
GLdouble vert[2000][3],norm[2000][3];
int showPoints = 0;
GLUnurbsObj *theNurb;
GLuint base;
int down = 0, lastx;

static Bool WaitForNotify(Display *d, XEvent *e, char *arg) {
  return (e->type == MapNotify) && (e->xmap.window == (Window)arg); }
static void ClearText(Widget w, XtPointer text_ptr, XtPointer call_data) {
    Widget text = (Widget) text_ptr;
    XtVaSetValues(text, XtNstring, "", NULL);
}
static void DropText(Widget w, XtPointer text_ptr, XtPointer call_data) {
	onPult = 0;
}
static void Syntax( XtAppContext app, char *call) {
    XtDestroyApplicationContext(app);
    fprintf( stderr, "Usage: %s \n", call);
    exit(1);
}


void init_surface(void) {
	int u, v;
	for (u = 0; u < 4; u++) {
		for (v = 0; v < 4; v++) {
			ctlpoints[u][v][0] = 2.0*((GLfloat)u - 1.5);
			ctlpoints[u][v][1] = 2.0*((GLfloat)v - 1.5);
			
			if ( (u == 1 || u == 2) && (v == 1 || v == 2))
				ctlpoints[u][v][2] = 7.0;
			else
				ctlpoints[u][v][2] = -3.0;
		}
	}
}
void gl_get_val ( int f,  /* IN */ GLdouble *v, /* IN v[n_var] */ GLdouble *value,  /* OUT value */ GLdouble grad[3])  /* OUT grad[n_var] */ {
	if ( f == 0 ) {
		*value = 5;
//		*grad = v[0] + v[1];
	}
	if ( f == 2 ) {
		*value = v[0] + v[1];
				//		*grad = v[0] + v[1];
	}
}
void init_surface_gl_model(void) {
	int u, v, n_var = 100, n_fan = 0, i = 0; // need are value of  n_var and n_fan
	int n_var1, n_fan1;
	GLdouble x1,y1,z1,x2,y2,z2,x,y,z,n;
	GLdouble var1[MAX_Q * MAX_Q][3];
	GLdouble  value, grad[3];
	//	gl_init_model(&n_var1,&n_fan1);
	for (u = 0; u < MAX_Q; u++) {
		for (v = 0; v < MAX_Q; v++) {
			var1[i][0] = 5.0 / MAX_Q * (GLdouble) (v - u);
			var1[i][1] = 5.0 / MAX_Q * (GLdouble) (-u); 
			var1[i][2] = 5.0 / MAX_Q * (GLdouble) (-u);
			gl_get_val(n_fan, var1[i], &value, grad);
			vert[i][0] = (GLdouble)u;
			vert[i][2] = (GLdouble)v;
			vert[i][1] = value;
			i++;
		}
	}
	for ( i=0; i< MAX_Q*( MAX_Q - 1) ; i++ ) {
		x1 = vert[i+MAX_Q][0] - vert[i][0];
		y1 = vert[i+MAX_Q][1] - vert[i][1];
		z1 = vert[i+MAX_Q][2] - vert[i][2];
		
		x2 = vert[i+1][0] - vert[i][0];
		y2 = vert[i+1][1] - vert[i][1];
		z2 = vert[i+1][2] - vert[i][2];
		
		x = y1 * z2 - y2 * z1;
		y = - (x1 * z2 - x2 * z1);
		z = x1 * y2 - x2 * y1;
		n = x*x + y*y + z*z;
		
		norm[i][0] = x/n;
		norm[i][1] = y/n;
		norm[i][2] = z/n;
	}
	//	gl_close_model();
}

void makeRasterFont(void) {
    XFontStruct *fontInfo;
    Font id;
    unsigned int first, last;
    Display *xdisplay;

   // xdisplay = auxXDisplay ();
	xdisplay = dpy;
    fontInfo = XLoadQueryFont(xdisplay,
	"-adobe-helvetica-medium-r-normal--17-120-100-100-p-88-iso8859-1");
    if (fontInfo == NULL) {
        printf ("no font found\n");
	exit (0);
    }

    id = fontInfo->fid;
    first = fontInfo->min_char_or_byte2;
    last = fontInfo->max_char_or_byte2;

    base = glGenLists((GLuint) last+1);
    if (base == 0) {
        printf ("out of display lists\n");
//	exit (0);
    }
    glXUseXFont(id, first, last-first+1, base+first);
/*    *height = fontInfo->ascent + fontInfo->descent;
    *width = fontInfo->max_bounds.width;  */
}

void init_node(void) {
	c1->a1 = 0; 
	c1->a2 = 0; 
	c1->a3 = 3;
	c2->a1 = 1; 
	c2->a2 = 0; 
	c2->a3 = 3;
		pNewNode = new Node(c2,"BREK",5);
	pCurNode = ReadIndex ();
	fprintf(stdout, "ReadIndex complit. \n");
		pCurNode = pCurNode->insert(pNewNode);
		pCurNode->_top = pCurNode->prev()->top();
	pNewNode = new Node(c2,"TWO WORD",9);
	pCurNode = pCurNode->insert(pNewNode);
	pCurNode->_top = pCurNode->prev()->top();
	
	//	glShadeModel (GL_FLAT);
	glShadeModel(GL_SMOOTH); // extapolate color drow
	makeRasterFont();
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, checkImageWidth1, checkImageHeight1, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage1[1]);
	
}
void nurbsError(GLenum errorCode) {
	const GLubyte *estring;
	
	estring = gluErrorString(errorCode);
	fprintf (stderr, "Nurbs Error: %s\n", estring);
	exit (0);
}
void init(void) {
	static float ambient[] = {0.1, 0.1, 0.1, 1.0};
	static float diffuse[] = {0.1, 1.0, 1.0, 1.0};
	static float position0[] = {0.0, 0.0, 2.0, 0.0};
	static float position1[] = {1.0, 0.0, 2.0, 0.0};
	static float position2[] = {1.0, 4.0, 2.0, 0.0};
	static float position3[] = {0.0, 4.0, 2.0, 0.0};
	static float front_mat_shininess[] = {80.0};
	static float front_mat_specular[] = {0.3, 0.3, 0.3, 1.0};
	static float front_mat_diffuse[] = {0.5, 0.28, 0.38, 1.0};
	//	GLfloat mat_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };   // RGBA color diffuse reflect of material
	//	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // RGBA color mirror reflect of material
	//	GLfloat mat_shininess[] = { 100.0 }; // power of  mirror reflect of material
    /*
    static float back_mat_shininess[] = {60.0};
    static float back_mat_specular[] = {0.5, 0.5, 0.2, 1.0};
    static float back_mat_diffuse[] = {1.0, 1.0, 0.2, 1.0};
    */
	static float lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
	static float lmodel_twoside[] = {GL_FALSE};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	glEnable(GL_LIGHT1);
	
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, position2);
	glEnable(GL_LIGHT2);
	
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT3, GL_POSITION, position3);
	glEnable(GL_LIGHT3);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
	glEnable(GL_LIGHTING);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_mat_diffuse);
	
	
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	
	//	glEnable(GL_LIGHTING);   // ligt enable
	//	glEnable(GL_LIGHT0);   // 0 sourse light turn on
	//	glEnable(GL_DEPTH_TEST);  //  z - bufer turn on
	//	glEnable(GL_AUTO_NORMAL); //  analitic compute normal vectors of surface
	//	glEnable(GL_NORMALIZE); // the normalize normal vectors
	//	glEnable(GL_COLOR_MATERIAL);
	
	//	init_surface();
	// init_surface_gl_model();
	
		theNurb = gluNewNurbsRenderer();
		gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
		gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
		//gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (CALLBACK*) ()) nurbsError);
	
	
	init_node();
}
void printString(char *s) {
    glPushAttrib (GL_LIST_BIT);
    glListBase(base);
    glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *)s);
    glPopAttrib ();
}

/*
void printStringImageLetter(GLdouble x, GLdouble y, GLdouble z, char *s) {
    GLfloat white[3] = { 1.0, 1.0, 0.0 };
    int i,j, numString ,lenString = (GLsizei) strlen(s);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3fv(white);
    glRasterPos3d(x, y, z);
    printString("Abc");
    glFlush ();
}
*/

void display_node (void) {
	Node *tmp;
	tmp = pCurNode;
	glVertex3d(20.0 + 0.02, 20.0 + 0.02, 0.0 + 0.02); //Ignat
	glutSolidSphere(0.1,20,20); //Ignat
	do {
		pCurNode = pCurNode->_next;
		
		glPushMatrix();
		glTranslated ((GLdouble)pCurNode->data.a1+Xpos, (GLdouble)pCurNode->data.a2+Ypos, (GLdouble)pCurNode->data.a3+Zpos);
		glRotatef(Xangle, 1.,0.,0.);
		glRotatef(Yangle, 0.,1.,0.);
	    glScalef (0.5, 0.5, 0.5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		glutSolidSphere(0.1,10,10);

			glDisable(GL_LIGHTING);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d((GLdouble)pCurNode->next()->data.a1 - (GLdouble)pCurNode->data.a1, //- Xpos ,
					(GLdouble)pCurNode->next()->data.a2 - (GLdouble)pCurNode->data.a2,// - Ypos,
					(GLdouble)pCurNode->next()->data.a3 - (GLdouble)pCurNode->data.a3 ); //- Zpos);
		glColor3f(0.0, 1.0, 0.0);
			glVertex3d(0.0 - 0.03, 0.0 - 0.03, 0.0 - 0.03);
			glVertex3d((GLdouble)pCurNode->prev()->data.a1 - (GLdouble)pCurNode->data.a1, //- Xpos ,
					(GLdouble)pCurNode->prev()->data.a2 - (GLdouble)pCurNode->data.a2,// - Ypos,
					(GLdouble)pCurNode->prev()->data.a3 - (GLdouble)pCurNode->data.a3 ); //- Zpos);
		glColor3f(0.0, 0.0, 1.0);
			glVertex3d(0.0 - 0.02, 0.0 - 0.02, 0.0 - 0.02);
			glVertex3d((GLdouble)pCurNode->top()->data.a1 - (GLdouble)pCurNode->data.a1, //- Xpos ,
					(GLdouble)pCurNode->top()->data.a2 - (GLdouble)pCurNode->data.a2,// - Ypos,
					(GLdouble)pCurNode->top()->data.a3 - (GLdouble)pCurNode->data.a3 ); //- Zpos);
		glColor3f(1.0, 1.0, 0.0);
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d((GLdouble)pCurNode->bottom()->data.a1 - (GLdouble)pCurNode->data.a1, //- Xpos ,
					(GLdouble)pCurNode->bottom()->data.a2 - (GLdouble)pCurNode->data.a2,// - Ypos,
					(GLdouble)pCurNode->bottom()->data.a3 - (GLdouble)pCurNode->data.a3 ); //- Zpos);

		glColor3f(0.0, 1.0, 1.0);
			glVertex3d(0.0 + 0.02, 0.0 + 0.02, 0.0 + 0.02);
			glVertex3d((GLdouble)pCurNode->_sviaz_nos->data.a1 - (GLdouble)pCurNode->data.a1, //- Xpos ,
					(GLdouble)pCurNode->_sviaz_nos->data.a2 - (GLdouble)pCurNode->data.a2,// - Ypos,
					(GLdouble)pCurNode->_sviaz_nos->data.a3 - (GLdouble)pCurNode->data.a3 ); //- Zpos);

		glColor3f(1.0, 0.0, 1.0);
			glVertex3d(0.0 + 0.04, 0.0 + 0.04, 0.0 + 0.04);
			glVertex3d((GLdouble)pCurNode->_sviaz_hvost->data.a1 - (GLdouble)pCurNode->data.a1, //- Xpos ,
					(GLdouble)pCurNode->_sviaz_hvost->data.a2 - (GLdouble)pCurNode->data.a2,// - Ypos,
					(GLdouble)pCurNode->_sviaz_hvost->data.a3 - (GLdouble)pCurNode->data.a3 ); //- Zpos);


		glColor3f(1.0, 1.0, 0.0);
		glEnd();


	glColor3fv(white);
	glRasterPos3d( (GLdouble)pCurNode->data.a1, (GLdouble)pCurNode->data.a2, (GLdouble)pCurNode->data.a3);
	printString(pCurNode->name);
//	printStringImageLetter( (GLdouble)pCurNode->data.a1,(GLdouble)pCurNode->data.a2, (GLdouble)pCurNode->data.a3,pCurNode->name);
//	printStringImageLetterRastr( 0.0, 0.0, 0.0,pCurNode->name);
    glRasterPos3d(0.0, 0.0, 0.15);
    printString(pCurNode->name);
		glPopMatrix();
	} while ( pCurNode != tmp);
	//  Cursor is you
	glPushMatrix();
	glTranslated ((GLdouble)pCurNode->data.a1+Xpos, (GLdouble)pCurNode->data.a2+Ypos, (GLdouble)pCurNode->data.a3+Zpos);
    glColor3f(1.0, 1.0, 0.0);
    glRasterPos3d(-0.5, 0.5, 0.5);
    printString(pCurNode->top()->name);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
	glColor3f(0.0, 0.0, 1.0);
	glutWireCube(1.0);
//	printStringImageLetter( -0.5, 0.5, 0.5,pCurNode->top()->name);
	glPopMatrix();
	glColor3f(1.0, 1.0, 0.0);
	
}
void display(void) {
	GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
	int i, j, i0, ik;
	GLfloat edgePt[5][2] = /* counter clockwise */{{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}};
	GLfloat curvePt[4][2] = /* clockwise */ {{0.25, 0.5}, {0.25, 0.75}, {0.75, 0.75}, {0.75, 0.5}};
	GLfloat curveKnots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
	GLfloat pwlPt[4][2] = /* clockwise */ {{0.75, 0.5}, {0.5, 0.25}, {0.25, 0.5}};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(330.0, 1.,0.,0.);
	glTranslatef (0.0+Xpos, 0.0+Ypos, 2.0+Zpos); // Z look at me
	glScalef (0.1, 0.1, 0.1);
	
		gluBeginSurface(theNurb);
		gluNurbsSurface(theNurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0],4, 4, GL_MAP2_VERTEX_3);
	
	gluBeginTrim (theNurb);
	gluPwlCurve (theNurb, 5, &edgePt[0][0], 2, GLU_MAP1_TRIM_2);
	gluEndTrim (theNurb);
	gluBeginTrim (theNurb);
	gluNurbsCurve (theNurb, 8, curveKnots, 2, &curvePt[0][0], 4, GLU_MAP1_TRIM_2);
	gluPwlCurve (theNurb, 3, &pwlPt[0][0], 2, GLU_MAP1_TRIM_2);
	gluEndTrim (theNurb);
	
		gluEndSurface(theNurb);
	
		glDisable(GL_LIGHTING);
	glPointSize(5.0);
	glColor3f(1.0, 1.0, 0.0);
	ik = MAX_Q;
	for(i0 = 0; i0 < ik*ik; i0 += ik) {
		glBegin(GL_TRIANGLE_STRIP);
		for(i=i0;i<i0+ik;i++) {
			glNormal3dv(norm[i]);		
			glVertex3d(vert[i][0],vert[i][1],vert[i][2]);
			glNormal3dv(norm[i+ik]);		
			glVertex3d(vert[i+ik][0],vert[i+ik][1],vert[i+ik][2]);
		}
		glEnd();
	}
		glEnable(GL_LIGHTING);
	/*
		if(showPoints) {
		glPointSize(5.0);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		for(i=0;i<4;i++) {
			for(j=0;j<4;j++) {
				glVertex3f(ctlpoints[i][j][0], ctlpoints[i][j][1], ctlpoints[i][j][2]);
			}
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}*/
	
	glPopMatrix();
	//	glutSwapBuffers();
	
	display_node();
	//if(doubleBuffer) 
	glXSwapBuffers(dpy,win);
	//else glFlush();
}
void blow_display(void) {
	GLdouble tempXpos = Xpos, tempYpos = Ypos, tempZpos = Zpos, deltaX,deltaY,deltaZ;
	deltaX = tempXpos/20.0; deltaY = tempYpos/20.0; deltaZ = tempZpos/20.0;
	int i;
	for ( i=0; i < 20; i++) {
		Xpos -= deltaX;
		Ypos -= deltaY;
		Zpos -= deltaZ;
		display();
	}
	Xpos = tempXpos;
	Ypos = tempYpos;
	Zpos = tempZpos;
}
void blow_display_in(void) {
	GLdouble tempXpos = Xpos, tempYpos = Ypos, tempZpos = Zpos, deltaX,deltaY,deltaZ;
	deltaX = tempXpos/20.0; deltaY = tempYpos/20.0; deltaZ = tempZpos/20.0;
	int i;
	for ( i=0; i < 20; i++) {
		Xpos -= deltaX;
		Ypos -= deltaY;
		Zpos += deltaZ;
		display();
	}
	Xpos = tempXpos;
	Ypos = tempYpos;
	Zpos = tempZpos;
}
void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//   glFrustum(-(GLdouble)w/2,(GLdouble)w/2,-(GLdouble)h/2,(GLdouble)h/2,-(GLdouble)2.0,(GLdouble)2.0);
	
	gluPerspective (45.0, (GLdouble)w/(GLdouble)h, 1.0, 100.0);
//    glOrtho (0.0, w, 0.0, h, -1.0, 1.0);
	
//   glOrtho (-(GLdouble)w/2,(GLdouble)w/2,-(GLdouble)h/2,(GLdouble)h/2, -2.0, 2.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef (0.0, 0.0, -5.0); // Z look at me
	
}
void menu(int value) {
	switch (value) {
		case 0:
			glDisable(GL_LIGHTING);
			//			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case 1:
			glEnable(GL_LIGHTING);
			//showPoints = value;
			//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 2:
						gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 3:
						gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
	}
	glutPostRedisplay();
}
/* ARGSUSED1 */
void motion(int x, int y) {
	if (down) {
		glRotatef(lastx - x, 0, 1, 0);
		lastx = x;
		glutPostRedisplay();
	}
}
/* ARGSUSED3 */
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			lastx = x;
			down = 1;
		} else {
			down = 0;
		}
	}
}
/* ARGSUSED1 */
void map_state_changed(Widget w, XtPointer data, XEvent * event, Boolean * count) {
	switch (event->type) {
	case MapNotify:
		break;
	case UnmapNotify:
		break;
	}
}

static void PrintText(Widget w, XtPointer text_ptr, XtPointer call_data) {
    Widget text = (Widget) text_ptr;
    String str;
//	char str[15];

    XtVaGetValues(text, XtNstring, &str, NULL);
			strcat(str,"\0");
//    fprintf(stdout, "Text String is:\n--------\n%s\n--------\n", str);
		
			pNewNode =  pCurNode;
			delete pNewNode->name;
			pNewNode->str_len = strlen(str)+1;
//    fprintf(stdout, "Text str_len is:\n--------\n%d\n--------\n", strlen(str));

//			pNewNode->name = new char[pNewNode->str_len];
			pNewNode->get_name(pNewNode->str_len);
			strcpy(pNewNode->name , str);
//    fprintf(stdout, "Text name is:\n--------\n%s\n--------\n", pNewNode->name);
}
static void PrintText1(Widget w, XtPointer text_ptr, XtPointer call_data) {
//    Widget text1 = (Widget) text_ptr;
    String str;
    XtVaGetValues(text1, XtNstring, &str, NULL);
//			strcat(str,"\0");
			pCurNode->data.a1 = atoi(str);}
static void PrintText2(Widget w, XtPointer text_ptr, XtPointer call_data) {
    String str;
    XtVaGetValues(text2, XtNstring, &str, NULL);
   pCurNode->data.a2 = atoi(str); }
static void PrintText3(Widget w, XtPointer text_ptr, XtPointer call_data) {
    String str;
    XtVaGetValues(text3, XtNstring, &str, NULL);
   pCurNode->data.a3 = atoi(str); }
static void PrintText4(Widget w, XtPointer text_ptr, XtPointer call_data) {
    String str;
    XtVaGetValues(text4, XtNstring, &str, NULL);
   pCurNode->data.a4 = atoi(str); }
static void PrintText5(Widget w, XtPointer text_ptr, XtPointer call_data) {
    String str;
    XtVaGetValues(text5, XtNstring, &str, NULL);
   pCurNode->data.a5 = atoi(str); }
static void PrintText6(Widget w, XtPointer text_ptr, XtPointer call_data) {
    String str;
    XtVaGetValues(text6, XtNstring, &str, NULL);
   pCurNode->data.a6 = atoi(str); }
static void PrintText7(Widget w, XtPointer text_ptr, XtPointer call_data) {
    String str;
    XtVaGetValues(text7, XtNstring, &str, NULL);
   pCurNode->data.a7 = atoi(str); }

static void process_input(Display *dpy) {
	XEvent event;
	GLboolean needRedraw = GL_FALSE, recalcModelView = GL_TRUE;
//	String str; // for XK_t
	do{
	   char buf[31];
	   KeySym keysym;

	 XNextEvent(dpy,&event);

        switch(event.type){
	case ButtonPress:
		recalcModelView = GL_TRUE;
                switch (event.xbutton.button) {
                case 1: xAngle += 10.0;
//			glDisable(GL_LIGHTING);
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
                case 2: yAngle += 10.0; break;
                case 3: zAngle += 10.0; break;
                }
             break;
        case Expose:
		needRedraw = GL_TRUE;
             //XClearWindow(dpy,win);
             //XDrawString(dpy,win,gr_context1,50,50,"Hello",5);
             //XDrawImageString(dpy,win,gr_context2,20,20,"Hello",5);
             //XFillRectangle(dpy,win,gr_context1,150,150,111,111);
             //XFillRectangle(dpy,win,gr_context2,200,180,111,111);
             break;
	case ConfigureNotify:
                glViewport(0, 0, event.xconfigure.width, event.xconfigure.height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective (45.0, (GLdouble) event.xconfigure.width/
							(GLdouble)event.xconfigure.height, 1.0, 10.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef (0.0, 0.0, -5.0); // Z look at me
	
                /* fall through... */
		needRedraw = GL_TRUE;
	     break;
        case KeyPress:
		(void) XLookupString(&event.xkey, buf, sizeof(buf), &keysym, NULL);
		switch (keysym) {

		case XK_Tab : //TabKEY:
			pCurNode = pCurNode->next();
			break;
		case XK_BackSpace : //BackspaceKEY 127:
			pCurNode = pCurNode->prev();
			break;
		case XK_b : //bKEY
			if (pCurNode->_bottom == pCurNode )
			{
			pCurNode->_bottom =  new Node(&pCurNode->data,"NEW\0",4);
			pCurNode->_bottom->_top = pCurNode;
			pCurNode = pCurNode->bottom();
			break;
			}
		case XK_Return : //EnterKEY 10
//			blow_display();
			pCurNode = pCurNode->bottom();
			break;
		case XK_h : //fKEY
//			blow_display_in();
			pCurNode = pCurNode->top();
			break;
		case XK_n : //nKEY:
			pNewNode =  new Node(&pCurNode->data,"NEW\0",4);
			pCurNode = pCurNode->insert(pNewNode);
			pCurNode->_top = pCurNode->prev()->top();
			break;
		case XK_d : //dKEY:
			pNewNode =  pCurNode;
			pCurNode = pCurNode->next();
			pNewNode = pNewNode->remove(); delete pNewNode;
			break;
		case XK_o :	//oKEY:
				pSviazHvost = pCurNode;
			break;
		case XK_p :	//pKEY:
				pSviazNos = pCurNode;
				pSviazHvost->_sviaz_nos = pCurNode;
				pCurNode->_sviaz_hvost = pSviazHvost;
			break;
		case XK_w :	//wKEY:
				pSetevaia = pCurNode;
			break;
		case XK_e :	//eKEY:
				pSetevaia->_bottom = pCurNode;
			break;
		case XK_f :	//fKEY:
				OperatorF(pCurNode);
			break;

		case XK_t : //tKEY:
		 	onPult = 1;
			XtVaSetValues(text, XtNstring, pCurNode->name, NULL);
			(void)sprintf(data_a1, "%d", pCurNode->data.a1);
			XtVaSetValues(text1, XtNstring, data_a1, NULL);
			(void)sprintf(data_a1, "%d", pCurNode->data.a2);
			XtVaSetValues(text2, XtNstring, data_a1, NULL);
			(void)sprintf(data_a1, "%d", pCurNode->data.a3);
			XtVaSetValues(text3, XtNstring, data_a1, NULL);
			(void)sprintf(data_a1, "%d", pCurNode->data.a4);
			XtVaSetValues(text4, XtNstring, data_a1, NULL);
			(void)sprintf(data_a1, "%d", pCurNode->data.a5);
			XtVaSetValues(text5, XtNstring, data_a1, NULL);
			(void)sprintf(data_a1, "%d", pCurNode->data.a6);
			XtVaSetValues(text6, XtNstring, data_a1, NULL);
			(void)sprintf(data_a1, "%d", pCurNode->data.a7);
			XtVaSetValues(text7, XtNstring, data_a1, NULL);

			XtRealizeWidget(top_level);
			//XtAppMainLoop(app);
			
			while (onPult){
				XtAppNextEvent(app,&event);
				
				switch(event.type){
					case ButtonPress:
				             switch (event.xbutton.button) {
				                case 1:  break;
				                case 2:  break;
				                case 3:  break;
						default: break;
				             }
				             break;
				        case Expose:
				             break;
					case ConfigureNotify:
				                glViewport(0, 0, event.xconfigure.width, event.xconfigure.height);
					     break;
				        case KeyPress:
						(void) XLookupString(&event.xkey, buf, sizeof(buf), &keysym, NULL);
						switch (keysym) {

						case XK_Tab : //TabKEY:
							pCurNode = pCurNode->next();
							break;
						case XK_Escape:
							onPult = 0;
					             //XCloseDisplay(dpy);
						default:
							break;
						} //switch (keysym)
					default:
						break;
				} //switch(event.type)
				XtDispatchEvent(&event);
                        } // while(onPult)
			XtUnrealizeWidget(top_level);
			pNewNode =  pCurNode;
			delete pNewNode->name;
			pNewNode->str_len = 7;
			//pNewNode->name = new char[pNewNode->str_len];
			pNewNode->get_name(pNewNode->str_len);
			strcpy(pNewNode->name, "WORD A\0");
			break;

		case XK_F2 :
//      case 'z':
//      case 'Z':
			Yangle = -2.0;
			//			Xpos += 0.05;
//	gluLookAt(Xpos, Ypos, Zpos, XCpos, YCpos, ZCpos, 0, 1, 0);
/*	glTranslated (Xpos, Ypos, Zpos); */
			//			glRotatef(Xangle, 1.,0.,0.);
			glRotatef(Yangle, 0.,1.,0.);
			break;
		case XK_F1 :
//      case 'x':
//      case 'X':
			Yangle = +2.0;
			//			Xpos -= 0.05;
//	gluLookAt(Xpos, Ypos, Zpos, XCpos, YCpos, ZCpos, 0, 1, 0);
/*	glTranslated (Xpos, Ypos, Zpos); */
			//			glRotatef(Xangle, 1.,0.,0.);
			glRotatef(Yangle, 0.,1.,0.);
			break;
		case XK_End:
			Xangle = -2.0;
			//			Ypos -= 0.05;
//	gluLookAt(Xpos, Ypos, Zpos, XCpos, YCpos, ZCpos, 0, 1, 0);
/*	glTranslated (Xpos, Ypos, Zpos); */
			glRotatef(Xangle, 1.,0.,0.);
			//			glRotatef(Yangle, 0.,1.,0.);
			break;
		case XK_Home :
			Xangle = +2.0;
			//			Ypos += 0.05;
//	gluLookAt(Xpos, Ypos, Zpos, XCpos, YCpos, ZCpos, 0, 1, 0);
/*	glTranslated (Xpos, Ypos, Zpos);  */
			glRotatef(Xangle, 1.,0.,0.);
			//			glRotatef(Yangle, 0.,1.,0.);
			break;
		case XK_Up :
			Zpos += 0.2;
//	glTranslated (Xpos, Ypos, Zpos);
			break;
		case XK_Down :
			Zpos -= 0.2;
//	glTranslated (Xpos, Ypos, Zpos);
			break;
		case XK_Left :
			Xpos += 0.2;
//	glTranslated (Xpos, Ypos, Zpos);
			break;
		case XK_Right :
			Xpos -= 0.2;
//	glTranslated (Xpos, Ypos, Zpos);
			break;
		case XK_Page_Up :
			Ypos -= 0.2;
//	glTranslated (Xpos, Ypos, Zpos);
			break;
		case XK_Page_Down:
			Ypos += 0.2;
//	glTranslated (Xpos, Ypos, Zpos);
			break;
		
		case XK_Escape:
	             XCloseDisplay(dpy);
		     WriteIndexRec(pCurNode);
        	     exit(0);
		default:
			break;
		} //switch (keysym)
	default:
		break;
        } //switch(event.type)
     }while(XPending(dpy)); /* loop to compress events */ //do
         if (recalcModelView) {
//            glMatrixMode(GL_MODELVIEW);
            /* reset modelview matrix to the identity matrix */
//            glLoadIdentity();
            /* move the camera back three units */
//            glTranslatef(0.0, 0.0, -3.0);
//	glTranslatef (0.0+Xpos, 0.0+Ypos, 2.0+Zpos); // Z look at me
//	glScalef (0.1, 0.1, 0.1); // my
            /* rotate by X, Y, and Z angles */
//            glRotatef(xAngle, 0.1, 0.0, 0.0);
//            glRotatef(yAngle, 0.0, 0.1, 0.0);
//            glRotatef(zAngle, 0.0, 0.0, 1.0);
            recalcModelView = GL_FALSE;
            needRedraw = GL_TRUE;
        }
        if (needRedraw) {
            display();
            needRedraw = GL_FALSE;
        }
} //process_input
int main(int argc, char** argv) {
//      int foo = 1;
//      char * bar[1] = {" "};

      XSetWindowAttributes swa;
      static struct win_val dis_rec;
//      Widget calc_form, display_win, off_key, vbox[BUTTONS/4];
//      XtAppContext app;
//      GC gr_context1, gr_context2;
      XGCValues gr_values;
      int     screen, dummy;
	/* Xt pult */
	Arg args[25];
	int i,j,k,n;
	static char *welcome_message= "Error in Event\n";
	MmapOperator.insert(mmtype::value_type("OPERATOR_COPY", Vichislenie));
	MmapOperator.insert(mmtype::value_type("OPERATOR_HELLO", HelloF));
	MmapOperator.insert(mmtype::value_type("OPERATOR_COMPIL", OperatorCompil));

	MmapOperator.insert(mmtype::value_type("OPERATOR_POTENC", VichisleniePot));

//=====================================================================

	top_level = XtVaAppInitialize( &app, "Control_Trim01", (XrmOptionDescRec*) NULL, 0, &argc, argv, fallback_resources, NULL);
//					NULL, NULL);
//	if (argc != 2) Syntax(app, argv[0]);
      /* get a connection */
      dpy = XtDisplay(top_level);
//	dpy = XOpenDisplay(0);
	if (dpy == NULL) XtAppError(app, "could not open display");
    /*** (2) make sure OpenGL's GLX extension supported ***/
    	if(!glXQueryExtension(dpy, &dummy, &dummy)) XtAppError(app,"X server has no OpenGL GLX extension");
      //screen  = XDefaultScreen(dpy);
      /* get an appropriate visual */
      //vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
      //if(vi == NULL) {
	vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
	if (vi == NULL) XtAppError(app, "no RGB visual with depth buffer");
	doubleBuffer = GL_FALSE;
      //}
      /* create a GLX context */
	cx = glXCreateContext(dpy, vi, None, GL_TRUE);
	if (cx == NULL) XtAppError(app, "could not create rendering context");
      /* create a color map */
	cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	/*
	 * Establish the visual, depth, and colormap of the top_level
	 * widget _before_ the widget is realized.
	 */
//      XtVaSetValues(top_level, /* XtNvisual, vi->visual,*/ XtNdepth, vi->depth, XtNcolormap, cmap, NULL);
      XtAddEventHandler(top_level, StructureNotifyMask, False, map_state_changed, NULL);
//=============== Xt pult widgets         ================================
    paned = XtVaCreateManagedWidget("paned", panedWidgetClass, top_level, NULL);

      n = 0;
	XtSetArg(args[n], XtNwidth, 700);              n++;
	XtSetArg(args[n], XtNheight, 22);                  n++;
    drop = XtCreateManagedWidget("drop", commandWidgetClass, paned, args, n);
    clear = XtCreateManagedWidget("clear", commandWidgetClass, paned, args, n);
    print = XtCreateManagedWidget("print", commandWidgetClass, paned, args, n);
    text = XtVaCreateManagedWidget("text", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);
    print1 = XtCreateManagedWidget("print1", commandWidgetClass, paned, args, n);
    text1 = XtVaCreateManagedWidget("text1", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);
    print2 = XtCreateManagedWidget("print2", commandWidgetClass, paned, args, n);
    text2 = XtVaCreateManagedWidget("text2", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);
    print3 = XtCreateManagedWidget("print3", commandWidgetClass, paned, args, n);
    text3 = XtVaCreateManagedWidget("text3", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);
    print4 = XtCreateManagedWidget("print4", commandWidgetClass, paned, args, n);
    text4 = XtVaCreateManagedWidget("text4", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);
    print5 = XtCreateManagedWidget("print5", commandWidgetClass, paned, args, n);
    text5 = XtVaCreateManagedWidget("text5", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);
    print6 = XtCreateManagedWidget("print6", commandWidgetClass, paned, args, n);
    text6 = XtVaCreateManagedWidget("text6", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);
    print7 = XtCreateManagedWidget("print7", commandWidgetClass, paned, args, n);
    text7 = XtVaCreateManagedWidget("text7", asciiTextWidgetClass, paned, XtNtype, XawAsciiString, XtNstring, welcome_message, NULL);

    XtAddCallback(drop, XtNcallback, DropText, (XtPointer) text);
    XtAddCallback(clear, XtNcallback, ClearText, (XtPointer) text);
    XtAddCallback(print, XtNcallback, PrintText, (XtPointer) text);
    XtAddCallback(print1, XtNcallback, PrintText1, (XtPointer) text);
    XtAddCallback(print2, XtNcallback, PrintText2, (XtPointer) text);
    XtAddCallback(print3, XtNcallback, PrintText3, (XtPointer) text);
    XtAddCallback(print4, XtNcallback, PrintText4, (XtPointer) text);
    XtAddCallback(print5, XtNcallback, PrintText5, (XtPointer) text);
    XtAddCallback(print6, XtNcallback, PrintText6, (XtPointer) text);
    XtAddCallback(print7, XtNcallback, PrintText7, (XtPointer) text);

	n = 0;
	XtSetArg(args[n], XtNheight, 300);                        n++;
	XtSetArg(args[n], XtNwidth, 300);                        n++;
	calc_form = XtCreateManagedWidget("form", formWidgetClass, top_level, args, n);
	XtManageChild(calc_form);
//	n = 0;
//	XtSetArg(args[n], XtNheight, 300);                        n++;
//	XtSetArg(args[n], XtNwidth, 300);                        n++;
//	calc_view = XtCreateManagedWidget("text", textWidgetClass, calc_form, NULL, 0);
//	XtManageChild(calc_view);
//	n = 0;
//	XtSetArg(args[n], XtNheight, 200);                        n++;
//	XtSetArg(args[n], XtNwidth, 200);                        n++;
//	dialog_win = XtCreateManagedWidget("dialog", dialogWidgetClass, top_level, args, n);
//	XtManageChild(dialog_win);
	n = 0;
	XtSetArg(args[n], XtNheight, 30);                        n++;
	XtSetArg(args[n], XtNwidth, 180);                        n++;
	XtSetArg(args[n], XtNhorizDistance, 6);                  n++;
	XtSetArg(args[n], XtNjustify, XtJustifyRight);           n++;
	XtSetArg(args[n], XtNlabel, "bar");                     n++;
	display_win = XtCreateManagedWidget("display", labelWidgetClass, calc_form, args, n);
	XtManageChild(display_win);
	dis_rec.widget = display_win;
	dis_rec.value = XtMalloc(MAXSIZE);
	dis_rec.operation = CLR_DISPLAY;

//========================================================================
      /* create a window */
      swa.colormap = cmap;
      swa.border_pixel = 10;
      swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
      win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 700, 800, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
      XSetStandardProperties(dpy, win, "trim_lamanga", "trim_lamanga", None, argv, argc, NULL); 
      //XStoreName(dpy, win, "Trimo1");
      //XSelectInput(dpy,win,ExposureMask | KeyPressMask) ;
/*
      gr_values.function =   GXcopy;
      gr_values.plane_mask = AllPlanes;
      gr_values.foreground = BlackPixel(dpy,screen);
      gr_values.background = WhitePixel(dpy,screen);
      gr_context1=XCreateGC(dpy,win,GCFunction | GCPlaneMask | GCForeground | GCBackground, &gr_values);
      gr_values.function =   GXxor;
      gr_values.foreground = WhitePixel(dpy,screen);
      gr_values.background = BlackPixel(dpy,screen);
      gr_context2=XCreateGC(dpy,win,GCFunction | GCPlaneMask | GCForeground | GCBackground, &gr_values);
*/
      // XIfEvent(dpy, &event, WaitForNotify, (char*)win);  // Look to process_input

      /* connect the context to the window */ /*** (6) bind the rendering context to the window ***/
      glXMakeCurrent(dpy, /* XtWindow(calc_view)*/ win  , cx);
	/*** (7) request the X window to be displayed on the screen ***/
      XMapWindow(dpy, win);
/*** (8) configure the OpenGL context for rendering ***/
	glEnable(GL_DEPTH_TEST);


 char *myargv [1];
 int myargc=1;
 myargv [0]=strdup ("trim_lamanga");
 glutInit(&myargc, myargv);

	init();

	display();

      /* clear the buffer */
 //     glClearColor(1,1,0,1);  // Yellow background
 //     glClear(GL_COLOR_BUFFER_BIT);
      glFlush();


	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	
      /* wait a while */
//      sleep(10);
	 //XtRealizeWidget(top_level);
      while(1) process_input(dpy);
	//XtAppMainLoop(app);
}
/*=====================================================================
//	
//	static struct win_val dis_rec;

//	static char *arith_instance_names[]={"/","x","+","-",};
//	static char *num_instance_names[] = {"7","4","1","0","8","5","2",".","9","6","3","=" };
	
//	static XtCallbackRec NumCallbacks[] = {
//		{ StoreValues, (caddr_t)&dis_rec },
//		{ DisplayUpdate, (caddr_t)&dis_rec },
//		{ (XtCallbackProc)NULL, (caddr_t)NULL }
//		};
//	static XtCallbackRec OprCallbacks[] = {
//		{ SetOperator, (caddr_t)&dis_rec },
//		{ (XtCallbackProc)NULL, (caddr_t)NULL }
//		};
//	static XtCallbackRec EqlCallbacks[] = {
//		{ ComputeValue, (caddr_t)&dis_rec },
//		{ DisplayUpdate, (caddr_t)&dis_rec },
//		{ ClearValues, (caddr_t)&dis_rec },
//		{ (XtCallbackProc)NULL, (caddr_t)NULL }
//		};
//	static XtCallbackRec ClrCallbacks[] = {
//		{ ClearValues, (caddr_t)&dis_rec },
//		{ DisplayUpdate, (caddr_t)&dis_rec },
//		{ (XtCallbackProc)NULL, (caddr_t)NULL }
//		};

//for( i = j = n = 0; i < BUTTONS/4; i++ ) {
//	if ( i != 0 ) {
//		XtSetArg(args[n], XtNfromHoriz, vbox[i-1]);  n++;
//	}
//	XtSetArg(args[n], XtNhorizDistance, 8);                   n++;
//	XtSetArg(args[n], XtNvertDistance, 10);                    n++;
//	XtSetArg(args[n], XtNfromVert, display_win);                n++;
//	XtSetArg(args[n], XtNborderWidth, 0);                     n++;
//	XtSetArg(args[n], XtNvSpace, 10);                         n++;
//	XtSetArg(args[n], XtNhSpace, 5);                          n++;
//	vbox[i] = XtCreateManagedWidget("operators",
//					boxWidgetClass,
//					calc_form, args, n);
//	if ( i+1 < BUTTONS / 4 ) {
//		for( k = n = 0; k < BUTTONS/4; j++, k++ ) {
//			XtSetArg(args[n], XtNwidth, 27);              n++;
//			XtSetArg(args[n], XtNheight, 22);                  n++;
//			if( *(num_instance_names[j]) == '=' ) {
//				XtSetArg(args[n], XtNcallback, EqlCallbacks);
//				n++;
//			}
//			else {
//				XtSetArg(args[n], XtNcallback, NumCallbacks);
//				n++;
//			}
//			XtCreateManagedWidget(num_instance_names[j],
//					commandWidgetClass,
//					vbox[i], args, n);
//		}
//	}
//	else {
//		for( k=n=0; k < BUTTONS/4; n=0, k++) {
//			XtSetArg(args[n], XtNwidth,27); n++;
//			XtSetArg(args[n], XtNheight, 22); n++;
//			XtSetArg(args[n], XtNcallback, OprCallbacks);n++;
//			(void) XtCreateManagedWidget(
//					arith_instance_names[k],
//					commandWidgetClass,
//					vbox[i], args,n); }
//	}
//}
//n = 0;
//XtSetArg(args[n], XtNfromVert, vbox[0]); n++;
//XtSetArg(args[n], XtNvertDistance, 5); n++;
//XtSetArg(args[n], XtNhorizDistance, 30); n++;
//XtSetArg(args[n], XtNwidth, 50); n++;
//XtSetArg(args[n], XtNheight, 22); n++;
//off_key = XtCreateManagedWidget("off", commandWidgetClass, calc_form, args, n);
//XtAddCallback(off_key, XtNcallback, (XtCallbackProc)exit, NULL);
//n = 0;
//XtSetArg(args[n], XtNfromHoriz, off_key); n++;
//XtSetArg(args[n], XtNhorizDistance, 30); n++;
//XtSetArg(args[n], XtNfromVert, vbox[3]); n++;
//XtSetArg(args[n], XtNvertDistance, 5); n++;
//XtSetArg(args[n], XtNcallback, ClrCallbacks); n++;
//XtSetArg(args[n], XtNheight, 22); n++;
//XtSetArg(args[n], XtNwidth, 50); n++;
//(void)XtCreateManagedWidget("cir",
//		commandWidgetClass,
//		calc_form, args, n);
//}
} */
