#include <tcl/tcl.h>

int dbd_tcl_init(Tcl_Interp *interp);

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */

#ifdef NEED_MATHERR
extern int matherr();
int *tclDummyMathPtr = (int *) matherr;
#endif

int main(int argc, char *argv[])
{
    Tcl_Main(argc, argv, Tcl_AppInit);
    return 0;		
}

int Tcl_AppInit(Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

    if (dbd_tcl_init(interp) == TCL_ERROR) {
		return TCL_ERROR;
    }

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively. 
     */

    tcl_RcFileName = "~/.qservrc";
    return TCL_OK;
}
