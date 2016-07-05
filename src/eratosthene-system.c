/*
 *  liberatosthene - eratosthene indexation server
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016 EPFL CDH DHLAB
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

    # include "eratosthene-system.h"

/*
    source - constructor/destructor methods
 */

    le_system_t le_system_create( le_char_t const * const le_root ) {

        /* Returned structure variables */
        le_system_t le_system = LE_SYSTEM_C;

        /* Stream variables */
        FILE * le_stream = NULL;

        /* Open configuration stream */
        if ( ( le_stream = fopen( strcat( strcat( ( char * ) le_system.sm_root, ( char * ) le_root ), "/system" ), "r" ) ) == NULL ) {

            /* Send message - return created structure */
            le_system._status = LE_ERROR_IO_ACCESS; return( le_system );

        }

        /* Check configuration reading */
        if ( fscanf( le_stream, "%" _LE_SIZE_S " %" _LE_TIME_S, & le_system.sm_sparam, & le_system.sm_tparam ) != 2 ) {

            /* Close configuration stream */
            fclose( le_stream );

            /* Send message - return created structure */
            le_system._status = LE_ERROR_IO_READ; return( le_system );

        }

        /* Close configuration stream */
        fclose( le_stream );

        /* Check consistency */
        if ( ( le_system.sm_sparam <= 0 ) || ( le_system.sm_sparam >= _LE_USE_DEPTH ) ) {

            /* Send message - return created structure */
            le_system._status = LE_ERROR_DEPTH; return( le_system );

        }

        /* Check consistency */
        if ( le_system.sm_tparam <= 0 ) {

            /* Send message - return created structure */
            le_system._status = LE_ERROR_TIME; return( le_system );

        }

        /* Assign provided root path */
        strcpy( ( char * ) le_system.sm_root, ( char * ) le_root );

        /* Return created structure */
        return( le_system );

    }

    le_system_t le_system_delete( le_system_t * const le_system ) {

        /* Returned structure variables */
        le_system_t le_return = LE_SYSTEM_C;

        /* Delete streams stack */
        le_system_io_close( le_system );

        /* Return deleted structure */
        return( le_return );

    }

/*
    source - accessor methods
 */

    le_size_t le_system_get_sparam( le_system_t const * const le_system ) {

        /* Return system space discretisation */
        return( le_system->sm_sparam );

    }

    le_time_t le_system_get_tparam( le_system_t const * const le_system ) {

        /* Return system time discretisation */
        return( le_system->sm_tparam );

    }

/*
    source - mutator methods
 */

    le_hand_t le_system_set_format( le_system_t * const le_system, le_hand_t const le_hand ) {

        /* Handshake decomposition variables */
        le_hand_t le_mode = le_hand & 0xff;

        /* Assign transmission mode */
        le_system->sm_format = le_hand >> 8;

        /* Check consistency */
        if ( le_mode == LE_NETWORK_MODE_IMOD ) {

            /* Check consistency */
            if ( le_system->sm_format == LE_ARRAY_64S ) {

                /* Send authorisation */
                return( LE_NETWORK_MODE_IATH );

            }

        } else if ( le_mode == LE_NETWORK_MODE_QMOD ) {

            /* Check consistency */
            if ( ( le_system->sm_format == LE_ARRAY_64S ) || ( le_system->sm_format == LE_ARRAY_64R ) ) {

                /* Send authorisation */
                return( LE_NETWORK_MODE_QATH );

            }

        } else if ( le_mode == LE_NETWORK_MODE_AMOD ) {

            /* Check consistency */
            if ( le_system->sm_format == LE_ARRAY_64T ) {

                /* Send authorisation */
                return( LE_NETWORK_MODE_AATH );

            }

        } else if ( le_mode == LE_NETWORK_MODE_SMOD ) {

            /* Send authorisation */
            return( LE_NETWORK_MODE_SATH );

        } else if ( le_mode == LE_NETWORK_MODE_TMOD ) {

            /* Send authorisation */
            return( LE_NETWORK_MODE_TATH );

        }

        /* Authorisation failure */
        return( _LE_HAND_NULL );

    }

/*
    source - injection methods
 */

    le_void_t le_system_inject( le_system_t * const le_system, le_real_t * const le_pose, le_time_t const le_time, le_data_t const * const le_data ) {

        /* Address variables */
        le_address_t le_addr = LE_ADDRESS_C_SIZE( le_system->sm_sparam - 1 );

        /* Class variables */
        le_class_t le_class = LE_CLASS_C;

        /* Depth variables */
        le_size_t le_parse = 0;
        le_size_t le_panex = 1;

        /* Offset variables */
        le_size_t le_offset = 0;
        le_size_t le_offnex = 0;

        /* System scale stream management - send message */
        if ( le_system_io_open( le_system, le_time ) != LE_ERROR_SUCCESS ) return;

        /* Compute address index */
        le_address_set_pose( & le_addr, le_pose );

        /* Injection process */
        do {

            /* Class importation */
            if ( le_class_io_read( & le_class, le_offnex, le_system->sm_scale[le_parse] ) == LE_ERROR_SUCCESS ) {

                /* Inject element in class */
                le_class_set_push( & le_class, le_data );

            } else {

                /* Initialise class with element */
                le_class = le_class_create( le_data );

            }

            /* Retrieve daughter offset */
            le_offset = le_class_get_offset( & le_class, le_address_get_digit( & le_addr, le_parse ) );

            /* Check daughter state */
            if ( ( le_offset == _LE_OFFS_NULL ) && ( ( le_panex ) != le_system->sm_sparam ) ) {

                /* Seek next scale eof */
                fseek( le_system->sm_scale[le_panex], 0, SEEK_END );

                /* Assign eof offset */
                le_offset = ftell( le_system->sm_scale[le_panex] );

                /* Insert offset in class */
                le_class_set_offset( & le_class, le_address_get_digit( & le_addr, le_parse ), le_offset );

            }

            /* Class exportation */
            le_class_io_write( & le_class, le_offnex, le_system->sm_scale[le_parse] );

        /* Injection process condition */
        } while ( ( le_offnex = le_offset, ++ le_panex, ++ le_parse ) < le_system->sm_sparam );

    }

/*
    source - query methods
 */

    le_void_t le_system_query2( le_system_t * const le_system, le_address_t * const le_addr, le_array_t * const le_array, le_size_t const le_parse, le_size_t le_offset ) {

        /* Class variables */
        le_class_t le_class = LE_CLASS_C;

        /* Position array variables */
        le_real_t le_pose[3] = { 0.0 };

        /* Query initialisation */
        if ( le_parse == 0 ) {

            /* Check consistency */
            if ( ( le_address_get_size( le_addr ) + le_address_get_depth( le_addr ) ) >= le_system->sm_sparam ) return;

            /* System scale stream management */
            if ( le_system_io_open( le_system, le_address_get_time( le_addr ) ) != LE_ERROR_SUCCESS ) return;

        }

        /* Read class */
        if ( le_class_io_read( & le_class, le_offset, le_system->sm_scale[le_parse] ) == LE_ERROR_SUCCESS ) {

            /* Query process mode */
            if ( le_parse < le_address_get_size( le_addr ) ) {

                /* Retreive daughter class offset */
                if ( ( le_offset = le_class_get_offset( & le_class, le_address_get_digit( le_addr, le_parse ) ) ) != _LE_OFFS_NULL ) {

                    /* Recursive query */
                    le_system_query2( le_system, le_addr, le_array, le_parse + 1, le_offset );

                }

            } else {

                /* Check query depth */
                if ( le_parse == le_address_get_size( le_addr ) + le_address_get_depth( le_addr ) ) {

                    /* Retreive class representative */
                    le_address_get_pose( le_addr, le_pose );

                    /* Inject gathered element in array */
                    le_array_set_push( le_array, le_system->sm_format, le_pose, le_address_get_time( le_addr ), le_class_get_data( & le_class ) );


                } else {

                    /* Class daughters enumeration */
                    for ( le_size_t le_digit = 0; le_digit < _LE_USE_BASE; le_digit ++ ) {

                        /* Update address index size */
                        le_address_set_size( le_addr, le_address_get_size( le_addr ) + 1 );

                        /* Update address index digit */
                        le_address_set_digit( le_addr, le_address_get_size( le_addr ) - 1, le_digit );

                        /* Retreive daughter class offset */
                        le_offset = le_class_get_offset( & le_class, le_digit );

                        /* Recursive query */
                        le_system_query2( le_system, le_addr, le_array, le_parse + 1, le_offset );

                    }

                }

            }

        }

    }

    le_array_t le_system_query( le_system_t * const le_system, le_address_t * const le_addr ) {

        /* Query depth variables */
        le_size_t le_depth = 0;

        /* Offset tracker variables */
        le_size_t le_offset = 0;

        /* Class tracker variables */
        le_class_t le_class = LE_CLASS_C;

        /* Returned array variables */
        le_array_t le_return = LE_ARRAY_C;

        /* Check consistency - send array */
        if ( ( le_address_get_size( le_addr ) + le_address_get_depth( le_addr ) ) >= le_system->sm_sparam ) return( le_return );

        /* System scale stream management - send array */
        if ( le_system_io_open( le_system, le_address_get_time( le_addr ) ) != LE_ERROR_SUCCESS ) return( le_return );

        /* Query class search */
        do {

            /* Class importation */
            if ( le_class_io_read( & le_class, le_offset, le_system->sm_scale[le_depth] ) != LE_ERROR_SUCCESS ) {

                /* Return element array */
                return( le_return );

            } else {

                /* Extract daughter offset */
                le_offset = le_class_get_offset( & le_class, le_address_get_digit( le_addr, le_depth ) );

            }

        /* Query class search condition */
        } while ( ( ( ++ le_depth ) <= le_address_get_size( le_addr ) ) && ( le_offset != _LE_OFFS_NULL ) );

        /* Check query class search */
        if ( ( -- le_depth ) == le_address_get_size( le_addr ) ) {

            /* Gathering process */
            le_system_query_gather( le_system, & le_return, le_addr, & le_class, le_depth, le_depth + le_address_get_depth( le_addr ) );

        }

        /* Return element array */
        return( le_return );
           
    }

    le_void_t le_system_query_gather( le_system_t * const le_system, le_array_t * const le_array, le_address_t * const le_addr, le_class_t * const le_class, le_size_t const le_head, le_size_t const le_target ) {

        /* Parsing variables */
        le_size_t le_parse = 0;

        /* Offset variables */
        le_size_t le_offset = 0;

        /* Spatial coordinates variables */
        le_real_t le_pose[3] = { 0.0 };

        /* Class variables */
        le_class_t le_clnex = LE_CLASS_C;

        /* Check head and target */
        if ( le_head < le_target ) {

            /* Parsing class daughter */
            for ( ; le_parse < 8; le_parse ++ ) {

                /* Check daughter */
                if ( ( le_offset = le_class_get_offset( le_class, le_parse ) ) != _LE_OFFS_NULL ) {

                    /* Set address size */  
                    le_address_set_size( le_addr, le_head + 1 );

                    /* Set address digit */
                    le_address_set_digit( le_addr, le_head, le_parse );

                    /* Read daughter */
                    le_class_io_read( & le_clnex, le_offset, le_system->sm_scale[le_head+1] );

                    /* Recursive gathering process */
                    le_system_query_gather( le_system, le_array, le_addr, & le_clnex, le_head + 1, le_target );

                }

            }

        } else {

            /* Retreive class representative */
            le_address_get_pose( le_addr, le_pose );

            /* Inject gathered element in array */
            le_array_set_push( le_array, le_system->sm_format, le_pose, le_address_get_time( le_addr ), le_class_get_data( le_class ) );

        }

    }

/*
    source - temporal methods
 */

    le_array_t le_system_times( le_system_t const * const le_system ) {

        /* Time buffer variables */
        le_time_t le_buffer = _LE_TIME_NULL;

        /* Returned structure variables */
        le_array_t le_array = LE_ARRAY_C;

        /* Directory variables */
        DIR * le_directory = NULL;

        /* Directory entity variables */
        struct dirent * le_entity = NULL;

        /* Create directory handle - send message */
        if ( ( le_directory = opendir( ( char * ) le_system->sm_root ) ) == NULL ) return( le_array );

        /* Directory entity enumeration */
        while ( ( le_entity = readdir( le_directory ) ) != NULL ) {

            /* Check entity type */
            if ( le_entity->d_type == DT_DIR ) {

                /* Convert directory name */
                le_buffer = le_system->sm_tparam * strtoull( le_entity->d_name, NULL, 10 );

                /* Push buffer in array */
                le_array_set_push( & le_array, LE_ARRAY_64T, NULL, le_buffer, NULL );

            }

        }

        /* Delete directory handle */
        closedir( le_directory );

        /* Return structure */
        return( le_array );

    }

/*
    source - i/o methods
 */

    le_enum_t le_system_io_open( le_system_t * const le_system, le_time_t const le_time ) {

        /* Temporal class variables */
        static le_time_t le_flag = _LE_TIME_NULL;

        /* Path variables */
        le_char_t le_path[_LE_USE_STRING] = { 0 };

        /* Check necessities - send message */
        if ( ( le_time / le_system->sm_tparam ) == le_flag ) return( LE_ERROR_SUCCESS );

        /* Update temporal class */
        le_flag = le_time / le_system->sm_tparam;

        /* Compose temporal class directoy */
        sprintf( ( char * ) le_path, "%s/%" _LE_TIME_P, le_system->sm_root, le_flag );

        /* Create directory */
        mkdir( ( char * ) le_path, 0777 );

        /* Create scale streams */
        for ( le_size_t le_parse = 0; le_parse < le_system->sm_sparam; le_parse ++ ) {

            /* Check stream state */
            if ( le_system->sm_scale[le_parse] != NULL ) fclose( le_system->sm_scale[le_parse] );

            /* Create scale path */
            sprintf( ( char * ) le_path, "%s/%" _LE_TIME_P "/scale-%03" _LE_SIZE_P ".bin", le_system->sm_root, le_flag, le_parse );

            /* Create scale stream (r+) */
            if ( ( le_system->sm_scale[le_parse] = fopen( ( char * ) le_path, "r+" ) ) == NULL ) {

                /* Create scale stream (w+) - send message */
                if ( ( le_system->sm_scale[le_parse] = fopen( ( char * ) le_path, "w+" ) ) == NULL ) return( LE_ERROR_IO_ACCESS );

            }

        }

        /* Send message */
        return( LE_ERROR_SUCCESS );

    }

    le_void_t le_system_io_flush( le_system_t * const le_system ) {
        
        /* Parsing streams */
        for ( le_size_t le_parse = 0; le_parse < le_system->sm_sparam; le_parse ++ ) {

            /* Check stream state - flush stream */
            if ( le_system->sm_scale[le_parse] != NULL ) fflush( le_system->sm_scale[le_parse] );

        }

    }

    le_void_t le_system_io_close( le_system_t * const le_system ) {

        /* Parsing scales streams */
        for( le_size_t le_parse = 0; le_parse < le_system->sm_sparam; le_parse ++ ) {

            /* Check stream */
            if ( le_system->sm_scale[le_parse] != NULL ) {

                /* Close scale stream */
                fclose( le_system->sm_scale[le_parse] );

                /* Invalidate pointer */
                le_system->sm_scale[le_parse] = NULL;

            }

        }

    }
    
