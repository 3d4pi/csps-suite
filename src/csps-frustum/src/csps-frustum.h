/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch
 * Please read <http://foxel.ch/license> for more information.
 *
 *
 * Author(s):
 *
 *      Nils Hamel <n.hamel@foxel.ch>
 *
 *
 * This file is part of the FOXEL project <http://foxel.ch>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Additional Terms:
 *
 *      You are required to preserve legal notices and author attributions in
 *      that material or in the Appropriate Legal Notices displayed by works
 *      containing it.
 *
 *      You are required to attribute the work as explained in the "Usage and
 *      Attribution" section of <http://foxel.ch/license>.
 */

    /*! \file   csps-frustum.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-frustum
     *
     *  Front-end suite for CSPS library.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_FRUSTUM__
    # define __CS_FRUSTUM__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include <stdio.h>
    # include <stdlib.h>
    # include <string.h>
    # include <float.h>
    # include <csps-all.h>
    # include <fastcal-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"            \
    "  csps-frustum [Arguments] [Parameters] ...\n" \
    "Short arguments and parameters summary :\n"    \
    "  -p Record segment directory\n"               \
    "csps-cones - csps-suite\n"                     \
    "Copyright (c) 2013-2014 FOXEL SA\n"

    /* Define standard types */
    # define CS_NULL            0
    # define CS_STRING          1
    # define CS_CHAR            2
    # define CS_SHORT           3
    # define CS_INT             4
    # define CS_LONG            5
    # define CS_LLONG           6
    # define CS_UCHAR           7
    # define CS_USHORT          8
    # define CS_UINT            9
    # define CS_ULONG           10
    # define CS_ULLONG          11
    # define CS_FLOAT           12
    # define CS_DOUBLE          13

    /* Define standard output */
    # define CS_OUT             stdout

    /* Define detection results */
    # define CS_FALSE           0
    # define CS_TRUE            1

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  The main function is responsible for determining if two camera
     *  sensors have an intersection in thier frustum. It asks the
     *  frustum vectors and eyesis frame position in order to synchronize
     *  them on earth through libcsps. The frustum are then built and
     *  compared. If frustums have an intersection, the function display
     *  TRUE on the standard output, FALSE otherwise.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     */

    int main ( int argc, char ** argv );

    /*! \brief Frustum vector in eyesis4pi frame
     *  
     *  This function builds, using calibration data of the wanted sensor
     *  of the desired camera, the two vectors that define the sensor
     *  frustum.
     *
     *  \param csCamera Camera device (MAC address)
     *  \param csChannel Camera sensor index
     *  \param csNadir Three-entries vector that recieve the frustum nadir vector
     *  \param csRight Three-entries vector that recieve the frustum right vector
     *  \param csPosition Three-entries vector that recieve the frustum origin in eyesis4pi frame
     */

    void cs_frustum_eyesis4pi( 

        char   const * const csCamera, 
        int    const         csChannel, 
        double       * const csNadir, 
        double       * const csRight,
        double       * const csPosition

    );

    /*! \brief Frustum summit computer
     *
     *  This function takes the frustum definition vectors, the position
     *  vector and the camera calibration data to build the summit of
     *  the sensor frustum.
     *
     *  \param csNadir Three-entries vector that contains the frustum nadir vector
     *  \param csRight Three-entries vector that contains the frustum right vector
     *  \param csPosition Three-entries vector that contains the frustum origin
     *  \param csPixel Size, in meters, of the sensor pixels
     *  \param csFocal Focal length, in meters, of the sensor lense
     *  \param csWidth Width, in pixels, of the sensor
     *  \param csHeight Height, in pixels, of the sensor
     *  \param csNear Near plane of the built frustum
     *  \param csFar far plane of the built frustum
     *  \param csFX Eight-entries array that recieve the frustum summits x-coordinates
     *  \param csFY Eight-entries array that recieve the frustum summits y-coordinates
     *  \param csFZ Eight-entries array that recieve the frustum summits z-coordinates
     */

    void cs_frustum_summit( 

        double const * const csNadir, 
        double const * const csRight, 
        double const * const csPosition, 
        double const         csPixel, 
        double const         csFocal, 
        double const         csWidth, 
        double const         csHeight, 
        double const         csNear, 
        double const         csFar,
        double       * const csFX,
        double       * const csFY,
        double       * const csFZ

    );

    /*! \brief Frustum intersection detector
     *
     *  This function considers the summits of two frustums in order
     *  to determine if they have an intersection. The intersection
     *  condition is mathematically based on the fact that frustum are
     *  convex polyhedrons.
     *
     *  \param csFXa Eight-entries array that contains the first frustum summits x-coordinates
     *  \param csFYa Eight-entries array that contains the first frustum summit y-coordinates
     *  \param csFZa Eight-entries array that contains the first frustum summit z-coordinates
     *  \param csFXb Eight-entries array that contains the second frustum summit x-coordinates
     *  \param csFYb Eight-entries array that contains the second frustum summit y-coordinates
     *  \param csFZb Eight-entries array that contains the second frustum summit z-coordinates
     *  \return Returns CS_TRUE if an intersection is detected, CS_FALSE otherwise
     */

    int cs_frustum_intersection(

        double const * const csFXa,
        double const * const csFYa,
        double const * const csFZa,
        double const * const csFXb,
        double const * const csFYb,
        double const * const csFZb

    );

    /*! \brief Double arrays extremums extraction
     *
     *  This function extracts extremum values, that are minimum and
     *  maximum, from an array of double.
     *  
     *  \param csArray Pointer to double array
     *  \param csSize Size of the double array, in type units
     *  \param csMaximum Maximum value reciever
     *  \param csMinimum Minimum value reciever
     */

    void cs_frustum_extremum( 

        double const * const csArray, 
        int    const         csSize, 
        double       * const csMaximum, 
        double       * const csMinimum 

    );

    /*! \brief Search agrument position in argv
     *  
     *  This function search in the argv string array the position of the argument
     *  defined through ltag/stag and return the index of the corresponding parameter
     *  in argv.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     *  \param ltag Long-form argument string (--argument)
     *  \param stag Short-form argument string (-a)
     *  \return Index of parameter in argv
     */

    int  cs_stda ( int argc, char ** argv, const char * const ltag, const char * const stag );

    /*! \brief Parameter reader in argv
     *  
     *  This function interpret the parameter in the desired type and return it through
     *  the param variable. The argi variable is typically set using stda function. If
     *  argi is set to CS_NULL, the function does nothing.
     *  
     *  \param argi Index of the parameter in argv
     *  \param argv Standard main parameter
     *  \param param Pointer to the variable that recieve the parameter
     *  \param type Type to use for parameter interpretation
     */

    void cs_stdp ( int argi, char ** argv, void * param, int type );

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    }
    # endif

/*
    Header - Include guard
 */

    # endif


