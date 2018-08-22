/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016-2018 DHLAB, EPFL
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

    # include "eratosthene-switch.h"

/*
    source - constructor/destructor methods
 */

    le_switch_t le_switch_create( le_char_t * const le_path, le_size_t const le_scfg, le_time_t const le_tcfg ) {

        /* created structure variable */
        le_switch_t le_switch = LE_SWITCH_C_PST( le_path, le_scfg, le_tcfg );

        /* return created structure */
        return( le_switch );

    }

    le_void_t le_switch_delete( le_switch_t * const le_switch ) {

        /* deleted structure variable */
        le_switch_t le_delete = LE_SWITCH_C;

        /* delete switch chain */
        le_switch_set_erase( le_switch );

        /* delete structure */
        ( * le_switch ) = le_delete;

    }

/*
    source - accessor methods
 */

    le_door_t * le_switch_get_inject( le_switch_t * const le_switch, le_time_t const le_time, le_enum_t const le_mode ) {

        /* parsing variable */
        le_door_t * le_parse = le_switch->sw_door;

        /* parsing chain */
        while ( le_parse != NULL ) {

            /* detect door */
            if ( le_door_get_equal( le_parse, le_time ) == _LE_TRUE ) {

                /* return door */
                return( le_parse );

            /* update parser */
            } else { le_parse = le_door_get_next( le_parse ); }

        }

        /* check mode */
        if ( le_mode == LE_DOOR_WRITE ) {

            /* create and push door */
            return( le_switch_set_push( le_switch, le_time, LE_DOOR_WRITE ) );

        } else {

            /* send message */
            return( NULL );

        }

    }

    le_door_t * le_switch_get_query( le_switch_t * const le_switch, le_time_t const le_time, le_address_t const * const le_addr ) {

        /* reduced time variable */
        le_time_t le_reduced = le_time / le_switch->sw_tcfg;

        /* parsing variable */
        le_door_t * le_prev = NULL;
        le_door_t * le_next = NULL;

        /* parsing variable */
        le_door_t * le_parse = le_switch->sw_door;

        /* applicant variable */
        le_time_t le_applicant = 0;

        /* interval variable */
        le_time_t le_interval = _LE_TIME_MAX;

        /* parsing chain */
        while ( le_prev == NULL ) {

            /* compute interval */
            if ( ( le_applicant = le_door_get_interval( le_parse, le_time ) ) < 0 ) {

                /* compare applicants */
                if ( le_time_abs( le_applicant ) < le_interval ) {

                    /* assign search entry point */
                    le_prev = le_next = le_parse;

                } else {

                    /* assign search entry point */
                    le_prev = le_next = le_parse = le_door_get_prev( le_parse );

                }
                
            } else {

                /* chain termination detection */
                if ( ( le_next = le_door_get_next( le_parse ) ) == NULL ) {

                    /* assign search entry point */
                    le_next = le_prev = le_parse;

                } else {

                    /* update interval */
                    le_interval = le_applicant;

                    /* update parser */
                    le_parse = le_next;

                }

            }

        }

        /* searching algorithm */
        while ( le_parse != NULL ) {

            /* empty-cell detection */
            if ( ( le_door_io_mono_detect( le_parse, le_addr ) | le_door_io_poly_detect( le_parse, le_addr ) ) == _LE_TRUE ) {

                /* return door */
                return( le_parse );

            }

            /* compare doors */
            if ( le_door_get_nearest( le_prev, le_next, le_reduced ) == LE_DOOR_PREV ) {

                /* update parser */
                le_parse = le_prev = le_door_get_prev( le_prev );

            } else {

                /* update parser */
                le_parse = le_next = le_door_get_next( le_next );

            }

        }

        /* send message */
        return( NULL );

    }

/*
    source - mutator methods
 */

    le_enum_t le_switch_set_update( le_switch_t * const le_switch, le_time_t const le_lifespan ) {

        /* check lifespan condition */
        if ( ( time( NULL ) - le_switch->sw_load ) < le_lifespan ) {

            /* send message */
            return( LE_ERROR_SUCCESS );

        }

        /* erase switch */
        le_switch_set_erase( le_switch );

        /* build switch and send message */
        return( le_switch_set_build( le_switch ) );

    }

    le_enum_t le_switch_set_build( le_switch_t * const le_switch ) {

        /* directory entity variable */
        struct dirent * le_entity = NULL;

        /* directory variable */
        DIR * le_storage = NULL;

        /* time variable */
        le_time_t le_time = _LE_TIME_NULL;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* open directory */
        if ( ( le_storage = opendir( ( char * ) le_switch->sw_path ) ) == NULL ) {

            /* send message */
            return( LE_ERROR_IO_READ );

        }

        /* entitiy enumeration */
        while ( ( ( le_entity = readdir( le_storage ) ) != NULL ) && ( le_message == LE_ERROR_SUCCESS ) ) {

            /* directory filtering */
            if ( ( le_entity->d_type == DT_DIR ) && ( ( * le_entity->d_name ) != '.' ) ) {

                /* compute time form name */
                le_time = le_time_str( le_entity->d_name ) * le_switch->sw_tcfg;

                /* create and push door */
                if ( le_switch_set_push( le_switch, le_time, LE_DOOR_READ ) == NULL ) {

                    /* push message */
                    le_message = LE_ERROR_IO_READ;

                }

            }

        }

        /* close directory */
        closedir( le_storage );

        /* update switch load time */
        le_switch->sw_load = time( NULL );

        /* send message */
        return( le_message );

    }

    le_door_t * le_switch_set_push( le_switch_t * const le_switch, le_time_t const le_time, le_enum_t const le_mode ) {

        /* door pointer variable */
        le_door_t * le_door = NULL;

        /* parsing pointer variable */
        le_door_t * le_parse = NULL;

        /* allocate memory */
        if ( ( le_door = ( le_door_t * ) malloc( sizeof( le_door_t ) ) ) == NULL ) {

            /* send message */
            return( NULL );

        }

        /* create door */
        ( * le_door ) = le_door_create( le_switch->sw_path, le_switch->sw_scfg, le_switch->sw_tcfg, le_time, le_mode );

        /* check door */
        if ( le_get_status( ( * le_door ) ) != LE_ERROR_SUCCESS ) {

            /* release memory */
            free( le_door );

            /* send message */
            return( NULL );

        }

        /* check chain link */
        if ( le_switch->sw_door == NULL ) {

            /* update chain link */
            le_switch->sw_door = le_door;

        } else {

            /* chain link */
            le_parse = le_switch->sw_door;

            /* parsing chain */
            while ( le_parse != NULL ) {

                /* check insertion condition */
                if ( le_door_get_anterior( le_parse, le_door ) == _LE_TRUE ) {

                    /* insert door */
                    le_door_set_insert( le_door, le_door_get_prev( le_parse ), le_parse );

                    /* check chain link */
                    if ( le_door_get_prev( le_door ) == NULL ) {

                        /* update chain link */
                        le_switch->sw_door = le_door;

                    }

                    /* interrput chain */
                    le_parse = NULL;

                } else {

                    /* chain interrupt detection */
                    if ( le_door_get_next( le_parse ) == NULL ) {

                        /* insert door */
                        le_door_set_insert( le_door, le_parse, NULL );

                        /* interrupt chain */
                        le_parse = NULL;

                    } else {

                        /* update parser */
                        le_parse = le_door_get_next( le_parse );

                    }

                }

            }

        }

        /* send message */
        return( le_door );

    }

    le_void_t le_switch_set_erase( le_switch_t * const le_switch ) {

        /* parsing variable */
        le_door_t * le_parse = NULL;

        /* removal variable */
        le_door_t * le_remove = NULL;

        /* check chain link */
        if ( le_switch->sw_door == NULL ) {

            /* already cleared */
            return;

        }

        /* chain link */
        le_parse = le_switch->sw_door;

        /* parsing chain */
        while ( le_parse != NULL ) {

            /* update removal pointer */
            le_remove = le_parse;

            /* update parser */
            le_parse = le_door_get_next( le_parse );

            /* delete door */
            le_door_delete( le_remove );

            /* release memory */
            free( le_remove );

        }

        /* update chain link */
        le_switch->sw_door = NULL;

    }

/*
    source - i/o methods
 */

    le_enum_t le_switch_io_auth( le_switch_t * const le_switch, le_array_t * const le_array, le_sock_t const le_socket ) {

        /* check consistency */
        if ( le_array_get_size( le_array ) != 0 ) {

            /* send message */
            return( LE_ERROR_IO_ARRAY );

        }

        /* update socket-array size */
        le_array_set_size( le_array, LE_ARRAY_AUTH );

        /* serialise configuration */
        le_array_serial( le_array, & le_switch->sw_scfg, sizeof( le_size_t ), 0, _LE_SET );

        /* serialise configuration */
        le_array_serial( le_array, & le_switch->sw_tcfg, sizeof( le_time_t ), sizeof( le_size_t ), _LE_SET );

        /* write socket-array */
        if ( le_array_io_write( le_array, LE_MODE_AUTH, le_socket ) != LE_MODE_AUTH ) {
        
            /* send message */
            return( LE_ERROR_IO_WRITE );

        }

        /* send message */
        return( LE_ERROR_SUCCESS );
    }

    le_enum_t le_switch_io_inject( le_switch_t * const le_switch, le_array_t * const le_array, le_sock_t const le_socket ) {

        /* time variable */
        le_time_t le_time = _LE_TIME_NULL;

        /* door variable */
        le_door_t * le_door = NULL;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* check consistency */
        if ( le_array_get_size( le_array ) != LE_ARRAY_INJE ) {

            /* send message */
            return( LE_ERROR_IO_ARRAY );

        }

        /* serialise time */
        le_array_serial( le_array, & le_time, sizeof( le_time_t ), 0, _LE_GET );

        /* check consistency */
        if ( le_time == _LE_TIME_NULL ) {

            /* send message */
            return( LE_ERROR_TIME );

        }

        /* retreive and check door */
        if ( ( le_door = le_switch_get_inject( le_switch, le_time, LE_DOOR_WRITE ) ) == NULL ) {

            /* send message */
            return( LE_ERROR_IO_ACCESS );

        }

        /* lock door */
        if ( le_door_set_state( le_door, LE_DOOR_LOCK ) == _LE_FALSE ) {

            /* send message */
            return( LE_ERROR_IO_ACCESS );

        }

        /* socket-array reading */
        while ( ( le_array_io_read( le_array, le_socket ) == LE_MODE_INJE ) && ( le_message == LE_ERROR_SUCCESS ) ) {

            /* socket-array filtering */
            le_message = le_door_io_each_inject_filter( le_door, le_array );

        }

        /* check message */
        if ( le_message == LE_ERROR_SUCCESS ) {

            /* mono-vertex injection */
            if ( ( le_message = le_door_io_mono_inject( le_door ) ) == LE_ERROR_SUCCESS ) {

                /* multi-vertex injection */
                if ( ( le_message = le_door_io_poly_inject( le_door ) ) == LE_ERROR_SUCCESS ) {

                    /* remove containers */
                    le_message = le_door_io_each_inject_clean( le_door );

                }

            }

        }

        /* unlock door */
        if ( le_door_set_state( le_door, LE_DOOR_UNLOCK ) == _LE_FALSE ) {

            /* send message */
            return( LE_ERROR_IO_ACCESS );

        }

        /* send message */
        return( le_message );

    }

    le_enum_t le_switch_io_optimise( le_switch_t * const le_switch, le_array_t * const le_array, le_sock_t const le_socket ) {

        /* time variable */
        le_time_t le_time = _LE_TIME_NULL;

        /* door variable */
        le_door_t * le_door = NULL;

        /* check consistency */
        if ( le_array_get_size( le_array ) != LE_ARRAY_OPTM ) {

            /* send message */
            return( LE_ERROR_IO_ARRAY );

        }

        /* serialise time */
        le_array_serial( le_array, & le_time, sizeof( le_time_t ), 0, _LE_GET );

        /* check consistency */
        if ( le_time == _LE_TIME_NULL ) {

            /* send message */
            return( LE_ERROR_TIME );

        }

        /* retreive and check door */
        if ( ( le_door = le_switch_get_inject( le_switch, le_time, LE_DOOR_READ ) ) == NULL ) {

            /* send message */
            return( LE_ERROR_IO_ACCESS );

        }

        /* lock door */
        if ( le_door_set_state( le_door, LE_DOOR_LOCK ) == _LE_FALSE ) {

            /* send message */
            return( LE_ERROR_IO_ACCESS );

        }

        /* optimise unit storage */
        le_door_io_mono_optimise( le_door );

        /* unlock door */
        if ( le_door_set_state( le_door, LE_DOOR_UNLOCK ) == _LE_FALSE ) {

            /* send message */
            return( LE_ERROR_IO_ACCESS );

        }

        /* send message */
        return( _LE_TRUE );

    }

    le_enum_t le_switch_io_query( le_switch_t * const le_switch, le_array_t * const le_array, le_sock_t const le_socket ) {

        /* address variable */
        le_address_t le_addr = LE_ADDRESS_C;

        /* socket-array size variable */
        le_size_t le_length = le_array_get_size( le_array );

        /* parsing variable */
        le_size_t le_parse = 0;

        /* address mode variable */
        le_byte_t le_mode = 0;

        /* address size variable */
        le_size_t le_size = 0;

        /* span variable */
        le_size_t le_span = 0;

        /* door pointer variable */
        le_door_t * le_pdoor = NULL;

        /* door pointer variable */
        le_door_t * le_sdoor = NULL;

        /* check consistency */
        if ( ( le_length % LE_ARRAY_ADDR ) != 0 ) {

            /* send message */
            return( LE_ERROR_IO_ARRAY );

        }

        /* parsing socket-array */
        while ( ( le_parse = le_address_serial( & le_addr, le_array, le_parse, _LE_GET ) ) <= le_length ) {

            /* retrieve address size */
            le_size = le_address_get_size( & le_addr );

            /* retrieve address span */
            le_span = le_address_get_span( & le_addr ) + le_size;

            /* reset socket-array */
            le_array_set_size( le_array + 1, 0 );

            /* check address mode */
            if ( ( le_mode = le_address_get_mode( & le_addr ) ) < LE_ADDRESS_OR ) {

                /* query and check door */
                if ( ( le_pdoor = le_switch_get_query( le_switch, le_address_get_time( & le_addr, le_mode - 1 ), & le_addr ) ) != NULL ) {

                    /* check mono-vertex detection */
                    if ( le_door_get_mono( le_pdoor ) == _LE_TRUE ) {

                        /* gathering process - mono-vertex */
                        le_door_io_mono_gather( le_pdoor, & le_addr, le_size, le_span, le_array + 1 );

                    }

                    /* check poly-vertex detection */
                    if ( le_door_get_poly( le_pdoor ) == _LE_TRUE ) {

                        /* gathering process - poly-vertex */
                        le_door_io_poly_gather( le_pdoor, & le_addr, le_size, le_span, le_array + 1 );

                    }

                }

            } else {

                /* query door */
                le_pdoor = le_switch_get_query( le_switch, le_address_get_time( & le_addr, 0 ), & le_addr );

                /* query door */
                le_sdoor = le_switch_get_query( le_switch, le_address_get_time( & le_addr, 1 ), & le_addr );

                /* check door mono-vertex offset */
                if ( ( le_door_get_mono( le_pdoor ) | le_door_get_poly( le_sdoor ) ) == _LE_TRUE ) {

                    /* gathering process - parallel */
                    le_door_io_mono_parallel( le_pdoor, le_sdoor, & le_addr, le_mode, le_size, le_span, le_array + 1 );

                }

            }

            /* write socket-array */
            if ( le_array_io_write( le_array + 1, LE_MODE_QUER, le_socket ) != LE_MODE_QUER ) {

                /* send message */
                return( LE_ERROR_IO_WRITE );

            }

        }

        /* send message */
        return( LE_ERROR_SUCCESS );

    }

