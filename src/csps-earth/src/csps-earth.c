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


/* 
    Source - Includes
 */

    # include "csps-earth.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csRigs[256] = { 0 };
        char csiPly[256] = { 0 };
        char csoPly[256] = { 0 };

        /* Stream pointer variables */
        char csCAMd[256] = { 0 };
        char csCAMm[256] = { 0 };
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };

        /* Timestamp delay variables */
        long csDelay = 0;

        /* Rotation matrix variables */
        double csR[3][3] = { { 0.0 } };

        /* Translation vector variables */
        double csT[3] = { 0.0 };

        /* WGS84 curve alignment */
        cs_WGS84_t csWGS = { 0, 0, 0, 0 };

        /* Curve structure variables */
        cs_Curve_t csMVG = { 0, 0, NULL };
        cs_Curve_t csGPS = { 0, 0, NULL };

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"      , "-p" ), argv,   csPath , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--rigs-path" , "-r" ), argv,   csRigs , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--input-ply" , "-i" ), argv,   csiPly , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--output-ply", "-o" ), argv,   csoPly , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-tag"   , "-c" ), argv,   csCAMd , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-mod"   , "-m" ), argv,   csCAMm , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag"   , "-g" ), argv,   csGPSd , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod"   , "-n" ), argv,   csGPSm , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--delay"     , "-d" ), argv, & csDelay, LC_LONG   );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {
         
            /* MVG and GPS curve importation */
            cs_earth_curve( csPath, csRigs, csCAMd, csCAMm, csGPSd, csGPSm, & csMVG, & csGPS, csDelay );

            /* WGS84 frame alignment model computation */
            csWGS = cs_earth_model( & csGPS );

            /* Estimation of linear transformation  */
            cs_earth_lte( csGPS.cvSize / 3, csGPS.cvData, csMVG.cvData, csR, csT );

            /* Linear transformation application on ply file */
            cs_earth_transform( csiPly, csoPly, csR, csT, & csWGS );
   
        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - WGS84 frame alignment model
*/

    cs_WGS84_t cs_earth_model(

        cs_Curve_t * const csGPS

    ) {

        /* Parsing variables */
        size_t csParse = 0;

        /* Returned value variables */
        cs_WGS84_t csWGS = { 0.0, 0.0, 0.0, 0.0 };

        /* Parsing WGS84-GPS track */
        while ( csParse < csGPS->cvSize ) {

            /* Accumulate WGS84-GPS track */
            csWGS.wglonm += csGPS->cvData[csParse    ];
            csWGS.wglatm += csGPS->cvData[csParse + 1];
            csWGS.wgaltm += csGPS->cvData[csParse + 2];

            /* Update index */
            csParse += 3;

        }

        /* Compute WGS84-GPS track center of mass */
        csWGS.wglonm /= ( double ) ( csGPS->cvSize / 3 );
        csWGS.wglatm /= ( double ) ( csGPS->cvSize / 3 );
        csWGS.wgaltm /= ( double ) ( csGPS->cvSize / 3 );

        /* Compute metric factor */
        csWGS.wgfactor = ( 6378137.0 + csWGS.wgaltm ) * ( LP_PI / 180 );

        /* Reset parsing variables */
        csParse = 0;

        /* Parsing GPS track */
        while ( csParse < csGPS->cvSize ) {

            /* Apply alignment model */
            csGPS->cvData[csParse    ] = ( csGPS->cvData[csParse    ] - csWGS.wglonm ) * csWGS.wgfactor;
            csGPS->cvData[csParse + 1] = ( csGPS->cvData[csParse + 1] - csWGS.wglatm ) * csWGS.wgfactor;

            /* Update index */
            csParse += 3;

        }

        /* Return computed model */
        return( csWGS );

    }

/*
    Source - Stanford triangle format linear transformation
*/

    void cs_earth_transform( 

        char       const * const csiPly,
        char       const * const csoPly,
        double     const         csR[3][3],
        double     const         csT[3],
        cs_WGS84_t const * const csWGS

    ) {

        /* Token variables */
        char csToken[256];

        /* Reading mode variables */
        int csMode = CS_HEADER;

        /* Interpreted value */
        double csValue = 0.0;
        double csValue2 = 0.0;
        double csValue3 = 0.0;
        double csnx = 0.0;
        double csny = 0.0;
        double csnz = 0.0;

        /* Stream variables */
        FILE * csiStream = NULL;
        FILE * csoStream = NULL;

        /* Create input stream */
        if ( ( csiStream = fopen( csiPly, "r" ) ) == NULL ) {

            /* Display message */
            fprintf( LC_ERR, "Error : unable to load %s for input\n", basename( ( char * ) csiPly ) );

        } else {

            /* Create output stream */
            if ( ( csoStream = fopen( csoPly, "w" ) ) == NULL ) {

                /* Display message */
                fprintf( LC_ERR, "Error : unable to create %s for output\n", basename( ( char * ) csoPly ) );

            } else {

                /* Read initial token */
                if ( strcmp( cs_earth_transform_token( csToken, csiStream ), "ply" ) != 0 ) {

                    /* Display message */
                    fprintf( LC_ERR, "Error : unknown input format\n" );

                } else {

                    int cols = 0;
                    int rets = 0;

                    /* Reading loop */
                    while ( fscanf( csiStream, "%s", csToken ) == 1 ) {

                        if ( csMode == 0 ) {

                            if ( strcmp( csToken, "end_header" ) == 0 ) {

                                fprintf( csoStream, "%s\n", csToken ); 
                                csMode = 1;
                                rets = 0;

                            } else
                            if ( strcmp( csToken, "property" ) == 0 ) {

                                fprintf( csoStream, "%s ", csToken );
                                rets = fscanf( csiStream, "%s", csToken ); fprintf( csoStream, "%s " , csToken );
                                rets = fscanf( csiStream, "%s", csToken ); fprintf( csoStream, "%s\n", csToken );
                                cols++;

                            } else
                            if ( strcmp( csToken, "ply" ) == 0 ) {

                                fprintf( csoStream, "%s\n", csToken );

                            }  else {

                                fprintf( csoStream, "%s ", csToken );
                                rets = fscanf( csiStream, "%s", csToken ); fprintf( csoStream, "%s " , csToken );
                                rets = fscanf( csiStream, "%s", csToken ); fprintf( csoStream, "%s\n", csToken );

                            }

                        } else {

                            if ( ( rets % cols ) == 0 ) {

                                csValue = atof( csToken ); 
                                fscanf( csiStream, "%s", csToken ); csValue2 = atof( csToken ); 
                                fscanf( csiStream, "%s", csToken ); csValue3 = atof( csToken ); 

                                csnx = ( csValue - csT[0] ) * csR[0][0] + ( csValue2 - csT[1] ) * csR[1][0] + ( csValue3 - csT[2] ) * csR[2][0];
                                csny = ( csValue - csT[0] ) * csR[0][1] + ( csValue2 - csT[1] ) * csR[1][1] + ( csValue3 - csT[2] ) * csR[2][1];
                                csnz = ( csValue - csT[0] ) * csR[0][2] + ( csValue2 - csT[1] ) * csR[1][2] + ( csValue3 - csT[2] ) * csR[2][2];

                                fprintf( csoStream, "%.16lf %.16lf %.16lf ", ( csnx / csWGS->wgfactor ) + csWGS->wglonm, ( csny / csWGS->wgfactor ) + csWGS->wglatm, csnz );

                                rets += 2;

                            } else {

                                fprintf( csoStream, "%s ", csToken );

                            }

                            if ( ( (++rets) % cols ) == 0 ) fprintf( csoStream, "\n" );

                        }

                }

                }

                /* Close input stream */
                fclose( csoStream );

            }

            /* Close input stream */
            fclose( csiStream );

        }

    }

    char * cs_earth_transform_token(

        char * const csToken,
        FILE * const csStream

    ) {

        /* Read token from file */
        if ( fscanf( csStream, "%s", csToken ) != 1 ) {

            /* Empty token */
            csToken[0] = '\0';

        }

        /* Return token pointer */
        return( csToken );

    }

/*
    Source - Curve creation
*/

    void cs_earth_curve( 

        char       const * const csPath, 
        char       const * const csRigs, 
        char       const * const csCAMd, 
        char       const * const csCAMm, 
        char       const * const csGPSd, 
        char       const * const csGPSm, 
        cs_Curve_t       * const csMVG,
        cs_Curve_t       * const csGPS,
        long       const         csDelay

    ) {

        /* CSPS query variables */
        lp_Trigger_t csTrigger;
        lp_Geopos_t  csGeopos;

        /* Enumeration variables */
        char csFile[256] = { 0 };

        /* Importation variables */
        double csVoid = 0.0;
        double csLng = 0.0;
        double csLat = 0.0;
        double csAlt = 0.0;

        /* Timestamp variables */
        lp_Time_t csTime = 0;
        lp_Time_t csUse = 0;

        /* Stream variables */
        FILE * csStream = NULL;

        /* Create query descriptors */
        csTrigger = lp_query_trigger_create ( csPath, csCAMd, csCAMm );
        csGeopos  = lp_query_position_create( csPath, csGPSd, csGPSm );

        /* Directory entity enumeration */
        while ( lc_file_enum( csRigs, csFile ) != LC_FALSE ) {

            /* Consider only file entity */
            if ( lc_file_detect( csFile, LC_FILE ) == LC_TRUE ) {

                /* Open file */
                if ( ( csStream = fopen( csFile, "r" ) ) != NULL ) {

                    /* Read position */
                    if ( fscanf( csStream, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", & csVoid, & csVoid, & csVoid, & csVoid, & csVoid, & csVoid, & csVoid, & csVoid, & csVoid, & csLng, & csLat, & csAlt ) == 12 ) {

                        /* Retrieve master timestamp */
                        sscanf( basename( csFile ), "%" lp_Time_i "_%" lp_Time_i, & csTime, & csUse );

                        /* Query synch timestamp */
                        lp_query_trigger_bymaster( & csTrigger, lp_timestamp_compose( csTime + csDelay, csUse ) );

                        if ( lp_query_trigger_status( & csTrigger ) == LP_TRUE ) {

                            /* Query position */
                            lp_query_position( & csGeopos, csTrigger.qrSynch );

                            if ( lp_query_position_status( & csGeopos ) == LP_TRUE ) {                                

                                /* Push position */
                                cs_earth_curve_push( csMVG, csLng, csLat, csAlt );

                                /* Push position */
                                cs_earth_curve_push( csGPS, csGeopos.qrLongitude, csGeopos.qrLatitude, csGeopos.qrAltitude );

                            }

                        }

                    }

                    /* Close file */
                    fclose( csStream );

                }

            }

        }

        /* Delete query descriptors */
        lp_query_trigger_delete( & csTrigger );
        lp_query_position_delete( & csGeopos );

    }

    void cs_earth_curve_push( 

        cs_Curve_t * const csCurve, 
        double const cvLng,
        double const cvLat,
        double const cvAlt

    ) {

        /* Array memory verification */
        if ( csCurve->cvSize >= csCurve->cvGhost ) {

            /* Update real memory size */
            csCurve->cvGhost += 3 * 1024;

            /* Re-allocate memory */
            csCurve->cvData = realloc( csCurve->cvData, sizeof( double ) * csCurve->cvGhost );

        }

        /* Assign pushed elements */
        csCurve->cvData[csCurve->cvSize++] = cvLng;
        csCurve->cvData[csCurve->cvSize++] = cvLat;
        csCurve->cvData[csCurve->cvSize++] = cvAlt;

    }

/*
    Source - Linear transformation estimation
*/

    void cs_earth_lte( 

        int    const         csN, 
        double const * const csrData, 
        double const * const csaData, 
        double               csR[3][3], 
        double               csT[3]

    ) {
        /* Parsing variables */
        unsigned long csParse = 0;

        /* Coordinates mean variables */
        double csMean[6] = { 0.0 };

        /* Matrix variables */
        double csH[9] = { 0.0 };
        double csU[9] = { 0.0 };
        double csS[3] = { 0.0 };
        double csV[9] = { 0.0 };

        /* Matrix parameter variables */
        int csld   = 3;
        int csRows = 3;
        int csCols = 3;

        /* Decomposition buffer variables */
        int csReturn = 0;
        int csWork   = -1;

        /* Decomposition buffer variables */
        double   csQuery  = 0.0;
        double * csBuffer = NULL;

        /* Coordinates accumulation */
        for ( csParse = 0; csParse < csN; csParse ++ ) {

            csMean[0] += * ( csrData + csParse * 3     );
            csMean[1] += * ( csrData + csParse * 3 + 1 );
            csMean[2] += * ( csrData + csParse * 3 + 2 );
            csMean[3] += * ( csaData + csParse * 3     );
            csMean[4] += * ( csaData + csParse * 3 + 1 );
            csMean[5] += * ( csaData + csParse * 3 + 2 );

        }

        /* Compute mean of coordinates */
        csMean[0] /= ( double ) csN;
        csMean[1] /= ( double ) csN;
        csMean[2] /= ( double ) csN;
        csMean[3] /= ( double ) csN;
        csMean[4] /= ( double ) csN;
        csMean[5] /= ( double ) csN;

        /* Compute accumulated matrix */
        for ( csParse = 0; csParse < csN; csParse ++ ) {

            csH[0] += ( * ( csrData + csParse * 3     ) - csMean[0] ) * ( * ( csaData + csParse * 3     ) - csMean[3] );
            csH[1] += ( * ( csrData + csParse * 3     ) - csMean[0] ) * ( * ( csaData + csParse * 3 + 1 ) - csMean[4] );
            csH[2] += ( * ( csrData + csParse * 3     ) - csMean[0] ) * ( * ( csaData + csParse * 3 + 2 ) - csMean[5] );
            csH[3] += ( * ( csrData + csParse * 3 + 1 ) - csMean[1] ) * ( * ( csaData + csParse * 3     ) - csMean[3] );
            csH[4] += ( * ( csrData + csParse * 3 + 1 ) - csMean[1] ) * ( * ( csaData + csParse * 3 + 1 ) - csMean[4] );
            csH[5] += ( * ( csrData + csParse * 3 + 1 ) - csMean[1] ) * ( * ( csaData + csParse * 3 + 2 ) - csMean[5] );
            csH[6] += ( * ( csrData + csParse * 3 + 2 ) - csMean[2] ) * ( * ( csaData + csParse * 3     ) - csMean[3] );
            csH[7] += ( * ( csrData + csParse * 3 + 2 ) - csMean[2] ) * ( * ( csaData + csParse * 3 + 1 ) - csMean[4] );
            csH[8] += ( * ( csrData + csParse * 3 + 2 ) - csMean[2] ) * ( * ( csaData + csParse * 3 + 2 ) - csMean[5] );

        }

        /* Singular values decompostion - size computation */
        dgesvd_( "All", "All", & csRows, & csCols, csH, & csld, csS, csU, & csld, csV, & csld, & csQuery, & csWork, & csReturn );

        /* Allocate computation buffer */
        csBuffer = ( double * ) malloc( csWork = ( ( int ) csQuery ) * sizeof( double ) );

        /* Singular values decompostion */
        dgesvd_( "All", "All", & csRows, & csCols, csH, & csld, csS, csU, & csld, csV, & csld, csBuffer, & csWork, & csReturn );

        /* Unallocate computation buffer */
        free( ( void * ) csBuffer );

        /* Compute rotation matrix */
        csR[0][0] = * ( csV     ) * * ( csU     ) + * ( csV + 1 ) * * ( csU + 3 ) + * ( csV + 2 ) * * ( csU + 6 );
        csR[0][1] = * ( csV + 3 ) * * ( csU     ) + * ( csV + 4 ) * * ( csU + 3 ) + * ( csV + 5 ) * * ( csU + 6 );
        csR[0][2] = * ( csV + 6 ) * * ( csU     ) + * ( csV + 7 ) * * ( csU + 3 ) + * ( csV + 8 ) * * ( csU + 6 );
        csR[1][0] = * ( csV     ) * * ( csU + 1 ) + * ( csV + 1 ) * * ( csU + 4 ) + * ( csV + 2 ) * * ( csU + 7 );
        csR[1][1] = * ( csV + 3 ) * * ( csU + 1 ) + * ( csV + 4 ) * * ( csU + 4 ) + * ( csV + 5 ) * * ( csU + 7 );
        csR[1][2] = * ( csV + 6 ) * * ( csU + 1 ) + * ( csV + 7 ) * * ( csU + 4 ) + * ( csV + 8 ) * * ( csU + 7 );
        csR[2][0] = * ( csV     ) * * ( csU + 2 ) + * ( csV + 1 ) * * ( csU + 5 ) + * ( csV + 2 ) * * ( csU + 8 );
        csR[2][1] = * ( csV + 3 ) * * ( csU + 2 ) + * ( csV + 4 ) * * ( csU + 5 ) + * ( csV + 5 ) * * ( csU + 8 );
        csR[2][2] = * ( csV + 6 ) * * ( csU + 2 ) + * ( csV + 7 ) * * ( csU + 5 ) + * ( csV + 8 ) * * ( csU + 8 );

        /* Verification of rotation matrix determinant */
        if ( (

            csR[0][0] * ( csR[1][1] * csR[2][2] - csR[2][1] * csR[1][2] ) -
            csR[0][1] * ( csR[1][0] * csR[2][2] - csR[2][0] * csR[1][2] ) +
            csR[0][2] * ( csR[1][0] * csR[2][1] - csR[2][0] * csR[1][1] )

        ) < 0.0 ) {

            /* Recompute rotation matrix */
            csR[0][0] = * ( csV     ) * * ( csU     ) + * ( csV + 1 ) * * ( csU + 3 ) - * ( csV + 2 ) * * ( csU + 6 );
            csR[0][1] = * ( csV + 3 ) * * ( csU     ) + * ( csV + 4 ) * * ( csU + 3 ) - * ( csV + 5 ) * * ( csU + 6 );
            csR[0][2] = * ( csV + 6 ) * * ( csU     ) + * ( csV + 7 ) * * ( csU + 3 ) - * ( csV + 8 ) * * ( csU + 6 );
            csR[1][0] = * ( csV     ) * * ( csU + 1 ) + * ( csV + 1 ) * * ( csU + 4 ) - * ( csV + 2 ) * * ( csU + 7 );
            csR[1][1] = * ( csV + 3 ) * * ( csU + 1 ) + * ( csV + 4 ) * * ( csU + 4 ) - * ( csV + 5 ) * * ( csU + 7 );
            csR[1][2] = * ( csV + 6 ) * * ( csU + 1 ) + * ( csV + 7 ) * * ( csU + 4 ) - * ( csV + 8 ) * * ( csU + 7 );
            csR[2][0] = * ( csV     ) * * ( csU + 2 ) + * ( csV + 1 ) * * ( csU + 5 ) - * ( csV + 2 ) * * ( csU + 8 );
            csR[2][1] = * ( csV + 3 ) * * ( csU + 2 ) + * ( csV + 4 ) * * ( csU + 5 ) - * ( csV + 5 ) * * ( csU + 8 );
            csR[2][2] = * ( csV + 6 ) * * ( csU + 2 ) + * ( csV + 7 ) * * ( csU + 5 ) - * ( csV + 8 ) * * ( csU + 8 );

        }

        /* Compute translation vector */
        csT[0] = - csR[0][0] * csMean[0] - csR[0][1] * csMean[1] - csR[0][2] * csMean[2] + csMean[3];
        csT[1] = - csR[1][0] * csMean[0] - csR[1][1] * csMean[1] - csR[1][2] * csMean[2] + csMean[4];
        csT[2] = - csR[2][0] * csMean[0] - csR[2][1] * csMean[1] - csR[2][2] * csMean[2] + csMean[5];

    }

