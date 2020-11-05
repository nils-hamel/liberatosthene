/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@alumni.epfl.ch
 *      Copyright (c) 2016-2020 DHLAB, EPFL
 *      Copyright (c) 2020 Republic and Canton of Geneva
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

    # include "eratosthene-uv3.h"

/*
    source - accessor methods
 */

    le_void_t le_uv3_get_record_pose( le_byte_t const * const le_record, le_real_t * const le_pose ) {

        /* copy uv3 record coordinates */
        memcpy( le_pose, le_record, sizeof( le_real_t ) * 3 );

    }

/*
    source - mutator methods
 */

    le_void_t le_uv3_set_sort_circle( le_byte_t * const le_primitive, le_size_t const le_type, le_size_t const le_scfg ) {

        /* minimum pointer */
        le_byte_t * le_circle = le_primitive;

        /* array pointer */
        le_byte_t * le_head = le_primitive + LE_ARRAY_DATA;
        le_byte_t * le_edge = le_primitive + LE_ARRAY_DATA * le_type;

        /* circular buffer */
        le_byte_t le_buffer[LE_ARRAY_DATA << 1];

        /* detect minimum */
        while ( le_head < le_edge ) {

            /* minimum detection */
            if ( le_address_get_greater( ( le_real_t * ) le_circle, ( le_real_t * ) le_head, le_scfg ) == _LE_TRUE ) {

                /* update minimum pointer */
                le_circle = le_head;

            }

            /* update head */
            le_head += LE_ARRAY_DATA;

        }

        /* check circular permutation requirement */
        if ( le_circle != le_primitive ) {

            /* push primitive vertex */
            memcpy( le_buffer, le_primitive, le_circle - le_primitive );

            /* apply circular permutation */
            memcpy( le_primitive, le_circle, le_edge - le_circle );

            /* pop primitive vertex */
            memcpy( le_primitive + ( le_edge - le_circle ), le_buffer, le_circle - le_primitive );

        }

    }

    le_byte_t * le_uv3_set_sort_primitive( le_byte_t * const le_buffer, le_size_t const le_size, le_size_t const le_scfg ) {

        /* primitive stack */
        le_size_t le_stack = 1;

        /* primitive type */
        le_size_t le_type = 0;

        /* buffer pointer */
        le_byte_t * le_push = NULL;
        le_byte_t * le_head = le_buffer;

        /* parsing buffer */
        while ( ( le_head - le_buffer ) < le_size ) {

            /* stack tracking */
            if ( ( -- le_stack ) == 0 ) {

                /* check pushed head */
                if ( ( le_push != NULL ) && ( le_type > LE_UV3_POINT ) ) {

                    /* apply circular permutation */
                    le_uv3_set_sort_circle( le_push, le_type, le_scfg );

                }

                /* reset stack */
                le_stack = ( le_type = * ( le_head + LE_ARRAY_DATA_POSE ) );

                /* push head */
                le_push = le_head;

            }

            /* update head */
            le_head += LE_ARRAY_DATA;

        }

        /* return pointer */
        return( le_buffer );

    }

    le_byte_t * le_uv3_set_sort( le_byte_t * const le_buffer, le_size_t const le_size, le_size_t le_memory, le_size_t const le_scfg ) {

        /* buffer variable */
        le_byte_t * le_dual[2] = { le_buffer, NULL };

        /* switch variable */
        le_size_t le_swip = 0;
        le_size_t le_swis = 1;

        /* size variable */
        le_size_t le_copy = 0;

        /* flag variable */
        le_enum_t le_flag = 0;

        /* stack variable */
        le_size_t le_stack = 0;

        /* algorithm variable */
        le_size_t le_range = 1;
        le_size_t le_merge = 2;
        le_size_t le_count = 0;

        /* merge variable */
        le_size_t le_index = 0;

        /* merge variable */
        le_size_t le_phead = 0;
        le_size_t le_pedge = 0;
        le_size_t le_qhead = 0;
        le_size_t le_qedge = 0;

        /* allocate buffer memory */
        if ( ( le_dual[le_swis] = ( le_byte_t * ) malloc( le_memory ) ) == NULL ) {

            /* send message */
            return( NULL );

        }

        /* sorting algorithm */
        while ( le_merge > 1 ) {

            /* reset algorithm */
            le_merge = 0;

            /* reset index */
            le_index = 0;

            /* initial primitive */
            le_stack = le_uv3_get_type( le_dual[le_swip] );

            /* merging procedure */
            while ( le_index < le_size ) {

                /* range specification */
                le_qedge = ( le_phead = le_index ) + LE_ARRAY_DATA_POSE;

                /* reset flag */
                le_flag = _LE_FALSE;

                /* reset range */
                le_count = 0;

                /* search range boundary */
                while ( le_flag == _LE_FALSE ) {

                    /* stream boundary detection */
                    if ( ( le_qedge += LE_ARRAY_DATA ) < le_size ) {

                        /* primitive stack */
                        if ( ( -- le_stack ) == 0 ) {

                            /* primitive size */
                            le_stack = le_dual[le_swip][le_qedge];

                            /* update and check range */
                            if ( ( ++ le_count ) == le_range ) {

                                /* compute range domain */
                                le_pedge = ( le_qhead = le_qedge - LE_ARRAY_DATA_POSE );

                                /* update flag */
                                le_flag = _LE_TRUE;

                            }

                        }

                    } else {

                        /* compute range domain */
                        le_qedge = ( le_pedge = ( le_qhead = le_size ) );

                        /* update flag */
                        le_flag = _LE_TRUE;

                    }

                }

                /* check range boundary */
                if ( le_qedge < le_size ) {

                    /* reset flag */
                    le_flag = _LE_FALSE;

                    /* reset range */
                    le_count = 0;

                    /* search range boundary */
                    while ( le_flag == _LE_FALSE ) {

                        /* stream boundary detection */
                        if ( ( le_qedge += LE_ARRAY_DATA ) < le_size ) {

                            /* primitive stack */
                            if ( ( -- le_stack ) == 0 ) {

                                /* primitive size */
                                le_stack = le_dual[le_swip][le_qedge];

                                /* update and check range */
                                if ( ( ++ le_count ) == le_range ) {

                                    /* compute range domain */
                                    le_qedge -= LE_ARRAY_DATA_POSE;

                                    /* update flag */
                                    le_flag = _LE_TRUE;

                                }

                            }

                        } else {

                            /* compute range domain */
                            le_qedge = le_size;

                            /* update flag */
                            le_flag = _LE_TRUE;

                        }

                    }

                }

                /* merge range */
                while ( le_index < le_qedge ) {

                    /* check exhaust condition */
                    if ( le_phead == le_pedge ) {

                        /* copy remaining range */
                        memcpy( le_dual[le_swis] + le_index, le_dual[le_swip] + le_qhead, le_qedge - le_qhead );

                        /* update merge index */
                        le_index = le_qedge;

                    } else if ( le_qhead == le_qedge ) {

                        /* copy remaining range */
                        memcpy( le_dual[le_swis] + le_index, le_dual[le_swip] + le_phead, le_pedge - le_phead );

                        /* update merge index */
                        le_index = le_qedge;

                    } else {

                        /* sorting condition */
                        if ( le_address_get_greater( ( le_real_t * ) ( le_dual[le_swip] + le_phead ), ( le_real_t * ) ( le_dual[le_swip] + le_qhead ), le_scfg ) == _LE_TRUE ) {

                            /* compute primitive size */
                            le_copy = le_uv3_get_type( le_dual[le_swip] + le_qhead ) * LE_ARRAY_DATA;

                            /* copy primitive */
                            memcpy( le_dual[le_swis] + le_index, le_dual[le_swip] + le_qhead, le_copy );

                            /* update range domain */
                            le_qhead += le_copy;

                        } else {

                            /* compute primitive size */
                            le_copy = le_uv3_get_type( le_dual[le_swip] + le_phead ) * LE_ARRAY_DATA;

                            /* copy primitive */
                            memcpy( le_dual[le_swis] + le_index, le_dual[le_swip] + le_phead, le_copy );

                            /* update range domain */
                            le_phead += le_copy;

                        }

                        /* update merge index */
                        le_index += le_copy;

                    }

                }

                /* update algorithm */
                le_merge ++;

            }

            /* update algorithm */
            le_range *= 2;

            /* update switch */
            le_swip = 1 - le_swip;
            le_swis = 1 - le_swis;

        }

        /* release buffer memory */
        free( le_dual[le_swis] );

        /* return buffer pointer */
        return( le_dual[le_swip] );

    }

    le_enum_t le_uv3_set_merge( le_char_t const * const le_pfile, le_char_t const * const le_qfile, le_char_t const * const le_efile, le_size_t const le_scfg ) {

        /* buffer variable */
        le_byte_t * le_pbase = NULL;
        le_byte_t * le_qbase = NULL;

        /* switch variable */
        le_size_t le_switch = 0;

        /* stack variable */
        le_size_t le_stack = 1;

        /* head variable */
        le_size_t le_phead = 1;
        le_size_t le_qhead = 1;

        /* size variable */
        le_size_t le_pread = 1;
        le_size_t le_qread = 1;

        /* stream variable */
        le_file_t le_pstream = NULL;
        le_file_t le_qstream = NULL;
        le_file_t le_estream = NULL;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* allocate buffer memory */
        if ( ( le_pbase = ( le_char_t * ) malloc( ( LE_UV3_CHUNK * LE_ARRAY_DATA ) << 1 ) ) == NULL ) {

            /* push message */
            le_message = LE_ERROR_MEMORY;

        } else {

            /* compute buffer pointer */
            le_qbase = le_pbase + LE_UV3_CHUNK * LE_ARRAY_DATA;

            /* create stream */
            if ( ( le_pstream = fopen( ( char * ) le_pfile, "rb" ) ) == NULL ) {

                /* push message */
                le_message = LE_ERROR_IO_READ;

            } else {

                /* create stream */
                if ( ( le_qstream = fopen( ( char * ) le_qfile, "rb" ) ) == NULL ) {

                    /* push message */
                    le_message = LE_ERROR_IO_READ;

                } else {

                    /* create stream */
                    if ( ( le_estream = fopen( ( char * ) le_efile, "wb" ) ) == NULL ) {

                        /* push message */
                        le_message = LE_ERROR_IO_WRITE;

                    } else {

                        /* merge process */
                        while ( ( ( le_pread | le_qread ) != 0 ) && ( le_message == LE_ERROR_SUCCESS ) ) {

                            /* exhaust condition */
                            if ( le_phead == le_pread ) {

                                /* import stream chunk */
                                le_pread = fread( le_pbase, sizeof( le_byte_t ), LE_UV3_CHUNK * LE_ARRAY_DATA, le_pstream );

                                /* reset head */
                                le_phead = 0;

                            }

                            /* exhause condition */
                            if ( le_qhead == le_qread ) {

                                /* import stream chunk */
                                le_qread = fread( le_qbase, sizeof( le_byte_t ), LE_UV3_CHUNK * LE_ARRAY_DATA, le_qstream );

                                /* reset head */
                                le_qhead = 0;

                            }

                            /* exhause condition */
                            if ( ( le_pread | le_qread ) != 0 ) {

                                /* primitive stack */
                                if ( ( -- le_stack ) == 0 ) {

                                    /* exhaust condition */
                                    if ( le_pread == 0 ) {

                                        /* update stack */
                                        le_stack = le_uv3_get_type( le_qbase + le_qhead );

                                        /* update switch */
                                        le_switch = 0;

                                        /* export remaining range */
                                        if ( fwrite( le_qbase + le_qhead, sizeof( le_byte_t ), LE_ARRAY_DATA, le_estream ) != LE_ARRAY_DATA ) {

                                            /* push message */
                                            le_message = LE_ERROR_IO_WRITE;

                                        /* update head */
                                        } else { le_qhead += LE_ARRAY_DATA; }

                                    } else if ( le_qread == 0 ) {

                                        /* update stack */
                                        le_stack = le_uv3_get_type( le_pbase + le_phead );

                                        /* update switch */
                                        le_switch = 1;

                                        /* export remaining range */
                                        if ( fwrite( le_pbase + le_phead, sizeof( le_byte_t ), LE_ARRAY_DATA, le_estream ) != LE_ARRAY_DATA ) {

                                            /* push message */
                                            le_message = LE_ERROR_IO_WRITE;

                                        /* update head */
                                        } else { le_phead += LE_ARRAY_DATA; }

                                    } else {

                                        /* merging condition */
                                        if ( le_address_get_greater( ( le_real_t * ) ( le_pbase + le_phead ), ( le_real_t * ) ( le_qbase + le_qhead ), le_scfg ) == _LE_TRUE ) {

                                            /* update stack */
                                            le_stack = le_uv3_get_type( le_qbase + le_qhead );

                                            /* update switch */
                                            le_switch = 0;

                                            /* export record */
                                            if ( fwrite( le_qbase + le_qhead, sizeof( le_byte_t ), LE_ARRAY_DATA, le_estream ) != LE_ARRAY_DATA ) {

                                                /* push message */
                                                le_message = LE_ERROR_IO_WRITE;

                                            /* update head */
                                            } else { le_qhead += LE_ARRAY_DATA; }

                                        } else {

                                            /* update stack */
                                            le_stack = le_uv3_get_type( le_pbase + le_phead );

                                            /* update switch */
                                            le_switch = 1;

                                            /* export record */
                                            if ( fwrite( le_pbase + le_phead, sizeof( le_byte_t ), LE_ARRAY_DATA, le_estream ) != LE_ARRAY_DATA ) {

                                                /* push message */
                                                le_message = LE_ERROR_IO_WRITE;

                                            /* update head */
                                            } else { le_phead += LE_ARRAY_DATA; }

                                        }

                                    }

                                } else {

                                    /* check switch value */
                                    if ( le_switch == 0 ) {

                                        /* export record */
                                        if ( fwrite( le_qbase + le_qhead, sizeof( le_byte_t ), LE_ARRAY_DATA, le_estream ) != LE_ARRAY_DATA ) {

                                            /* push message */
                                            le_message = LE_ERROR_IO_WRITE;

                                        /* update head */
                                        } else { le_qhead += LE_ARRAY_DATA; }

                                    } else {

                                        /* export record */
                                        if ( fwrite( le_pbase + le_phead, sizeof( le_byte_t ), LE_ARRAY_DATA, le_estream ) != LE_ARRAY_DATA ) {

                                            /* push message */
                                            le_message = LE_ERROR_IO_WRITE;

                                        /* update head */
                                        } else { le_phead += LE_ARRAY_DATA; }

                                    }

                                }

                            }

                        }

                        /* delete stream */
                        fclose( le_estream );

                    }

                    /* delete stream */
                    fclose( le_qstream );

                }

                /* delete stream */
                fclose( le_pstream );

            }

            /* release buffer memory */
            free( le_pbase );

        }

        /* send message */
        return( le_message );

    }

    le_void_t le_uv3_io_print( le_byte_t * le_uv3, le_size_t const le_size ) {

        for ( le_size_t le_parse = 0; le_parse < le_size; le_parse += LE_ARRAY_DATA ) {

            fprintf( stdout, "%+e %+e %+e %02x %02x %02x %02x\n", 

                ( ( le_real_t * ) ( le_uv3 + le_parse ) )[ 0],
                ( ( le_real_t * ) ( le_uv3 + le_parse ) )[ 1],
                ( ( le_real_t * ) ( le_uv3 + le_parse ) )[ 2],
                ( ( le_byte_t * ) ( le_uv3 + le_parse ) )[24],
                ( ( le_byte_t * ) ( le_uv3 + le_parse ) )[25],
                ( ( le_byte_t * ) ( le_uv3 + le_parse ) )[26],
                ( ( le_byte_t * ) ( le_uv3 + le_parse ) )[27]

            );

        }

    }

