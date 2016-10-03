
#include <math.h>


/* RGBCIELCH GLOBAL VARIABLES */
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    float xyz_x = 0;
    float xyz_y = 0;
    float xyz_z = 0;

    float cielab_l = 0;
    float cielab_a = 0;
    float cielab_b = 0;

    float cielch_l = 0;
    float cielch_c = 0;
    float cielch_h = 0;

/* RGBCIELCH FUNCTION DEFINITIONS */
    void bound ( float v, float l, float h ){
        return Math.min(h,Math.max(l,v));
    }
    
    void convertRBGtoXYZ ( void ){

        float tmp_r = (float)red / 255;
        float tmp_g = (float)green / 255;
        float tmp_b = (float)blue / 255;
   
        if (tmp_r > 0.04045) {
            tmp_r = Math.pow(((tmp_r + 0.055) / 1.055), 2.4);
        } else {
            tmp_r = tmp_r / 12.92;
        }
        if (tmp_g > 0.04045) {
            tmp_g = Math.pow(((tmp_g + 0.055) / 1.055), 2.4);
        } else {
            tmp_g = tmp_g / 12.92;
        }
        if (tmp_b > 0.04045) {
            tmp_b = Math.pow(((tmp_b + 0.055) / 1.055), 2.4);
        } else {
            tmp_b = tmp_b / 12.92;
        }
 
        tmp_r = tmp_r * 100;
        tmp_g = tmp_g * 100;
        tmp_b = tmp_b * 100;

        xyz_x = tmp_r * 0.4124 + tmp_g * 0.3576 + tmp_b * 0.1805;
        xyz_y = tmp_r * 0.2126 + tmp_g * 0.7152 + tmp_b * 0.0722;
        xyz_z = tmp_r * 0.0193 + tmp_g * 0.1192 + tmp_b * 0.9505;

    }

    void convertXYZtoCIELab ( void ){

        float x = xyz_x / 95.047;
        float y = xyz_y / 100.000;
        float z = xyz_z / 108.883;

        if (x > 0.008856) {
            x = Math.pow(x, 1 / 3);
        } else {
            x = (7.787 * x) + (16 / 116);
        }
        if (y > 0.008856) {
            y = Math.pow(y, 1 / 3);
        } else {
            y = (7.787 * y) + (16 / 116);
        }
        if (z > 0.008856) {
            z = Math.pow(z, 1 / 3);
        } else {
            z = (7.787 * z) + (16 / 116);
        }

        if (y > 0.008856) {
            cielab_l = (116 * y) - 16;
        } else {
            cielab_l = 903.3 * y;
        }

        cielab_a = 500 * (x - y);
        cielab_b = 200 * (y - z);

    }

    void convertCIELabToCIELCh ( void ){
        
        float var_H = Math.atan2(cielab_b, cielab_a);;

        if (var_H > 0) {
            var_H = (var_H / Math.PI) * 180;
        } else {
            var_H = 360 - (Math.abs(var_H) / Math.PI) * 180
        }

        cielch_l = cielab_l;
        cielch_c = Math.sqrt(Math.pow(cielab_a, 2) + Math.pow(cielab_b, 2));
        cielch_h = var_H < 360 ? var_H : (var_H - 360);
    }

    void convertCIELChToCIELab ( void ){
        
        var hradi = cielch_h * (Math.PI / 180);
        
        cielab_l = cielch_l;
        cielab_a = Math.cos(hradi) * cielch_c;
        cielab_b = Math.sin(hradi) * cielch_c;
    }

    void convertCIELabToXYZ ( void ){
      
        float var_Y = (cielab_l + 16) / 116;
        float var_X = cielab_a / 500 + var_Y;
        float var_Z = var_Y - cielab_b / 200;

        if (Math.pow(var_Y, 3) > 0.008856) {
            var_Y = Math.pow(var_Y, 3);
        } else {
            var_Y = (var_Y - 16 / 116) / 7.787;
        }
        if (Math.pow(var_X, 3) > 0.008856) {
            var_X = Math.pow(var_X, 3);
        } else {
            var_X = (var_X - 16 / 116) / 7.787;
        }
        if (Math.pow(var_Z, 3) > 0.008856) {
            var_Z = Math.pow(var_Z, 3);
        } else {
            var_Z = (var_Z - 16 / 116) / 7.787;
        }

        xyz_x = 95.047 * var_X;
        xyz_y = 100.000 * var_Y;
        xyz_z = 108.883 * var_Z;
    }

    void convertXYZtoRGB ( void ){
     
        float var_R = (xyz_x / 100) * 3.2406 + (xyz_y / 100) * -1.5372 + (xyz_z / 100) * -0.4986;
        float var_G = (xyz_x / 100) * -0.9689 + (xyz_y / 100) * 1.8758 + (xyz_z / 100) * 0.0415;
        float var_B = (xyz_x / 100) * 0.0557 + (xyz_y / 100) * -0.2040 + (xyz_z / 100) * 1.0570;

        if (var_R > 0.0031308) {
            var_R = 1.055 * Math.pow(var_R, (1 / 2.4)) - 0.055;
        } else {
            var_R = 12.92 * var_R;
        }
        if (var_G > 0.0031308) {
            var_G = 1.055 * Math.pow(var_G, (1 / 2.4)) - 0.055;
        } else {
            var_G = 12.92 * var_G;
        }
        if (var_B > 0.0031308) {
            var_B = 1.055 * Math.pow(var_B, (1 / 2.4)) - 0.055;
        } else {
            var_B = 12.92 * var_B;
        }

        red = bound((var_R * 255), 0, 255);
        green = bound((var_G * 255), 0, 255);
        blue = bound((var_B * 255), 0, 255);
    }

    void convertRGBtoCIELCh ( void ){
        convertRBGtoXYZ();
        convertXYZtoCIELab();
        convertCIELabToCIELCh();
    }

    void convertCIELChToRGB ( void ){
        convertCIELChToCIELab();
        convertCIELabToXYZ();
        convertXYZtoRGB();
    }
    

    /*  TO BE DEFERRED  */
    void rgbcielchCleanVars ( void ){
        red=green=blue=xyz_x=xyz_y=xyz_z=cielab_l=cielab_a=cielab_b=cielch_l=cielch_c=cielch_h=0;
    }
