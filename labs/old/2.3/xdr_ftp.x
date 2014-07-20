const MAXNAMELEN = 255;   /*  max length of a file name  */

/*
 * Types of files:
 */

enum dataselect {
     COMMAND = 0,       /*  Command  */
     DATA = 1       /*  Size  */
};

/*
 * File information, per kind of file:
 */

union datatype switch (dataselect select) {
      case COMMAND:
      	   string filename<MAXNAMELEN>;        /*  name of file  */
      case DATA:
	   int data;            /*  file data  */
};
