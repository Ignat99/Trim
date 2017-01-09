#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Cardinals.h>
#define BUTTONS 16
#define MAXSIZE 50

enum OPS {CLR_DISPLAY, DIVIDE, MULTIPLY, ADD, SUBTRACT};

struct win_val {
String value;
Widget widget;
enum OPS operation;
};
char first_num[MAXSIZE], second_num[MAXSIZE];

static void
DisplayUpdate(Widget w, XtPointer client_data, XtPointer cd)
{
	struct win_val *display=(struct win_val*)client_data;

		if ( display->value )
			XtVaSetValues(display->widget, XtNlabel,
			display->value, (String)0);
}

static void
ClearValues(Widget w, XtPointer client_data, XtPointer cd)
{
	struct win_val *display = (struct win_val*) client_data;
	char *buf;

		XtVaGetValues(w, XtNlabel, &buf, (String)0);
		if( *buf != '=' )
			(void) strcpy(display->value, "0.00");
		first_num[0] = second_num[0] = (char)NULL;
		display->operation = CLR_DISPLAY;
}
static void
StoreValues(Widget w, XtPointer client_data, XtPointer cd)
{
	struct win_val *display = (struct win_val*)client_data;
	char *buf;

		XtVaGetValues(w, XtNlabel, &buf, (String)0);
		if ( display->operation == CLR_DISPLAY) {
			(void)strncat(first_num, buf, 1);
			(void)strcpy(display->value, first_num);
		}
		else {
			*(display->value) = NULL;
			(void)strncat(second_num, buf, 1);
			(void)strcpy(display->value, second_num);
		}
}

static void
SetOperator(Widget w, XtPointer client_data, XtPointer cd)
{
	struct win_val *display = (struct win_val*)client_data;
	char *buf;

		XtVaGetValues(w, XtNlabel, &buf, (String)0);
		switch(*buf)
		{
			case '/': display->operation = DIVIDE; break;
			case 'x': display->operation = MULTIPLY; break;
			case '+': display->operation = ADD; break;
			case '-': display->operation = SUBTRACT; break;
		}
}

static void
ComputeValue(Widget w, XtPointer client_data, XtPointer cd)
{
	struct win_val *display = (struct win_val*)client_data;
	double operand1, operand2, result; //, atof();
	char zeroflag = 0;

		operand1 = atof(first_num);
		operand2 = atof(second_num);
		switch (display->operation) {
			case DIVIDE: if( operand2 != 0.0 ) result = operand1 / operand2; else zeroflag++; break;
			case MULTIPLY: result = operand1 * operand2; break;
			case ADD: result = operand1 + operand2; break;
			case SUBTRACT: result = operand1 - operand2; break;
		}
		if ( !zeroflag )
			(void)sprintf(display->value, "%.2lf", result);
		else
			(void)strcpy(display->value, "ERROR");
}
/*
int main(int argc, char **argv)
{
	Arg args[25];
	int i,j,k,n;
	static struct win_val dis_rec;
	Widget top_level, calc_form, display_win, off_key, vbox[BUTTONS/4];
	XtAppContext ac;

	static char *arith_instance_names[]={"/","x","+","-",};
	static char *num_instance_names[] = {"7","4","1","0","8","5","2",".","9","6","3","=" };
	
	static XtCallbackRec NumCallbacks[] = {
		{ StoreValues, (caddr_t)&dis_rec },
		{ DisplayUpdate, (caddr_t)&dis_rec },
		{ (XtCallbackProc)NULL, (caddr_t)NULL }
		};
	static XtCallbackRec OprCallbacks[] = {
		{ SetOperator, (caddr_t)&dis_rec },
		{ (XtCallbackProc)NULL, (caddr_t)NULL }
		};
	static XtCallbackRec EqlCallbacks[] = {
		{ ComputeValue, (caddr_t)&dis_rec },
		{ DisplayUpdate, (caddr_t)&dis_rec },
		{ ClearValues, (caddr_t)&dis_rec },
		{ (XtCallbackProc)NULL, (caddr_t)NULL }
		};
	static XtCallbackRec ClrCallbacks[] = {
		{ ClearValues, (caddr_t)&dis_rec },
		{ DisplayUpdate, (caddr_t)&dis_rec },
		{ (XtCallbackProc)NULL, (caddr_t)NULL }
		};
	top_level = XtAppInitialize( &ac, "Calc",
					(XrmOptionDescRec*)NULL,
					0, &argc, argv,
					NULL, NULL, 0);

	calc_form = XtCreateManagedWidget("form", formWidgetClass, top_level, NULL, 0);
	n = 0;
	XtSetArg(args[n], XtNheight, 30);                        n++;
	XtSetArg(args[n], XtNwidth, 180);                        n++;
	XtSetArg(args[n], XtNhorizDistance, 6);                  n++;
	XtSetArg(args[n], XtNjustify, XtJustifyRight);           n++;
	XtSetArg(args[n], XtNlabel, "0.00");                     n++;
	display_win = XtCreateManagedWidget("display",
				labelWidgetClass,
				calc_form,
				args, n);
	dis_rec.widget = display_win;
	dis_rec.value = XtMalloc(MAXSIZE);
	dis_rec.operation = CLR_DISPLAY;

for( i = j = n = 0; i < BUTTONS/4; i++ ) {
	if ( i != 0 ) {
		XtSetArg(args[n], XtNfromHoriz, vbox[i-1]);  n++;
	}
	XtSetArg(args[n], XtNhorizDistance, 8);                   n++;
	XtSetArg(args[n], XtNvertDistance, 10);                    n++;
	XtSetArg(args[n], XtNfromVert, display_win);                n++;
	XtSetArg(args[n], XtNborderWidth, 0);                     n++;
	XtSetArg(args[n], XtNvSpace, 10);                         n++;
	XtSetArg(args[n], XtNhSpace, 5);                          n++;
	vbox[i] = XtCreateManagedWidget("operators",
					boxWidgetClass,
					calc_form, args, n);
	if ( i+1 < BUTTONS / 4 ) {
		for( k = n = 0; k < BUTTONS/4; j++, k++ ) {
			XtSetArg(args[n], XtNwidth, 27);              n++;
			XtSetArg(args[n], XtNheight, 22);                  n++;
			if( *(num_instance_names[j]) == '=' ) {
				XtSetArg(args[n], XtNcallback, EqlCallbacks);
				n++;
			}
			else {
				XtSetArg(args[n], XtNcallback, NumCallbacks);
				n++;
			}
			XtCreateManagedWidget(num_instance_names[j],
					commandWidgetClass,
					vbox[i], args, n);
		}
	}
	else {
		for( k=n=0; k < BUTTONS/4; n=0, k++) {
			XtSetArg(args[n], XtNwidth,27); n++;
			XtSetArg(args[n], XtNheight, 22); n++;
			XtSetArg(args[n], XtNcallback, OprCallbacks);n++;
			(void) XtCreateManagedWidget(
					arith_instance_names[k],
					commandWidgetClass,
					vbox[i], args,n); }
	}
}
n = 0;
XtSetArg(args[n], XtNfromVert, vbox[0]); n++;
XtSetArg(args[n], XtNvertDistance, 5); n++;
XtSetArg(args[n], XtNhorizDistance, 30); n++;
XtSetArg(args[n], XtNwidth, 50); n++;
XtSetArg(args[n], XtNheight, 22); n++;
off_key = XtCreateManagedWidget("off", commandWidgetClass, calc_form, args, n);
XtAddCallback(off_key, XtNcallback, (XtCallbackProc)exit, NULL);
n = 0;
XtSetArg(args[n], XtNfromHoriz, off_key); n++;
XtSetArg(args[n], XtNhorizDistance, 30); n++;
XtSetArg(args[n], XtNfromVert, vbox[3]); n++;
XtSetArg(args[n], XtNvertDistance, 5); n++;
XtSetArg(args[n], XtNcallback, ClrCallbacks); n++;
XtSetArg(args[n], XtNheight, 22); n++;
XtSetArg(args[n], XtNwidth, 50); n++;
(void)XtCreateManagedWidget("cir",
		commandWidgetClass,
		calc_form, args, n);
XtRealizeWidget(top_level);
XtAppMainLoop(ac);
}
*/
