/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
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

    /*! \file   csps-elphel-repair.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-elphel-repair
     *
     *  \section csps-suite
     *  \section _ CSPS library front-end suite
     *
     *  The csps-suite is dedicated to CSPS processing using libcsps. It provides
     *  a serie of softwares used for camera logs-files preparation and 
     *  validation according to manufacturers standards. It comes with a main 
     *  software that is responsible of CSPS processing, using libcsps, of the
     *  camera logs-files and also offers softwares for CSPS processing results
     *  visualization and exportation.
     *
     *  \section Documentation
     *
     *  A detailed documentation can be generated through doxygen. A more general
     *  documentation can be consulted at https://github.com/FoxelSA/csps-suite/wiki.
     *
     *  \section Copyright
     * 
     *  Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch \n
     *  This program is part of the FOXEL project <http://foxel.ch>.
     *  
     *  Please read the COPYRIGHT.md file for more information.
     *
     *  \section License
     *
     *  This program is licensed under the terms of the GNU Affero General Public
     *  License v3 (GNU AGPL), with two additional terms. The content is licensed
     *  under the terms of the Creative Commons Attribution-ShareAlike 4.0
     *  International (CC BY-SA) license.
     *
     *  You must read <http://foxel.ch/license> for more information about our
     *  Licensing terms and our Usage and Attribution guidelines.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_ELPHEL_REPAIR__
    # define __CS_ELPHEL_REPAIR__

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
    # include <stdint.h>
    # include <libgen.h>
    # include <csps-all.h>
    # include <common-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n\n"                       \
    "\tcsps-elphel-repair [Arguments] [Parameters] ...\n\n"      \
    "Short arguments and parameters summary :\n\n"               \
    "\t-s\tDirectory containing logs-files to repair\n"          \
    "\t-d\tDirectory where repaired logs-files are exported\n\n" \
    "csps-elphel-repair - csps-suite\n"                          \
    "Copyright (c) 2013-2015 FOXEL SA\n"

    /* Define GPS sequence parameters */
    # define CS_MEASURE     4
    # define CS_GROUPS      5
    # define CS_BUFFERS     CS_MEASURE * CS_GROUPS

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
     *  The main function expects logs-files that have been processed in order
     *  to remove anomalous records or block of records. It parses the logs-files
     *  contained in the source directory and calls the GPS decimation process
     *  for each logs-file. The decimated logs-files are then written in the
     *  destination directory.
     *  
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *
     *  \return Returns exit code
     */

    int main ( int argc, char ** argv );

    /*! \brief Logs-file repair procedure
     *
     */

    unsigned long cs_elphel_repair(

        char * const csiFile,
        char * const csoFile

    );

    /*! \brief Record probabilist validation
     *
     *  This function detects event record filled with invalid data, typically
     *  random bytes. It returns boolean value that indicates provided record
     *  validity.
     *
     *  \param  csBuffer Pointer to record buffer
     *
     *  \return Returns boolean value that indicates record validity
     */

    int cs_elphel_repair_record(

        lp_Byte_t const * const csBuffer

    );

    /*! \brief GPS events specific procedure
     *
     */

    int cs_elphel_repair_detect(

        lp_Byte_t const * const csBuffer

    );

    /*! \brief GPS events specific procedure
     *
     */

    int cs_elphel_repair_filter(

        lp_Byte_t csgpsStack[CS_BUFFERS][LC_RECORD]

    );

    /*! \brief GPS clock access
     *
     *  This function parses the provided NMEA/GGA or NMEA/RMC sentence string
     *  buffer and extract the GPS clock value. It the returns the found clock
     *  value.
     *
     *  The provided NMEA/GGA or NMEA RMC sentence is expected without its type
     *  header "$GPGGA," or "$GPRMC," and should start directly with GPS clock.
     *
     *  \param  csSentence NMEA/GGA or NMEA/RMC sentence string buffer
     *
     *  \return Returns GGA/RMC sentence GPS clock
     */

    double cs_elphel_repair_clock(

        lp_Char_t const * const csSentence

    );

    /*! \brief GPS fix access
     *
     *  This function parses the provided NMEA/GGA sentence string buffer to
     *  retrieve the value of the GPS measure fix. It then returns the found
     *  fix value.
     *
     *  The provided NMEA/GGA sentence is expected without its type header
     *  "$GPGGA," and has to start directly by the GPS clock.
     *
     *  \param  csSentence NMEA/GGA sentence string buffer
     *
     *  \return Returns GGA sentence fix value
     */

    unsigned long cs_elphel_repair_fix(

        lp_Char_t const * const csSentence

    );

    /*! \brief GPS timestamp computation
     * 
     *  This function rebuilds the GPS events timestamp on the base of the last
     *  reference timestamp and the number of repetition of this reference
     *  timestamp, assuming a frequency of measure of five hertz.
     *
     *  \param  csReference Last reference timestamp
     *  \param  csRepet     Number of repetition of the reference timestamp
     *
     *  \return Returns rebuilded GPS event timestamp
     */

    lp_Time_t cs_elphel_repair_timestamp( 

        lp_Time_t     const csReference,
        unsigned long const csDistance 

    );

    /*! \brief Replace record timestamp
     *  
     *  This function interprets the eight first bytes of a logs-file record
     *  buffer as a timestamp value and replaces its original timestamp by the 
     *  provided one.
     * 
     *  \param csHeader Pointer to record buffer
     *  \param csTime   Timestamp to write in record
     */

    void cs_elphel_repair_header( 

        lp_Time_t * const csRecord, 
        lp_Time_t   const csTime 

    );

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


