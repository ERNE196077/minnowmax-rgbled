#ifndef RGBCIELCH_H_
#define RGBCIELCH_H_

/*
 * rgbcielch.h
 *
 * RGB color convertion functions for correct color gradients/mixing.
 *
 *  Created on: Sep 18, 2016
 *      Author: minnow
 */

/*  RGB <-> CIELCH FUNCTIONS */
    void bound ( float v, float l, float h );
    void convertRBGtoXYZ ( void );
    void convertXYZtoCIELab ( void );
    void convertCIELabToCIELCh ( void );
    void convertCIELChToCIELab ( void );
    void convertCIELabToXYZ ( void );
    void convertXYZtoRGB ( void );
    void convertRGBtoCIELCh ( void );
    void convertCIELChToRGB ( void );
    void rgbcielchCleanVars ( void );


#endif
